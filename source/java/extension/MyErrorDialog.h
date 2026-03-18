#ifndef SOLOADER_JAVA_MY_ERROR_DIALOG_H
#define SOLOADER_JAVA_MY_ERROR_DIALOG_H

#include <falso_jni/FalsoJNI.h>

void MyErrorDialog_setMessage(jmethodID id, va_list args);
void MyErrorDialog_setCode(jmethodID id, va_list args);

#endif