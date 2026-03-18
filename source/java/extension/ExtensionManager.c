#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/ExtensionManager
 */

// "getExtensonByName", "(Ljava/lang/String;)Lcom/acrodea/vividruntime/launcher/extension/MyExtensionInterface;"
jobject ExtensionManager_getExtensonByName(jmethodID id, va_list args) {
    const char *value = va_arg(args, char *);
    const char *res   = value;
    l_debug("[JNI] ExtensionManager::getExtensonByName(%s): %p", value, res);
    return (jobject)jni->NewStringUTF(&jni, res);
}
