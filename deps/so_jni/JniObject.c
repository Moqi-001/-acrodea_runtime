#include "so_jni_internal.h"

jobject JniAllocObject(jclass clazz) {
    JObject obj = JavaAllocObject(JniObjectToJava(clazz), NULL);
    if (!obj) {
        return NULL;
    }

    jobject result = JniNewLocalRefFromJava(obj);
    JavaReleaseObject(obj);

    return result;
}

jobject JniNewObject(jclass clazz, jmethodID methodID, ...) {
    jobject object;

    va_list args;
    va_start(args, methodID);
    object = JniNewObjectV(clazz, methodID, args);
    va_end(args);

    return object;
}

jobject JniNewObjectV(jclass clazz, jmethodID methodID, va_list args) {
    jobject object = JniAllocObject(clazz);
    if (!object) return NULL;

    JniCallCommonMethodV(NULL, object, methodID, args);
    return object;
}

jobject JniNewObjectA(jclass clazz, jmethodID methodID, const jvalue *args) {
    jobject object = JniAllocObject(clazz);
    if (!object) return NULL;

    JniCallCommonMethodA(NULL, object, methodID, args);
    return object;
}

jclass JniGetObjectClass(jobject object) {
    return JniNewLocalRefFromJava(JavaGetObjectClass(JniObjectToJava(object)));
}

jboolean JniIsSameObject(jobject ref1, jobject ref2) {
    JObject *obj1 = JniObjectToJava(ref1);
    JObject *obj2 = JniObjectToJava(ref2);

    return (obj1 == obj2) ? JNI_TRUE : JNI_FALSE;
}
