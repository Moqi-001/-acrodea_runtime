#include <string.h>

#include "so_internal.h"
#include "so_linker.h"

int so_hook_function_safe(void *target, void *boundary, void *replacement, so_hook_ref *p_hook) {
    if (!target || !replacement) return -1;

    uintptr_t target_addr = (uintptr_t)target;
    uint8_t   new_instr[12];
    int       instr_len = 0;

    if (target_addr & 1) {
        // Thumb
        target_addr &= ~1;
        if (target_addr & 2) {
            // 非4字节对齐时插入NOP填充，使LDR.W PC指令能够4字节对齐
            *(uint16_t *)(new_instr + 0) = 0xBF00;      // Thumb NOP
            *(uint32_t *)(new_instr + 2) = 0xF000F8DF;  // LDR.W PC, [PC]
            *(uint32_t *)(new_instr + 6) = (uint32_t)replacement;
            instr_len                    = 10;
        } else {
            *(uint32_t *)(new_instr + 0) = 0xF000F8DF;  // LDR.W PC, [PC]
            *(uint32_t *)(new_instr + 4) = (uint32_t)replacement;
            instr_len                    = 8;
        }
    } else {
        // Arm
        *(uint32_t *)(new_instr + 0) = 0xE51FF004;  // LDR PC, [PC, #-4]
        *(uint32_t *)(new_instr + 4) = (uint32_t)replacement;
        instr_len                    = 8;
    }

    // boundary为NULL时视为无限边界，跳过检查
    if (boundary && target_addr + instr_len > (uintptr_t)boundary) {
        return -1;  // 超出边界
    }

    if (p_hook) {
        p_hook->target    = target;
        p_hook->instr_len = instr_len;
        so_memcpy(p_hook->old_instr, (void *)target_addr, instr_len);
        so_memcpy(p_hook->new_instr, new_instr, instr_len);
    }

    so_memcpy((void *)target_addr, new_instr, instr_len);
    so_cacheflush((void *)target_addr, instr_len);
    return 0;
}

int so_hook_function(void *target, void *replacement, so_hook_ref *p_hook) {
    return so_hook_function_safe(target, NULL, replacement, p_hook);
}

int so_unhook(so_hook_ref *p_hook) {
    if (!p_hook || !p_hook->target) return -1;

    uintptr_t target_addr = (uintptr_t)p_hook->target & ~1;
    so_memcpy((void *)target_addr, p_hook->old_instr, p_hook->instr_len);
    so_cacheflush((void *)target_addr, p_hook->instr_len);
    p_hook->target = NULL;

    return 0;
}

int so_inject_data_safe(void *target, void *boundary, const void *data, size_t len) {
    if (!target || !data || len == 0) return -1;

    if (boundary && (uintptr_t)target + len > (uintptr_t)boundary) {
        return -1;  // 超出边界
    }

    so_memcpy(target, data, len);
    so_cacheflush(target, len);
    return 0;
}

int so_inject_data(void *target, const void *data, size_t len) {
    return so_inject_data_safe(target, NULL, data, len);
}
