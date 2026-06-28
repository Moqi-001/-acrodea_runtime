#include "so_jni_internal.h"

jstring JniNewString(const jchar *utf16, jsize len) {
    JString str = JavaNewString(utf16, len);
    if (!str) return NULL;

    jstring result = JniNewLocalRefFromJava(str);
    JavaReleaseObject(str);// 由 LocalRef 托管

    return result;
}

jsize JniGetStringLength(jstring string) {
    return JavaGetStringLength(JniObjectToJava(string));
}

const jchar *JniGetStringChars(jstring string, jboolean *isCopy) {
    const jchar *result = JavaGetStringChars(JniObjectToJava(string));
    if (isCopy) *isCopy = JNI_FALSE;

    return result;
}

void JniReleaseStringChars(jstring string, const jchar *utf16) {
    // 无需做任何事
}

jint JniGetStringRegion(jstring string, jsize start, jsize length, jchar *buf) {
    return JavaGetStringRegion(JniObjectToJava(string), start, length, buf);
}

jstring JniNewStringUTF(const char *utf8) {
    JString str = JavaNewStringUTF(utf8);
    if (!str) return NULL;

    jstring result = JniNewLocalRefFromJava(str);
    JavaReleaseObject(str);// 由 LocalRef 托管

    return result;
}

jsize JniGetStringUTFLength(jstring string) {
    return JavaGetStringUTFLength(JniObjectToJava(string));
}

const char *JniGetStringUTFChars(jstring string, jboolean *isCopy) {
    const char *result = JavaGetStringUTFChars(JniObjectToJava(string));
    if (isCopy) *isCopy = JNI_FALSE;

    return result;
}

void JniReleaseStringUTFChars(jstring string, const char *utf8) {
    // 无需做任何事
}

jint JniGetStringUTFRegion(jstring string, jsize start, jsize length, char *buf) {
    return JavaGetStringUTFRegion(JniObjectToJava(string), start, length, buf);
}
