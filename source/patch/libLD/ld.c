#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kubridge.h>
#include <so_common/so_logger.h>
#include <so_linker/so_internal.h>
#include <so_linker/so_linker.h>

#include "ld.h"
#include "utils/utils.h"

uint32_t (*ldModuleSymHash)(const char *name);
void *(*ldModuleLookupSymbol)(LdModule *mod, const char *sym_name, uint32_t sym_hash, int visibility,
                              const char *ver_name, uint32_t ver_hash);

extern int ld_patch_preinit_disgaea(LdModule *mod);

extern int ld_patch_libc(LdModule *mod);
extern int ld_patch_libcpp(LdModule *mod);
extern int ld_patch_bootstrap(LdModule *mod);
extern int ld_patch_disgaea(LdModule *mod);

static int ld_patch_preinit(LdModule *mod) {
    if (strcmp(mod->so_name, "Mugenmakai_Disgaea_En_gstore.exe") == 0) {
        ld_patch_preinit_disgaea(mod);
    }

    return 0;
}

static int ld_patch(LdModule *mod) {
    if (strcmp(mod->so_name, "libc.so") == 0) {
        ld_patch_libc(mod);
    } else if (strcmp(mod->so_name, "libstdc++.so") == 0) {
        ld_patch_libcpp(mod);
    } else if (strcmp(mod->so_name, "bootstrap.exe") == 0) {
        ld_patch_bootstrap(mod);
    } else if (strcmp(mod->so_name, "Mugenmakai_Disgaea_En_gstore.exe") == 0) {
        ld_patch_disgaea(mod);
    }

    return 0;
}

so_hook_ref ldModuleAllocAndLoadImage_hook_ref;

int ldModuleAllocAndLoadImage(LdModule *mod, const char *path, const char *err_msg) {
    char dump_path[1024];
    snprintf(dump_path, 1024, "%s%s", DATA_PATH, mod->so_name);
    file_save(dump_path, mod->load_base, mod->load_size);
    so_logd("dump_so: %s", dump_path);

    return SO_CONTINUE(int, &ldModuleAllocAndLoadImage_hook_ref, mod, path, err_msg);
}

int ldModuleInitialize(LdModule *mod) {
    if (!mod)
        return -1;

    if (!mod->load_base) {
        so_logd("ldModuleInitialize(%p): %s, no need to initialize", mod, mod->so_name);
        return 0;
    }

    if (mod->status == LD_MODULE_STATUS_INITIALIZED) {
        so_logd("ldModuleInitialize(%p): %s is already initialized", mod, mod->so_name);
        return 0;
    }

    Elf32_Phdr *dynamic_phdr = mod->dynamic_phdr;
    if (!dynamic_phdr)
        return 0;

    // 初始化所有依赖库
    for (int i = 0; i < mod->needed_num; i++) {
        ldModuleInitialize(mod->needed_mods[i]);
    }

    uintptr_t  load_bias     = (uintptr_t)mod->load_base + mod->load_offset;
    Elf32_Dyn *dynamic       = (Elf32_Dyn *)(load_bias + dynamic_phdr->p_vaddr);
    int        dynamic_num   = dynamic_phdr->p_filesz / sizeof(Elf32_Dyn);
    int (**init_array)(void) = NULL;
    uint32_t init_array_size = 0;
    int (*init_func)(void)   = NULL;

    // 遍历动态段
    for (int i = 0; i < dynamic_num; i++) {
        Elf32_Dyn *dyn = &dynamic[i];
        switch (dyn->d_tag) {
            case DT_INIT_ARRAY:
                init_array = (int (**)(void))(load_bias + dyn->d_un.d_ptr);
                break;
            case DT_INIT_ARRAYSZ:
                init_array_size = dyn->d_un.d_val;
                break;
            case DT_INIT:
                init_func = (int (*)(void))(load_bias + dyn->d_un.d_ptr);
                break;
        }
    }

    // init_func/init_array 会调用一些函数来初始化，所以有些hook需要在这之前执行。
    ld_patch_preinit(mod);

    // 执行初始化函数
    if (init_func && (intptr_t)init_func != -1) {
        // so_logd("ldModuleInitialize(%p): %s, init_func()", mod, mod->so_name);
        init_func();
    }

    // 执行初始化函数数组
    if (init_array && init_array_size > 0) {
        int init_func_num = init_array_size / sizeof(void *);
        for (int i = 0; i < init_func_num; i++) {
            // so_logd("ldModuleInitialize(%p): %s, init_array[%d]()", mod, mod->so_name, i);
            if (init_array[i] && (intptr_t)init_array[i] != -1) {
                init_array[i]();
            }
        }
    }

    mod->status = LD_MODULE_STATUS_INITIALIZED;
    so_logd("ldModuleInitialize(%p): %s success!", mod, mod->so_name);
    so_loga("ldModule: %s, load_base=%p, load_size=0x%x", mod->so_name, mod->load_base, mod->load_size);

    // 在初始化后对 so 进行一些 patch 操作
    ld_patch(mod);
    return 0;
}


int soloader_patch_libLD(so_module *mod) {
    so_logd("soloader_patch_libLD(%p)", mod);
    ldModuleSymHash      = (void *)so_module_find_symbol(mod, "ldModuleSymHash");
    ldModuleLookupSymbol = (void *)so_module_find_symbol(mod, "ldModuleLookupSymbol");

    so_module_hook_symbol(mod, "ldModuleInitialize", &ldModuleInitialize, NULL);
    // so_module_hook_symbol(mod, "ldModuleAllocAndLoadImage", &ldModuleAllocAndLoadImage, &ldModuleAllocAndLoadImage_hook_ref);

    return 0;
}
