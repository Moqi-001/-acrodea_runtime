#include <stddef.h>
#include <stdint.h>

#include "reimpl/android/Thread.h"
#include "utils/logger.h"

struct Thread {
    uint32_t reserved;
};

// Thread 构造函数
void _ZN7android6ThreadC2Eb(Thread *self, int canCallJava) {
    l_debug("[Android][Thread] construct(%p, %d): unsupport!", self, canCallJava);
}

// Thread 析构函数
void _ZN7android6ThreadD0Ev(Thread *self) {
    l_debug("[Android][Thread] destructDeleting(%p): unsupport!", self);
}

void _ZN7android6ThreadD1Ev(Thread *self) {
    l_debug("[Android][Thread] destructComplete(%p): unsupport!", self);
}

void _ZN7android6ThreadD2Ev(Thread *self) {
    l_debug("[Android][Thread] destructBase(%p): unsupport!", self);
}

void _ZTv0_n12_N7android6ThreadD0Ev(Thread *self) {
    l_debug("[Android][Thread] destructDeleting_thunk(%p): unsupport!", self);
}

void _ZTv0_n12_N7android6ThreadD1Ev(Thread *self) {
    l_debug("[Android][Thread] destructComplete_thunk(%p): unsupport!", self);
}

int _ZN7android6Thread10readyToRunEv(Thread *self) {
    l_debug("[Android][Thread] readyToRun(%p): unsupport!", self);
    return 1;
}

void _ZN7android6Thread11requestExitEv(Thread *self) {
    l_debug("[Android][Thread] requestExit(%p): unsupport!", self);
}

int _ZN7android6Thread3runEPKcij(Thread *self, const char *name, int priority, size_t stack) {
    l_debug("[Android][Thread] run(%p, %s, %d, %zu): unsupport!",
            self, name ? name : "NULL", priority, stack);
    return 1;
}
