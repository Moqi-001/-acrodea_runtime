#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kubridge.h>

#include "config.h"
#include "reimpl/libLD/ld.h"
#include "reimpl/libLD/ld_mem.h"
#include "utils/logger.h"

// 导出已解密的so文件 (在调用ldModuleAllocAndLoadImage函数时导出效为合适，否则之后原数据会被释放并被替换)
static int dump_so_file(LdModule *mod, const char *dst_dir) {
    char dst_name[256];
    char dst_path[1024];

    const char *p = strrchr(mod->so_name, '.');  // 把后缀合改为.so，因为有些.so文件以.exe后缀命名
    if (!p) {
        snprintf(dst_name, sizeof(dst_name), "%s.so", mod->so_name);
    } else {
        size_t len = p - mod->so_name;
        strncpy(dst_name, mod->so_name, len);
        strcpy(dst_name + len, ".so");
    }

    snprintf(dst_path, sizeof(dst_path), "%s%s", dst_dir, dst_name);

    FILE *f = fopen(dst_path, "wb");
    if (!f) {
        l_debug("[Error] Dump so file error: failed to fopen %s!\n", dst_path);
        return -1;
    }

    fwrite(mod->load_mem, 1, mod->load_size, f);
    fclose(f);

    l_debug("[DUMP] Dump so file '%s' success: -> '%s'\n", mod->so_name, dst_path);
    return 0;
}

int ldModuleAllocAndLoadImage(LdModule *mod, const char *path, const char *err_msg) {
    if (!mod)
        return -1;

    if (!mod->load_mem) {
        l_debug("ldModuleAllocAndLoadImage(%p,%s): No need to execute", mod, path);
        return 0;
    }

    // 导出.so文件到 DATA_PATH 路径下
    // dump_so_file(mod, DATA_PATH);

    uintptr_t         load_addr    = LOAD_ADDRESS;
    LdLoadMemContext *mem_context  = NULL;
    char             *file_mem     = (char *)mod->load_mem;
    int               failed_count = 0;
    mod->load_mem                  = NULL;
    mod->load_size                 = 0;
    mod->load_offset               = 0;

LOAD:
    for (int i = 0; i < mod->ehdr->e_phnum; i++) {
        Elf32_Phdr *phdr = &mod->phdr[i];
        if (phdr->p_type == PT_LOAD) {
            if (!mem_context) {
                mem_context = ldLoadMemCreateContext(load_addr);
                if (!mem_context)
                    goto FAILED;
                mod->load_mem = (void *)mem_context->load_addr;
            }

            if (phdr->p_memsz > 0) {
                int             flags = ((phdr->p_flags & PF_X) == PF_X) ? SCE_KERNEL_MEMBLOCK_TYPE_USER_RX : SCE_KERNEL_MEMBLOCK_TYPE_USER_RW;
                uintptr_t       addr  = mem_context->load_addr + phdr->p_vaddr;
                LdLoadMemEntry *entry = ldLoadMemCreateEntry(addr, flags, phdr->p_memsz, phdr->p_align);
                if (!entry)
                    goto FAILED;
                ldLoadMemAddEntry(mem_context, entry);

                kuKernelCpuUnrestrictedMemcpy((void *)addr, (void *)((uintptr_t)file_mem + phdr->p_offset), phdr->p_filesz);

                if (phdr->p_memsz > phdr->p_filesz) {
                    size_t zero_size = phdr->p_memsz - phdr->p_filesz;
                    char  *zero      = malloc(zero_size);
                    if (zero) {
                        memset(zero, 0, zero_size);
                        kuKernelCpuUnrestrictedMemcpy((void *)(addr + phdr->p_filesz), zero, zero_size);
                        free(zero);
                    }
                }
            }

            if (mod->load_size < phdr->p_vaddr + phdr->p_memsz)
                mod->load_size = phdr->p_vaddr + phdr->p_memsz;
        }
    }

    free(file_mem);
    l_debug("ldModuleAllocAndLoadImage(%p,%s) success: %s, load_mem=0x%x", mod, path, mod->so_name, mod->load_mem);
    return 0;

FAILED:
    if (mem_context) {
        ldLoadMemDestroyContext(mem_context);
        mem_context = NULL;
    }

    if (failed_count < 100) {  // 如果失败次数<100，将映射地址递增，重新尝试
        // l_debug("ldModuleAllocAndLoadImage(%p) failed on addr 0x%x, will retry!", mod, load_addr);
        failed_count++;
        load_addr += 0x01000000;
        goto LOAD;
    }

    free(file_mem);
    l_debug("ldModuleAllocAndLoadImage(%p,%s) failed!", mod, path);
    return -1;
}

void ldModuleDestroyImage(LdModule *mod) {
    if (mod && mod->load_mem) {
        l_debug("ldModuleDestroyImage(%p): %s", mod, mod->so_name);
        ldLoadMemDestroyContext(ldLoadMemGetContextFromData(mod->load_mem));
        mod->load_mem  = NULL;
        mod->load_size = 0;
    }
}

int ldModuleProtectImage(LdModule *mod) {
    return 0;
}
