#ifndef ANDROID_REF_BASE_H
#define ANDROID_REF_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RefBase RefBase;

void _ZN7android7RefBaseC2Ev(RefBase *self);
void _ZN7android7RefBaseD2Ev(RefBase *self);
void _ZN7android7RefBase10onFirstRefEv(RefBase *self);
void _ZN7android7RefBase15onLastStrongRefEPKv(RefBase *self, const void *id);
void _ZN7android7RefBase13onLastWeakRefEPKv(RefBase *self, const void *id);
int  _ZN7android7RefBase20onIncStrongAttemptedEjPKv(RefBase *self, unsigned int flags, const void *id);
void _ZNK7android7RefBase9incStrongEPKv(RefBase *self, const void *id);
void _ZNK7android7RefBase9decStrongEPKv(RefBase *self, const void *id);

#ifdef __cplusplus
}
#endif

#endif