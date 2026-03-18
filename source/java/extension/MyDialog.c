#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MyDialog
 */

// "showDialog", "(Ljava/lang/String;)I"
jint MyDialog_showDialog(jmethodID id, va_list args) {
    int         res   = 0;
    const char *value = va_arg(args, char *);
    l_debug("[JNI] MyDialog::showDialog(%s): %d", value, res);
    return (jint)res;
}
