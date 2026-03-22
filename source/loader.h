#ifndef SOLOADER_LOADER_H
#define SOLOADER_LOADER_H

#include <so_util/so_util.h>

#ifdef __cplusplus
extern "C" {
#endif

int       soloader_resolve_imports(so_module *mod);
uintptr_t soloader_find_symbol_from_default_dynlib(const char *symbol);

int  soloader_init();
int  soloader_run();
void soloader_term();

#ifdef __cplusplus
};
#endif

#endif  // SOLOADER_INIT_H
