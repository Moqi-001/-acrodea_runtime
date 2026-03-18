#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MyDevice
 */

// "getCarrierName", "()Ljava/lang/String;"

//  "getCellState", "()I"

// 


// "getImei", "()Ljava/lang/String;"
jobject MyDevice_getImei(jmethodID id, va_list args) {
    const char *res = "MACW:0026e879f969"; // 这个码有验证，似乎和 DRM 解密有关
    l_debug("[JNI] MyDevice::getImei(): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "getImsi", "()Ljava/lang/String;"
jobject MyDevice_getImsi(jmethodID id, va_list args) {
    const char *res = "460021234567890"; // 中国移动假码 (测试用，实际运行游戏不需要调用这个函数)
    l_debug("[JNI] MyDevice::getImsi(): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "setNavigationHiddenEnable", "(I)I"
jint MyDevice_setNavigationHiddenEnable(jmethodID id, va_list args) {
    int res   = 0;
    int value = va_arg(args, int);
    l_debug("[JNI] MyDevice::setNavigationHiddenEnable(%d): %d", value, res);
    return (jint)res;
}

// "getNavigationHiddenState", "()I"
jint MyDevice_getNavigationHiddenState(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyDevice::getNavigationHiddenState(): %d", res);
    return (jint)res;
}

// "getPlatformInfo", "()Ljava/lang/String;"
jobject MyDevice_getPlatformInfo() {
    const char *res = "PSVita";
    l_debug("[JNI] MyDevice::getPlatformInfo(): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "getMutedState", "()I"
jint MyDevice_getMutedState(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyDevice::getMutedState(): %d", res);
    return (jint)res;
}
