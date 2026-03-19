#ifndef SOLOADER_JAVA_MY_HTTP_H
#define SOLOADER_JAVA_MY_HTTP_H

#include <falso_jni/FalsoJNI.h>

jobject MyHttp_construct(jobject obj, jmethodID id, va_list args);
jint    MyHttp_destroy(jobject obj, jmethodID id, va_list args);
jint    MyHttp_obj_init(jobject obj, jmethodID id, va_list args);
jint    MyHttp_obj_setUserPtr(jobject obj, jmethodID id, va_list args);
jint    MyHttp_obj_connect(jobject obj, jmethodID id, va_list args);

#endif