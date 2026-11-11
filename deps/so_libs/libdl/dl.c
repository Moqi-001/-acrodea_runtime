#include <stdint.h>
#include <string.h>

#include <so_common/so_logger.h>
#include <so_linker/so_linker.h>

// 默认 dlsym 找不到符号时返回 NULL，保持“可选探测 + 回退下一个库”的语义。
// 打开 DL_TRAP_MISSING 后改为返回一个 16 字节的 ARM 桩：桩里带着库名和符号名，
// 首次被调用时打 ERROR 并返回 0。用来定位“把 dlsym 结果直接存表、不判空”导致的
// PC=0（如 Acrodea 运行时），默认关闭，因为它会让缺失符号看起来“存在”。
#ifndef DL_TRAP_MISSING
#define DL_TRAP_MISSING 0
#endif

#if DL_TRAP_MISSING
#include <kubridge.h>
#include <psp2/kernel/sysmem.h>

#define TRAP_MAX     256
#define TRAP_SIZE    16
#define TRAP_NAME_SZ 64

static uint8_t *s_trap_base;
static char     s_trap_names[TRAP_MAX][TRAP_NAME_SZ];
static char     s_trap_sonames[TRAP_MAX][TRAP_NAME_SZ];
static uint8_t  s_trap_reported[TRAP_MAX];
static int      s_trap_count;

static int so_missing_import_trap(uint32_t index) {
    if (index < TRAP_MAX && s_trap_names[index][0] && !s_trap_reported[index]) {
        so_loge("[libdl] %s: missing import '%s' was called (trap stub returned 0)",
                s_trap_sonames[index][0] ? s_trap_sonames[index] : "?", s_trap_names[index]);
        s_trap_reported[index] = 1;
    }

    return 0;
}

static void *trap_alloc(const char *soname, const char *name) {
    if (!s_trap_base) {
        SceUID uid = sceKernelAllocMemBlock("missing_import_trap", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW,
                                            TRAP_MAX * TRAP_SIZE, NULL);
        void *base = NULL;

        if (uid < 0 || sceKernelGetMemBlockBase(uid, &base) < 0 || !base) {
            so_loge("[libdl] failed to allocate trap stub memory");
            return NULL;
        }
        kuKernelMemProtect(base, TRAP_MAX * TRAP_SIZE,
                           KU_KERNEL_PROT_READ | KU_KERNEL_PROT_WRITE | KU_KERNEL_PROT_EXEC);
        s_trap_base = (uint8_t *)base;
    }

    if (s_trap_count >= TRAP_MAX) {
        so_loge("[libdl] trap stub table is full (%d entries)", TRAP_MAX);
        return NULL;
    }

    int       index = s_trap_count++;
    uint32_t *trap  = (uint32_t *)(s_trap_base + index * TRAP_SIZE);

    // 符号名可能来自调用方的临时缓冲区，这里复制一份，避免以后悬空
    strncpy(s_trap_names[index], name, TRAP_NAME_SZ - 1);
    strncpy(s_trap_sonames[index], soname ? soname : "?", TRAP_NAME_SZ - 1);

    trap[0] = 0xE59F0000;  // ldr r0, [pc, #0] -> index
    trap[1] = 0xE59FF000;  // ldr pc, [pc, #0] -> trap handler
    trap[2] = (uint32_t)index;
    trap[3] = (uint32_t)(uintptr_t)&so_missing_import_trap;

    kuKernelFlushCaches(trap, TRAP_SIZE);
    return trap;
}
#endif  // DL_TRAP_MISSING

void *dlopen_soloader(const char *filename, int flag) {
    if (!filename) {
        return NULL;
    }

    const char *soname = strrchr(filename, '/');
    if (!soname++) {
        soname = filename;
    }

    so_module *mod = so_linker_find_module(soname);
    if (!mod) {
        so_logw("[libdl] dlopen(%s,0x%x): not found!", filename, flag);
        return NULL;
    }

    so_logd("[libdl] dlopen(%s,0x%x): %p", filename, flag, mod);
    return (void *)mod;
}

void *dlsym_soloader(void *handle, const char *symbol) {
    if (!handle || !symbol) {
        return NULL;
    }

    so_module *mod = (so_module *)handle;
    void      *sym = so_module_find_symbol(mod, symbol);

    if (!sym) {
        const char *soname = so_module_get_name(mod);

        so_logw("[libdl] %s: '%s' not found!", soname ? soname : "?", symbol);
#if DL_TRAP_MISSING
        return trap_alloc(soname, symbol);
#else
        return NULL;
#endif
    }

    so_logd("[libdl] dlsym(%p,%s): %p", handle, symbol, sym);
    return sym;
}

void dlclose_soloader(void *handle) {
    so_logd("[libdl] dlclose(%p): ignored", handle);
}

char *dlerror_soloader(void) {
    so_logd("[libdl] dlerror(): ignored");
    return NULL;
}
