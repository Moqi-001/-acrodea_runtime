#ifndef SOLOADER_JAVA_MY_HTTP_H
#define SOLOADER_JAVA_MY_HTTP_H

#include <falso_jni/FalsoJNI.h>

jobject MyHttp_construct(jmethodID id, va_list args);
jint    MyHttp_destroy(jmethodID id, va_list args);
jint    MyHttp_obj_init(jmethodID id, va_list args);
jint    MyHttp_obj_setUserPtr(jmethodID id, va_list args);
jint    MyHttp_obj_connect(jmethodID id, va_list args);

#endif