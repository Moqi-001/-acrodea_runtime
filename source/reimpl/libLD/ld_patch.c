#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <so_util/so_util.h>

#include "ld.h"
#include "ld_patch.h"
#include "reimpl/io.h"
#include "reimpl/pthr.h"
#include "utils/logger.h"

static int sub_52860(int a1) {  // 某种验证，返回0为无错误
    int res = 0;
    l_debug("sub_52860(%d): %d", res);
    return res;
}

static void patch_bootstrap(LdModule *mod) {
    l_debug("patch_bootstrap(%p): %s", mod, mod->mod_name);

    __ldModuleHookAddr(mod, 0x52860 + 1, (uintptr_t)&sub_52860);
}

static void patch_libc(LdModule *mod) {
    l_debug("patch_libc(%p): %s", mod, mod->mod_name);

    // __ldModuleHookSymbol(mod, "fopen", (uintptr_t)&fopen_soloader);
    // __ldModuleHookSymbol(mod, "fclose", (uintptr_t)&fclose_soloader);
    __ldModuleHookSymbol(mod, "fseek", (uintptr_t)&fseek_soloader);
    __ldModuleHookSymbol(mod, "ftell", (uintptr_t)&ftell_soloader);
    // __ldModuleHookSymbol(mod, "fread", (uintptr_t)&fread);
    // __ldModuleHookSymbol(mod, "fwrite", (uintptr_t)&fwrite);
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
    l_debug("patch_libstdcxx(%p): %s", mod, mod->mod_name);
}

static void patch_libobjc(LdModule *mod) {
    l_debug("patch_libobjc(%p): %s", mod, mod->mod_name);
}

static void patch_DevilMayCry4(LdModule *mod) {
    l_debug("patch_DevilMayCry4(%p): %s", mod, mod->mod_name);

    // __ldModuleHookSymbol(mod, "slCreateEngine", (uintptr_t)&slCreateEngine);
}

void ldModuleInitialize_patch(LdModule *mod) {
    if (strcmp(mod->mod_name, "bootstrap.exe") == 0) {
        patch_bootstrap(mod);
    } else if (strcmp(mod->mod_name, "libc.so") == 0) {
        patch_libc(mod);
    } else if (strcmp(mod->mod_name, "libstdc++.so") == 0) {
        patch_libstdcxx(mod);
    } else if (strcmp(mod->mod_name, "libobjc.so") == 0) {
        patch_libobjc(mod);
    } else if (strcmp(mod->mod_name, "DevilMayCry4.exe") == 0) {
        patch_DevilMayCry4(mod);
    }
}

int soloader_patch_ld(so_module *mod) {
    ldModuleGetSymbol    = (void *)so_find_symbol(mod, "ldModuleGetSymbol");
    ldModuleLookupSymbol = (void *)so_find_symbol(mod, "ldModuleLookupSymbol");

    hook_addr((uintptr_t)so_find_symbol(mod, "ldGetErrorMessage"), (uintptr_t)&ldGetErrorMessage);
    hook_addr((uintptr_t)so_find_symbol(mod, "ldModuleGetEntryPoint"), (uintptr_t)&ldModuleGetEntryPoint);
    hook_addr((uintptr_t)so_find_symbol(mod, "ldModuleRelocate"), (uintptr_t)&ldModuleRelocate);
    hook_addr((uintptr_t)so_find_symbol(mod, "ldModuleAllocAndLoadImage"), (uintptr_t)&ldModuleAllocAndLoadImage);
    hook_addr((uintptr_t)so_find_symbol(mod, "ldModuleDestroyImage"), (uintptr_t)&ldModuleDestroyImage);
    hook_addr((uintptr_t)so_find_symbol(mod, "ldModuleInitialize"), (uintptr_t)&ldModuleInitialize);
    hook_addr((uintptr_t)so_find_symbol(mod, "ldModuleDeinitialize"), (uintptr_t)&ldModuleDeinitialize);

    // hook_addr((uintptr_t)so_find_symbol(mod, "slCreateEngine"), (uintptr_t)&slCreateEngine);
    // hook_addr((uintptr_t)so_find_symbol(mod, "slQueryNumSupportedEngineInterfaces"), (uintptr_t)&slQueryNumSupportedEngineInterfaces);
    // hook_addr((uintptr_t)so_find_symbol(mod, "slQuerySupportedEngineInterfaces"), (uintptr_t)&slQuerySupportedEngineInterfaces);

    return 0;
}