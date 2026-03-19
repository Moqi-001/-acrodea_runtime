#ifndef SOLOADER_JAVA_MY_TEXT_INPUT_H
#define SOLOADER_JAVA_MY_TEXT_INPUT_H

#include <falso_jni/FalsoJNI.h>

jobject MyTextInput_getText(jobject obj, jmethodID id, va_list args);
jint    MyTextInput_setPropertycv(jobject obj, jmethodID id, va_list args);
jint    MyTextInput_doModal(jobject obj, jmethodID id, va_list args);

#endif