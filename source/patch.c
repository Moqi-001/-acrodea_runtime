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
#include <stdio.h>
#include <so_util/so_util.h>
#include "utils/logger.h"
#define _DWORD uintptr_t
extern so_module so_mod;

int ldGetErrorMessage(char *a1)
{
  char *s = a1 != 0 ? a1 + 64 : 0;
  debugPrintf("ErrorMessage:%s", s);
  return s;
}

int kdExit(int Context)
{
  debugPrintf("kdExit(%d)", Context);
  return 0;
}

int __kdFlushICache(char *a1, int a2)
{
  debugPrintf("__kdFlushICache(%p,%d)", a1, a2);
  kuKernelFlushCaches(a1, a2);
  return 0;
}

int (*EntryPoint)(int, int, _DWORD);

int start(int a1, int a2, _DWORD a3)
{
  debugPrintf("EntryPoint(%d,%p,%d)", a1, a2, a3);

  char **argv = (char **)a2;
  for (size_t i = 0; i < a1 && argv[i]; i++)
  {
    debugPrintf("EntryPoint argv[%d]:(%s)", i, argv[i]);
  }

  debugPrintf("EntryPoint start");
  int ret = EntryPoint(a1, a2, a3);
  debugPrintf("EntryPoint return %p", ret);
  return ret;
}

int ldModuleGetEntryPoint(_DWORD *a1)
{
  debugPrintf("ldModuleGetEntryPoint(%p)", a1);
  int v1; // r3
  int v2; // r3
  int addr = 0;
  if (a1 && (v1 = a1[17]) != 0 && (v2 = *(_DWORD *)(v1 + 24)) != 0)
  {
    addr = v2 + a1[10] + a1[12];
  }
  EntryPoint = (int (*)(int, int, _DWORD))addr;
  debugPrintf("ldModuleGetEntryPoint return(%p)", addr);
  return (int)&start;
}

int ldModuleInitialize(_DWORD *a1)
{
  int v3;             // r5
  int v4;             // r2
  int v5;             // r1
  int *v6;            // r5
  int v7;             // r7
  int i;              // r6
  int v9;             // r3
  int v10;            // r3
  int v11;            // r6
  void (**v12)(void); // r5
  void (*v13)(void);  // r7
  unsigned int v14;   // [sp+0h] [bp-20h]
  signed int v15;     // [sp+0h] [bp-20h]
  int v16;            // [sp+4h] [bp-1Ch]
  debugPrintf("ldModuleInitialize(%p)", a1);
  if (!a1)
    return -1;
  debugPrintf("ldModuleInitialize 22(%p)", a1[22]);

  if (a1[22])
  {
    v3 = 0;
    debugPrintf("ldModuleInitialize 4(%d)", a1[4]);

    if (a1[4] != 2)
    {
      debugPrintf("ldModuleInitialize 7(%d)", a1[7]);

      while (v3 < a1[7])
      {
        v4 = 4 * v3++;
        debugPrintf("ldModuleInitialize 8(%p)\n", a1[8]);
        ldModuleInitialize(*(_DWORD **)(v4 + a1[8]));
      }

      v5 = a1[22];
      v16 = *(_DWORD *)(v5 + 20) >> 3;
      v14 = 0;
      debugPrintf("ldModuleInitialize 10(%p)", a1[10]);
      debugPrintf("ldModuleInitialize 12(%d)", a1[12]);
      v6 = (int *)(*(_DWORD *)(v5 + 8) + a1[10] + a1[12]);
      debugPrintf("ldModuleInitialize v6(%p)", v6);
      debugPrintf("ldModuleInitialize v16(%d)", v16);

      v7 = 0;
      for (i = 0; i < v16; ++i)
      {
        v9 = *v6;
        if (*v6 == 25)
        {
          v7 = v6[1] + a1[10] + a1[12];
        }
        else if (v9 == 27)
        {
          v14 = v6[1];
        }
        else if (v9 == 12)
        {
          v10 = v6[1];
          if (v10)
          {
            debugPrintf("ldModuleInitialize v10(%d)", v10);
            int addr = v10 + a1[10] + a1[12];
            debugPrintf("ldModuleInitializev addr:(%p)", (void *)addr);
            ((void (*)(void))(addr))();
            debugPrintf("ldModuleInitializev v10 finish");
          }
        }
        v6 += 2;
      }
      if (v7)
      {
        v15 = v14 >> 2;
        v11 = 0;
        v12 = (void (**)(void))v7;
        while (v11 < v15)
        {
          debugPrintf("ldModuleInitialize v12(%p)", v12);
          v13 = *v12++;
          debugPrintf("ldModuleInitialize v13(%p)", v13);
          v13();
          debugPrintf("ldModuleInitialize v13 finish");
          ++v11;
        }
      }
      a1[4] = 2;
    }
  }
  debugPrintf("ldModuleInitializev over");

  return 0;
}

