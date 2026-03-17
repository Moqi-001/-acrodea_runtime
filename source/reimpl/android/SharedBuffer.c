#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "reimpl/android/atomic.h"
#include "reimpl/android/SharedBuffer.h"
#include "utils/logger.h"

struct SharedBuffer {
    int32_t  refs;
    size_t   size;
    uint32_t reserved[2];
};

#define onlyOwner(sb) (sb->refs == 1)
#define getData(sb)   ((void *)(sb + 1))

SharedBuffer *ASharedBuffer_alloc(size_t size) {
    l_debug("[Android] ASharedBuffer_alloc(%u)", size);

    SharedBuffer *sb = (SharedBuffer *)malloc(sizeof(SharedBuffer) + size);
    if (!sb) {
        l_debug("[Android] ASharedBuffer_alloc failed: cannot alloc memory");
        return NULL;
    }

    sb->refs = 1;
    sb->size = size;

    l_debug("[Android] ASharedBuffer_alloc over: sb=%p,data=%p", sb, getData(sb));
    return sb;
}

int ASharedBuffer_dealloc(SharedBuffer *sb) {
    l_debug("[Android] ASharedBuffer_dealloc(%p)", sb);

    if (!sb) {
        l_debug("[Android] ASharedBuffer_dealloc failed: null ptr");
        return -1;
    }

    if (sb->refs != 0) {
        l_debug("[Android] ASharedBuffer_dealloc invalid operation: refs=%d", sb->refs);
        return -1;  // XXX: invalid operation
    }

    free(sb);

    l_debug("[Android] ASharedBuffer_dealloc ok");
    return 0;
}

SharedBuffer *ASharedBuffer_attemptEdit(SharedBuffer *sb) {
    l_debug("[Android] ASharedBuffer_attemptEdit(%p)", sb);

    if (!sb) {
        l_debug("[Android] ASharedBuffer_attemptEdit failed: null ptr");
        return NULL;
    }

    SharedBuffer *buf = onlyOwner(sb) ? sb : NULL;

    l_debug("[Android] ASharedBuffer_attemptEdit over: %p", buf);
    return buf;
}

SharedBuffer *ASharedBuffer_edit(SharedBuffer *sb) {
    l_debug("[Android] ASharedBuffer_edit(%p)", sb);

    if (!sb) {
        l_debug("[Android] ASharedBuffer_edit failed: null ptr");
        return NULL;
    }

    SharedBuffer *buf;

    if (onlyOwner(sb)) {
        buf = sb;
    } else {
        buf = ASharedBuffer_alloc(sb->size);
        if (buf) {
            memcpy(getData(buf), getData(sb), sb->size);
            ASharedBuffer_release(sb, 0);
        }
    }

    l_debug("[Android] ASharedBuffer_edit over: %p", buf);
    return sb;
}

SharedBuffer *ASharedBuffer_editResize(SharedBuffer *sb, size_t new_size) {
    l_debug("[Android] ASharedBuffer_editResize(%p,%u)", sb, new_size);

    if (!sb) {
        l_debug("[Android] ASharedBuffer_editResize failed: null ptr");
        return NULL;
    }

    SharedBuffer *buf;

    if (onlyOwner(sb)) {
        buf = sb;
        if (buf->size != new_size) {
            buf = (SharedBuffer *)realloc(buf, sizeof(SharedBuffer) + new_size);
            if (buf != NULL)
                buf->size = new_size;
        }
    } else {
        buf = ASharedBuffer_alloc(new_size);
        if (buf) {
            memcpy(getData(buf), getData(sb), buf->size < sb->size ? buf->size : sb->size);
            ASharedBuffer_release(sb, 0);
        }
    }

    l_debug("[Android] ASharedBuffer_editResize over: %p", buf);
    return buf;
}

SharedBuffer *ASharedBuffer_reset(SharedBuffer *sb, size_t new_size) {
    l_debug("[Android] ASharedBuffer_reset(%p,%u)", sb, new_size);

    // cheap-o-reset.
    SharedBuffer *buf = ASharedBuffer_alloc(new_size);
    if (buf)
        ASharedBuffer_release(sb, 0);

    l_debug("[Android] ASharedBuffer_reset over: %p", buf);
    return buf;
}

void ASharedBuffer_acquire(SharedBuffer *sb) {
    l_debug("[Android] ASharedBuffer_acquire(%p)", sb);

    if (!sb) {
        l_debug("[Android] ASharedBuffer_acquire failed: null ptr");
        return;
    }

    android_atomic_inc(&sb->refs);

    l_debug("[Android] ASharedBuffer_acquire over: %d", sb->refs);
}

int32_t ASharedBuffer_release(SharedBuffer *sb, uint32_t flags) {
    l_debug("[Android] ASharedBuffer_release(%p,%u)", sb, flags);

    if (!sb) {
        l_debug("[Android] ASharedBuffer_release failed: null ptr");
        return 0;
    }

    int32_t prev = 1;
    if (onlyOwner(sb) || ((prev = android_atomic_dec(&sb->refs)) == 1)) {
        sb->refs = 0;
        if ((flags & ANDFROID_SHARED_BUFFER_KEEP_STORAGE) == 0)
            free(sb);
    }

    l_debug("[Android] ASharedBuffer_release over: prev_refs=%d", prev);
    return prev;
}

size_t ASharedBuffer_size(SharedBuffer *sb) {
    return sb ? sb->size : 0;
}

void *ASharedBuffer_data(SharedBuffer *sb) {
    return sb ? (void *)(sb + 1) : NULL;
}

SharedBuffer *ASharedBuffer_bufferFromData(void *data) {
    return data ? (SharedBuffer *)data - 1 : NULL;
}

size_t ASharedBuffer_sizeFromData(void *data) {
    return data ? (ASharedBuffer_bufferFromData(data))->size : 0;
}

bool ASharedBuffer_onlyOwner(SharedBuffer *sb) {
    return sb ? sb->refs == 1 : false;
}
