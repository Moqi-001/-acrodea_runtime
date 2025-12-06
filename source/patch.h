#ifndef SOLOADED_PATCH_H
#define SOLOADED_PATCH_H

#include "reimpl/libLD/ld.h"

extern LdModule *bootstrap_mod;
extern int (*bootstrap_start)(int argc, char *argv[], char *envp[]);
int bootstrap_start_soloader(int argc, char *argv[], char *envp[]);
void HttpResponse_soloader(void *a1, int a2, int a3, int a4, void *a5, void *a6, void *a7);
#endif