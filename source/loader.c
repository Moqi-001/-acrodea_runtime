#include <string.h>

#include <falso_jni/FalsoJNI.h>
#include <psp2/kernel/threadmgr.h>
#include <so_util/so_util.h>
#include <vitaGL.h>

#include "config.h"
#include "controls.h"
#include "loader.h"
#include "patch.h"
#include "utils/dialog.h"
#include "utils/logger.h"
#include "utils/utils.h"

so_module so_mod;

void (*navite_surfaceChanged)(JNIEnv *env, jobject thiz, jint width, jint height);
void (*navite_focusChanged)(JNIEnv *env, jobject thiz, jboolean hasFocus);

jint (*navite_launch)(JNIEnv *env, jobject thiz, jint sdkInt, jobject surface, jstring bootstrapPath, jstring apkPath,
                      jstring rootDir, jstring appId, jstring params, jstring fromManager, jstring args, jstring serverEnvId);
jint (*navite_marketlaunch)(JNIEnv *env, jobject thiz, jint sdkInt, jobject surface, jstring bootstrapPath, jstring apkPath,
                            jstring rootDir, jstring appId, jstring marketExePath, jstring marketResDir, jstring marketTmpDir,
                            jstring params, jint fromManager, jstring str9, jobjectArray args);
void (*navite_ExtensionInit)(JNIEnv *env, jobject thiz, jobject obj);
void (*navite_MediaManagerInit)(JNIEnv *env, jobject thiz, jobject obj);

static int checkKubridge() {
    if (!module_loaded("kubridge")) {
        fatal_error("Error: kubridge.skprx is not installed.");
        return -1;
    }

    return 0;
}

static int checkOpenGL() {
    if (!file_exists("ur0:/data/libshacccg.suprx") && !file_exists("ur0:/data/external/libshacccg.suprx")) {
        fatal_error(
            "Error: libshacccg.suprx is not installed. "
            "Google \"ShaRKBR33D\" for quick installation.");
        return -1;
    }

    return 0;
}

static int checkDataFiles() {
    if (!file_exists(SO_PATH)) {
        fatal_error(
            "Looks like you haven't installed the data files for this "
            "port, or they are in an incorrect location. Please make "
            "sure that you have %s file exactly at that path.",
            SO_PATH);
        return -1;
    }

    return 0;
}

int RuntimeContextBase_Launch(void *this, int a2) {
    void *(*RuntimeContext_Get)() = (void *)so_find_symbol(&so_mod, "_ZN14RuntimeContext3GetEv");
    void *RuntimeContext_Instance = RuntimeContext_Get();
    l_debug("[LOADER] RuntimeContext_Instance: %p", RuntimeContext_Instance);

    int (*RuntimeThread_threadLoop)(void *) = (void *)so_find_symbol(&so_mod, "_ZN13RuntimeThread10threadLoopEv");
    void *RuntimeThread_Instance            = *(void **)((uintptr_t)RuntimeContext_Instance + 208);
    l_debug("[LOADER] RuntimeThread_Instance: %p", RuntimeThread_Instance);

    int ret = RuntimeThread_threadLoop(RuntimeThread_Instance);
    l_debug("[LOADER] RuntimeThread_threadLoop: %d", ret);
}

static int controls_thread(SceSize args, void *argp) {
    while (1) {
        controls_poll();
        sceKernelDelayThread(10 * 1000);
    }

    sceKernelExitThread(0);
    return 0;
}

static int main_thread(SceSize args, void *argp) {
    int ret = navite_launch(&jni,                                          // env
                            NULL,                                          // thizz
                            19,                                            // sdkInt
                            NULL,                                          // surface
                            "/runtime/",                                   // bootstrapPath
                            "/app/com.acrodea.vividruntime.launcher.apk",  // apkPath
                            "/RuntimeApps",                                // rootDir
                            "1760",                                        // appId
                            "",                                            // params
                            "",                                            // fromManager
                            "",                                            // args
                            0);                                            // serverEnvId

    l_debug("[LOADER] navite_launch over: return %d\n", ret);
    sceKernelExitThread(0);
    return ret;
}

