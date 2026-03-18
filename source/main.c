#include <stdio.h>

#include <falso_jni/FalsoJNI.h>
#include <psp2/kernel/threadmgr.h>
#include <so_util/so_util.h>

#include "config.h"
#include "controls.h"
#include "init.h"
#include "utils/glutil.h"
#include "utils/logger.h"

#define RES_PATH "/com.acrodea.gamecenter/res"

int       _newlib_heap_size_user = MEMORY_NEWLIB_MB * 1024 * 1024;
so_module so_mod;

int controls_thread(SceSize args, void *argp) {
    while (1) {
        controls_poll();
        sceKernelDelayThread(10 * 1000);
    }

    sceKernelExitThread(0);
    return 0;
}

void (*navite_surfaceChanged)(JNIEnv *env, jobject thiz, jint width, jint height);
void (*navite_focusChanged)(JNIEnv *env, jobject thiz, jboolean hasFocus);

jint (*navite_launch)(JNIEnv *env, jobject thiz, jint sdkVersion, jobject surface, jstring bootstrapPath, jstring apkPath,
                      jstring rootDir, jstring appId, jstring params, jstring fromManager, jstring args, jstring serverEnvId);
jint (*navite_marketlaunch)(JNIEnv *env, jobject thiz, jint sdkVersion, jobject surface, jstring str1, jstring str2, jstring str3, jstring str4,
                            jstring str5, jstring str6, jstring str7, jstring str8, jint arg2, jstring str9, jobjectArray strArray);
void (*navite_ExtensionInit)(JNIEnv *env, jobject thiz, jobject obj);
void (*navite_MediaManagerInit)(JNIEnv *env, jobject thiz, jobject obj);

int RuntimeContextBase_Launch(void *this, int a2) {
    void *(*RuntimeContext_Get)() = (void *)so_symbol(&so_mod, "_ZN14RuntimeContext3GetEv");
    void *RuntimeContext_Instance = RuntimeContext_Get();
    l_debug("RuntimeContext_Instance: %p", RuntimeContext_Instance);

    int (*RuntimeThread_threadLoop)(void *) = (void *)so_symbol(&so_mod, "_ZN13RuntimeThread10threadLoopEv");
    void *RuntimeThread_Instance            = *(void **)((uintptr_t)RuntimeContext_Instance + 208);
    l_debug("RuntimeThread_Instance: %p", RuntimeThread_Instance);

    int ret = RuntimeThread_threadLoop(RuntimeThread_Instance);
    l_debug("RuntimeThread_threadLoop: %d", ret);
}

int main_thread(SceSize args, void *argp) {
    int ret = navite_launch(&jni,
                            NULL,
                            10,
                            NULL,
                            "/runtime/",
                            "/app/com.acrodea.vividruntime.launcher.apk",
                            "/RuntimeApps",
                            "1760",
                            NULL,
                            "from_manager:log_print",
                            NULL,
                            0);

    l_debug("navite_launch over: return %d\n", ret);

    sceKernelExitThread(0);
    return ret;
}

int main() {
    remove(LOG_PATH);
    soloader_init_all();
    
    int (*JNI_OnLoad)(void *jvm, int reserved) = (void *)so_symbol(&so_mod, "JNI_OnLoad");
    JNI_OnLoad(&jvm, 0);
    l_debug("JNI_OnLoad over \n");

    hook_addr((uintptr_t)so_symbol(&so_mod, "_ZN18RuntimeContextBase6LaunchEb"), (uintptr_t)&RuntimeContextBase_Launch);

    navite_marketlaunch   = (void *)(LOAD_ADDRESS + 0x416CD);
    navite_launch         = (void *)(LOAD_ADDRESS + 0x413F9);
    navite_surfaceChanged = (void *)(LOAD_ADDRESS + 0x40291);
    navite_focusChanged   = (void *)(LOAD_ADDRESS + 0x40269);

    navite_ExtensionInit    = (void *)(LOAD_ADDRESS + 0x41221);
    navite_MediaManagerInit = (void *)(LOAD_ADDRESS + 0x41285);

    navite_ExtensionInit(&jni, NULL, "");
    l_debug("navite_ExtensionInit over \n");

    navite_MediaManagerInit(&jni, NULL, NULL);
    l_debug("navite_MediaManagerInit over \n");

    navite_focusChanged(&jni, 0, 1);
    l_debug("navite_focusChanged over");

    navite_surfaceChanged(NULL, 0, SCREEN_W, SCREEN_H);
    l_debug("navite_surfaceChanged over %p\n", navite_surfaceChanged);

    SceUID controls_thid = sceKernelCreateThread("controls_thread", (SceKernelThreadEntry)controls_thread, 0x40, 128 * 1024, 0, 0, NULL);
    sceKernelStartThread(controls_thid, 0, NULL);

    SceUID main_thid = sceKernelCreateThread("main_thread", (SceKernelThreadEntry)main_thread, 0x40, 128 * 1024, 0, 0, NULL);
    sceKernelStartThread(main_thid, 0, NULL);
    sceKernelWaitThreadEnd(main_thid, NULL, NULL);

    return 0;
}
