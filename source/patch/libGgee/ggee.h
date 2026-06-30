#ifndef SOLOADED_PATCH_LIBGGEE_H
#define SOLOADED_PATCH_LIBGGEE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_linker/so_linker.h>

int soloader_patch_libGgee(so_module *mod);

#ifdef __cplusplus
};
#endif

#endif