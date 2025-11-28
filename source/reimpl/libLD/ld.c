#include <stdlib.h>
#include <string.h>

#include <kubridge.h>
#include <so_util/so_util.h>

#include "config.h"
#include "ld.h"
#include "ld_mem.h"
#include "patch.h"
#include "utils/logger.h"

const char *ldGetErrorMessage(LdContext *context) {
    if (context) {
        l_debug("ldGetErrorMessage: %s", context->error_msg);
        return (const char *)context->error_msg;
    }
    return NULL;
}

void *ldModuleGetEntryPoint(LdModule *mod) {
    l_debug("ldModuleGetEntryPoint(%p)", mod);
    if (mod && mod->ehdr && mod->ehdr->e_entry) {
        void *entry_point = (char *)mod->code_mem + mod->load_offset + mod->ehdr->e_entry;
        l_debug("mod_name=\"%s\", entry_point=%p", mod->mod_name, entry_point);
        if (strcmp(mod->mod_name, "bootstrap.exe") == 0) {
            bootstrap_start = entry_point;
            entry_point     = (void *)&bootstrap_start_soloader;
        }
        return entry_point;
    }

    return NULL;
}

int ldModuleAllocAndLoadImage(LdModule *mod, const char *path, const char *err_msg) {
    LdLoadMemContext *mem_context;
    int               failed_count;
    char             *file_mem;
    uintptr_t         load_addr;

    if (!mod || !mod->code_mem)
        return -1;

    l_debug("ldModuleAllocAndLoadImage(%p): name=%s", mod, mod->mod_name);

    load_addr        = LOAD_ADDRESS;
    mem_context      = NULL;
    failed_count     = 0;
    file_mem         = (char *)mod->code_mem;
    mod->code_mem    = NULL;
    mod->code_size   = 0;
    mod->load_offset = 0;

LOAD:
    for (int i = 0; i < mod->ehdr->e_phnum; i++) {
        Elf32_Phdr *phdr = &mod->phdr[i];
        if (phdr->p_type == PT_LOAD) {
            if (!mem_context) {
                mem_context = ldLoadMemCreateContext(load_addr);
                if (!mem_context)
                    goto FAILED;
                mod->code_mem = (void *)mem_context->load_addr;
            }

            if (phdr->p_memsz > 0) {
                int             flags = ((phdr->p_flags & PF_X) == PF_X) ? SCE_KERNEL_MEMBLOCK_TYPE_USER_RX : SCE_KERNEL_MEMBLOCK_TYPE_USER_RW;
                uintptr_t       addr  = mem_context->load_addr + phdr->p_vaddr;
                LdLoadMemEntry *entry = ldLoadMemCreateEntry(addr, flags, phdr->p_memsz, phdr->p_align);
                if (!entry)
                    goto FAILED;
                ldLoadMemAddEntry(mem_context, entry);

                kuKernelCpuUnrestrictedMemcpy((void *)addr, (void *)((uintptr_t)file_mem + phdr->p_offset), phdr->p_filesz);

                size_t zero_size = phdr->p_memsz - phdr->p_filesz;
                if (zero_size > 0) {
                    char *zero = malloc(zero_size);
                    if (zero) {
                        memset(zero, 0, zero_size);
                        kuKernelCpuUnrestrictedMemcpy((void *)(addr + phdr->p_filesz), zero, zero_size);
                        free(zero);
                    }
                }
            }

            if (mod->code_size < phdr->p_vaddr + phdr->p_memsz)
                mod->code_size = phdr->p_vaddr + phdr->p_memsz;
        }
    }

    free(file_mem);
    l_debug("ldModuleAllocAndLoadImage success!");
    return 0;

FAILED:
    if (mem_context) {
        ldLoadMemDestroyContext(mem_context);
        mem_context = NULL;
    }

    if (failed_count < 50) {  // 如果失败次数<50，将映射地址递增，重新尝试
        l_debug("ldModuleAllocAndLoadImage failed on addr 0x%x, will retry!", load_addr);
        failed_count++;
        load_addr += 0x01000000;
        goto LOAD;
    }

    free(file_mem);
    return -1;
}

void ldModuleDestroyImage(LdModule *mod) {
    if (mod && mod->code_mem) {
        l_debug("ldModuleDestroyImage(%p): name=%s", mod, mod->mod_name);
        ldLoadMemDestroyContext(ldLoadMemGetContextFromData(mod->code_mem));
        mod->code_mem  = NULL;
        mod->code_size = 0;
    }
}

