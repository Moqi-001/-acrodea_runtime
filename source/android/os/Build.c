#include <stdio.h>
#include <string.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "Build.h"

struct _Build {
    uint32_t reserved;
};

extern void android_os_Build_VERSION_class_init(void);

void android_os_Build_class_init(void) {
    android_os_Build_VERSION_class_init();
}
