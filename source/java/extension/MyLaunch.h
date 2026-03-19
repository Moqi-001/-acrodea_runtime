#ifndef SOLOADER_JAVA_MY_LAUNCH_H
#define SOLOADER_JAVA_MY_LAUNCH_H

#include <falso_jni/FalsoJNI.h>

jint MyLaunch_setLaunchBehavior(jobject obj, jmethodID id, va_list args);
jint MyLaunch_launch(jobject obj, jmethodID id, va_list args);

#endif