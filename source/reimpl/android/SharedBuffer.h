#ifndef ANDROID_SHARED_BUFFER_H
#define ANDROID_SHARED_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    ANDFROID_SHARED_BUFFER_KEEP_STORAGE = 0x00000001
};

typedef struct SharedBuffer SharedBuffer;

SharedBuffer *ASharedBuffer_alloc(size_t size);
int           ASharedBuffer_dealloc(SharedBuffer *sb);
SharedBuffer *ASharedBuffer_attemptEdit(SharedBuffer *sb);
SharedBuffer *ASharedBuffer_edit(SharedBuffer *sb);
SharedBuffer *ASharedBuffer_editResize(SharedBuffer *sb, size_t new_size);
SharedBuffer *ASharedBuffer_reset(SharedBuffer *sb, size_t new_size);
void          ASharedBuffer_acquire(SharedBuffer *sb);
int32_t       ASharedBuffer_release(SharedBuffer *sb, uint32_t flags);

size_t        ASharedBuffer_size(SharedBuffer *sb);
void         *ASharedBuffer_data(SharedBuffer *sb);
SharedBuffer *ASharedBuffer_bufferFromData(void *data);
size_t        ASharedBuffer_sizeFromData(void *data);
bool          ASharedBuffer_onlyOwner(SharedBuffer *sb);

#ifdef __cplusplus
}
#endif

#endif