#include <stdint.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "MyMediaPlayer.h"

struct _MyMediaPlayer {
    uint32_t dummy;
};

static JClass sClass = NULL;

void MyMediaPlayer_ctor(MyMediaPlayer thiz) {
}

void MyMediaPlayer_destroy(MyMediaPlayer thiz) {
}

static JavaSymbol class_symbols[] = {
    // JAVA
    JAVA_SYMBOL_METHOD("<init>", "()V", &MyMediaPlayer_ctor),
    JAVA_SYMBOL_METHOD("destroy", "()V", &MyMediaPlayer_destroy),
};

static JavaClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/MyMediaPlayer",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _MyMediaPlayer),
};

void MyMediaPlayer_class_init(void) {
    sClass = JavaCreateClass(&class_meta);
}
