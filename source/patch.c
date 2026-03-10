// #include <string.h>
// #include <stdlib.h>
#include <stdio.h>

#include <kubridge.h>
#include <so_util/so_util.h>
#include "config.h"

#include "reimpl/libGgee/ggee.h"
#include "reimpl/libKD/kd.h"
#include "reimpl/libLD/ld.h"
#include "utils/logger.h"
#include "reimpl/io.h"

#define _DWORD uintptr_t
#define _WORD uint16_t
extern so_module so_mod;

struct stru_
{
    char* st_name;
}stru_10100;


LdModule *bootstrap_mod                                      = NULL;
int (*bootstrap_start)(int argc, char *argv[], char *envp[]) = NULL;

int sub_52860(int a1)
{
    debugPrintf("sub_52860(%d)",a1);
    return 0;
}

int sub_51780()
{
    debugPrintf("sub_51780()");
    return 0;
}

void kdFree(void* a)
{
    free(a);
}

void * kdMalloc(size_t a1,int i,int v)
{
    int v3; // r3

  v3 = 0;
  if ( a1 )
    return malloc(a1);
  return (void *)v3;
}

char* (*ggeeLibGetRoot)();
char* (*ggeeAvatarGetRoot)();
int (*embSigCheckerPostLoadHook)(_DWORD *);
//int (*sub_4DF34)(int);
int (*sub_104114)(int);
//int (*sub_104CC0)(int,int,FILE *,int,char);

int ggeeLibExpandFiles(_DWORD  *compressed_data,uint32_t compressed_size,void    **expanded_data , uint32_t* expanded_size,char *dstpath) {
    int      res             = 1;
   
l_debug("ggeeLibExpandFiles : %u   %u   %u\n",compressed_data[0],compressed_data[1], compressed_size);
    if (!ggeeLibExpand(compressed_data, compressed_size, expanded_data, expanded_size)) {
        l_debug("ggeeLibExpand failed!\n");
        goto FAILED;
    }

    FILE    *handle = fopen(dstpath, "wb");
    if (!handle) {
        l_debug("fopen dstpath failed: %s\n", dstpath);
        goto FAILED;
    }
    fwrite(*expanded_data, 1, *expanded_size, handle);
        l_debug("fopen : %u\n", *expanded_size);
    
END:
    if (handle)
        fclose(handle);
    // if (expanded_data)
    //     free(expanded_data);
    return res;

FAILED:
    res = 0;
    goto END;
}
int inde=0;
int sub_4DF34(int a1)
{
   _DWORD *v2; // r0
  int v3; // r3
  int v4; // r0
  signed int v5; // r2
  uint32_t v7; // [sp+0h] [bp-10h] BYREF
  void *v8; // [sp+4h] [bp-Ch] BYREF

  v8 = 0;
  v7 = 0;
  v2 = *(_DWORD **)(a1 + 40);
  _DWORD s=*(_DWORD *)(a1 + 44);
  debugPrintf("sub_4DF34(%u,%u)",*v2,s);
  if ( *v2 == 336863541 || (v3 = 1, *v2 == 573965945) )
  {
  debugPrintf("sub_4DF34 strart");
if(inde<4)
{
  debugPrintf("sub_4DF34 strart1  %d",inde);

    v4 = ggeeLibExpand(v2, *(_DWORD *)(a1 + 44), &v8, &v7);
   //inde++;
}
else 
{
  debugPrintf("sub_4DF34 strart2");

   v4 = ggeeLibExpandFiles(v2, *(_DWORD *)(a1 + 44), &v8, &v7,"ux0:data/Acrodea/Game.so");
   inde++;
}

    v3 = 0;
    if ( v4 )
    {
      kdFree(*(_DWORD *)(a1 + 40));
      v5 = v7;
      *(_DWORD *)(a1 + 40) = v8;
      *(_DWORD *)(a1 + 44) = v5;
      return 1;
    }
  }
  return v3;
}

int sub_86048(int a1, int a2, char *a3, int a4, _DWORD *a5)
{
  debugPrintf("sub_86048 a1:%p",a1);
  debugPrintf("sub_86048 a2:%s",*((char**)a2));
  debugPrintf("sub_86048 a3:%p",a3);
  debugPrintf("sub_86048 a3:%s",a3);
return 0;
}

