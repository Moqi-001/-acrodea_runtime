#ifndef SOLOADER_JAVA_RUNTIME_H
#define SOLOADER_JAVA_RUNTIME_H

#include <falso_jni/FalsoJNI.h>

void     Runtime_vibrate(jobject obj, jmethodID id, va_list args);
jobject  Runtime_getLocale(jobject obj, jmethodID id, va_list args);
void     Runtime_launchRuntime(jobject obj, jmethodID id, va_list args);
void     Runtime_trialSetTime(jobject obj, jmethodID id, va_list args);
void     Runtime_appFinish(jobject obj, jmethodID id, va_list args);
void     Runtime_updateResourceCheck(jobject obj, jmethodID id, va_list args);
void     Runtime_invisibleRuntimeLogo(jobject obj, jmethodID id, va_list args);
jboolean Runtime_isUseRecord(jobject obj, jmethodID id, va_list args);
jobject  Runtime_getCommonDir(jobject obj, jmethodID id, va_list args);
jobject  Runtime_getCommonAppDir(jobject obj, jmethodID id, va_list args);
jobject  Runtime_getEnv(jobject obj, jmethodID id, va_list args);
void     Runtime_jniTrackerSetPage(jobject obj, jmethodID id, va_list args);
void     Runtime_jniTrackerEvent(jobject obj, jmethodID id, va_list args);
void     Runtime_jniTrackerAssignAttribute(jobject obj, jmethodID id, va_list args);
void     Runtime_jniTrackerRemoveAttribute(jobject obj, jmethodID id, va_list args);
void     Runtime_jniTrackerSetAttribute(jobject obj, jmethodID id, va_list args);
jint     Runtime_stereoIsAvailability(jobject obj, jmethodID id, va_list args);
void     Runtime_stereoSetView(jobject obj, jmethodID id, va_list args);
jfloat   Runtime_stereoGetDepth(jobject obj, jmethodID id, va_list args);

#endif