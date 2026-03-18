#ifndef SOLOADER_LOADER_H
#define SOLOADER_LOADER_H

#include <so_util/so_util.h>

#ifdef __cplusplus
extern "C" {
#endif

void resolve_imports(so_module *mod);

void soloader_patch();

void soloader_init();

uintptr_t soloader_find_symbol_from_default_dynlib(const char *symbol);

#ifdef __cplusplus
};
#endif

#endif // SOLOADER_INIT_H
