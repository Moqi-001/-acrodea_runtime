/*
 * Copyright (C) 2022-2024 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/threadmgr.h>

#include "config.h"
#include "utils/logger.h"

#define COLOR_RED    "\x1B[38;5;196m"
#define COLOR_PINK   "\x1B[38;5;212m"
#define COLOR_ORANGE "\x1B[38;5;202m"
#define COLOR_BLUE   "\x1B[38;5;32m"
#define COLOR_GREEN  "\x1B[32m"
#define COLOR_CYAN   "\x1B[36m"

#define COLOR_END "\033[0m"

static SceKernelLwMutexWork _log_mutex;
static atomic_bool          _log_mutex_ready = ATOMIC_VAR_INIT(false);

// Buffer A is used to adjust the format string.
static char buffer_a[2048];
// Buffer B is used to compile the final log using the updated format string.
static char buffer_b[2048];

void _log_print(int t, const char *fmt, ...) {
    if (!atomic_load_explicit(&_log_mutex_ready, memory_order_relaxed)) {
        int ret = sceKernelCreateLwMutex(&_log_mutex, "log_lock", 0, 0, NULL);
        if (ret < 0) {
            sceClibPrintf("Error: failed to create log mutex: 0x%x\n", ret);
            return;
        }
        atomic_store_explicit(&_log_mutex_ready, true, memory_order_relaxed);
    }
    sceKernelLockLwMutex(&_log_mutex, 1, NULL);

    va_list list;
    va_start(list, fmt);
    sceClibVsnprintf(buffer_a, sizeof(buffer_a), fmt, list);
    va_end(list);

#ifdef DEBUG_LOGFILE
    SceUID fd = sceIoOpen(LOG_PATH, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 0777);
    if (fd >= 0) {
        sceIoWrite(fd, buffer_a, strlen(buffer_a));
        sceIoWrite(fd, "\n", 1);
        sceIoClose(fd);
    }
#endif

    switch (t) {
        case LT_DEBUG:
            sceClibSnprintf(buffer_b, sizeof(buffer_b), "[DEBUG] %s\n", buffer_a);
            break;
        case LT_INFO:
            sceClibSnprintf(buffer_b, sizeof(buffer_b), " %s[INFO]%s %s\n",
                            COLOR_BLUE, COLOR_END, buffer_a);
            break;
        case LT_WARN:
            sceClibSnprintf(buffer_b, sizeof(buffer_b), " %s[WARN]%s %s\n",
                            COLOR_ORANGE, COLOR_END, buffer_a);
            break;
        case LT_ERROR:
            sceClibSnprintf(buffer_b, sizeof(buffer_b), " %s[ERRROR]%s %s\n",
                            COLOR_RED, COLOR_END, buffer_a);
            break;
        case LT_FATAL:
            sceClibSnprintf(buffer_b, sizeof(buffer_b), " %s[FATAL]%s %s\n",
                            COLOR_RED, COLOR_END, buffer_a);
        default:
            sceClibPrintf(buffer_a);
            if (atomic_load_explicit(&_log_mutex_ready, memory_order_relaxed)) {
                sceKernelUnlockLwMutex(&_log_mutex, 1);
            }
            return;
    }

    sceClibPrintf(buffer_b);

    if (atomic_load_explicit(&_log_mutex_ready, memory_order_relaxed)) {
        sceKernelUnlockLwMutex(&_log_mutex, 1);
    }
}
