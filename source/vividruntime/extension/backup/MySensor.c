#include <stdint.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "MySensor.h"

struct _MySensor {
    uint32_t dummy;
};

static JClass sClass = NULL;

static void dummy() {
}

static JavaSymbol class_symbols[] = {
    // static native
    JAVA_SYMBOL_STATIC_METHOD("dummy", "()V", &dummy),
};

static JavaClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/MySensor",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _MySensor),
};

void MySensor_class_init(void) {
    sClass = JavaCreateClass(&class_meta);
}
