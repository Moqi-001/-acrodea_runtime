/*
 * Copyright (C) 2023 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

/**
 * @file  patch.c
 * @brief Patching some of the .so internal functions or bridging them to native
 *        for better compatibility.
 */

#include <kubridge.h>
// #include <string.h>
// #include <stdlib.h>
#include <so_util/so_util.h>
#include "utils/logger.h"

extern so_module so_mod;

void MC_knlPrintk(char* name)
{
    debugPrintf(name);
}

int __addIOGroup()
{
    debugPrintf("__addIOGroup");
    return 0;
}

void so_patch(void) {
    // Sample hook
    // hook_addr((uintptr_t)so_symbol(&so_mod, "__addIOGroup"), (uintptr_t)&__addIOGroup);

    // int * UserConfirm = (int *) so_symbol(&so_mod, "_UserConfirm");
    // * UserConfirm = 1;
    
    // char ** GVfs = (char **) so_symbol(&so_mod, "GVfs");
    // *GVfs = malloc(60);
    // //**GVfs = 0;
    // memset(*GVfs,0,60);
    //debugPrintf("set GVfs over");
}
