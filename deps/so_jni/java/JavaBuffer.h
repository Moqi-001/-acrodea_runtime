#ifndef SO_JAVA_BUFFER_H
#define SO_JAVA_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../jni.h"
#include "JavaTypes.h"

JavaBuffer JavaNewBuffer(jlong size);
JavaBuffer JavaNewDirectBuffer(void *address, jlong size, jboolean copy, jboolean adopt);

void *JavaGetBufferData(JavaBuffer buffer);
jlong JavaGetBufferSize(JavaBuffer buffer);

#ifdef __cplusplus
}
#endif

#endif