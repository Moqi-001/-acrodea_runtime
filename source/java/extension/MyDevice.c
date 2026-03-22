#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MyDevice
 */

// "getCarrierName", "()Ljava/lang/String;"

// "getCellState", "()I"

// "getImei", "()Ljava/lang/String;"
jobject MyDevice_getImei(jobject obj, jmethodID id, va_list args) {
    const char *res = "MACW:0026e879f969";  // 这个码有验证，似乎和 DRM 解密有关
    l_debug("[JNI] MyDevice::getImei(): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "getImsi", "()Ljava/lang/String;"
jobject MyDevice_getImsi(jobject obj, jmethodID id, va_list args) {
    const char *res = "460021234567890";  // 中国移动假码 (测试用，实际运行游戏不需要调用这个函数)
    l_debug("[JNI] MyDevice::getImsi(): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "getKeyboardHiddenState", "()I"

// "getMutedState", "()I"
jint MyDevice_getMutedState(jobject obj, jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyDevice::getMutedState(): %d", res);
    return (jint)res;
}

// "getNavigationHiddenState", "()I"
jint MyDevice_getNavigationHiddenState(jobject obj, jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyDevice::getNavigationHiddenState(): %d", res);
    return (jint)res;
}

// "getPhoneNumber", "()Ljava/lang/String;"

// "getPlatformInfo", "()Ljava/lang/String;"
jobject MyDevice_getPlatformInfo() {
    const char *res = "Psvita";
    l_debug("[JNI] MyDevice::getPlatformInfo(): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "getRoamingState", "()I"

// "getWifiState", "()I"

// "setCellEnable", "(I)I"

// "setKeyboardHiddenEnable", "(I)I"

// "setMutedEnable", "(I)I"

// "setNavigationHiddenEnable", "(I)I"
jint MyDevice_setNavigationHiddenEnable(jobject obj, jmethodID id, va_list args) {
    int res   = 0;
    int value = va_arg(args, int);
    l_debug("[JNI] MyDevice::setNavigationHiddenEnable(%d): %d", value, res);
    return (jint)res;
}

// "setRoamingEnable", "(I)I"

// "setWifiEnable", "(I)I"