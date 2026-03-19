#ifndef SOLOADER_JAVA_EXTENSION_MANAGER_H
#define SOLOADER_JAVA_EXTENSION_MANAGER_H

#include <falso_jni/FalsoJNI.h>

jobject ExtensionManager_getExtensonByName(jobject obj, jmethodID id, va_list args);

#endif