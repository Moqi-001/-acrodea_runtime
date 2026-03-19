#include <stdarg.h>
#include <string.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/Runtime
 */

// "vibrate", "(Z)V"
void Runtime_vibrate(jobject obj, jmethodID id, va_list args) {
    jboolean value = va_arg(args, int);
    l_debug("[JNI] Runtime::vibrate(%d)", value);
}

// "getLocale", "()Ljava/lang/String;"
jobject Runtime_getLocale(jobject obj, jmethodID id, va_list args) {
    const char *res = "en";
    l_debug("[JNI] Runtime::getLocale(): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "launchRuntime", "(Ljava/lang/String;)V"
void Runtime_launchRuntime(jobject obj, jmethodID id, va_list args) {
    const char *value = va_arg(args, char *);
    l_debug("[JNI] Runtime::launchRuntime(%s)", value);
}

// "trialSetTime", "(I)V"
void Runtime_trialSetTime(jobject obj, jmethodID id, va_list args) {
    int time = va_arg(args, int);
    l_debug("[JNI] Runtime::trialSetTime(%d)", time);
}

// "appFinish", "()V"
void Runtime_appFinish(jobject obj, jmethodID id, va_list args) {
    l_debug("[JNI] Runtime::appFinish()");
}

// "updateResourceCheck", "(Ljava/lang/String;[F)V"
void Runtime_updateResourceCheck(jobject obj, jmethodID id, va_list args) {
    const char *value  = va_arg(args, char *);
    float       value2 = va_arg(args, double);
    l_debug("[JNI] Runtime::updateResourceCheck(%s,%f)", value, value2);
}

// "invisibleRuntimeLogo", "()V"
void Runtime_invisibleRuntimeLogo(jobject obj, jmethodID id, va_list args) {
    l_debug("[JNI] Runtime::invisibleRuntimeLogo()");
}

// "isUseRecord", "()Z"
jboolean Runtime_isUseRecord(jobject obj, jmethodID id, va_list args) {
    jboolean res = 0;
    l_debug("[JNI] Runtime::isUseRecord(): %u", res);
    return res;
}

// "getCommonDir", "()Ljava/lang/String;"
jobject Runtime_getCommonDir(jobject obj, jmethodID id, va_list args) {
    const char *res = "ggee/";
    l_debug("[JNI] Runtime::getCommonDir(): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "getCommonAppDir", "(Ljava/lang/String;)Ljava/lang/String;"
jobject Runtime_getCommonAppDir(jobject obj, jmethodID id, va_list args) {
    const char *res   = DATA_PATH;
    const char *value = va_arg(args, char *);
    l_debug("[JNI] Runtime::getCommonAppDir(%s): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "getEnv", "(Ljava/lang/String;)Ljava/lang/String;"
jobject Runtime_getEnv(jobject obj, jmethodID id, va_list args) {
    const char *res   = "";
    const char *value = va_arg(args, char *);

    if (strcmp(value, "DEVICE_VENDOR") == 0) {
        res = "Sony";
    } else if (strcmp(value, "DEVICE_MODEL") == 0) {
        res = "Psvita";
    } else if (strcmp(value, "OS_VERSION") == 0) {
        res = "2.1";
    } else if (strcmp(value, "OS") == 0) {
        res = "Android";
    } else if (strcmp(value, "GL_RENDERER") == 0) {
        res = "Adreno";
    }

    l_debug("[JNI] Runtime::getEnv(%s): %s", value, res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "jniTrackerSetPage", "(Ljava/lang/String;)V"
void Runtime_jniTrackerSetPage(jobject obj, jmethodID id, va_list args) {
    const char *value = va_arg(args, char *);
    l_debug("[JNI] Runtime::jniTrackerSetPage(%s)", value);
}

// "jniTrackerEvent", "(Ljava/lang/String;Ljava/lang/String;)V"
void Runtime_jniTrackerEvent(jobject obj, jmethodID id, va_list args) {
    const char *value  = va_arg(args, char *);
    const char *value2 = va_arg(args, char *);
    l_debug("[JNI] Runtime::jniTrackerEvent(%s,%s)", value, value2);
}

// "jniTrackerAssignAttribute", "(ILjava/lang/String;)V"
void Runtime_jniTrackerAssignAttribute(jobject obj, jmethodID id, va_list args) {
    int         value  = va_arg(args, int);
    const char *value2 = va_arg(args, char *);
    l_debug("[JNI] Runtime::jniTrackerSetAttribute(%d,%s)", value, value2);
}

// "jniTrackerRemoveAttribute", "(I)V"
void Runtime_jniTrackerRemoveAttribute(jobject obj, jmethodID id, va_list args) {
    int value = va_arg(args, int);
    l_debug("[JNI] Runtime::jniTrackerRemoveAttribute(%d)", value);
}

// "jniTrackerSetAttribute", "(ILjava/lang/String;)V"
void Runtime_jniTrackerSetAttribute(jobject obj, jmethodID id, va_list args) {
    int         value  = va_arg(args, int);
    const char *value2 = va_arg(args, char *);
    l_debug("[JNI] Runtime::jniTrackerSetAttribute(%d,%s)", value, value2);
}

// "stereoIsAvailability", "()I"
jint Runtime_stereoIsAvailability(jobject obj, jmethodID id, va_list args) {
    int res = 1;
    l_debug("[JNI] Runtime::stereoIsAvailability(): %d", res);
    return (jint)res;
}

// "stereoSetView", "(Z)V"
void Runtime_stereoSetView(jobject obj, jmethodID id, va_list args) {
    jboolean value = va_arg(args, int);
    l_debug("[JNI] Runtime::stereoSetView(%u)", value);
}

// "stereoGetDepth", "()F"
jfloat Runtime_stereoGetDepth(jobject obj, jmethodID id, va_list args) {
    float res = 1.0;
    l_debug("[JNI] Runtime::stereoGetDepth(): %f", res);
    return res;
}
