#ifndef SOLOADED_PATCH_H
#define SOLOADED_PATCH_H

extern int (*bootstrap_start)(int argc, char *argv[], char *envp[]);
int bootstrap_start_soloader(int argc, char *argv[], char *envp[]);

#endif