int sub_4DF80(_DWORD *a1, int a2, int a3)
{
    int v6; // r1
  int v7; // r2
  void *v8; // r0
  int v9; // r3
  int v10; // r6
  const char *v11; // r4
  char *Root; // r6
  char *v13; // r0
  size_t v14; // r0
  const char *v15; // r4
  char *v16; // r7
  char *v17; // r0
  size_t v18; // r0
  int v19; // r6
  int v20; // r2
  int v21; // r1
  int v22; // r3
  int v23; // r0
  signed int v25; // [sp+Ch] [bp-34h]
  int v26; // [sp+10h] [bp-30h] BYREF
  int v27; // [sp+14h] [bp-2Ch]
  int v28; // [sp+18h] [bp-28h]
  int v29; // [sp+1Ch] [bp-24h]
  _DWORD *v30; // [sp+20h] [bp-20h]
  int v31; // [sp+24h] [bp-1Ch]

  if ( !a2 || !a1 )
    return -1;
  debugPrintf("sub_4DF80(%d)",v25);
  (*(void (__fastcall **)(int, _DWORD, _DWORD, _DWORD, int))(a3 + 12))(a2, *(_DWORD *)(a3 + 12), 0, 0, 2);
  v25 = (*(int (__fastcall **)(int))(a3 + 8))(a2);
  (*(void (__fastcall **)(int, _DWORD, _DWORD, _DWORD, _DWORD))(a3 + 12))(a2, *(_DWORD *)(a3 + 12), 0, 0, 0);

  v8 = kdMalloc(v25, v6, v7);
  debugPrintf("v25(%d)",v25);
  a1[10] = v8;
  a1[11] = v25;
  if ( !v8 )
  {
    ldSetErrorCode(a1[5], 235077633, a1[1]);
LABEL_7:
    (*(void (__fastcall **)(int))(a3 + 16))(a2);
    return -1;
  }
      debugPrintf("555");
  if ( (*(int (__fastcall **)(void *, int, signed int, int))(a3 + 4))(v8, 1, v25, a2) < v25 )
  {
    ldSetErrorCode(a1[5], 235078400, a1[1]);
    kdFree(a1[10]);
    a1[10] = 0;
    goto LABEL_7;
  }
      debugPrintf("555444");

  (*(void (__fastcall **)(int))(a3 + 16))(a2);
  v9 = a1[5];
      debugPrintf("555444333");
_DWORD p= *(_DWORD *)(v9 + 20) ;
  if (p)
  {
    v26 = a1[5];
    v28 = a1[10];
    v27 = a1[2];
    v10 = a1[11];
    v30 = a1 + 12;
    v29 = v10;
    v31 = *(_DWORD *)(v9 + 24);
      debugPrintf("5554443332222(%p)",p);
      debugPrintf("5554443332222 a1(%p)",a1[5]+16);
      debugPrintf("5554443332222 a2(%s)",a1[1]);
      debugPrintf("5554443332222 a3(%s)",a1[2]);

    if ( (*(int (**)(void*))(v9 + 20))(a1+1) )
     {
        debugPrintf("rrrrffff(%p)",p);
          kdFree(a1[10]);
       a1[10] = 0;
       return -1;
     }
        debugPrintf("trrrrr(%p)",p);

  }
    debugPrintf("346666");

  if ( !sub_4DF34((int)a1) )
  {
    debugPrintf("346666777");

    ldSetErrorCode(a1[5], 235078656, a1[1]);
    kdFree(a1[10]);
    a1[10] = 0;
    return -1;
  }
    debugPrintf("345");

  v11 = (const char *)a1[2];
  if ( !strncmp(v11, "/lib/", 5u) && !strchr(v11 + 5, 47) )
    goto LABEL_30;
  Root = ggeeLibGetRoot();
  v13 = ggeeLibGetRoot();
  v14 = strlen(v13);
    debugPrintf("34");

  if ( !strncmp(v11, Root, v14)
    || (v15 = (const char *)a1[2],
        v16 = ggeeAvatarGetRoot(),
        v17 = ggeeAvatarGetRoot(),
        v18 = strlen(v17),
        !strncmp(v15, v16, v18)) )
  {
LABEL_30:
    v26 = a1[5];
    v28 = a1[10];
    v27 = a1[2];
    v19 = a1[11];
    v30 = a1 + 12;
    v29 = v19;
    v31 = *(_DWORD *)(v26 + 24);
    debugPrintf("embSigCheckerPostLoadHook");
    if ( embSigCheckerPostLoadHook(&v26) )
    {
LABEL_25:
      kdFree(a1[10]);
      a1[10] = 0;
    debugPrintf("error--");

      return -1;
    }
  }
  //stru_10100.st_name="tt";
  v20 = a1[10];
  v21 = a1[12];
  debugPrintf("a10(%p),a12(%u)",v20,v21);
  v22 = v20 + v21;
  a1[17] = v20 + v21;
//   if ( *(_DWORD *)(v20 + v21) != 1179403647
//     || (Elf32_Sym *)(*(_DWORD *)(v22 + 4) & 0xFFFFFF) != (Elf32_Sym *)((char *)&stru_10100.st_name + 1)
//     || (_WORD)(*(_WORD *)(v22 + 16) - 2) > 1u
//     || *(_WORD *)(v22 + 18) != 40
//     || *(_DWORD *)(v22 + 20) != 1
//     || (v23 = *(_DWORD *)(v22 + 28)) == 0
//     || !*(_DWORD *)(v22 + 32) )
//   {
//     ldSetErrorCode(a1[5], 235078912, a1[1]);
//     goto LABEL_25;
//   }
 // debugPrintf("st_name(%s)",*(_DWORD *)(v22 + 4) & 0xFFFFFF);

  if ( *(_DWORD *)(v20 + v21) != 1179403647)
  {
    ldSetErrorCode(a1[5], 1, a1[1]);
     goto LABEL_25;
  }
  if ( (_WORD)(*(_WORD *)(v22 + 16) - 2) > 1u)
  {
    ldSetErrorCode(a1[5], 2, a1[1]);
    goto LABEL_25;
  }
  if ( *(_WORD *)(v22 + 18) != 40)
  {
    ldSetErrorCode(a1[5], 3, a1[1]);
    goto LABEL_25;
  }
  if ( *(_DWORD *)(v22 + 20) != 1)
  {
    ldSetErrorCode(a1[5], 4, a1[1]);
    goto LABEL_25;
  }
  v23 = *(_DWORD *)(v22 + 28);
  if (v23 == 0)
  {
    ldSetErrorCode(a1[5], 5, a1[1]);
    goto LABEL_25;
  }
  if (!*(_DWORD *)(v22 + 32) )
  {
    ldSetErrorCode(a1[5], 6, a1[1]);
    goto LABEL_25;
  }

  a1[21] = v20 + v23 + v21;
  return 0;
}

