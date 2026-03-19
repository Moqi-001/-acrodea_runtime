#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MyErrorDialog
 */

// "setMessage", "(Ljava/lang/String;)V"
void MyErrorDialog_setMessage(jobject obj, jmethodID id, va_list args) {
    const char *value = va_arg(args, char *);
    l_debug("[JNI] MyErrorDialog::setMessage(%s)", value);
}

// "setCode", "(I)V"
void MyErrorDialog_setCode(jobject obj, jmethodID id, va_list args) {
    int value = va_arg(args, int);
    l_debug("[JNI] MyErrorDialog::setCode(0x%x)", value);
}
