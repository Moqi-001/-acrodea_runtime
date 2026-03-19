#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MyPopup
 */

// "getText", "()Ljava/lang/String;"
jobject MyPopup_getText(jobject obj, jmethodID id, va_list args) {
    const char *res = "test";
    l_debug("[JNI] MyPopup::getText: %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "setPropertycv", "(ILjava/lang/String;)I"
jint MyPopup_setPropertycv(jobject obj, jmethodID id, va_list args) {
    int         res   = 0;
    const char *value = va_arg(args, char *);
    l_debug("[JNI] MyPopup::setPropertycv(%s): %d", value, res);
    return (jint)res;
}

// "show", "()I"
jint MyPopup_show(jobject obj, jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyPopup::show: %d", res);
    return (jint)res;
}
