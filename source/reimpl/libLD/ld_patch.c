#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <so_util/so_util.h>

#include "ld.h"
#include "ld_patch.h"
#include "reimpl/io.h"
#include "utils/logger.h"
#include "reimpl/pthr.h"

int sub_52860(int a1)
{
    debugPrintf("sub_52860(%d)",a1);
    return 0;
}

void* objc_calloc(size_t a1,size_t a2)
{
    debugPrintf("objc_calloc(%u,%u)",a1,a2);
    void* p= calloc(a1,a2);
    debugPrintf("objc_calloc() retutn:%p",p);

    return p;
}

static void patch_bootstrap(LdModule *mod) {
    bootstrap_mod = mod;
    // __ldModuleHookAddr(bootstrap_mod, 0x101154 + 1, (uintptr_t)&sub_101154);
    __ldModuleHookAddr(bootstrap_mod, 0x52860 + 1, (uintptr_t)&sub_52860);
}

static void patch_libobjc(LdModule *mod) {
    __ldModuleHookSymbol(mod, "objc_calloc", (uintptr_t)&objc_calloc);
}

static void patch_libc(LdModule *mod) {
    __ldModuleHookSymbol(mod, "fseek", (uintptr_t)&fseek_soloader);
    __ldModuleHookSymbol(mod, "ftell", (uintptr_t)&ftell_soloader);
    /*
    __ldModuleHookSymbol(mod, "fopen", (uintptr_t)&fopen_soloader);
    __ldModuleHookSymbol(mod, "fread", (uintptr_t)&fread);
    __ldModuleHookSymbol(mod, "fwrite", (uintptr_t)&fwrite);
    __ldModuleHookSymbol(mod, "fclose", (uintptr_t)&fclose_soloader);
    __ldModuleHookSymbol(mod, "fflush", (uintptr_t)&fflush);
    __ldModuleHookSymbol(mod, "ferror", (uintptr_t)&ferror);
    __ldModuleHookSymbol(mod, "fileno", (uintptr_t)&fileno);
    __ldModuleHookSymbol(mod, "fcntl", (uintptr_t)&fcntl_soloader);
    __ldModuleHookSymbol(mod, "fstat", (uintptr_t)&fstat_soloader);
    __ldModuleHookSymbol(mod, "feof", (uintptr_t)&feof);
    __ldModuleHookSymbol(mod, "fgetc", (uintptr_t)&fgetc);
    __ldModuleHookSymbol(mod, "fgetpos", (uintptr_t)&fgetpos);
    __ldModuleHookSymbol(mod, "fgets", (uintptr_t)&fgets);
    */
    __ldModuleHookSymbol(mod, "malloc", (uintptr_t)&malloc);
    __ldModuleHookSymbol(mod, "calloc", (uintptr_t)&calloc);
    __ldModuleHookSymbol(mod, "realloc", (uintptr_t)&realloc);
    __ldModuleHookSymbol(mod, "free", (uintptr_t)&free);
    __ldModuleHookSymbol(mod, "memalign", (uintptr_t)&memalign);
}

void ldModuleInitialize_patch(LdModule *mod) {
    l_debug("ldModuleInitialize_patch(%p): %s", mod, mod->mod_name);

    if (strcmp(mod->mod_name, "bootstrap.exe") == 0) {
        patch_bootstrap(mod);
    } else if (strcmp(mod->mod_name, "libc.so") == 0) {
        patch_libc(mod);
    }else if (strcmp(mod->mod_name, "libobjc.so") == 0) {
        patch_libobjc(mod);
    }
}

int so_patch_ld(so_module *mod) {
    ldModuleGetSymbol    = (void *)so_symbol(mod, "ldModuleGetSymbol");
    ldModuleLookupSymbol = (void *)so_symbol(mod, "ldModuleLookupSymbol");

    hook_addr((uintptr_t)so_symbol(mod, "ldGetErrorMessage"), (uintptr_t)&ldGetErrorMessage);
    hook_addr((uintptr_t)so_symbol(mod, "ldModuleGetEntryPoint"), (uintptr_t)&ldModuleGetEntryPoint);
    hook_addr((uintptr_t)so_symbol(mod, "ldModuleRelocate"), (uintptr_t)&ldModuleRelocate);
    hook_addr((uintptr_t)so_symbol(mod, "ldModuleAllocAndLoadImage"), (uintptr_t)&ldModuleAllocAndLoadImage);
    hook_addr((uintptr_t)so_symbol(mod, "ldModuleDestroyImage"), (uintptr_t)&ldModuleDestroyImage);
    hook_addr((uintptr_t)so_symbol(mod, "ldModuleInitialize"), (uintptr_t)&ldModuleInitialize);
    hook_addr((uintptr_t)so_symbol(mod, "ldModuleDeinitialize"), (uintptr_t)&ldModuleDeinitialize);

    return 0;
}