char *__fastcall sub_101154(int a1)
{
  _DWORD *v1; // r2
  char *result; // r0
    l_debug("sub_101154(%p)", a1);

  if ( !*(_DWORD *)a1 )
  {
    l_debug("sub_101154(%p) 1", a1);//*a1==0
    return "";
  }
  v1 = *(_DWORD **)(*(_DWORD *)a1 + 16);
  if ( !v1 )
  {
    l_debug("sub_101154(%p) 2", a1);

    return "";
  }
  while ( 1 )
  {
    result = (char *)v1[3];
    if ( result )
    {
      if ( (uint8_t)((*v1 & 7) - 2) <= 1u )
        break;
    }
    v1 = (_DWORD *)v1[6];
    if ( !v1 )
    {
    l_debug("sub_101154(%p) 3", a1);

      return "";
    }
  }
  l_debug("sub_101154(%p) 4", a1);
  return result;
}

int __fastcall sub_100BB0(_DWORD *a1)
{
  l_debug("sub_100BB0(%u) --------", *a1);//*a1==0

  return -(*a1 == 0);
}

int __fastcall sub_106DC0(int a1)
{
  l_debug("sub_106DC0(%d) -----",*(_DWORD *)(a1 + 12) == *(_DWORD *)(a1 + 16));//1
  return *(_DWORD *)(a1 + 12) == *(_DWORD *)(a1 + 16);
}

