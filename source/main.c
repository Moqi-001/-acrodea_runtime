#include <stdio.h>

#include <falso_jni/FalsoJNI.h>
#include <psp2/kernel/threadmgr.h>
#include <so_util/so_util.h>

#include "config.h"
#include "init.h"
#include "reimpl/controls.h"
#include "utils/glutil.h"
#include "utils/logger.h"
#include "patch.h"

#define RES_PATH "/com.acrodea.gamecenter/res"

#ifdef USE_SCELIBC_IO
int sceLibcHeapSize = 4 * 1024 * 1024;
#endif
int _newlib_heap_size_user = 256 * 1024 * 1024;
#define SCREEN_W 960
#define SCREEN_H 544
so_module so_mod;

jobjectArray JobjectArray;
int          StartOffset;
char        *dspUserID;

int controls_thread(SceSize args, void *argp) {
    
    while (1)
    {
       controls_poll();
    }
    
    return 0;
}

int (*marketlaunch)(void *, int, int, int, int, int, int, int, int, int, int, int, int, int, int);
int (*launch)(void *, int, int, int, int, int, int, int, char, int, int, int);
int (*surfaceChanged)(void *, int, int, int);
int (*extensionInit)(void *, int, int);
int (*mediaManagerInit)(void);

jobjectArray JobjectArray;

int Launch(void *this, int a2) {
    void *(*Get)(void *)         = (void *)so_symbol(&so_mod, "_ZN14RuntimeContext3GetEv");
    void *RuntimeContextInstance = Get(NULL);
    debugPrintf("RuntimeContextInstance  %p \n", RuntimeContextInstance);

    int (*threadLoop)(void *)   = (void *)so_symbol(&so_mod, "_ZN13RuntimeThread10threadLoopEv");
    int   addr                  = *((int *)(RuntimeContextInstance + 208));
    void *RuntimeThreadInstance = (void *)(addr);
    debugPrintf("RuntimeThreadInstance  %p \n", RuntimeThreadInstance);

    int value = threadLoop(RuntimeThreadInstance);
    debugPrintf("threadLoop over %d\n", value);
}

int main() {
    remove(LOG_PATH);
    soloader_init_all();
    int (*JNI_OnLoad)(void *jvm, int reserved) = (void *)so_symbol(&so_mod, "JNI_OnLoad");
    JNI_OnLoad(&jvm, 0);
    debugPrintf("JNI_OnLoad over \n");

    hook_addr((uintptr_t)so_symbol(&so_mod, "_ZN18RuntimeContextBase6LaunchEb"), (uintptr_t)&Launch);

    gl_init();
    eglSwapInterval(0, 2);

    marketlaunch = (void *)(LOAD_ADDRESS + 0x416CD);
    launch       = (void *)(LOAD_ADDRESS + 0x413F9);
    surfaceChanged = (void *)(LOAD_ADDRESS + 0x40291);
    extensionInit = (void *)(LOAD_ADDRESS + 0x41221);
    mediaManagerInit = (void *)(LOAD_ADDRESS + 0x41285);

    extensionInit(&jni,0,"");
    debugPrintf("extensionInit over \n");

    mediaManagerInit();
    debugPrintf("mediaManagerInit over \n");

    debugPrintf("marketlaunch start %p\n", marketlaunch);
    char *appId   = "com.acrodea.gamecenter";
    int   SDK_INT = 10;
    int   Surface = 0;
    char *a5      = "/runtime/";
    char *a6      = "/data/app/com.acrode.vividruntime.launcher-1.apk";
    //char *a6      = "/data/app/com.acrodea.vividruntime.launcher-1.apk";
    char *a7      = "/assets/";
    char *a8      = "com.acrodea.gamecenter";
    char *a9      = "/lib/com.acrodea.gamecenter/app.exe";
    char *a10     = "/RuntimeApps";
    char *a11     = "/com.acrodea.gamecenter/res";
    char *a12     = "/com.acrodea.gamecenter/.tmp";
    int   a13     = 0;
    //char *a14     = "from_manager:log_print";
    char *a14     = "";
    char *a15[]   = {"http://","http://"};
    //int   len     = sizeof(a15);
    int   len     = 2;
    
    JobjectArray  = NewObjectArray(&jni, len, "a15", NULL);
    for (size_t i = 0; i < len; i++) {
        SetObjectArrayElement(&jni, JobjectArray, i, a15[i]);
    }

    FocusChanged(&jni,1);
    debugPrintf("FocusChanged over\n");

    surfaceChanged(NULL,0,SCREEN_W,SCREEN_H);
    debugPrintf("surfaceChanged over %p\n", surfaceChanged);

    SceUID thid = sceKernelCreateThread("controls_thread", (SceKernelThreadEntry)controls_thread, 0x40,2* 128 * 1024, 0, 0, NULL);
    sceKernelStartThread(thid, 0, NULL);

    //marketlaunch(&jni, appId, SDK_INT, Surface, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, JobjectArray);
    //debugPrintf("marketlaunch over %p\n", marketlaunch);
    
    char* bw[0];
    launch(&jni,appId,15,0,
    "/runtime/",
    "/data/app/com.acrodea.vividruntime.launcher-1.apk",
    "/RuntimeApps", 
    "1760",
    a13,
    "",
    bw,
    0);
    debugPrintf("launch over %p\n",launch);

    // ... do some initialization
    return 0;

    return sceKernelExitDeleteThread(0);
}
