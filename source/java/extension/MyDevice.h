#ifndef SOLOADER_JAVA_MY_HTTP_H
#define SOLOADER_JAVA_MY_HTTP_H

#include <falso_jni/FalsoJNI.h>

jobject MyDevice_getImei(jobject obj, jmethodID id, va_list args);
jobject MyDevice_getImsi(jobject obj, jmethodID id, va_list args);
jint    MyDevice_setNavigationHiddenEnable(jobject obj, jmethodID id, va_list args);
jint    MyDevice_getNavigationHiddenState(jobject obj, jmethodID id, va_list args);
jobject MyDevice_getPlatformInfo();
jint    MyDevice_getMutedState(jobject obj, jmethodID id, va_list args);

#endif