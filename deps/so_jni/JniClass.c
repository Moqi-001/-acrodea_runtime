#include "so_jni_internal.h"

const char *JniGetClassName(jclass clazz) {
    return JavaGetClassName(JniObjectToJava(clazz));
}

jclass JniFindClass(const char *name) {
    return JniNewLocalRefFromJava(JavaFindClass(name));
}

jclass JniGetSuperclass(jclass clazz) {
    return JniNewLocalRefFromJava(JavaGetSuperclass(JniObjectToJava(clazz)));
}

jboolean JniIsAssignableFrom(jclass from, jclass thiz) {
    return JavaIsAssignableFrom(JniObjectToJava(thiz), JniObjectToJava(from));
}

jboolean JniIsInstanceOf(jobject obj, jclass clazz) {
    return JavaIsInstanceOf(JniObjectToJava(obj), JniObjectToJava(clazz));
}

jint JniRegisterNatives(jclass clazz, const JNINativeMethod *methods, jint num_methods) {
    return JavaRegisterNatives(JniObjectToJava(clazz), methods, num_methods);
}

jint JniUnregisterNatives(jclass clazz) {
    return JavaUnregisterNatives(JniObjectToJava(clazz));
}
