#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MyLaunch
 */

// "setLaunchBehavior", "(I)I"
jint MyLaunch_setLaunchBehavior(jobject obj, jmethodID id, va_list args) {
    int res   = 0;
    int value = va_arg(args, int);
    l_debug("[JNI] MyLaunch::setLaunchBehavior(%d): %d", res);
    return (jint)res;
}

// "launch", "(Ljava/lang/String;)I"
jint MyLaunch_launch(jobject obj, jmethodID id, va_list args) {
    int         res   = 0;
    const char *value = va_arg(args, char *);
    l_debug("[JNI] MyLaunch::launch(%s): %d", res);
    return (jint)res;
}
