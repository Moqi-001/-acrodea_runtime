#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <so_common/so_logger.h>

#include "ld.h"

// 此函数用于一些结构体的初始分配，但原代码对成员没有置0，
// 这导致了后面以0判断指针是否有效的结果出错，从而free野指针造成崩溃。
static void *NNMalloc_hook(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        so_loge("[Mugenmakai] NNMalloc(%u): error!", size);
        return NULL;
    }
    memset(ptr, 0, size);
    // so_loga("[Mugenmakai] NNMalloc(%u): %p", size, ptr);
    return ptr;
}

int ld_patch_preinit_mugenmakai(LdModule *mod) {
    __ldModuleHookAddr(mod, 0x1a6d98, (uintptr_t)&NNMalloc_hook, NULL);
    return 0;
}

int ld_patch_mugenmakai(LdModule *mod) {
    return 0;
}