int (*ggeeLibExpand)(void *compressed_data, uint32_t compressed_size, void *expanded_data, uint32_t expanded_size);

int ggeeLibExpandFile(const char *srcpath, const char *dstpath)
{
  int res = 0;
  FILE *handle = NULL;
  void *compressed_data = NULL;
  uint32_t compressed_size;
  void *expanded_data = NULL;
  ;
  uint32_t expanded_size;

  handle = fopen(srcpath, "rb");
  if (!handle)
  {
    debugPrintf("fopen srcpath failed: %s\n", srcpath);
    goto FAILED;
  }

  fseek(handle, 0, SEEK_END);
  compressed_size = ftell(handle);
  fseek(handle, 0, SEEK_SET);

  compressed_data = malloc(compressed_size);
  if (!compressed_data)
  {
    debugPrintf("malloc compressed_data failed!\n");
    goto FAILED;
  }

  fread(compressed_data, 1, compressed_size, handle);
  fclose(handle);

  if (!ggeeLibExpand(compressed_data, compressed_size, &expanded_data, &expanded_size))
  {
    debugPrintf("ggeeLibExpand failed!\n");
    goto FAILED;
  }

  handle = fopen(dstpath, "wb");
  if (!handle)
  {
    debugPrintf("fopen dstpath failed: %s\n", srcpath);
    goto FAILED;
  }
  fwrite(expanded_data, 1, expanded_size, handle);

END:
  if (handle)
    fclose(handle);
  if (compressed_data)
    free(compressed_data);
  if (expanded_data)
    free(expanded_data);
  return res;

FAILED:
  res = -1;
  goto END;
}

void plugin_gles_procaddress_terminate()
{
  debugPrintf("plugin_gles_procaddress_terminate()");
}

void so_patch(void)
{
  // Sample hook
  //hook_addr((uintptr_t)so_symbol(&so_mod, "kdExit"), (uintptr_t)&kdExit);
  hook_addr((uintptr_t)so_symbol(&so_mod, "ldGetErrorMessage"), (uintptr_t)&ldGetErrorMessage);
  hook_addr((uintptr_t)so_symbol(&so_mod, "__kdFlushICache"), (uintptr_t)&__kdFlushICache);
  hook_addr((uintptr_t)so_symbol(&so_mod, "ldModuleInitialize"), (uintptr_t)&ldModuleInitialize);
  hook_addr((uintptr_t)so_symbol(&so_mod, "ldModuleGetEntryPoint"), (uintptr_t)&ldModuleGetEntryPoint);
  //hook_addr((uintptr_t)so_symbol(&so_mod, "plugin_gles_procaddress_terminate"), (uintptr_t)&plugin_gles_procaddress_terminate);

  ggeeLibExpand = (void *)so_symbol(&so_mod, "ggeeLibExpand");
  //ggeeLibExpandFile("ux0:data/Acrodea/lib4_5_0/libc.so", "ux0:data/Acrodea/libc.so");

  // int * UserConfirm = (int *) so_symbol(&so_mod, "_UserConfirm");
  // * UserConfirm = 1;

  // char ** GVfs = (char **) so_symbol(&so_mod, "GVfs");
  // *GVfs = malloc(60);
  // //**GVfs = 0;
  // memset(*GVfs,0,60);
  //debugPrintf("set GVfs over");
}
