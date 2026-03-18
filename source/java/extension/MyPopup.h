#ifndef SOLOADER_JAVA_MY_POPUP_H
#define SOLOADER_JAVA_MY_POPUP_H

#include <falso_jni/FalsoJNI.h>

jobject MyPopup_getText(jmethodID id, va_list args);
jint    MyPopup_setPropertycv(jmethodID id, va_list args);
jint    MyPopup_show(jmethodID id, va_list args);

#endif