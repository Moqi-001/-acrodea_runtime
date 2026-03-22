#include <stdlib.h>
#include <string.h>

#include <kubridge.h>
#include <so_util/so_util.h>

#include "config.h"
#include "loader.h"
#include "patch.h"
#include "reimpl/libLD/ld.h"
#include "reimpl/libLD/ld_patch.h"
#include "utils/logger.h"

uintptr_t (*ldModuleGetSymbol)(LdModule *in_mod, const char *sym_name, uint32_t sym_hash, int type,
                               const char *ver_name, uint32_t ver_hash, LdModule **out_mod);

uintptr_t (*ldModuleLookupSymbol)(LdModule *mod, const char *sym_name, uint32_t sym_hash, int visibility,
                                  const char *ver_name, uint32_t ver_hash);

void ldSetErrorCode(LdContext *context, int code, const char *msg) {
    if (context) {
        context->error_code = code;
        if (msg)
            strncpy(context->error_msg, msg, 255);
        else
            context->error_msg[0] = '\0';
        l_debug("ldSetErrorCode: %s", context->error_code);
    }
}

const char *ldGetErrorMessage(LdContext *context) {
    if (context) {
        l_debug("ldGetErrorMessage: %s", context->error_msg);
        return (const char *)context->error_msg;
    }
    return NULL;
}

void *ldModuleGetEntryPoint(LdModule *mod) {
    if (mod && mod->ehdr && mod->ehdr->e_entry) {
        void *entry_point = (char *)mod->load_mem + mod->load_offset + mod->ehdr->e_entry;
        if (strcmp(mod->so_name, "bootstrap.exe") == 0) {
            // 将启动函数跳转到我们的函数
            entry_point = (void *)&bootstrap_start_soloader;
        }
        l_debug("ldModuleGetEntryPoint(%p): %s, return %p", mod, mod->so_name, entry_point);
        return entry_point;
    }

    l_debug("ldModuleGetEntryPoint(%p): failed!", mod);
    return NULL;
}

uint32_t ldModuleSymHash(const uint8_t *name) {
    uint32_t h = 0, g;
    while (*name) {
        h = (h << 4) + *name++;
        if ((g = (h & 0xf0000000)) != 0)
            h ^= (g >> 24);
        h &= 0x0fffffff;
    }
    return h;
}

int ldModuleInitialize(LdModule *mod) {
    if (!mod)
        return -1;

    if (!mod->load_mem) {
        l_debug("ldModuleInitialize(%p): %s, no need to initialize", mod, mod->so_name);
        return 0;
    }

    if (mod->status == LD_MODULE_STATUS_INITIALIZED) {
        l_debug("ldModuleInitialize(%p): %s is already initialized", mod, mod->so_name);
        return 0;
    }

    Elf32_Phdr *dynamic_phdr = mod->dynamic_phdr;
    if (!dynamic_phdr)
        return 0;

    // 初始化所有依赖库
    for (int i = 0; i < mod->needed_num; i++) {
        ldModuleInitialize(mod->needed_mods[i]);
    }

    uintptr_t  load_base     = (uintptr_t)mod->load_mem + mod->load_offset;
    Elf32_Dyn *dynamic       = (Elf32_Dyn *)(load_base + dynamic_phdr->p_vaddr);
    int        dynamic_num   = dynamic_phdr->p_filesz / sizeof(Elf32_Dyn);
    int (**init_array)(void) = NULL;
    uint32_t init_array_size = 0;
    int (*init_func)(void)   = NULL;

    // 遍历动态段
    for (int i = 0; i < dynamic_num; i++) {
        Elf32_Dyn *dyn = &dynamic[i];
        switch (dyn->d_tag) {
            case DT_INIT_ARRAY:
                init_array = (int (**)(void))(load_base + dyn->d_un.d_ptr);
                break;
            case DT_INIT_ARRAYSZ:
                init_array_size = dyn->d_un.d_val;
                break;
            case DT_INIT:
                init_func = (int (*)(void))(load_base + dyn->d_un.d_ptr);
                break;
        }
    }

    ldModuleInitialize_pre_patch(mod);

    // 执行初始化函数
    if (init_func) {
        // l_debug("ldModuleInitialize(%p): %s, init_func()", mod, mod->so_name);
        init_func();
    }

    // 执行初始化函数数组
    if (init_array && init_array_size > 0) {
        int init_func_num = init_array_size / sizeof(void *);
        for (int i = 0; i < init_func_num; i++) {
            // l_debug("ldModuleInitialize(%p): %s, init_array[%d]()", mod, mod->so_name, i);
            init_array[i]();
        }
    }

    mod->status = LD_MODULE_STATUS_INITIALIZED;
    l_debug("ldModuleInitialize(%p): %s success!", mod, mod->so_name);
    ldModuleInitialize_post_patch(mod);
    return 0;
}

int ldModuleDeinitialize(LdModule *mod) {
    if (!mod)
        return -1;

    l_debug("ldModuleDeinitialize(%p): name=%s, status=%u", mod, mod->so_name, mod->status);

    if (mod->context->exit_state) {
        mod->status = LD_MODULE_STATUS_DEINITIALIZED;
        return 0;
    }

    if (mod->status != LD_MODULE_STATUS_INITIALIZED) {
        l_debug("ldModuleDeinitialize(%p): %s no need to deinitialize", mod, mod->so_name);
        return 0;
    }

    if (!mod->dynamic_phdr)
        return 0;

    Elf32_Phdr *dynamic_phdr = mod->dynamic_phdr;
    if (!dynamic_phdr)
        return 0;

    uintptr_t  load_base      = (uintptr_t)mod->load_mem + mod->load_offset;
    Elf32_Dyn *dynamic        = (Elf32_Dyn *)(load_base + dynamic_phdr->p_vaddr);
    int        dynamic_num    = dynamic_phdr->p_filesz / sizeof(Elf32_Dyn);
    void (**fini_array)(void) = NULL;
    size_t fini_array_size    = 0;
    void (*fini_func)(void)   = NULL;

    // 遍历动态段
    for (int i = 0; i < dynamic_num; i++) {
        Elf32_Dyn *dyn = &dynamic[i];
        switch (dyn->d_tag) {
            case DT_FINI_ARRAY:
                fini_array = (void (**)(void))(load_base + dyn->d_un.d_ptr);
                break;
            case DT_FINI_ARRAYSZ:
                fini_array_size = dyn->d_un.d_val;
                break;
            case DT_FINI:
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

    mod->status = LD_MODULE_STATUS_DEINITIALIZED;

    l_debug("ldModuleDeinitialize(%p) %s success!", mod, mod->so_name);
    return 0;
}

so_hook __ldModuleHookAddr(LdModule *mod, uintptr_t vaddr, uintptr_t dst) {
    if (!mod) {
        so_hook h;
        return h;
    }

    uintptr_t addr = (uintptr_t)mod->load_mem + mod->load_offset + vaddr;
    return so_hook_addr(addr, dst);
}

so_hook __ldModuleHookSymbol(LdModule *mod, const char *symbol, uintptr_t dst) {
    if (!mod) {
        so_hook h;
        return h;
    }

    uint32_t  hash = ldModuleSymHash(symbol);
    uintptr_t addr = ldModuleLookupSymbol(mod, symbol, hash, STV_DEFAULT, NULL, 0);
    return so_hook_addr(addr, dst);
}
