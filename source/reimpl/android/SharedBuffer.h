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

typedef struct ASharedBuffer ASharedBuffer;

ASharedBuffer *ASharedBuffer_alloc(size_t size);
int            ASharedBuffer_dealloc(ASharedBuffer *sb);
ASharedBuffer *ASharedBuffer_attemptEdit(ASharedBuffer *sb);
ASharedBuffer *ASharedBuffer_edit(ASharedBuffer *sb);
ASharedBuffer *ASharedBuffer_editResize(ASharedBuffer *sb, size_t new_size);
ASharedBuffer *ASharedBuffer_reset(ASharedBuffer *sb, size_t new_size);
void           ASharedBuffer_acquire(ASharedBuffer *sb);
int32_t        ASharedBuffer_release(ASharedBuffer *sb, uint32_t flags);

size_t         ASharedBuffer_size(ASharedBuffer *sb);
void          *ASharedBuffer_data(ASharedBuffer *sb);
ASharedBuffer *ASharedBuffer_bufferFromData(void *data);
size_t         ASharedBuffer_sizeFromData(void *data);
bool           ASharedBuffer_onlyOwner(ASharedBuffer *sb);

#ifdef __cplusplus
}
#endif

#endif