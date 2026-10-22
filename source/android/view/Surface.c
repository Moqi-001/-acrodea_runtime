#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "Surface.h"

struct _Surface {
    int mSurface;
    int mNativeSurface;
    int mNativeObject;
};

static JavaClass sClass = NULL;

void AndroidSurface_ctor(Surface thiz) {
    memset(thiz, 0, sizeof(struct _Surface));
}

void AndroidSurface_dtor(Surface thiz) {
}

Surface AndroidSurface_new() {
    Surface surface = JavaAllocObject(sClass);
    if (!surface) {
        return NULL;
    }

    AndroidSurface_ctor(surface);

    return surface;
}

static JavaSymbol class_symbols[] = {
    // static native
    JAVA_SYMBOL_FIELD("mSurface", "I", offsetof(struct _Surface, mSurface)),
    JAVA_SYMBOL_FIELD("mNativeSurface", "I", offsetof(struct _Surface, mNativeSurface)),
    JAVA_SYMBOL_FIELD("mNativeObject", "I", offsetof(struct _Surface, mNativeObject)),
};

static JavaClassMeta class_meta = {
    .name          = "android/view/Surface",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _Surface),
    .dtor          = (JavaDtor)AndroidSurface_dtor,
};

void android_view_Surface_class_init(void) {
    sClass = JavaCreateClass(&class_meta);
}
