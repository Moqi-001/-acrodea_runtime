#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "FalsoJNI_logger.h"
#include "FalsoJNI_SharedBuffer.h"

struct JSharedBuffer {
    int32_t  refs;
    size_t   size;
    uint32_t reserved[2];
};

#define onlyOwner(sb)        (sb->refs == 1)
#define dataFromBuffer(sb)   ((void *)(sb + 1))
#define bufferFromData(data) ((JSharedBuffer *)data - 1)

int32_t java_atomic_inc(volatile int32_t *addr) {
    return atomic_fetch_add_explicit(
        (volatile atomic_int_least32_t *)addr,
        1,
        memory_order_acq_rel);
}

int32_t java_atomic_dec(volatile int32_t *addr) {
    return atomic_fetch_sub_explicit(
        (volatile atomic_int_least32_t *)addr,
        1,
        memory_order_acq_rel);
}

JSharedBuffer *JSharedBuffer_alloc(size_t size) {
    JSharedBuffer *sb = (JSharedBuffer *)malloc(sizeof(JSharedBuffer) + size);
    if (!sb) {
        fjni_log_err("[JNI] JSharedBuffer_alloc(%u): cannot alloc memory", size);
        return NULL;
    }

    sb->refs = 1;
    sb->size = size;

    fjni_log_dbg("[JNI] JSharedBuffer_alloc(%u): %p", size, sb);
    return sb;
}

int JSharedBuffer_dealloc(JSharedBuffer *sb) {
    if (!sb) {
        fjni_log_err("[JNI] JSharedBuffer_dealloc(%p): null ptr", sb);
        return -1;
    }

    if (sb->refs != 0) {
        fjni_log_dbg("[JNI] JSharedBuffer_dealloc(%p): invalid operation", sb);
        return -1;  // XXX: invalid operation
    }

    free(sb);

    fjni_log_dbg("[JNI] JSharedBuffer_dealloc(%p): 0", sb);
    return 0;
}

JSharedBuffer *JSharedBuffer_attemptEdit(JSharedBuffer *sb) {
    if (!sb) {
        fjni_log_err("[JNI] JSharedBuffer_attemptEdit(%p): null ptr", sb);
        return NULL;
    }

    JSharedBuffer *buf = onlyOwner(sb) ? sb : NULL;

    fjni_log_dbg("[JNI] JSharedBuffer_attemptEdit(%p): %p", sb, buf);
    return buf;
}

JSharedBuffer *JSharedBuffer_edit(JSharedBuffer *sb) {
    if (!sb) {
        fjni_log_dbg("[JNI] JSharedBuffer_edit(%p): null ptr", sb);
        return NULL;
    }

    JSharedBuffer *buf;

    if (onlyOwner(sb)) {
        buf = sb;
    } else {
        buf = JSharedBuffer_alloc(sb->size);
        if (buf) {
            memcpy(dataFromBuffer(buf), dataFromBuffer(sb), sb->size);
            JSharedBuffer_release(sb);
        }
    }

    fjni_log_dbg("[JNI] JSharedBuffer_edit(%p): %p", sb, buf);
    return buf;
}

JSharedBuffer *JSharedBuffer_editResize(JSharedBuffer *sb, size_t new_size) {
    if (!sb) {
        fjni_log_dbg("[JNI] JSharedBuffer_editResize(%p): null ptr", sb);
        return NULL;
    }

    JSharedBuffer *buf;

    if (onlyOwner(sb)) {
        buf = sb;
        if (buf->size != new_size) {
            buf = (JSharedBuffer *)realloc(buf, sizeof(JSharedBuffer) + new_size);
            if (buf != NULL) {
                buf->size = new_size;
            }
        }
    } else {
        buf = JSharedBuffer_alloc(new_size);
        if (buf) {
            memcpy(dataFromBuffer(buf), dataFromBuffer(sb), buf->size < sb->size ? buf->size : sb->size);
            JSharedBuffer_release(sb);
        }
    }

    fjni_log_dbg("[JNI] JSharedBuffer_editResize(%p): %p", sb, buf);
    return buf;
}

JSharedBuffer *JSharedBuffer_reset(JSharedBuffer *sb, size_t new_size) {
    JSharedBuffer *buf = JSharedBuffer_alloc(new_size);
    if (!sb) {
        fjni_log_dbg("[JNI] JSharedBuffer_reset(%p,%u): failed!", sb, new_size);
        return NULL;
    }
    JSharedBuffer_release(sb);

    fjni_log_dbg("[JNI] JSharedBuffer_reset(%p,%u): %p", sb, new_size, buf);
    return buf;
}

void JSharedBuffer_acquire(JSharedBuffer *sb) {
    if (!sb) {
        fjni_log_dbg("[JNI] JSharedBuffer_acquire(%p): null ptr", sb);
    }

    java_atomic_inc(&sb->refs);
    fjni_log_dbg("[JNI] JSharedBuffer_acquire(%p): %d", sb, sb->refs);
}

int32_t JSharedBuffer_release(JSharedBuffer *sb) {
    int32_t prev = 1;
    if (sb) {
        if (onlyOwner(sb) || ((prev = java_atomic_dec(&sb->refs)) == 1)) {
            free(sb);
        }
    }

    fjni_log_dbg("[JNI] JSharedBuffer_release(%p): %d", sb, prev);
    return prev;
}

size_t JSharedBuffer_size(JSharedBuffer *sb) {
    return sb ? sb->size : 0;
}

void *JSharedBuffer_data(JSharedBuffer *sb) {
    return sb ? dataFromBuffer(sb) : NULL;
}

JSharedBuffer *JSharedBuffer_bufferFromData(void *data) {
    return data ? bufferFromData(data) : NULL;
}

size_t JSharedBuffer_sizeFromData(void *data) {
    return data ? (bufferFromData(data))->size : 0;
}

bool JSharedBuffer_onlyOwner(JSharedBuffer *sb) {
    return sb ? onlyOwner(sb) : false;
}
