#ifndef SOLOADER_JAVA_MY_POPUP_H
#define SOLOADER_JAVA_MY_POPUP_H

#include <falso_jni/FalsoJNI.h>

jobject MyPopup_getText(jobject obj, jmethodID id, va_list args);
jint    MyPopup_setPropertycv(jobject obj, jmethodID id, va_list args);
jint    MyPopup_show(jobject obj, jmethodID id, va_list args);

#endif