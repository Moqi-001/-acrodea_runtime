#include <stdlib.h>
#include <string.h>

#include "../jni.h"
#include "JavaObject.h"
#include "JavaTypes.h"

#include "JavaBuffer.h"

typedef struct JavaBuffer {
    uint32_t magic;
    jboolean adopt;
    void    *data;
    jlong    size;
} JavaBuffer;

#define JAVA_BUFFER_MAGIC    0x2F706522  // "JavaBuffer".hashCode()
#define JAVA_BUFFER_CAST(_o) JAVA_BASE_CAST(JavaBuffer *, _o, JAVA_BUFFER_MAGIC)

static jint _buffer_ctor(JavaBuffer *buf, jlong size) {
    if (!buf || size <= 0) return JNI_EINVAL;

    buf->magic = JAVA_BUFFER_MAGIC;
    buf->adopt = JNI_TRUE;
    buf->data  = malloc(size);
    if (!buf->data) {
        return JNI_ENOMEM;
    }
    buf->size = size;

    return JNI_OK;
}

static jint _direct_buffer_ctor(JavaBuffer *buf, void *address, jlong size, jboolean copy, jboolean adopt) {
    if (!buf || !address || size <= 0) return JNI_EINVAL;

    buf->magic = JAVA_BUFFER_MAGIC;
    if (copy) {
        buf->adopt = JNI_TRUE;
        buf->data  = malloc(size);
        if (!buf->data) {
            return JNI_ENOMEM;
        }
        memcpy(buf->data, address, size);
    } else {
        buf->adopt = adopt;
        buf->data  = address;
    }
    buf->size = size;

    return JNI_OK;
}

static void _buffer_dtor(JavaBuffer *buf) {
    if (!buf) return;

    if (buf->adopt && buf->data) {
        free(buf->data);
    }
}

JBuffer JavaNewBuffer(jlong size) {
    JavaBuffer *buf = (JavaBuffer *)JavaAllocObjectNoClass(sizeof(JavaBuffer), (JObjectDtor)_buffer_dtor);
    if (!buf) {
        return NULL;
    }

    if (_buffer_ctor(buf, size) != JNI_OK) {
        JavaReleaseObject(buf);
        return NULL;
    }

    return (JBuffer)buf;
}

JBuffer JavaNewDirectBuffer(void *address, jlong size, jboolean copy, jboolean adopt) {
    JavaBuffer *buf = (JavaBuffer *)JavaAllocObjectNoClass(sizeof(JavaBuffer), (JObjectDtor)_buffer_dtor);
    if (!buf) {
        return NULL;
    }

    if (_direct_buffer_ctor(buf, address, size, copy, adopt) != JNI_OK) {
        JavaReleaseObject(buf);
        return NULL;
    }

    return (JBuffer)buf;
}

void *JavaGetBufferData(JBuffer buffer) {
    JavaBuffer *buf = JAVA_BUFFER_CAST(buffer);
    if (!buf) return NULL;

    return buf->data;
}

jlong JavaGetBufferSize(JBuffer buffer) {
    JavaBuffer *buf = JAVA_BUFFER_CAST(buffer);
    if (!buf) return 0;

    return buf->size;
}
