#ifndef SOLOADER_JAVA_MY_IMAGE_PICKER_H
#define SOLOADER_JAVA_MY_IMAGE_PICKER_H

#include <falso_jni/FalsoJNI.h>

jint MyImagePicker_addHeader(jobject obj, jmethodID id, va_list args);
jint MyImagePicker_setType(jobject obj, jmethodID id, va_list args);

#endif