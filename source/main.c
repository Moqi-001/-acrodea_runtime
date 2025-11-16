#include "utils/init.h"
#include "utils/glutil.h"
#include "utils/logger.h"
#include <psp2/kernel/threadmgr.h>

#include <falso_jni/FalsoJNI.h>
#include <so_util/so_util.h>
#include "reimpl/controls.h"
#include <stdio.h>
int _newlib_heap_size_user = 256 * 1024 * 1024;

#ifdef USE_SCELIBC_IO
int sceLibcHeapSize = 4 * 1024 * 1024;
#endif
#define SCREEN_W 960
#define SCREEN_H 544
//#define SCREEN_H 640
so_module so_mod;

#define LOAD_ADDRESS 0x98000000

jobjectArray JobjectArray;
int StartOffset;
char* dspUserID;

int main_thread(SceSize args, void *argp)
{

	int (* NativeFrame)(void) = (void *)so_symbol(&so_mod, "Java_com_Nexon_DnFSwordR_NativeClass_OnNativeFrame");
    
    int fpsLock=33;
    uint32_t last_render_time = sceKernelGetProcessTimeLow();
        uint32_t delta = (1000000 / (fpsLock+1));

        while (1) {
            //controls_poll();
            //NativeFrame();

            while (sceKernelGetProcessTimeLow() - last_render_time < delta) {
                sched_yield();
            }

            last_render_time = sceKernelGetProcessTimeLow();

			 gl_swap();
        }
    debugPrintf("RenderUpdate over \n");
	return 0;
}

int (*marketlaunch)(void*, int, int,int,int,int,int,int,int,int,int,int,int,int,int);
int (*launch)(void*, int, int,int,int,int,int,int,char,int,int,int);

int main() {
    soloader_init_all();

    int (* JNI_OnLoad)(void *jvm,int reserved) = (void *)so_symbol(&so_mod, "JNI_OnLoad");
    JNI_OnLoad(&jvm,0);
    debugPrintf("JNI_OnLoad over \n");

    void* (* Get)(void *) = (void *) so_symbol(&so_mod, "_ZN14RuntimeContext3GetEv");
    void * RuntimeContextInstance=Get(NULL);
    debugPrintf("RuntimeContextInstance  %p \n",RuntimeContextInstance);

    int (* threadLoop)(void *) = (void *)so_symbol(&so_mod, "_ZN13RuntimeThread10threadLoopEv");
    int addr=*((int*)(RuntimeContextInstance+208));
    void* RuntimeThreadInstance=(void*)(addr);
    debugPrintf("RuntimeThreadInstance  %p \n",RuntimeThreadInstance);

    //uintptr_t a1=*((uintptr_t*)(RuntimeThreadInstance)+9);
    // void* a1=RuntimeContextInstance;
    // debugPrintf("a1(%u)",a1);
    // debugPrintf("a1p(%p)",(void*)a1);
    // void* a22=a1+1144;
    // debugPrintf("a22(%u)",a22);
    // debugPrintf("a22s(%s)",(char*)a22);
    // void* a33=a1+148;
    // debugPrintf("a33(%u)",a33);
    // debugPrintf("a33s(%s)",(char*)a33);
    // void* a44=a1+164;
    // debugPrintf("a44(%u)",a44);
    // debugPrintf("a44s(%s)",(char*)a44);

    //int value = threadLoop(RuntimeThreadInstance);
    //debugPrintf("threadLoop over %d\n",value);

    marketlaunch=(void*)(LOAD_ADDRESS+0x416CD);
    launch=(void*)(LOAD_ADDRESS+0x413F9);

    debugPrintf("marketlaunch start %p\n",marketlaunch);
    char* appId="com.acrodea.gamecenter";
    int SDK_INT=10;
    int Surface=-1;
    char* a5="/runtime/";
    char* a6="";
    char* a7="com.acrodea.gamecenter";
    char* a8="com.acrodea.gamecenter";
    char* a9="./com.acrodea.gamecenter/app.exe";
    char* a10="./RuntimeApps";
    char* a11="./com.acrodea.gamecenter/res";
    char* a12="./.tmp";
    int a13=1;
    char* a14="from_manager:log_print";
    char* a15[1]={"boot_from=com.acrodea.gamecenter"};
    marketlaunch(&jni,appId,SDK_INT,Surface,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15);
    debugPrintf("marketlaunch over %p\n",marketlaunch);

    //launch(&jni,appId,SDK_INT,Surface,a5,a6,a7,a8,a13,a14,a15,0);
    //debugPrintf("marketlaunch over %p\n",marketlaunch);
    
    gl_init();
    // int (* kdMain)(void *,void *) = (void *)so_symbol(&so_mod, "kdMain");
    // value= kdMain(NULL,NULL);
    // debugPrintf("kdMain over %d \n",value);

    eglSwapInterval(0, 2);

    // ... do some initialization
    return 0;

    //SceUID thid = sceKernelCreateThread("main_thread", (SceKernelThreadEntry)main_thread, 0x40,2* 128 * 1024, 0, 0, NULL);
	//sceKernelStartThread(thid, 0, NULL);
	return sceKernelExitDeleteThread(0);
}
