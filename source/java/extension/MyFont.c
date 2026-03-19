#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MyFont
 */

// "getTextSize", "(Ljava/lang/String;[I)I"
jint MyFont_getTextSize(jobject obj, jmethodID id, va_list args) {
    int         res    = 0;
    const char *value  = va_arg(args, char *);
    jintArray   value2 = va_arg(args, jintArray);
    l_debug("[JNI] MyFont::renderText(%s,%p): %u", value, value2, res);
    return (jint)res;
}

// "getWidth", "(Ljava/lang/String;[I)I"
jint MyFont_getWidth(jobject obj, jmethodID id, va_list args) {
    int         res    = 0;
    const char *value  = va_arg(args, char *);
    jintArray   value2 = va_arg(args, jintArray);
    l_debug("[JNI] MyFont::getWidth(%s,%p): %u", value, value2, res);
    return (jint)res;
}

// "getHeightEx", "(Ljava/lang/String;[I)I"
jint MyFont_getHeightEx(jobject obj, jmethodID id, va_list args) {
    int         res    = 0;
    const char *value  = va_arg(args, char *);
    jintArray   value2 = va_arg(args, jintArray);
    l_debug("[JNI] MyFont::getHeightEx(%s,%p): %u", value, value2, res);
    return (jint)res;
}

// "getFontInfo", "([I)V"
void MyFont_getFontInfo(jobject obj, jmethodID id, va_list args) {
    jintArray value = va_arg(args, jintArray);
    l_debug("[JNI] MyFont::getFontInfo(%p): %u", value, res);
}

// "drawString", "(Ljava/lang/String;[I)[I"
jbyteArray MyFont_drawString(jobject obj, jmethodID id, va_list args) {
    jbyteArray  res    = 0;
    const char *value  = va_arg(args, char *);
    jintArray   value2 = va_arg(args, jintArray);
    l_debug("[JNI] MyFont::drawString(%s,%p): %u", value, value2, res);
    return res;
}

// "drawStringEx", "(Ljava/lang/String;[I)[I"
jbyteArray MyFont_drawStringEx(jobject obj, jmethodID id, va_list args) {
    jbyteArray  res    = 0;
    const char *value  = va_arg(args, char *);
    jintArray   value2 = va_arg(args, jintArray);
    l_debug("[JNI] MyFont::drawStringEx(%s,%p): %u", value, value2, res);
    return res;
}

// "renderText", "(Ljava/lang/String;[I)[B"
jbyteArray MyFont_renderText(jobject obj, jmethodID id, va_list args) {
    jbyteArray  res    = 0;
    const char *value  = va_arg(args, char *);
    jintArray   value2 = va_arg(args, jintArray);
    l_debug("[JNI] MyFont::renderText(%s,%p): %u", value, value2, res);
    return res;
}
