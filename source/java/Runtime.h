#ifndef SOLOADER_JAVA_RUNTIME_H
#define SOLOADER_JAVA_RUNTIME_H

#include <falso_jni/FalsoJNI.h>

void     Runtime_vibrate(jmethodID id, va_list args);
jobject  Runtime_getLocale(jmethodID id, va_list args);
void     Runtime_launchRuntime(jmethodID id, va_list args);
void     Runtime_trialSetTime(jmethodID id, va_list args);
void     Runtime_appFinish(jmethodID id, va_list args);
void     Runtime_updateResourceCheck(jmethodID id, va_list args);
void     Runtime_invisibleRuntimeLogo(jmethodID id, va_list args);
jboolean Runtime_isUseRecord(jmethodID id, va_list args);
jobject  Runtime_getCommonDir(jmethodID id, va_list args);
jobject  Runtime_getCommonAppDir(jmethodID id, va_list args);
jobject  Runtime_getEnv(jmethodID id, va_list args);
void     Runtime_jniTrackerSetPage(jmethodID id, va_list args);
void     Runtime_jniTrackerEvent(jmethodID id, va_list args);
void     Runtime_jniTrackerAssignAttribute(jmethodID id, va_list args);
void     Runtime_jniTrackerRemoveAttribute(jmethodID id, va_list args);
void     Runtime_jniTrackerSetAttribute(jmethodID id, va_list args);
jint     Runtime_stereoIsAvailability(jmethodID id, va_list args);
void     Runtime_stereoSetView(jmethodID id, va_list args);
jfloat   Runtime_stereoGetDepth(jmethodID id, va_list args);

#endif