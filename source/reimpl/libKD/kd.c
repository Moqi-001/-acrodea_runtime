#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <kubridge.h>
#include <so_util/so_util.h>

#include "reimpl/libKD/kd.h"
#include "reimpl/libLD/ld_mem.h"
#include "utils/logger.h"

char *__kdMapFilename(const char *filename, const char *mode) {
    if (!filename) {
        l_debug("__kdMapFilename(%s,%s): failed!", filename, mode);
        return NULL;
    }

    char *conv_path = malloc(1024);
    if (!conv_path) {
        l_debug("__kdMapFilename(%s,%s) failed: out of memory!", filename, mode);
        return NULL;
    }

    if (filename[0] != '/') {
        snprintf(conv_path, 1024, "%s", DATA_PATH, filename);
    } else {
        if (strncmp(filename, "/native/", 8) == 0) {
            snprintf(conv_path, 1024, "%s%s", DATA_PATH, filename + 8);
        } else if (strncmp(filename, "/lib/", 5) == 0) {
            snprintf(conv_path, 1024, "%sassets/%s", DATA_PATH, filename + 5);
        } else if (strncmp(filename, "/data/", 6) == 0) {
            snprintf(conv_path, 1024, "%sruntime/%s", DATA_PATH, filename + 6);
        } else if (strncmp(filename, "/res/", 5) == 0) {
            snprintf(conv_path, 1024, "%sRuntimeApps/1760/res/%s", DATA_PATH, filename + 5);
        } else if (strncmp(filename, "/tmp/", 5) == 0) {
            snprintf(conv_path, 1024, "%sRuntimeApps/1760/tmp/%s", DATA_PATH, filename + 5);
        } else {
            strcpy(conv_path, filename);
        }
    }

    l_debug("__kdMapFilename(%s,%s): -> %s", filename, mode, conv_path);
    return conv_path;
}

int __kdFlushICache(const void *ptr, size_t len) {
    l_debug("__kdFlushICache(%p,%d)", ptr, len);
    ldLoadMemFlushCaches(ldLoadMemGetContextFromData(ptr));
    // kuKernelFlushCaches(ptr, len);
    return 0;
}

int soloader_patch_kd(so_module *mod) {
    so_hook_symbol(mod, "__kdMapFilename", (uintptr_t)&__kdMapFilename);
    so_hook_symbol(mod, "__kdFlushICache", (uintptr_t)&__kdFlushICache);

    return 0;
}