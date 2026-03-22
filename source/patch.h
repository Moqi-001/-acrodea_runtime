#ifndef SOLOADED_PATCH_H
#define SOLOADED_PATCH_H

#include "reimpl/libLD/ld.h"

extern int (*bootstrap_start)(int argc, char *argv[], char *envp[]);
int bootstrap_start_soloader(int argc, char *argv[], char *envp[]);

int soloader_patch(so_module *mod);

#endif