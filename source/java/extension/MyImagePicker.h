#ifndef SOLOADER_JAVA_MY_IMAGE_PICKER_H
#define SOLOADER_JAVA_MY_IMAGE_PICKER_H

#include <falso_jni/FalsoJNI.h>

jint MyImagePicker_addHeader(jmethodID id, va_list args);
jint MyImagePicker_setType(jmethodID id, va_list args);

#endif