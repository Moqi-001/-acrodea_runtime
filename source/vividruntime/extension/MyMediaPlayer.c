#include <stdint.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "MyMediaPlayer.h"

struct _MyMediaPlayer {
    uint32_t dummy;
};

static JavaClass sClass = NULL;

void MyMediaPlayer_ctor(MyMediaPlayer thiz) {
}

void MyMediaPlayer_destroy(MyMediaPlayer thiz) {
}

// 成功返回 0，失败返回 -4。
jint MyMediaPlayer_openFile(MyMediaPlayer thiz, JavaString path, jlong offset, jlong size) {
    const char *path_cstr = JavaGetStringUTFChars(path);
    so_logd("[MyMediaPlayer] openFile(%p,%s,%lld,%lld): media playback unsupported, return -4",
            thiz, path_cstr ? path_cstr : "(null)", offset, size);
    return -4;
}

static JavaSymbol class_symbols[] = {
    // JAVA
    JAVA_SYMBOL_METHOD("<init>", "()V", &MyMediaPlayer_ctor),
    JAVA_SYMBOL_METHOD("destroy", "()V", &MyMediaPlayer_destroy),
    JAVA_SYMBOL_METHOD("openFile", "(Ljava/lang/String;JJ)I", &MyMediaPlayer_openFile),
};

static JavaClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/MyMediaPlayer",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _MyMediaPlayer),
    .dtor          = NULL,
};

void MyMediaPlayer_class_init(void) {
    sClass = JavaCreateClass(&class_meta);
}
