#include <stdint.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "MyMovieBridge.h"

struct _MyMovieBridge {
    JString  path;
    jlong    offset;
    jlong    size;
    jboolean stop;
};

static JClass sClass = NULL;

jint MyMovieBridge_setType(MyMovieBridge thiz, jint type) {
    jint result = 0;
    so_logd("[MyMovieBridge] setType(%p,%d): %d", type, result);
    return result;
}

jint MyMovieBridge_play(MyMovieBridge thiz, JString path, jlong offset, jlong size) {
    jint result  = 0;
    thiz->path   = Object_retain(path);
    thiz->offset = offset;
    thiz->size   = size;

    const char *path_cstr = String_getUTFChars(path);
    so_logd("[MyMovieBridge] play(%p,%s,%lld,%lld): %d", thiz, path_cstr, offset, size, result);
    return result;
}

static JSymbol class_symbols[] = {
    // static native
    JSYMBOL_METHOD("setType", "(I)I", &MyMovieBridge_setType),
    JSYMBOL_METHOD("play", "(Ljava/lang/String;JJ)I", &MyMovieBridge_play),
};

static JClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/MyMovieBridge",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _MyMovieBridge),
    .dtor          = NULL,
};

void setup_MyMovieBridge(void) {
   sClass = JavaCreateClass(&class_meta);
}
