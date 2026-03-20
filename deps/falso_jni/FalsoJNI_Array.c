/*
 * FalsoJNI_ImplBridge.c
 *
 * Fake Java Native Interface, providing JavaVM and JNIEnv objects.
 *
 * Copyright (C) 2021 Andy Nguyen
 * Copyright (C) 2021 Rinnegatamante
 * Copyright (C) 2022 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include "FalsoJNI_Array.h"
#include "FalsoJNI_ImplBridge.h"
#include "FalsoJNI_Logger.h"
#include "FalsoJNI_SharedBuffer.h"

#define JAVA_DYN_ARRAY_MAGIC 0x4F8E3A2D  // "JavaDynArray".hashCode()

#define JDA_CAST(ptr) \
    ((JavaDynArray *)((ptr) && ((JavaDynArray *)(ptr))->magic == JAVA_DYN_ARRAY_MAGIC ? (ptr) : NULL))

typedef struct JavaDynArray {
    uint32_t   magic;
    FIELD_TYPE type;
    void      *data;
    jsize      len;
} JavaDynArray;

jarray *jarray_alloc(jsize len, FIELD_TYPE type) {
    size_t         header_size = sizeof(JavaDynArray);
    size_t         data_size   = len * getFieldTypeSize(type);
    JSharedBuffer *buffer      = JSharedBuffer_alloc(header_size + data_size);
    JavaDynArray  *jda         = (JavaDynArray *)JSharedBuffer_data(buffer);
    if (!jda) {
        return NULL;
    }

    jda->magic = JAVA_DYN_ARRAY_MAGIC;
    jda->type  = type;
    jda->len   = len;
    jda->data  = (void *)(jda + 1);

    return (jarray *)jda;
}

jboolean jarray_acquire(jarray *array) {
    JavaDynArray *jda = JDA_CAST(array);
    if (!jda) return JNI_FALSE;

    JSharedBuffer_acquire(JSharedBuffer_bufferFromData(jda));
    return JNI_TRUE;
}

jboolean jarray_release(jarray *array) {
    JavaDynArray *jda = JDA_CAST(array);
    if (!jda) return JNI_FALSE;

    JSharedBuffer_release(JSharedBuffer_bufferFromData(jda));
    return JNI_TRUE;
}

FIELD_TYPE jarray_type(jarray *array) {
    JavaDynArray *jda = JDA_CAST(array);
    return jda ? jda->type : FIELD_TYPE_UNKNOWN;
}

void *jarray_data(jarray *array) {
    JavaDynArray *jda = JDA_CAST(array);
    return jda ? jda->data : NULL;
}

jsize jarray_length(jarray *array) {
    JavaDynArray *jda = JDA_CAST(array);
    return jda ? jda->len : -1;
}
