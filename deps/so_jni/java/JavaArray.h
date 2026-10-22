#ifndef SO_JAVA_ARRAY_H
#define SO_JAVA_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../jni.h"
#include "JavaTypes.h"

JavaArray JavaNewArray(jsize length, JavaType type);
JavaType  JavaGetArrayType(JavaArray array);
jsize     JavaGetArrayLength(JavaArray array);
jint      JavaGetArrayRegion(JavaArray array, jsize start, jsize length, void *dst);
jint      JavaSetArrayRegion(JavaArray array, jsize start, jsize length, const void *src);

#ifdef __cplusplus
}
#endif

#endif