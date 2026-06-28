#ifndef ANDROID_CUTILS_ATOMIC_H
#define ANDROID_CUTILS_ATOMIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int32_t android_atomic_add(int32_t increment, volatile int32_t *ptr);
int32_t android_atomic_inc(int32_t *addr);
int32_t android_atomic_dec(int32_t *addr);
int32_t android_atomic_or(int32_t value, int32_t *addr);
int32_t android_atomic_and(int32_t value, int32_t *addr);

int32_t android_atomic_acquire_load(volatile const int32_t *ptr);
int32_t android_atomic_release_load(volatile const int32_t *ptr);
void    android_atomic_acquire_store(int32_t value, volatile int32_t *ptr);
void    android_atomic_release_store(int32_t value, volatile int32_t *ptr);

int android_atomic_cas(int32_t old_value, int32_t new_value, volatile int32_t *ptr);
int android_atomic_acquire_cas(int32_t old_value, int32_t new_value, volatile int32_t *ptr);
int android_atomic_release_cas(int32_t old_value, int32_t new_value, volatile int32_t *ptr);

#define android_atomic_write   android_atomic_release_store
#define android_atomic_cmpxchg android_atomic_release_cas

#ifdef __cplusplus
}
#endif

#endif  // ANDROID_CUTILS_ATOMIC_H