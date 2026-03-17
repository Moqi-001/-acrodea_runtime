#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 类名: com/acrodea/vividruntime/launcher/extension/ExtensionManager
 */

// "getExtensonByName", "(Ljava/lang/String;)Lcom/acrodea/vividruntime/launcher/extension/MyExtensionInterface;"
jobject ExtensionManager_getExtensonByName(jmethodID id, va_list args) {
    const char *res   = "MyExtensionInterface";
    const char *value = va_arg(args, char *);
    l_debug("[JNI] getExtensonByName(%s): %p", value, res);
    return (jobject)jni->NewStringUTF(&jni, res);
}
