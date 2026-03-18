#ifndef SOLOADER_JAVA_MY_HTTP_H
#define SOLOADER_JAVA_MY_HTTP_H

#include <falso_jni/FalsoJNI.h>

jobject MyDevice_getImei(jmethodID id, va_list args);
jobject MyDevice_getImsi(jmethodID id, va_list args);
jint    MyDevice_setNavigationHiddenEnable(jmethodID id, va_list args);
jint    MyDevice_getNavigationHiddenState(jmethodID id, va_list args);
jobject MyDevice_getPlatformInfo();
jint    MyDevice_getMutedState(jmethodID id, va_list args);

#endif