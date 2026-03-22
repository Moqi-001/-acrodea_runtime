#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <so_util/so_util.h>

#include "reimpl/io.h"
#include "reimpl/libKD/kd.h"
#include "reimpl/libLD/ld.h"
#include "reimpl/libLD/ld_patch.h"
#include "reimpl/pthr.h"
#include "utils/logger.h"

static int sub_52860(int *a1) {  // 某种验证，返回0为无错误
    int res = 0;
    l_debug("sub_52860(%p): %d", a1, res);
    return res;
}

static void patch_bootstrap(LdModule *mod) {
    l_debug("patch_bootstrap(%p): %s", mod, mod->so_name);

    bootstrap_start = (void *)((uintptr_t)mod->load_mem + mod->load_offset + mod->ehdr->e_entry);

    __ldModuleHookAddr(mod, 0x52860 + 1, (uintptr_t)&sub_52860);
}

static void patch_libc(LdModule *mod) {
    l_debug("patch_libc(%p): %s", mod, mod->so_name);

    // __ldModuleHookSymbol(mod, "fopen", (uintptr_t)&fopen_soloader);
    // __ldModuleHookSymbol(mod, "fclose", (uintptr_t)&fclose_soloader);
    __ldModuleHookSymbol(mod, "fseek", (uintptr_t)&fseek_soloader);
    __ldModuleHookSymbol(mod, "ftell", (uintptr_t)&ftell_soloader);
    // __ldModuleHookSymbol(mod, "fread", (uintptr_t)&fread_soloader);
    // __ldModuleHookSymbol(mod, "fwrite", (uintptr_t)&fwrite_soloader);
    // __ldModuleHookSymbol(mod, "fflush", (uintptr_t)&fflush);
    // __ldModuleHookSymbol(mod, "ferror", (uintptr_t)&ferror);
    // __ldModuleHookSymbol(mod, "fileno", (uintptr_t)&fileno);
    // __ldModuleHookSymbol(mod, "fcntl", (uintptr_t)&fcntl_soloader);
    // __ldModuleHookSymbol(mod, "fstat", (uintptr_t)&fstat_soloader);
    // __ldModuleHookSymbol(mod, "feof", (uintptr_t)&feof);
    // __ldModuleHookSymbol(mod, "fgetc", (uintptr_t)&fgetc);
    // __ldModuleHookSymbol(mod, "fgetpos", (uintptr_t)&fgetpos);
    // __ldModuleHookSymbol(mod, "fgets", (uintptr_t)&fgets);

    // __ldModuleHookSymbol(mod, "malloc", (uintptr_t)&malloc_soloader);
    // __ldModuleHookSymbol(mod, "calloc", (uintptr_t)&calloc);
    // __ldModuleHookSymbol(mod, "realloc", (uintptr_t)&realloc);
    // __ldModuleHookSymbol(mod, "memalign", (uintptr_t)&memalign);
    // __ldModuleHookSymbol(mod, "free", (uintptr_t)&free_soloader);
}

static void patch_libstdcxx(LdModule *mod) {
    l_debug("patch_libstdcxx(%p): %s", mod, mod->so_name);
}

static void patch_libobjc(LdModule *mod) {
    l_debug("patch_libobjc(%p): %s", mod, mod->so_name);
}

static void pre_patch_libobjc(LdModule *mod) {
    l_debug("pre_patch_libobjc(%p): %s", mod, mod->so_name);
}

static void patch_DevilMayCry4(LdModule *mod) {
    l_debug("patch_DevilMayCry4(%p): %s", mod, mod->so_name);

    // __ldModuleHookSymbol(mod, "slCreateEngine", (uintptr_t)&slCreateEngine);
}

void ldModuleInitialize_post_patch(LdModule *mod) {
    if (strcmp(mod->so_name, "bootstrap.exe") == 0) {
        patch_bootstrap(mod);
    } else if (strcmp(mod->so_name, "libc.so") == 0) {
        patch_libc(mod);
    } else if (strcmp(mod->so_name, "libstdc++.so") == 0) {
        patch_libstdcxx(mod);
    } else if (strcmp(mod->so_name, "libobjc.so") == 0) {
        patch_libobjc(mod);
    } else if (strcmp(mod->so_name, "DevilMayCry4.exe") == 0) {
        patch_DevilMayCry4(mod);
    }
}

void ldModuleInitialize_pre_patch(LdModule *mod) {
    if (strcmp(mod->so_name, "libobjc.so") == 0) {
        pre_patch_libobjc(mod);
    }
}

int soloader_patch_ld(so_module *mod) {
    ldModuleGetSymbol    = (void *)so_find_symbol(mod, "ldModuleGetSymbol");
    ldModuleLookupSymbol = (void *)so_find_symbol(mod, "ldModuleLookupSymbol");

    so_hook_symbol(mod, "ldGetErrorMessage", (uintptr_t)&ldGetErrorMessage);
    so_hook_symbol(mod, "ldModuleGetEntryPoint", (uintptr_t)&ldModuleGetEntryPoint);
    so_hook_symbol(mod, "ldModuleRelocate", (uintptr_t)&ldModuleRelocate);
    so_hook_symbol(mod, "ldModuleAllocAndLoadImage", (uintptr_t)&ldModuleAllocAndLoadImage);
    so_hook_symbol(mod, "ldModuleDestroyImage", (uintptr_t)&ldModuleDestroyImage);
    so_hook_symbol(mod, "ldModuleInitialize", (uintptr_t)&ldModuleInitialize);
    so_hook_symbol(mod, "ldModuleDeinitialize", (uintptr_t)&ldModuleDeinitialize);

    // so_hook_symbol(mod, "slCreateEngine", (uintptr_t)&slCreateEngine);
    // so_hook_symbol(mod, "slQueryNumSupportedEngineInterfaces", (uintptr_t)&slQueryNumSupportedEngineInterfaces);
    // so_hook_symbol(mod, "slQuerySupportedEngineInterfaces", (uintptr_t)&slQuerySupportedEngineInterfaces);

    return 0;
}