int __fastcall sub_10EF4C(int a1, int **a2, _DWORD *a3)
{
  int *v3; // r3
  int v5; // r1
  v3 = *a2;
  if ( *a2 )
  {
    v5 = *v3;
      l_debug("sub_10EF4C(0)");//0

    if ( *(int8_t *)(*v3 + 1) != 1 )
    {
      l_debug("sub_10EF4C(1)");
    }
  }
      l_debug("sub_10EF4C(2)");//2
      *(_DWORD *)(a1 + 12) = 0;
  *(_DWORD *)(a1 + 16) = 0;
  return 0;
}

void __fastcall sub_106C5C(_DWORD *a1, char *a2, char *a3)
{
   unsigned int v6; // r3
  unsigned int v7; // r8
  _DWORD *v11; // r0
  _DWORD *v12; // r0
  _DWORD *v13; // r10
    if ( a2 > a3 )
    {
      l_debug("sub_106C5C(1)");

    }
      l_debug("sub_106C5C(2)");

  v6 = (a3 - a2) >> 3;
  v7 = v6;
  if ( v6 <= 1 )
  {
      l_debug("sub_106C5C(3)");
   v12 = (_DWORD *)a1[3];
    v13 = a1 + 1;
    if ( v12 != a1 + 1 )
    {
      //free(v12);
 l_debug("sub_106C5C(5)");
    }
    if ( a2 != a3 )
    {
      l_debug("sub_106C5C(6)");
      a1[1] = *(_DWORD *)a2;
      a1[2] = *((_DWORD *)a2 + 1);
    }
    a1[3] = v13;
    a1[4] = &v13[2 * v7];
      l_debug("sub_106C5C3(%p)",v13);
      l_debug("sub_106C5C4(%p)",a1[4]);

  }
  else 
  {
      l_debug("sub_106C5C(xxx)");
   
  }
  
}

int __fastcall sub_106D4C(int a1, int a2, int a3, char a4)
{
   *(_DWORD *)(a1 + 12) = 0;
  *(_DWORD *)(a1 + 16) = 0;
      l_debug("sub_106D4C2(%p)",a2);//0
      l_debug("sub_106D4C3(%p)",a3);//0
   return 0;
}

int __fastcall sub_106F48(int a1, uint8_t *a2, int a3, int *a4)
{
  unsigned int v7; // r0
  v7 = *a2 - 14;
  if ( v7 > 0x2B )
    {
      l_debug("sub_106F48(%p)0",a2);

    }
      l_debug("sub_106F48(%p)1",a2);

}

int (*sub_104C9C)(int,int,void *,int,int,uint8_t);
int  sub_104CC0(int a1, int a2, FILE *stream, int a4, char a5)
{
  debugPrintf("1");
  fseek(stream, 0, 2);
  debugPrintf("2");

  //int v8 = ftell(stream);
  int v8 = ftell_soloader(stream);
  debugPrintf("3");

  fseek(stream, 0, 0);
  debugPrintf("4");

  debugPrintf("v8:%ld",v8);

 void * v10 = (void *)malloc(v8);
 size_t v12 = fread(v10, 1u, v8, stream);
  if ( v12 == v8 )
    {
      debugPrintf("sub_104C9C in");
      sub_104C9C(a1, a2, v10, v8, a4, (uint8_t)a5);
      return a1;
    }
      debugPrintf("sub_104C9C not in");

  return 0;
}

// int sub_104DF8(int a1, int a2, const char *a3, int a4, char a5)
// {
//   FILE *v9; // r0
//   sub_104114(a2);
//   debugPrintf("sub_104DF8 path: %s",a3);
//   char* new_a3=__kdMapFilename(a3,"rb");
//   debugPrintf("sub_104DF8 new_path: %s",new_a3);

//   v9 = fopen(new_a3, "rb");

//   debugPrintf("sub_104DF8 v9:%x",v9);

//   sub_104CC0(a1, a2, v9, a4, a5);

//   return a1;
// }

