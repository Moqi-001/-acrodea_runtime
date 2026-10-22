#include <stdint.h>

#include <sys/time.h>

#include <so_common/so_logger.h>

#include "Timers.h"

nsecs_t systemTime(int clock) {
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &t);
    nsecs_t nanoSeconds = (nsecs_t)t.tv_sec * 1000000000LL + t.tv_nsec;
    // so_logd("systemTime(%d): (%lld)", clock, nanoSeconds);
    return nanoSeconds;
}