int soloader_init() {
    checkKubridge();
    l_debug("[LOADER] kubridge check passed.");

    checkOpenGL();
    l_debug("[LOADER] kubridge check passed.");

    checkDataFiles();
    l_debug("[LOADER] Data files check passed.");

    if (so_file_load(&so_mod, SO_PATH, LOAD_ADDRESS) < 0) {
        fatal_error("Error: could not load %s.", SO_PATH);
        return -1;
    }
    l_debug("[LOADER] SO loaded: %p", so_mod.text_base);

    so_relocate(&so_mod);
    l_debug("[LOADER] SO relocated.");

    soloader_resolve_imports(&so_mod);
    l_debug("[LOADER] SO imports resolved.");

    soloader_patch(&so_mod);
    l_debug("[LOADER] SO patched.");

    so_flush_caches(&so_mod);
    l_debug("[LOADER] SO caches flushed.");

    so_initialize(&so_mod);
    l_debug("[LOADER] SO initialized.");

    jni_init();
    l_debug("[LOADER] FalsoJNI initialized.");

    controls_init();
    l_debug("[LOADER] Controls initialized.");

    vglInitExtended(0, 960, 544, MEMORY_VITAGL_THRESHOLD_MB * 1024 * 1024, SCE_GXM_MULTISAMPLE_4X);
#ifdef USE_GLSL_SHADERS
    // vglSetSemanticBindingMode(VGL_MODE_POSTPONED);
#endif
    // eglSwapInterval(0, 2);

    return 0;
}

int soloader_run() {
    int (*JNI_OnLoad)(void *jvm, int reserved) = (void *)so_find_symbol(&so_mod, "JNI_OnLoad");
    JNI_OnLoad(&jvm, 0);
    l_debug("[LOADER] JNI_OnLoad over");

    so_hook_symbol(&so_mod, "_ZN18RuntimeContextBase6LaunchEb", (uintptr_t)&RuntimeContextBase_Launch);

    navite_marketlaunch   = (void *)(LOAD_ADDRESS + 0x416CD);
    navite_launch         = (void *)(LOAD_ADDRESS + 0x413F9);
    navite_surfaceChanged = (void *)(LOAD_ADDRESS + 0x40291);
    navite_focusChanged   = (void *)(LOAD_ADDRESS + 0x40269);

    navite_ExtensionInit    = (void *)(LOAD_ADDRESS + 0x41221);
    navite_MediaManagerInit = (void *)(LOAD_ADDRESS + 0x41285);

    navite_ExtensionInit(&jni, NULL, "");
    l_debug("[LOADER] navite_ExtensionInit over");

    navite_MediaManagerInit(&jni, NULL, NULL);  // 没有实际作用
    l_debug("[LOADER] navite_MediaManagerInit over");

    navite_focusChanged(&jni, NULL, 1);
    l_debug("[LOADER] navite_focusChanged over");

    navite_surfaceChanged(&jni, NULL, SCREEN_W, SCREEN_H);
    l_debug("[LOADER] navite_surfaceChanged over");

    SceUID controls_thid = sceKernelCreateThread("controls_thread", (SceKernelThreadEntry)controls_thread, 0x40, 128 * 1024, 0, 0, NULL);
    sceKernelStartThread(controls_thid, 0, NULL);

    SceUID main_thid = sceKernelCreateThread("main_thread", (SceKernelThreadEntry)main_thread, 0x40, 128 * 1024, 0, 0, NULL);
    sceKernelStartThread(main_thid, 0, NULL);
    sceKernelWaitThreadEnd(main_thid, NULL, NULL);

    return 0;
}

void soloader_term() {
    // Cleanup function called when soloader module is unloaded.
}