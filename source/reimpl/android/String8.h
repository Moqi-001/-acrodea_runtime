#ifndef ANDROID_STRING8_H
#define ANDROID_STRING8_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AString8 AString8;

int  AString8_construct(AString8 *self);
int  AString8_constructFromCstr(AString8 *self, const char *cstr);
int  AString8_constructFromOther(AString8 *self, const AString8 *other);
void AString8_destruct(AString8 *self);

int AString8_setTo(AString8 *self, const AString8 *other);
int AString8_setToCstr(AString8 *self, const char *cstr);
int AString8_append(AString8 *self, const AString8 *other);
int AString8_appendCstr(AString8 *self, const char *cstr);
int AString8_appendPath(AString8 *self, const AString8 *other);
int AString8_appendPathCstr(AString8 *self, const char *cstr);
int AString8_getPathDir(AString8 *self, const AString8 *other);
int AString8_walkPath(AString8 *self, const AString8 *other, AString8 *out_remains);
int AString8_empty(AString8 *self);

const char *AString8_string(AString8 *self);
size_t      AString8_length(AString8 *self);
int         AString8_isEmpty(AString8 *self);

#ifdef __cplusplus
}
#endif

#endif