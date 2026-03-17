#include <stdarg.h>
#include <string.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 类名: com/acrodea/vividruntime/launcher/Runtime
 */

// "vibrate", "(Z)V"
void Runtime_vibrate(jmethodID id, va_list args) {
    jboolean value = va_arg(args, int);
    l_debug("[JNI] vibrate(%d)", value);
}

// "getLocale", "()Ljava/lang/String;"
jobject Runtime_getLocale(jmethodID id, va_list args) {
    const char *res = "en";
    l_debug("[JNI] getLocale(): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "launchRuntime", "(Ljava/lang/String;)V"
void Runtime_launchRuntime(jmethodID id, va_list args) {
    const char *value = va_arg(args, char *);
    l_debug("[JNI] launchRuntime(%s)", value);
}

// "trialSetTime", "(I)V"
void Runtime_trialSetTime(jmethodID id, va_list args) {
    int time = va_arg(args, int);
    l_debug("[JNI] trialSetTime(%d)", time);
}

// "appFinish", "()V"
void Runtime_appFinish(jmethodID id, va_list args) {
    l_debug("[JNI] appFinish()");
}

// "updateResourceCheck", "(Ljava/lang/String;[F)V"
void Runtime_updateResourceCheck(jmethodID id, va_list args) {
    const char *value  = va_arg(args, char *);
    float       value2 = va_arg(args, double);
    l_debug("[JNI] updateResourceCheck(%s,%f)", value, value2);
}

// "invisibleRuntimeLogo", "()V"
void Runtime_invisibleRuntimeLogo(jmethodID id, va_list args) {
    l_debug("[JNI] invisibleRuntimeLogo()");
}

// "isUseRecord", "()Z"
jboolean Runtime_isUseRecord(jmethodID id, va_list args) {
    jboolean res = 0;
    l_debug("[JNI] isUseRecord(): %u", res);
    return res;
}

// "getCommonDir", "()Ljava/lang/String;"
jobject Runtime_getCommonDir(jmethodID id, va_list args) {
    const char *res = "ggee/";
    l_debug("[JNI] getCommonDir(): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "getCommonAppDir", "(Ljava/lang/String;)Ljava/lang/String;"
jobject Runtime_getCommonAppDir(jmethodID id, va_list args) {
    const char *res   = DATA_PATH;
    const char *value = va_arg(args, char *);
    l_debug("[JNI] getCommonAppDir(%s): %s", res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "getEnv", "(Ljava/lang/String;)Ljava/lang/String;"
jobject Runtime_getEnv(jmethodID id, va_list args) {
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

    l_debug("[JNI] getEnv(%s): %s", value, res);
    return (jobject)jni->NewStringUTF(&jni, res);
}

// "jniTrackerSetPage", "(Ljava/lang/String;)V"
void Runtime_jniTrackerSetPage(jmethodID id, va_list args) {
    const char *value = va_arg(args, char *);
    l_debug("[JNI] jniTrackerSetPage(%s)", value);
}

// "jniTrackerEvent", "(Ljava/lang/String;Ljava/lang/String;)V"
void Runtime_jniTrackerEvent(jmethodID id, va_list args) {
    const char *value  = va_arg(args, char *);
    const char *value2 = va_arg(args, char *);
    l_debug("[JNI] jniTrackerEvent(%s,%s)", value, value2);
}

// "jniTrackerAssignAttribute", "(ILjava/lang/String;)V"
void Runtime_jniTrackerAssignAttribute(jmethodID id, va_list args) {
    const char *value = va_arg(args, char *);
    l_debug("[JNI] jniTrackerAssignAttribute(%s)", value);
}

// "jniTrackerRemoveAttribute", "(I)V"
void Runtime_jniTrackerRemoveAttribute(jmethodID id, va_list args) {
    int value = va_arg(args, int);
    l_debug("[JNI] jniTrackerRemoveAttribute(%d)", value);
}

// "jniTrackerSetAttribute", "(ILjava/lang/String;)V"
void Runtime_jniTrackerSetAttribute(jmethodID id, va_list args) {
    const char *value = va_arg(args, char *);
    l_debug("[JNI] jniTrackerSetAttribute(%s)", value);
}

// "stereoIsAvailability", "()I"
jint Runtime_stereoIsAvailability(jmethodID id, va_list args) {
    int res = 1;
    l_debug("[JNI] stereoIsAvailability(): %d", res);
    return (jint)res;
}

// "stereoSetView", "(Z)V"
void Runtime_stereoSetView(jmethodID id, va_list args) {
    int value = va_arg(args, int);
    l_debug("[JNI] stereoSetView(%u)", value);
}

// "stereoGetDepth", "()F"
jfloat Runtime_stereoGetDepth(jmethodID id, va_list args) {
    float res = 1.0;
    l_debug("[JNI] stereoGetDepth(): %f", res);
    return res;
}
