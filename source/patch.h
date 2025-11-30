#ifndef SOLOADED_PATCH_H
#define SOLOADED_PATCH_H

#include "reimpl/libLD/ld.h"

extern LdModule *bootstrap_mod;
extern int (*bootstrap_start)(int argc, char *argv[], char *envp[]);
int bootstrap_start_soloader(int argc, char *argv[], char *envp[]);

#endif