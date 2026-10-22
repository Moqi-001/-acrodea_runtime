#include <stdio.h>
#include <string.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "Build.h"

// JNI 类
static jclass sClass = NULL;

jint android_os_Build_VERSION_SDK_INT = 19;  // Android 4.4 / KitKat

static JavaSymbol class_symbols[] = {
    JAVA_SYMBOL_STATIC_FIELD("SDK_INT", "I", &android_os_Build_VERSION_SDK_INT),
};

static JavaClassMeta class_meta = {
    .name          = "android/os/Build$VERSION",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = 0,
    .dtor          = NULL,
};

void android_os_Build_VERSION_class_init(void) {
    sClass = JavaCreateClass(&class_meta);
}
