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
#define CONFIG_FILE_PATH DATA_PATH "config.txt"

jobjectArray JobjectArray;
int StartOffset;
char* dspUserID;

void loadConfig() {
    char buffer[30];
    int value;
    StartOffset=0;
    FILE *config = fopen(CONFIG_FILE_PATH, "r");

    if (config) {
        while (EOF != fscanf(config, "%[^ ] %d\n", buffer, &value)) {
            if (strcmp("StartOffset", buffer) == 0) 
                 StartOffset = value;
          
        }
        fclose(config);
    }
}

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

void Login()
{
    int (* OnNativeNxLoginResult)(int,int,int,char *) = (void *)so_symbol(&so_mod, "Java_com_Nexon_DnFSwordR_NativeClass_OnNativeNxLoginResult");
    dspUserID="268435456";

    OnNativeNxLoginResult(&jni,0,1,dspUserID);
    // int (* OnNativeRepleRegist)() = (void *)so_symbol(&so_mod, "Java_com_Nexon_DnFSwordR_NativeClass_OnNativeRepleRegist");
    // OnNativeRepleRegist();
}

int main() {
    soloader_init_all();
    loadConfig();
    int (* JNI_OnLoad)(void *jvm,int reserved) = (void *)so_symbol(&so_mod, "JNI_OnLoad");
    JNI_OnLoad(&jvm,0);
    debugPrintf("JNI_OnLoad over \n");

    void* (* Get)() = (void *) so_symbol(&so_mod, "_ZN14RuntimeContext3GetEv");
    void * RuntimeContextInstance=Get();
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

    int value = threadLoop(RuntimeThreadInstance);
    debugPrintf("threadLoop over %d\n",value);

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
