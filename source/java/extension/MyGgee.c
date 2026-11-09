#include <stdint.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "MyGgee.h"

struct _MyGgee {
    uint32_t dummy;
};

static JClass sClass = NULL;

jint MyGgee_commonIsSLESCompatibility() {
    jint result = 1;
    so_logd("[MyGgee] commonIsSLESCompatibility(): %d", result);
    return result;
}

static JSymbol class_symbols[] = {
    // JAVA
    JSYMBOL_METHOD("commonIsSLESCompatibility", "()I", &MyGgee_commonIsSLESCompatibility),
};

static JClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/MyGgee",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _MyGgee),
    .dtor          = NULL,
};

void setup_MyGgee(void) {
    sClass = JavaCreateClass(&class_meta);
}
