#include <stdlib.h>
#include <string.h>

#include <kubridge.h>
#include <so_util/so_util.h>

#include "ld_mem.h"
#include "utils/logger.h"

#ifndef SCE_KERNEL_MEMBLOCK_TYPE_USER_RX
#define SCE_KERNEL_MEMBLOCK_TYPE_USER_RX (0x0C20D050)
#endif

#define PAGE_SIZE 4096

#define ALIGN_UP(x, align)   (((x) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))

#define MMAPER_ALIGN_SIZE (ALIGN_UP(sizeof(LdMemMapper), PAGE_SIZE))

LdMemMapper *LdGetMemMapperFromData(const void *data) {
    return data ? (LdMemMapper *)((uintptr_t)data - MMAPER_ALIGN_SIZE) : NULL;
}

LdMemMapper *LdMemMapperCreate(uintptr_t addr) {
    LdMemMapper *mmapper  = NULL;
    size_t       mem_size = MMAPER_ALIGN_SIZE;
    uintptr_t    mem_addr = addr - mem_size;

    SceKernelAllocMemBlockKernelOpt opt;
    memset(&opt, 0, sizeof(SceKernelAllocMemBlockKernelOpt));
    opt.size       = sizeof(SceKernelAllocMemBlockKernelOpt);
    opt.attr       = 0x1;
    opt.field_C    = mem_addr;
    SceUID blockid = kuKernelAllocMemBlock("rw_block", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW, mem_size, &opt);
    if (blockid < 0) {
        // l_debug("LdMemMapperCreate(0x%x) failed: 0x%x", addr, blockid);
        return NULL;
    }

    sceKernelGetMemBlockBase(blockid, (void **)&mmapper);
    memset(mmapper, 0, sizeof(LdMemMapper));
    mmapper->blockid  = blockid;
    mmapper->map_addr = addr;

    // l_debug("LdMemMapperCreate(0x%x): %p", addr, mmapper);
    return mmapper;
}

void LdMemMapperDestroy(LdMemMapper *mmapper) {
    if (mmapper) {
        // l_debug("LdMemMapperDestroy(%p): load_addr=0x%x, length=%u", mmapper, mmapper->map_addr, mmapper->length);
        LdMemBlock *block = mmapper->head;
        while (block) {
            LdMemBlock *next = block->next;
            LdFreeMemBlock(block);
            free(block);
            block = next;
        }
        sceKernelFreeMemBlock(mmapper->blockid);
    }
}

LdMemBlock *LdAllocMemBlock(uintptr_t addr, int prot, size_t size, size_t align) {
    LdMemBlock *block = calloc(1, sizeof(LdMemBlock));
    if (!block) {
        return NULL;
    }

    uintptr_t mem_addr = ALIGN_DOWN(addr, PAGE_SIZE);
    size_t    mem_size = ALIGN_UP(size + (addr - mem_addr), align);

    int type = SCE_KERNEL_MEMBLOCK_TYPE_USER_RW;
    if (prot & PROT_EXEC)
        type = SCE_KERNEL_MEMBLOCK_TYPE_USER_RX;

    SceKernelAllocMemBlockKernelOpt opt;
    memset(&opt, 0, sizeof(SceKernelAllocMemBlockKernelOpt));
    opt.size       = sizeof(SceKernelAllocMemBlockKernelOpt);
    opt.attr       = 0x1;
    opt.field_C    = mem_addr;
    SceUID blockid = kuKernelAllocMemBlock("ld_block", type, mem_size, &opt);
    if (blockid < 0) {
        // l_error("LdAllocMemBlock(0x%x,0x%x,%zu,0x%x) failed: 0x%x", addr, prot, size, align, block);
        free(block);
        return NULL;
    }

    block->blockid  = blockid;
    block->prot     = prot;
    block->map_addr = addr;
    block->map_size = size;
    block->mem_size = mem_size;
    sceKernelGetMemBlockBase(blockid, (void **)&block->mem_addr);

    // l_debug("LdAllocMemBlock(0x%x,0x%x,%zu,0x%x): %p", addr, prot, size, align, block);
    return block;
}

void LdFreeMemBlock(LdMemBlock *block) {
    if (block) {
        int ret = sceKernelFreeMemBlock(block->blockid);
        if (ret < 0) {
            l_error("LdFreeMemBlock(%p) error: 0x%x", block, ret);
            return;
        }
        l_debug("LdFreeMemBlock(%p): 0x%x", block, ret);
        free(block);
    }
}

int LdMemMapperAddBlock(LdMemMapper *mmapper, LdMemBlock *block) {
    if (!mmapper || !block)
        return -1;

    if (!mmapper->head) {
        mmapper->head = block;
        mmapper->tail = block;
    } else {
        mmapper->tail->next = block;
        mmapper->tail       = block;
    }

    mmapper->length++;
    // l_debug("LdMemMapperAddBlock(%p,%p): addr=%p, size=0x%x", mmapper, block, block->map_addr, block->map_size);
    return 0;
}

void LdMemMapperFlushCaches(LdMemMapper *mmapper) {
    // l_debug("LdMemMapperFlushCaches(%p): addr=0x%x", mmapper, mmapper->map_addr);
    if (mmapper) {
        LdMemBlock *block = mmapper->head;
        while (block) {
            kuKernelFlushCaches((void *)block->mem_addr, block->mem_size);
            block = block->next;
        }
    }
}
