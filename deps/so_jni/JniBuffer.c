#include "so_jni_internal.h"

jobject JniNewDirectByteBuffer(void *address, jlong capacity) {
    JavaBuffer buf = JavaNewDirectBuffer(address, capacity, JNI_FALSE, JNI_FALSE);
    if (!buf) {
        return NULL;
    }

    jobject result = JniNewLocalRefFromJava(buf);
    JavaReleaseObject(buf);

    return result;
}

void *JniGetDirectBufferAddress(jobject buffer) {
    return JavaGetBufferData(JniObjectToJava(buffer));
}

jlong JniGetDirectBufferCapacity(jobject buffer) {
    return JavaGetBufferSize(JniObjectToJava(buffer));
}