int sub_B26B8(int result, int a2, int a3)
{
  debugPrintf("sub_B26B8 a42:%p  a43:%p",**(_DWORD **)(result + 68) + 68,result + 68);
  debugPrintf("sub_B26B8 a2:%p  a3:%u",a2,a3);
   return result;
}

int bootstrap_start_soloader(int argc, char *argv[], char *envp[]) {
    l_debug("bootstrap_start(%d,%p,%p)", argc, argv, envp);
    for (size_t i = 0; i < argc && argv[i]; i++) {
        l_debug("bootstrap_start: argv[%d]:(%s)", i, argv[i]);
    }

    // launch 命令
    char *launch_argv[] = {
        // 主参数
        "com.acrodea.vividruntime.launcher",  // +0 程序名？
        "launch",                             // +1 mode: 启动模式："launch", "marketlaunch", "rorequest", "install", "roinstall", "remove", "verify"
        "/native/RuntimeApps",                // +2 RuntimeApps 路径
        "1760",                               // +3 app_id (会将RuntimeApps和这id拼接，鬼泣的id为1760)
        "debug",                              // +4 params: "debug", "reschk"(resource check)
        // 额外参数传入子函数启动
        "argv5",  // +5
        "argv6",  // +6
        "argv7",  // +7
        "argv8",  // +8
        "argv9",  // +9
    };

    char *marketlaunch_argv[] = {
        "/native/com.acrodea.vividruntime.launcher",
        "marketlaunch",
        "/native/RuntimeApps/",
        "1760",
        "/lib/com.acrodea.gamecenter/app.exe",
        "/native/RuntimeApps",
        "/native/com.acrodea.gamecenter/res",
        "/native/.tmp",
        "from_manager:log_print",
    };
    
    //hook_addr((uintptr_t)(0x106F48+1+0x99000000), (uintptr_t)&sub_106F48);

    //hook_addr((uintptr_t)(0x101154+1+0x99000000), (uintptr_t)&sub_101154);
    // hook_addr((uintptr_t)(0x105D58+1+0x99000000), (uintptr_t)&sub_105D58);
    // hook_addr((uintptr_t)(0x10F07C+1+0x99000000), (uintptr_t)&sub_10F07C);
    //hook_addr((uintptr_t)(0x52860+1+0x99000000), (uintptr_t)&sub_52860);
    hook_addr((uintptr_t)(0x51780+1+0x99000000), (uintptr_t)&sub_51780);    
    //hook_addr((uintptr_t)(0xB26B8+1+0x99000000), (uintptr_t)&sub_B26B8);    
    //hook_addr((uintptr_t)(0x104CC0+1+0x99000000), (uintptr_t)&sub_104CC0);  
   
    //hook_addr((uintptr_t)(0x86048+1+0x99000000), (uintptr_t)&sub_86048);

    //hook_addr((uintptr_t)(0x104DF8+1+0x99000000), (uintptr_t)&sub_104DF8);    

    //sub_104114= (void *)(0x99000000 + 0x104114+1);
    //sub_104CC0= (void *)(0x99000000 + 0x104CC0+1);
    sub_104C9C= (void *)(0x99000000 + 0x104C9C+1);
    //int ret = bootstrap_start(sizeof(marketlaunch_argv) / sizeof(marketlaunch_argv[0]), marketlaunch_argv, envp);
    int ret = bootstrap_start(argc, argv, envp);
    l_debug("bootstrap_start_soloader return %p", ret);
    return ret;
}

int (*HttpResponse)(void *, int, int, int, void *, void *, void *);

void HttpResponse_soloader(void *a1, int a2, int a3, int a4, void *a5, void *a6, void *a7)
{
    l_debug("HttpResponse(%p)", a7);
    int ret = HttpResponse(a1,a2,a3,a4,a5,a6,a7);
    l_debug("HttpResponse return %d", ret);

}

int (*installed)(void *);

void Installed(void *a1)
{
    l_debug("Installed(%p)", a1);
    int ret = installed(a1);
    l_debug("Installed return %p", ret);
}

int (*install)(void* a1,int a2,char* a3,char* a4,char* a5,char* a6,char* a7,char* a8);

