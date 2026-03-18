#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MyHttp
 */

// "addHeader", "(Ljava/lang/String;)I"
jint MyHttp_addHeader(jmethodID id, va_list args) {
    int         res   = 0;
    const char *value = va_arg(args, char *);
    l_debug("[JNI] MyHttp::addHeader(%s): %d", value, res);
    return (jint)res;
}

// "setBoundaryBegin", "()I"
jint MyHttp_setBoundaryBegin(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyHttp::setBoundaryBegin(): %d", res);
    return (jint)res;
}

// "setBoundaryEnd", "()I"
jint MyHttp_setBoundaryEnd(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyHttp::setBoundaryEnd(): %d", res);
    return (jint)res;
}

// "setBoundaryBuffer", "([BI)I"
jint MyHttp_setBoundaryBuffer(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyHttp::setBoundaryBuffer(): %d", res);
    return (jint)res;
}

// "setBoundaryFile", "(Ljava/lang/String;)I"
jint MyHttp_setBoundaryFile(jmethodID id, va_list args) {
    int         res   = 0;
    const char *value = va_arg(args, char *);
    l_debug("[JNI] MyHttp::setBoundaryFile(%s): %d", value, res);
    return (jint)res;
}

// "setBoundaryHeader", "(Ljava/lang/String;)I"
jint MyHttp_setBoundaryHeader(jmethodID id, va_list args) {
    int         res   = 0;
    const char *value = va_arg(args, char *);
    l_debug("[JNI] MyHttp::setBoundaryHeader(%s): %d", value, res);
    return (jint)res;
}

// "connect", "()I"
jint MyHttp_connect(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyHttp::connect(): %d", res);
    return (jint)res;
}

// "<init>", "(I)V"
void MyHttp_construct(jmethodID id, va_list args) {
    int value = va_arg(args, int);
    l_debug("[JNI] MyHttp::<init>(%d)", value);
}

// "destroy", "()I"
jint MyHttp_destroy(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyHttp::destroy(): %d", res);
    return (jint)res;
}

// "checkBody", "()I"
jint MyHttp_checkBody(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyHttp::checkBody(): %d", res);
    return (jint)res;
}

// "getBody", "()[B"
jobject MyHttp_getBody(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyHttp::getBody(): %d", res);
    return (jobject)res;
}

// "getResponseCode", "()I"
jint MyHttp_getResponseCode(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyHttp::getResponseCode(): %d", res);
    return (jint)res;
}

// "getHeaderLength", "()I"
jint MyHttp_getHeaderLength(jmethodID id, va_list args) {
    int res = 0;
    l_debug("[JNI] MyHttp::getHeaderLength(): %d", res);
    return (jint)res;
}

// "getHeader", "(Ljava/lang/String;)Ljava/lang/String;"
jstring MyHttp_getHeader(jmethodID id, va_list args) {
    int         res   = 0;
    const char *value = va_arg(args, char *);
    l_debug("[JNI] MyHttp::getHeader(%s): %d", value, res);
    return (jstring)res;
}

// "init", "(Ljava/lang/String;III)I"
jint MyHttp_init(jmethodID id, va_list args) {
    int         res    = 0;
    const char *value  = va_arg(args, char *);
    const char *value1 = va_arg(args, char *);
    const char *value2 = va_arg(args, char *);
    l_debug("[JNI] MyHttp::init(%s,%d,%d,%d): %d", value, value1, value2, res);
    return (jint)res;
}

// "SetPostKey", "(Ljava/lang/String;)I"
jint MyHttp_SetPostKey(jmethodID id, va_list args) {
    int         res   = 0;
    const char *value = va_arg(args, char *);
    l_debug("[JNI] MyHttp::SetPostKey(%s): %d", value, value1, value2, res);
    return (jint)res;
}

// "SetPostValue", "(Ljava/lang/String;)I"
jint MyHttp_SetPostValue(jmethodID id, va_list args) {
    int         res   = 0;
    const char *value = va_arg(args, char *);
    l_debug("[JNI] MyHttp::SetPostValue(%s): %d", value, value1, value2, res);
    return (jint)res;
}

// "setUserPtr", "(I)I"
jint MyHttp_setUserPtr(jmethodID id, va_list args) {
    int res   = 0;
    int value = va_arg(args, int);
    l_debug("[JNI] MyHttp::setUserPtr(%d): %d", value, res);
    return (jstring)res;
}
