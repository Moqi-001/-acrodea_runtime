
#include "reimpl/android/RefBase.h"
#include "utils/logger.h"

struct RefBase {
    int ref_count;
};

// RefBase 构造函数
void _ZN7android7RefBaseC2Ev(RefBase *self) {
    l_debug("[Android][RefBase] construct(%p): unsupport!", self);
}

// RefBase 析构函数
void _ZN7android7RefBaseD2Ev(RefBase *self) {
    l_debug("[Android][RefBase] destruct(%p): unsupport!", self);
}

void _ZN7android7RefBase10onFirstRefEv(RefBase *self) {
    l_debug("[Android][RefBase] onFirstRef(%p): unsupport!", self);
}

void _ZN7android7RefBase15onLastStrongRefEPKv(RefBase *self, const void *id) {
    l_debug("[Android][RefBase] onLastStrongRef(%p, %p): unsupport!", self, id);
}

void _ZN7android7RefBase13onLastWeakRefEPKv(RefBase *self, const void *id) {
    l_debug("[Android][RefBase] onLastWeakRef(%p, %p): unsupport!", self, id);
}

int _ZN7android7RefBase20onIncStrongAttemptedEjPKv(RefBase *self, unsigned int flags, const void *id) {
    l_debug("[Android][RefBase] onIncStrongAttempted(%p, 0x%x, %p): unsupport!", self, flags, id);
    return 1;
}

void _ZNK7android7RefBase9incStrongEPKv(RefBase *self, const void *id) {
    self->ref_count++;
    l_debug("[Android][RefBase] incStrong(%p, %p): unsupport!", self, id);
}

void _ZNK7android7RefBase9decStrongEPKv(RefBase *self, const void *id) {
    self->ref_count--;
    l_debug("[Android][RefBase] decStrong(%p, %p): unsupport!", self, id);
}
