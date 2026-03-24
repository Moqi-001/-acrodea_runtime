#ifndef SOLOADED_LD_MEM_H
#define SOLOADED_LD_MEM_H

#include <stdint.h>

#define PROT_READ  0x1
#define PROT_WRITE 0x2
#define PROT_EXEC  0x4

typedef struct LdMemBlock {
    struct LdMemBlock *next;
    int                prot;
    SceUID             blockid;
    uintptr_t          mem_addr;
    size_t             mem_size;
    uintptr_t          map_addr;
    size_t             map_size;
} LdMemBlock;

typedef struct LdMemMapper {
    LdMemBlock *head;
    LdMemBlock *tail;
    size_t      length;
    SceUID      blockid;
    uintptr_t   map_addr;
} LdMemMapper;

LdMemMapper *LdGetMemMapperFromData(const void *data);
LdMemMapper *LdMemMapperCreate(uintptr_t addr);
void         LdMemMapperDestroy(LdMemMapper *mmapper);
LdMemBlock  *LdAllocMemBlock(uintptr_t addr, int prot, size_t size, size_t align);
void         LdFreeMemBlock(LdMemBlock *block);
int          LdMemMapperAddBlock(LdMemMapper *mmapper, LdMemBlock *block);
void         LdMemMapperFlushCaches(LdMemMapper *mmapper);

#endif