#ifndef SOLOADER_JAVA_MY_FONT_H
#define SOLOADER_JAVA_MY_FONT_H

#include <falso_jni/FalsoJNI.h>

jint     MyFont_getTextSize(jobject obj, jmethodID id, va_list args);
jboolean MyFont_renderText(jobject obj, jmethodID id, va_list args);

#endif