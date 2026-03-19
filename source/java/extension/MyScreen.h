#ifndef SOLOADER_JAVA_MY_SCREEN_H
#define SOLOADER_JAVA_MY_SCREEN_H

#include <falso_jni/FalsoJNI.h>

jint   MyScreen_getWidthPixels(jobject obj, jmethodID id, va_list args);
jint   MyScreen_getHeightPixels(jobject obj, jmethodID id, va_list args);
jfloat MyScreen_getDensity(jobject obj, jmethodID id, va_list args);
jint   MyScreen_getDensityDpi(jobject obj, jmethodID id, va_list args);
jfloat MyScreen_getXDensityDpi(jobject obj, jmethodID id, va_list args);
jfloat MyScreen_getYDensityDpi(jobject obj, jmethodID id, va_list args);

#endif