#ifndef SOLOADED_GGEE_H
#define SOLOADED_GGEE_H

#include <so_util/so_util.h>

int ggeeLibExpandFile(const char *srcpath, const char *dstpath);

int soloader_patch_ggee(so_module *mod);

#endif