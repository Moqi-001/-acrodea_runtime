#ifndef SOLOADER_JAVA_MY_TEXT_INPUT_H
#define SOLOADER_JAVA_MY_TEXT_INPUT_H

#include <falso_jni/FalsoJNI.h>

jobject MyTextInput_getText(jmethodID id, va_list args);
jint    MyTextInput_setPropertycv(jmethodID id, va_list args);
jint    MyTextInput_doModal(jmethodID id, va_list args);

#endif