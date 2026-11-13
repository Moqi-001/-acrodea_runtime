#include <stdint.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "MyErrorDialog.h"

struct _MyErrorDialog {
    uint32_t dummy;
};

static JClass sClass = NULL;

void MyErrorDialog_setMessage(MyErrorDialog thiz, JString msg) {
    const char *msg_cstr = String_getUTFChars(msg);
    so_loge("[MyErrorDialog] setMessage(%p,'%s')", thiz, msg_cstr);
}

void MyErrorDialog_setCode(MyErrorDialog thiz, jint code) {
    so_loge("[MyErrorDialog] setCode(%p,0x%X)", thiz, code);
}

static JSymbol class_symbols[] = {
    // JAVA
    JAVA_SYMBOL_METHOD("setMessage", "(Ljava/lang/String;)V", &MyErrorDialog_setMessage),
    JAVA_SYMBOL_METHOD("setCode", "(I)V", &MyErrorDialog_setCode),
};

static JClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/MyErrorDialog",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _MyErrorDialog),
    .dtor          = NULL,
};

void setup_MyErrorDialog(void) {
    sClass = JavaCreateClass(&class_meta);
}
