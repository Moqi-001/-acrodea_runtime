//
// Created by Lisen on 2025/10/31.
//

#ifndef ANDROID7STRING8_H
#define ANDROID7STRING8_H

#endif //ANDROID7STRING8_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    void* _ZN7android7String8C1Ev();
    void* _ZN7android7String8C1EPKc(const char* other);
    void*_ZN7android7String8C1EPKcj(const char* other, size_t len);
    void* _ZN7android7String8C1EPKcS1_(const char* other, const char* other2);
    void* _ZN7android7String8C1ERKS0_(void* other);
    void _ZN7android7String8D1Ev(void* self);
    void* _ZN7android7String85setToEPKc(void* self, const char* other);
    void* _ZN7android7String86appendEPKc(void* self, const char* other);
    void* _ZN7android7String86appendERKS0_(void* self, void* other);
    void* _ZN7android7String810appendPathEPKc(void* self, const char* component);
    const char* _ZNK7android7String86stringEv(void* self);
    size_t _ZNK7android7String86lengthEv(void* self);
    void* _ZNK7android7String810getPathDirEv(void* self);
    void* _ZNK7android7String88walkPathEPS0_(void* self, void* outRemains);

#ifdef __cplusplus
}
#endif