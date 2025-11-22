//
// Created by Lisen on 2025/10/31.
//

#ifndef ANDROID7STRING8_H
#define ANDROID7STRING8_H

#endif //ANDROID7STRING8_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void *_ZN7android7String8C1Ev(void **self);
    void *_ZN7android7String8C1EPKc(const void **self, const char *other2);
    void *_ZN7android7String8C1ERKS0_(const void **other, void **src);
    void _ZN7android7String8D1Ev(void *self);
    void *_ZN7android7String85setToEPKc(void *self, const char *other);
    void *_ZN7android7String86appendEPKc(void *self, const char *other);
    void *_ZN7android7String86appendERKS0_(char **self, const char *other);
    void *_ZN7android7String810appendPathEPKc(void *self, const char *component);
    const char *_ZNK7android7String86stringEv(void *self);
    size_t _ZNK7android7String86lengthEv(void *self);
    void *_ZNK7android7String810getPathDirEv(const void **other, void **src);
    void *_ZNK7android7String88walkPathEPS0_(void *self, void *outRemains);

    void *_ZN7android12SharedBuffer5allocEj(void *self, size_t size);
    void *_ZNK7android12SharedBuffer10editResizeEj(void *self, size_t size);
    int _ZNK7android12SharedBuffer7releaseEj(void *self, size_t flags);
    int _ZN7android12SharedBuffer7deallocEPKS0_(void *self, void *a2);
    int _ZNK7android12SharedBuffer7acquireEv(void *self);
    void *_ZNK7android12SharedBuffer11attemptEditEv(void *self);
#ifdef __cplusplus
}
#endif