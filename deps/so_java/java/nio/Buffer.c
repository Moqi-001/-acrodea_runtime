#include <stdlib.h>
#include <string.h>

#include "../lang/Object.h"
#include "../lang/types.h"
#include "Buffer.h"

typedef struct JavaBufferImpl {
    uint32_t magic;
    JBoolean adopt;
    void    *data;
    JLong    size;
} JavaBufferImpl;

#define JAVA_BUFFER_MAGIC    0x2F706522  // "JBuffer".hashCode()
#define JAVA_BUFFER_CAST(_o) JAVA_BASE_CAST(JavaBufferImpl *, _o, JAVA_BUFFER_MAGIC)

static JInt _buffer_ctor(JavaBufferImpl *buf, JLong size) {
    if (!buf || size <= 0)
        return J_ERR;

    buf->magic = JAVA_BUFFER_MAGIC;
    buf->adopt = J_TRUE;
    buf->data  = malloc(size);
    if (!buf->data) {
        return J_ERR;
    }
    buf->size = size;

    return J_OK;
}

static JInt _direct_buffer_ctor(JavaBufferImpl *buf, void *address, JLong size, JBoolean copy, JBoolean adopt) {
    if (!buf || !address || size <= 0)
        return J_ERR;

    buf->magic = JAVA_BUFFER_MAGIC;
    if (copy) {
        buf->adopt = J_TRUE;
        buf->data  = malloc(size);
        if (!buf->data) {
            return J_ERR;
        }
        memcpy(buf->data, address, size);
    } else {
        buf->adopt = adopt;
        buf->data  = address;
    }
    buf->size = size;

    return J_OK;
}

static void _buffer_dtor(JavaBufferImpl *buf) {
    if (!buf)
        return;

    if (buf->adopt && buf->data) {
        free(buf->data);
    }
}

JBuffer Buffer_new(JLong size) {
    JavaBufferImpl *buf = (JavaBufferImpl *)Object_newWithoutClass(sizeof(JavaBufferImpl), (JDtor)_buffer_dtor);
    if (!buf) {
        return NULL;
    }

    if (_buffer_ctor(buf, size) != J_OK) {
        Object_release(buf);
        return NULL;
    }

    return (JBuffer)buf;
}

JBuffer Buffer_newDirect(void *address, JLong size, JBoolean copy, JBoolean adopt) {
    JavaBufferImpl *buf = (JavaBufferImpl *)Object_newWithoutClass(sizeof(JavaBufferImpl), (JDtor)_buffer_dtor);
    if (!buf) {
        return NULL;
    }

    if (_direct_buffer_ctor(buf, address, size, copy, adopt) != J_OK) {
        Object_release(buf);
        return NULL;
    }

    return (JBuffer)buf;
}

void *Buffer_getData(JBuffer buffer) {
    JavaBufferImpl *buf = JAVA_BUFFER_CAST(buffer);
    if (!buf)
        return NULL;

    return buf->data;
}

JLong Buffer_getSize(JBuffer buffer) {
    JavaBufferImpl *buf = JAVA_BUFFER_CAST(buffer);
    if (!buf)
        return 0;

    return buf->size;
}
