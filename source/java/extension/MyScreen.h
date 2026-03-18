#ifndef SOLOADER_JAVA_MY_SCREEN_H
#define SOLOADER_JAVA_MY_SCREEN_H

#include <falso_jni/FalsoJNI.h>

jint   MyScreen_getWidthPixels(jmethodID id, va_list args);
jint   MyScreen_getHeightPixels(jmethodID id, va_list args);
jfloat MyScreen_getDensity(jmethodID id, va_list args);
jint   MyScreen_getDensityDpi(jmethodID id, va_list args);
jfloat MyScreen_getXDensityDpi(jmethodID id, va_list args);
jfloat MyScreen_getYDensityDpi(jmethodID id, va_list args);

#endif