void Install(void* a1,int a2,char* a3,char* a4,char* a5,char* a6,char* a7,char* a8)
{
    l_debug("Install(%p)", a1);
    int ret = install(a1,a2,a3,a4,a5,a6,a7,a8);
    l_debug("Install return %p", ret);
}

int (*focusChanged)(void *, int, int);
void FocusChanged(int jni, int value)
{
    l_debug("FocusChanged(%d)", value);
    int ret = focusChanged(jni,0,value);
    l_debug("FocusChanged return %p", ret);
}

void SetRuntimeContext(int index,int value)
{
    char *(*Get)(void *)         = (void *)so_symbol(&so_mod, "_ZN14RuntimeContext3GetEv");
    char *RuntimeContextInstance = Get(NULL);
    char *F=RuntimeContextInstance+index;
    debugPrintf("SetRuntimeContext form(%d)",*F);
    *(F)=value;
    debugPrintf("SetRuntimeContext to(%d)",*F);
}

void plugin_gles_procaddress_terminate() {
    debugPrintf("plugin_gles_procaddress_terminate()");
}

int *ldOpenModule(int a1, char *a2, int a3)
{
    debugPrintf("ldOpenModule(%p,%s,%p)",a1,a2,a3);
    char fopen_path_real[1024] = {0};
    char *s = "/native/RuntimeApps/1760/";
    int len = strlen(s);
    if (strlen(a2) == len)
    {
        sceClibSnprintf(fopen_path_real, sizeof(fopen_path_real), "%s", "/native/RuntimeApps/1760/DevilMayCry4.exe");
    }
    else
    {
        sceClibSnprintf(fopen_path_real, sizeof(fopen_path_real), "%s", a2);
    }

  int *v6; // r0
  int *v7; // r4
  int (*kdThreadMutexLock)(pthread_mutex_t *);
  kdThreadMutexLock = (void *)so_symbol(&so_mod, "kdThreadMutexLock");
  int (*kdThreadMutexUnlock)(pthread_mutex_t *);
  kdThreadMutexUnlock = (void *)so_symbol(&so_mod, "kdThreadMutexUnlock");
  int (*ldOpenModule_)(_DWORD *, char *, int, _DWORD *, int, int);
  ldOpenModule_ = (void *)so_symbol(&so_mod, "ldOpenModule_");
  int (*ldModuleRelocate)(int *);
  ldModuleRelocate = (void *)so_symbol(&so_mod, "ldModuleRelocate");
  int (*ldModuleRelease_nolock)(int *);
  ldModuleRelease_nolock = (void *)so_symbol(&so_mod, "ldModuleRelease_nolock");
  void (*kdCacheFlushACR)(int, char *, int);
  kdCacheFlushACR = (void *)so_symbol(&so_mod, "kdCacheFlushACR");
  void (*ldModuleInitialize)(_DWORD *);
  ldModuleInitialize = (void *)so_symbol(&so_mod, "ldModuleInitialize");

  kdThreadMutexLock(*(pthread_mutex_t **)(a1 + 12));
  v6 = (int *)ldOpenModule_((_DWORD **)a1, fopen_path_real, a3, 0, 0, 0);
  v7 = v6;
  if (!v6 || v6[4] == 2)
  {
    debugPrintf("ldOpenModule 1(%p)",v6);
    kdThreadMutexUnlock(*(pthread_mutex_t **)(a1 + 12));
  }
  else if (ldModuleRelocate(v6))
  {
    debugPrintf("ldOpenModule 2(%p,%s,%p)",a1,a2,a3);

    ldModuleRelease_nolock((int)v7);
    kdThreadMutexUnlock(*(pthread_mutex_t **)(a1 + 12));
    return 0;
  }
  else
  {
    kdCacheFlushACR(3, v7[10], v7[11]);
    kdThreadMutexUnlock(*(pthread_mutex_t **)(a1 + 12));
    ldModuleInitialize(v7);
  }
  debugPrintf("ldOpenModule over(%p,%s,%p)",a1,a2,a3);
  return v7;
}

long kdFtell(int a1)
{
  debugPrintf("kdFtell(%p)",a1);

  return ftell_soloader(*(FILE **)a1);
}

