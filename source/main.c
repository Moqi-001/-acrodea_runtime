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
            controls_poll();
            NativeFrame();

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
    gl_init();
    int (* kdMain)(void *,void *) = (void *)so_symbol(&so_mod, "kdMain");
    kdMain(NULL,NULL);
    debugPrintf("JNI_OnLoad over \n");
    // int (* OnNativeInitializeJNI)(void *) = (void *)so_symbol(&so_mod, "Java_com_Nexon_DnFSwordR_ItarosActivity_OnNativeInitializeJNI");
    // int (* NativePath)(void *,int,char *) = (void *)so_symbol(&so_mod, "Java_com_Nexon_DnFSwordR_NativeClass_OnNativePath");
    // int (* NativeInitialize)(void *) = (void *)so_symbol(&so_mod, "Java_com_Nexon_DnFSwordR_NativeClass_OnNativeInitialize");
    // int (* NativeSize)(int,int,int,int) = (void *)so_symbol(&so_mod, "Java_com_Nexon_DnFSwordR_NativeClass_OnNativeSize");
    // int (* OnNativeCreateRenderer)() = (void *)so_symbol(&so_mod, "Java_com_Nexon_DnFSwordR_NativeClass_OnNativeCreateRenderer");
   
	// int (* VFSInitialize)(void *,int,int,int,int,int,int) = (void *)so_symbol(&so_mod, "Java_com_Nexon_DnFSwordR_NativeClass_OnVFSInitialize");

    // OnNativeInitializeJNI(&jni);
    // debugPrintf("OnNativeInitializeJNI over \n");

    // NativePath(&jni,0,"ux0:data/DnfSword");
    // debugPrintf("NativePath over \n");
    
    // VFSInitialize(&jni,0,1,0,0,0,52108710);
    // debugPrintf("VFSInitialize over \n");

    // NativeInitialize(&jni);
    // debugPrintf("NativeInitialize over \n");

    // int (* setMiliSecond)(int) = (void *)so_symbol(&so_mod, "setMiliSecond");
    // setMiliSecond(31);
    // debugPrintf("setMiliSecond(31) over \n");

    eglSwapInterval(0, 2);

    NativeSize(0,0,SCREEN_W,SCREEN_H);

    debugPrintf("NativeSize over \n");
    OnNativeCreateRenderer();
    debugPrintf("OnNativeCreateRenderer over \n");

    // ... do some initialization

    SceUID thid = sceKernelCreateThread("main_thread", (SceKernelThreadEntry)main_thread, 0x40,2* 128 * 1024, 0, 0, NULL);
	sceKernelStartThread(thid, 0, NULL);
	return sceKernelExitDeleteThread(0);
}
