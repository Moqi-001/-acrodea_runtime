#ifndef SO_JAVA_ARRAY_H
#define SO_JAVA_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../jni.h"
#include "JavaTypes.h"

JArray   JavaNewArray(jsize length, JavaType type);
JavaType JavaGetArrayType(JArray array);
jsize    JavaGetArrayLength(JArray array);
jint     JavaGetArrayRegion(JArray array, jsize start, jsize length, void *dst);
jint     JavaSetArrayRegion(JArray array, jsize start, jsize length, const void *src);

#ifdef __cplusplus
}
#endif

#endif