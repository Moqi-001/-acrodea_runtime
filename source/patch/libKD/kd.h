#ifndef SOLOADED_PATCH_LIBKD_H
#define SOLOADED_PATCH_LIBKD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_linker/so_linker.h>

typedef void *KdFHandle;

typedef struct KdFile {
    FILE *fp;
    void *aasset;
} KdFile;

typedef struct FsEntry {

    int start_offset; // 24

    int file_size; // +32
} FsEntry; // +48

typedef struct FsFile {
    FsEntry *entry;   // +0
    KdFile  *file;    // +4
    int32_t  offset;   // +8
    int32_t  unk_12;  // +12
    int32_t  unk_16;  // +16
    int32_t  unk_20;  // +20
} FsFile;             // +24

int soloader_patch_libKD(so_module *mod);

#ifdef __cplusplus
};
#endif

#endif