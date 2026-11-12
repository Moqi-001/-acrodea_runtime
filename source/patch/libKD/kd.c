#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kubridge.h>
#include <so_common/so_logger.h>
#include <so_linker/so_linker.h>

#include "config.h"
#include "kd.h"

char *__kdMapFilename(const char *filename, const char *mode) {
    if (!filename) {
        so_loge("__kdMapFilename('%s','%s'): error!", filename, mode);
        return NULL;
    }

    char *map_path = malloc(1024);
    if (!map_path) {
        so_loge("__kdMapFilename('%s','%s') failed to alloc memory!", filename, mode);
        return NULL;
    }

    if (filename[0] == '/') {
        if (strncmp(filename, "/native/", 8) == 0) {
            snprintf(map_path, 1024, "%s%s", DATA_PATH, filename + 8);
        } else if (strncmp(filename, "/lib/", 5) == 0) {
            snprintf(map_path, 1024, "%sassets/%s", DATA_PATH, filename + 5);
        } else if (strncmp(filename, "/data/", 6) == 0) {
            snprintf(map_path, 1024, "%sruntime/%s/%s", DATA_PATH, RUNTIME_APP_ID_STR, filename + 6);
        } else {
            strcpy(map_path, filename);
        }
    } else {
        strcpy(map_path, filename);
    }

    so_logd("__kdMapFilename('%s','%s'): -> %s", filename, mode, map_path);
    return map_path;
}

int __kdFlushICache(const void *ptr, size_t len) {
    so_logd("__kdFlushICache(%p,%d)", ptr, len);
    kuKernelFlushCaches(ptr, len);
    return 0;
}

int soloader_patch_libKD(so_module *mod) {
    so_logd("soloader_patch_libKD(%p)", mod);

    so_module_hook_export(mod, "__kdMapFilename", &__kdMapFilename, NULL);
    so_module_hook_export(mod, "__kdFlushICache", &__kdFlushICache, NULL);

    return 0;
}
