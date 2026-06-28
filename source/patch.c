#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/unistd.h>

#include <so_common/so_logger.h>
#include <so_common/so_utils.h>
#include <so_jni/so_jni.h>
#include <so_linker/so_linker.h>

#include "java.h"
#include "utils/utils.h"

#include "patch.h"
#include "patch/libKD/kd.h"
#include "patch/libLD/ld.h"

int __wrap_chdir_soloader(const char *dir) {
    char buf[1024];

    if (strcmp(dir, "/runtime") == 0) {
        snprintf(buf, sizeof(buf), "%sruntime/", DATA_PATH);
        dir = buf;
    } else {
        int len = strlen(dir);
        if (dir[len - 1] != '/') {  // psvita的chdir需以/结尾
            if (dir == buf) {
                strcat(buf, "/");
            } else {
                strcpy(buf, dir);
                buf[len++] = '/';
                buf[len]   = '\0';
                dir        = buf;
            }
        }
    }

    int result = chdir(dir);
    so_logd("chdir(%s): %d", dir, result);
    return result;
}

FILE *__wrap_fopen_soloader(const char *filename, const char *mode) {
    char buf[1024];

    if (filename[0] == '/') {
        if (strncmp(filename, "/native/", 8) == 0) {
            snprintf(buf, 1024, "%s%s", DATA_PATH, filename + 8);
            filename = buf;
        } else if (strncmp(filename, "/lib/", 5) == 0) {
            snprintf(buf, 1024, "%sassets/%s", DATA_PATH, filename + 5);
            filename = buf;
        } else if (strncmp(filename, "/data/", 6) == 0) {
            snprintf(buf, 1024, "%sruntime/%s", DATA_PATH, filename + 6);
            filename = buf;
        }
    }

    FILE *result = fopen(filename, mode);
    so_logd("fopen_patch('%s','%s'): %p", filename, mode, result);
    return result;
}

int soloader_patch_runtime(so_module *mod) {
    soloader_patch_libKD(mod);
    soloader_patch_libLD(mod);

    return 0;
}