void so_patch(void) {
    // Sample hook
    hook_addr((uintptr_t)so_symbol(&so_mod, "kdFtell"), (uintptr_t)&kdFtell);

    // hook_addr((uintptr_t)so_symbol(&so_mod, "kdExit"), (uintptr_t)&kdExit);
    hook_addr((uintptr_t)so_symbol(&so_mod, "__kdMapFilename"), (uintptr_t)&__kdMapFilename);
    // hook_addr((uintptr_t)so_symbol(&so_mod, "__kdFlushICache"), (uintptr_t)&__kdFlushICache);
    hook_addr((uintptr_t)so_symbol(&so_mod, "__kdFlushICache"), (uintptr_t)&__kdFlushICache);

    hook_addr((uintptr_t)so_symbol(&so_mod, "ldGetErrorMessage"), (uintptr_t)&ldGetErrorMessage);
    hook_addr((uintptr_t)so_symbol(&so_mod, "ldModuleGetEntryPoint"), (uintptr_t)&ldModuleGetEntryPoint);
    ldModuleGetSymbol    = (void *)so_symbol(&so_mod, "ldModuleGetSymbol");
    ldModuleLookupSymbol = (void *)so_symbol(&so_mod, "ldModuleLookupSymbol");
    hook_addr((uintptr_t)so_symbol(&so_mod, "ldModuleRelocate"), (uintptr_t)&ldModuleRelocate);
    hook_addr((uintptr_t)so_symbol(&so_mod, "ldModuleAllocAndLoadImage"), (uintptr_t)&ldModuleAllocAndLoadImage);
    hook_addr((uintptr_t)so_symbol(&so_mod, "ldModuleDestroyImage"), (uintptr_t)&ldModuleDestroyImage);

    hook_addr((uintptr_t)so_symbol(&so_mod, "ldModuleInitialize"), (uintptr_t)&ldModuleInitialize);
    hook_addr((uintptr_t)so_symbol(&so_mod, "ldModuleDeinitialize"), (uintptr_t)&ldModuleDeinitialize);
    
    // hook_addr((uintptr_t)so_symbol(&so_mod, "plugin_gles_procaddress_terminate"), (uintptr_t)&plugin_gles_procaddress_terminate);

    ggeeLibExpand = (void *)so_symbol(&so_mod, "ggeeLibExpand");
    ggeeLibGetRoot = (void *)so_symbol(&so_mod, "ggeeLibGetRoot");
    ggeeAvatarGetRoot = (void *)so_symbol(&so_mod, "ggeeAvatarGetRoot");
    //sub_4DF34= (void *)(LOAD_ADDRESS + 0x4DF34+1);

    embSigCheckerPostLoadHook= (void *)so_symbol(&so_mod, "embSigCheckerPostLoadHook");
    HttpResponse = (void *)(LOAD_ADDRESS + 0x40FB4+1);
    installed = (void *)(LOAD_ADDRESS + 0x412C4+1);
    install = (void *)(LOAD_ADDRESS + 0x41BF4+1);
    focusChanged = (void *)(LOAD_ADDRESS + 0x40269);

    //hook_addr((uintptr_t)(0x4DF80+1+LOAD_ADDRESS), (uintptr_t)&sub_4DF80);
    //hook_addr((uintptr_t)(0x4DF34+1+LOAD_ADDRESS), (uintptr_t)&sub_4DF34);
    
    //hook_addr((uintptr_t)so_symbol(&so_mod, "ldOpenModule"), (uintptr_t)&ldOpenModule);
    hook_addr((uintptr_t)so_symbol(&so_mod, "ldSetErrorCode"), (uintptr_t)&ldSetErrorCode);

    // ggeeLibExpandFile("ux0:data/Acrodea/lib4_5_0/libc.so", "ux0:data/Acrodea/libc.so");

    // int * UserConfirm = (int *) so_symbol(&so_mod, "_UserConfirm");
    // * UserConfirm = 1;

    // char ** GVfs = (char **) so_symbol(&so_mod, "GVfs");
    // *GVfs = malloc(60);
    // //**GVfs = 0;
    // memset(*GVfs,0,60);
    // debugPrintf("set GVfs over");
}
