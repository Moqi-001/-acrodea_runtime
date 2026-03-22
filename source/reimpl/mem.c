/*
 * Copyright (C) 2021      Andy Nguyen
 * Copyright (C) 2022      Rinnegatamante
 * Copyright (C) 2022-2023 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include <kubridge.h>
#include <string.h>
#include <sys/types.h>

#include "reimpl/mem.h"
#include "utils/logger.h"

#define ALIGN(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

/*
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offs) {
    l_warn("mmap(%p, %i, %i, %i, %i, %li)", addr, length, prot, flags, fd, offs);

    if (length <= 0) {
        return MAP_FAILED;
    }
    void *ret = malloc(length);
    if (!ret) {
        return MAP_FAILED;
    }
    memset(ret, 0, length);
    l_debug("mmap return %p", ret);
    return ret;
}

int munmap(void *addr, size_t length) {
    if (addr) free(addr);
    return 0;
}
*/

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    void  *mem_data = NULL;
    size_t mem_size = ALIGN(length, 0x1000);

    SceKernelAllocMemBlockKernelOpt opt;
    memset(&opt, 0, sizeof(SceKernelAllocMemBlockKernelOpt));
    opt.size = sizeof(SceKernelAllocMemBlockKernelOpt);
    if (flags & MAP_FIXED) {
        opt.attr    = 0x1;
        opt.field_C = (SceUInt32)addr;
    } else {
        opt.attr     = 0x10000;
        opt.field_30 = 2;
    }

    SceUID blockid = kuKernelAllocMemBlock("mmap", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW, mem_size, &opt);
    if (blockid < 0) {
        l_debug("mmap(%p,%zu,0x%x,0x%x,%d,%ld) failed: 0x%x", addr, length, prot, flags, fd, offset, blockid);
        return MAP_FAILED;
    }
    sceKernelGetMemBlockBase(blockid, &mem_data);

    SceUInt32 ku_prot = KU_KERNEL_PROT_NONE;
    if (prot & PROT_READ)
        ku_prot |= KU_KERNEL_PROT_READ;
    if (prot & PROT_WRITE)
        ku_prot |= KU_KERNEL_PROT_WRITE;
    if (prot & PROT_EXEC)
        ku_prot |= KU_KERNEL_PROT_EXEC;
    kuKernelMemProtect(mem_data, mem_size, ku_prot);

    l_debug("mmap(%p,%zu,0x%x,0x%x,%d,%ld): %p", addr, length, prot, flags, fd, offset, mem_data);
    return mem_data;
}

int munmap(void *addr, size_t length) {
    int blockid = sceKernelFindMemBlockByAddr(addr, length);
    if (blockid < 0) {
        l_debug("munmap(%p,%zu) error: Could not find memory!", addr, length);
        return -1;
    }

    sceKernelFreeMemBlock(blockid);
    l_debug("munmap(%p,%zu): 0", addr, length);
    return 0;
}

int mprotect(void *addr, size_t length, int prot) {
    SceUInt32 ku_port = KU_KERNEL_PROT_NONE;
    if ((prot & PROT_READ) == PROT_READ)
        ku_port |= PROT_READ;
    if ((prot & PROT_WRITE) == PROT_WRITE)
        ku_port |= KU_KERNEL_PROT_WRITE;
    if ((prot & PROT_EXEC) == PROT_EXEC)
        ku_port |= KU_KERNEL_PROT_EXEC;

    int res = kuKernelMemProtect(addr, length, ku_port);
    l_debug("mprotect(%p,%zu,0x%x)\n", addr, length, prot);
    return res;
}