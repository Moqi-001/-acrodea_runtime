#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MyTextInput
 */

// "getText", "()Ljava/lang/String;"
jobject MyTextInput_getText(jmethodID id, va_list args) {
    const char *res = "test";
    l_debug("[JNI] MyTextInput::getText: %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "setPropertycv", "(ILjava/lang/String;)I"
jint MyTextInput_setPropertycv(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyTextInput::setPropertycv: %d", res);
    return (jint)res;
}

// "doModal", "()I"
jint MyTextInput_doModal(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyTextInput::doModal: %d", res);
    return (jint)res;
}
