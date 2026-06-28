#ifndef SO_JAVA_BUFFER_H
#define SO_JAVA_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../jni.h"
#include "JavaTypes.h"

JBuffer JavaNewBuffer(jlong size);
JBuffer JavaNewDirectBuffer(void *address, jlong size, jboolean copy, jboolean adopt);

void *JavaGetBufferData(JBuffer buffer);
jlong JavaGetBufferSize(JBuffer buffer);

#ifdef __cplusplus
}
#endif

#endif