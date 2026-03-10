#ifndef SOLOADED_LD_PATCH_H
#define SOLOADED_LD_PATCH_H

#include <so_util/so_util.h>

#include "ld.h"

extern LdModule *bootstrap_mod;
extern int (*bootstrap_start)(int argc, char *argv[], char *envp[]);

void ldModuleInitialize_patch(LdModule *mod);

int so_patch_ld(so_module *mod);

#endif