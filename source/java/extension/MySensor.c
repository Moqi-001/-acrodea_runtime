#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MySensor
 */

// "setAccelerometer", "(I)I"
jint MySensor_setAccelerometer(jobject obj, jmethodID id, va_list args) {
    int res   = 0;
    int value = va_arg(args, int);
    l_debug("[JNI] MySensor::setAccelerometer(%d): %u", value, res);
    return (jint)res;
}

// "checkType", "(I)I"
jint MySensor_checkType(jobject obj, jmethodID id, va_list args) {
    int res   = 0;
    int value = va_arg(args, int);
    l_debug("[JNI] MySensor::checkType(%d): %u", value, res);
    return (jint)res;
}

// "setGyro", "(I)I"
jint MySensor_setGyro(jobject obj, jmethodID id, va_list args) {
    int res   = 0;
    int value = va_arg(args, int);
    l_debug("[JNI] MySensor::setGyro(%d): %u", value, res);
    return (jint)res;
}

// "setMagnetic", "(I)I"
jint MySensor_setMagnetic(jobject obj, jmethodID id, va_list args) {
    int res   = 0;
    int value = va_arg(args, int);
    l_debug("[JNI] MySensor::setMagnetic(%d): %u", value, res);
    return (jint)res;
}
