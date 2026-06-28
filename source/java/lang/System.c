#include <sys/time.h>

#include <so_jni/so_jni.h>

#include "System.h"

// static, "currentTimeMillis", "()J"
jlong JavaSystem_currentTimeMillis() {
    struct timeval te;
    gettimeofday(&te, NULL);
    return (jlong)(te.tv_sec * 1000LL + te.tv_usec / 1000);
}

static JavaSymbol class_symbols[] = {
    // 静态方法
    JAVA_SYMBOL_STATIC_METHOD("currentTimeMillis", "()J", &JavaSystem_currentTimeMillis),
};

static JavaClassMeta class_meta = {
    .name          = "java/lang/System",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = 0,
};

void java_lang_System_class_init() {
    JavaCreateClass(&class_meta);
}
