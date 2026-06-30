#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <so_common/so_logger.h>
#include <so_libs/reimpl/bionic.h>

#include "config.h"
#include "ld.h"

int ld_patch_libc(LdModule *mod) {
    static int patched = 0;

    if (!patched) {
        patched = 1;
        so_logd("ld_patch_libc(%p): %s, %zu", mod, mod->so_path, mod->load_size);

        __ldModuleHookSymbol(mod, "fopen", (uintptr_t)&fopen_soloader, NULL);
        __ldModuleHookSymbol(mod, "fclose", (uintptr_t)&fclose_soloader, NULL);
        __ldModuleHookSymbol(mod, "fseek", (uintptr_t)&fseek_soloader, NULL);
        __ldModuleHookSymbol(mod, "ftell", (uintptr_t)&ftell_soloader, NULL);
        __ldModuleHookSymbol(mod, "fread", (uintptr_t)&fread_soloader, NULL);
        __ldModuleHookSymbol(mod, "fwrite", (uintptr_t)&fwrite_soloader, NULL);
        __ldModuleHookSymbol(mod, "fseeko", (uintptr_t)&fseeko_soloader, NULL);
        __ldModuleHookSymbol(mod, "ftello", (uintptr_t)&ftello_soloader, NULL);
        __ldModuleHookSymbol(mod, "fileno", (uintptr_t)&fileno_soloader, NULL);

        __ldModuleHookSymbol(mod, "open", (uintptr_t)&open_soloader, NULL);
        __ldModuleHookSymbol(mod, "close", (uintptr_t)&close_soloader, NULL);
        __ldModuleHookSymbol(mod, "lseek", (uintptr_t)&lseek_soloader, NULL);
        __ldModuleHookSymbol(mod, "read", (uintptr_t)&read_soloader, NULL);
        __ldModuleHookSymbol(mod, "write", (uintptr_t)&write_soloader, NULL);

        __ldModuleHookSymbol(mod, "fstat", (uintptr_t)&fstat_soloader, NULL);
        __ldModuleHookSymbol(mod, "stat", (uintptr_t)&stat_soloader, NULL);
    }

    __ldModuleHookSymbol(mod, "malloc", (uintptr_t)&malloc, NULL);
    __ldModuleHookSymbol(mod, "calloc", (uintptr_t)&calloc, NULL);
    __ldModuleHookSymbol(mod, "realloc", (uintptr_t)&realloc, NULL);
    __ldModuleHookSymbol(mod, "memalign", (uintptr_t)&memalign, NULL);
    __ldModuleHookSymbol(mod, "free", (uintptr_t)&free, NULL);

    return 0;
}
