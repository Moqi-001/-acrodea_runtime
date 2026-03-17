#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 类名: com/acrodea/vividruntime/launcher/extension/MyDevice
 */

// "getImei", "()Ljava/lang/String;"
jobject MyDevice_getImei(jmethodID id, va_list args) {
    const char *res = "MACW:0026e879f969";
    l_debug("[JNI] getImei(): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "setNavigationHiddenEnable", "(I)I"
jint MyDevice_setNavigationHiddenEnable(jmethodID id, va_list args) {
    int res   = 0;
    int value = va_arg(args, int);
    l_debug("[JNI] setNavigationHiddenEnable(%d): %d", value, res);
    return (jint)res;
}

// "getNavigationHiddenState", "()I"
jint MyDevice_getNavigationHiddenState(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] getNavigationHiddenState(): %d", res);
    return (jint)res;
}

// "getPlatformInfo", "()Ljava/lang/String;"
jobject MyDevice_getPlatformInfo() {
    const char *res = "PSVita";
    l_debug("[JNI] getPlatformInfo(): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "getMutedState", "()I"
jint MyDevice_getMutedState(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] getMutedState(): %d", res);
    return (jint)res;
}
