#ifndef SOLOADER_JAVA_MY_FONT_H
#define SOLOADER_JAVA_MY_FONT_H

#include <falso_jni/FalsoJNI.h>

jint     MyFont_getTextSize(jmethodID id, va_list args);
jboolean MyFont_renderText(jmethodID id, va_list args);

#endif