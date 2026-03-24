#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "reimpl/android/atomic.h"
#include "reimpl/android/SharedBuffer.h"
#include "utils/logger.h"

struct ASharedBuffer {
    int32_t  refs;
    size_t   size;
    uint32_t reserved[2];
};

#define onlyOwner(sb)        (sb->refs == 1)
#define dataFromBuffer(sb)   ((void *)(sb + 1))
#define bufferFromData(data) ((ASharedBuffer *)data - 1)

ASharedBuffer *ASharedBuffer_alloc(size_t size) {
    ASharedBuffer *sb = (ASharedBuffer *)malloc(sizeof(ASharedBuffer) + size);
    if (!sb) {
        l_error("[Android] ASharedBuffer_alloc(%u): cannot alloc memory", size);
        return NULL;
    }

    sb->refs = 1;
    sb->size = size;

    // l_debug("[Android] ASharedBuffer_alloc(%u): %p", size, sb);
    return sb;
}

int ASharedBuffer_dealloc(ASharedBuffer *sb) {
    if (!sb) {
        l_error("[Android] ASharedBuffer_dealloc(%p): null ptr", sb);
        return -1;
    }

    if (sb->refs != 0) {
        l_error("[Android] ASharedBuffer_dealloc(%p): invalid operation", sb);
        return -1;  // XXX: invalid operation
    }

    free(sb);

    // l_debug("[Android] ASharedBuffer_dealloc(%p): 0", sb);
    return 0;
}

ASharedBuffer *ASharedBuffer_attemptEdit(ASharedBuffer *sb) {
    if (!sb) {
        l_error("[Android] ASharedBuffer_attemptEdit(%p): null ptr", sb);
        return NULL;
    }

    ASharedBuffer *buf = onlyOwner(sb) ? sb : NULL;

    // l_debug("[Android] ASharedBuffer_attemptEdit(%p): %p", sb, buf);
    return buf;
}

ASharedBuffer *ASharedBuffer_edit(ASharedBuffer *sb) {
    if (!sb) {
        l_error("[Android] ASharedBuffer_edit(%p): null ptr", sb);
        return NULL;
    }

    ASharedBuffer *buf;

    if (onlyOwner(sb)) {
        buf = sb;
    } else {
        buf = ASharedBuffer_alloc(sb->size);
        if (buf) {
            memcpy(dataFromBuffer(buf), dataFromBuffer(sb), sb->size);
            ASharedBuffer_release(sb, 0);
        }
    }

    // l_debug("[Android] ASharedBuffer_edit(%p): %p", sb, buf);
    return buf;
}

ASharedBuffer *ASharedBuffer_editResize(ASharedBuffer *sb, size_t new_size) {
    if (!sb) {
        l_error("[Android] ASharedBuffer_editResize(%p, %u): null ptr", sb, new_size);
        return NULL;
    }

    ASharedBuffer *buf;

    if (onlyOwner(sb)) {
        buf = sb;
        if (buf->size != new_size) {
            buf = (ASharedBuffer *)realloc(buf, sizeof(ASharedBuffer) + new_size);
            if (buf != NULL) {
                buf->size = new_size;
            }
        }
    } else {
        buf = ASharedBuffer_alloc(new_size);
        if (buf) {
            memcpy(dataFromBuffer(buf), dataFromBuffer(sb), buf->size < sb->size ? buf->size : sb->size);
            ASharedBuffer_release(sb, 0);
        }
    }

    // l_debug("[Android] ASharedBuffer_editResize(%p): %p", sb, buf);
    return buf;
}

ASharedBuffer *ASharedBuffer_reset(ASharedBuffer *sb, size_t new_size) {
    ASharedBuffer *buf = ASharedBuffer_alloc(new_size);
    if (!sb) {
        l_error("[Android] ASharedBuffer_reset(%p,%u): failed!", sb, new_size);
        return NULL;
    }
    ASharedBuffer_release(sb, 0);

    // l_debug("[Android] ASharedBuffer_reset(%p,%u): %p", sb, new_size, buf);
    return buf;
}

void ASharedBuffer_acquire(ASharedBuffer *sb) {
    if (!sb) {
        l_error("[Android] ASharedBuffer_acquire(%p): null ptr", sb);
        return;
    }

    android_atomic_inc(&sb->refs);
    // l_debug("[Android] ASharedBuffer_acquire(%p): %d", sb, sb->refs);
}

int32_t ASharedBuffer_release(ASharedBuffer *sb, uint32_t flags) {
    int32_t prev = 1;
    if (sb) {
        if (onlyOwner(sb) || ((prev = android_atomic_dec(&sb->refs)) == 1)) {
            sb->refs = 0;
            if ((flags & ANDFROID_SHARED_BUFFER_KEEP_STORAGE) == 0)
                free(sb);
        }
    }

    // l_debug("[Android] ASharedBuffer_release(%p,0x%x): %d", sb, flags, prev);
    return prev;
}

size_t ASharedBuffer_size(ASharedBuffer *sb) {
    return sb ? sb->size : 0;
}

void *ASharedBuffer_data(ASharedBuffer *sb) {
    return sb ? dataFromBuffer(sb) : NULL;
}

ASharedBuffer *ASharedBuffer_bufferFromData(void *data) {
    return data ? bufferFromData(data) : NULL;
}

size_t ASharedBuffer_sizeFromData(void *data) {
    return data ? (bufferFromData(data))->size : 0;
}

bool ASharedBuffer_onlyOwner(ASharedBuffer *sb) {
    return sb ? onlyOwner(sb) : false;
}
