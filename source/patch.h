#ifndef SOLOADED_PATCH_H
#define SOLOADED_PATCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_linker/so_linker.h>

int soloader_patch_runtime(so_module *mod);

#ifdef __cplusplus
};
#endif

#endif