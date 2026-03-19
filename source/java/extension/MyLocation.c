#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MyLocation
 */

// "setEnable", "(III)I"
jint MyLocation_setEnable(jobject obj, jmethodID id, va_list args) {
    int res    = 0;
    int value  = va_arg(args, int);
    int value1 = va_arg(args, int);
    int value2 = va_arg(args, int);
    l_debug("[JNI] MyLocation::setEnable(%d,%d,%d): %d", value, value1, value2, res);
    return (jint)res;
}