int ldModuleInitialize(LdModule *mod) {
    if (!mod)
        return -1;

    l_debug("ldModuleInitialize(%p): name=%s, status=%u", mod, mod->mod_name, mod->status);

    if (mod->status == LD_MODULE_STATUS_INITIALIZED) {
        l_debug("ldModuleInitialize(%p): %s is already initialized", mod, mod->mod_name);
        return 0;
    }

    Elf32_Phdr *dyn_phdr = mod->dyn_phdr;
    if (!dyn_phdr)
        return 0;

    // 初始化所有依赖库
    for (int i = 0; i < mod->needed_num; i++) {
        ldModuleInitialize(mod->needed_mods[i]);
    }

    uintptr_t  load_base      = (uintptr_t)mod->code_mem + mod->load_offset;
    Elf32_Dyn *dynamic        = (Elf32_Dyn *)(load_base + dyn_phdr->p_vaddr);
    int        dynamic_num    = dyn_phdr->p_memsz / sizeof(Elf32_Dyn);
    void (**init_array)(void) = NULL;
    uint32_t init_array_size  = 0;
    void (*init_func)(void)   = NULL;

    // 遍历动态段
    for (int i = 0; i < dynamic_num; i++) {
        Elf32_Dyn *dyn = &dynamic[i];
        switch (dyn->d_tag) {
            case DT_INIT_ARRAY:  // 25
                init_array = (void (**)(void))(load_base + dyn->d_un.d_ptr);
                break;
            case DT_INIT_ARRAYSZ:  // 27
                init_array_size = dyn->d_un.d_val;
                break;
            case DT_INIT:  // 12
                l_debug("ldModuleInitialize(%p): init_func: base=0x%x, d_ptr=0x%x", mod, load_base, dyn->d_un.d_ptr);
                init_func = (void (*)(void))(load_base + dyn->d_un.d_ptr);
                l_debug("ldModuleInitialize(%p): init_func=%p", mod, init_func);
                break;
        }
    }

    // 执行初始化函数
    if (init_func) {
        l_debug("ldModuleInitialize(%p): init_func=%p", mod, init_func);
        init_func();
        l_debug("ldModuleInitialize(%p): init_func success!", mod);
    }

    // 执行初始化函数数组
    if (init_array && init_array_size > 0) {
        int init_func_num = init_array_size / sizeof(void *);
        for (int i = 0; i < init_func_num; i++) {
            l_debug("ldModuleInitialize(%p): init_array[%d]=%p", mod, i, init_array[i]);
            init_array[i]();
            l_debug("ldModuleInitialize(%p): init_array[%d] success!", mod, i);
        }
    }

    mod->status = LD_MODULE_STATUS_INITIALIZED;  // 2

    l_debug("ldModuleInitialize %s success!", mod->mod_name);
    return 0;
}

int ldModuleDeinitialize(LdModule *mod) {
    if (!mod)
        return -1;

    l_debug("ldModuleDeinitialize(%p): name=%s, status=%u", mod, mod->mod_name, mod->status);

    if (mod->context->exit_state) {
        mod->status = LD_MODULE_STATUS_DEINITIALIZED;  // 3
        return 0;
    }

    if (mod->status != LD_MODULE_STATUS_INITIALIZED) {
        l_debug("ldModuleDeinitialize(%p): %s is not initialized", mod, mod->mod_name);
        return 0;
    }

    if (!mod->dyn_phdr)
        return 0;

    Elf32_Phdr *dyn_phdr = mod->dyn_phdr;
    if (!dyn_phdr)
        return 0;

    char      *load_base      = (char *)mod->code_mem + mod->load_offset;
    Elf32_Dyn *dynamic        = (Elf32_Dyn *)(load_base + dyn_phdr->p_vaddr);
    int        dynamic_num    = dyn_phdr->p_memsz / sizeof(Elf32_Dyn);
    void (**fini_array)(void) = NULL;
    size_t fini_array_size    = 0;
    void (*fini_func)(void)   = NULL;

    // 遍历动态段
    for (int i = 0; i < dynamic_num; i++) {
        Elf32_Dyn *dyn = &dynamic[i];
        switch (dyn->d_tag) {
            case DT_FINI_ARRAY:  // 26
                fini_array = (void (**)(void))(load_base + dyn->d_un.d_ptr);
                break;
            case DT_FINI_ARRAYSZ:  // 28
                fini_array_size = dyn->d_un.d_val;
                break;
            case DT_FINI:  // 13
                fini_func = (void (*)(void))(load_base + dyn->d_un.d_ptr);
                break;
        }
    }

    // 执行反初始化函数数组
    if (fini_array) {
        int fini_array_num = fini_array_size / sizeof(void *);
        for (int i = 0; i < fini_array_num; i++) {
            fini_array[i]();
        }
    }

    // 执行反初始化函数
    if (fini_func) {
        fini_func();
    }

    mod->status = LD_MODULE_STATUS_DEINITIALIZED;  // 3

    l_debug("ldModuleDeinitialize %s success!", mod->mod_name);
    return 0;
}
