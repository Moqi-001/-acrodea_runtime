#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MyImagePicker
 */

// "getImage", "(Ljava/lang/String;)I"
jint MyImagePicker_addHeader(jobject obj, jmethodID id, va_list args) {
    int         res   = 0;
    const char *value = va_arg(args, char *);
    l_debug("[JNI] MyImagePicker::getImage(%s): %d", value, res);
    return (jint)res;
}

// "setType", "(I)I"
jint MyImagePicker_setType(jobject obj, jmethodID id, va_list args) {
    int res   = 0;
    int value = va_arg(args, int);
    l_debug("[JNI] MyImagePicker::setType(%d): %d", value, res);
    return (jint)res;
}
