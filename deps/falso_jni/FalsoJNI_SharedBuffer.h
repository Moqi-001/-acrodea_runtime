#ifndef FALSOJNI_SHARED_BUFFER_H
#define FALSOJNI_SHARED_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct JSharedBuffer JSharedBuffer;

JSharedBuffer *JSharedBuffer_alloc(size_t size);
int            JSharedBuffer_dealloc(JSharedBuffer *sb);
JSharedBuffer *JSharedBuffer_attemptEdit(JSharedBuffer *sb);
JSharedBuffer *JSharedBuffer_edit(JSharedBuffer *sb);
JSharedBuffer *JSharedBuffer_editResize(JSharedBuffer *sb, size_t new_size);
JSharedBuffer *JSharedBuffer_reset(JSharedBuffer *sb, size_t new_size);
void           JSharedBuffer_acquire(JSharedBuffer *sb);
int32_t        JSharedBuffer_release(JSharedBuffer *sb);

size_t         JSharedBuffer_size(JSharedBuffer *sb);
void          *JSharedBuffer_data(JSharedBuffer *sb);
JSharedBuffer *JSharedBuffer_bufferFromData(void *data);
size_t         JSharedBuffer_sizeFromData(void *data);
bool           JSharedBuffer_onlyOwner(JSharedBuffer *sb);

#ifdef __cplusplus
}
#endif

#endif