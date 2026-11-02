#include <string.h>

#include <psp2/kernel/processmgr.h>
#include <so_common/so_logger.h>

#include "config.h"
#include "controls.h"
#include "Runtime.h"

#include "Launcher.h"

static jboolean runtimeIsRunning = 0;

static int controls_thread(SceSize args, void *argp) {
    while (runtimeIsRunning) {
        controls_poll();
        sceKernelDelayThread(10 * 1000);
    }

    sceKernelExitThread(0);
    return 0;
}

void Launcher_launchRuntime(JavaString appid) {
    Runtime_ExtensionInit(NULL);
    so_logd("[Launcher] ExtensionInit over");
    Runtime_MediaManagerInit(NULL);
    so_logd("[Launcher] MediaManagerInit over");
    Runtime_surfaceChanged(SCREEN_W, SCREEN_H);
    so_logd("[Launcher] surfaceChanged over");

    jint       sdkInt     = 19;
    Surface    surface    = AndroidSurface_new();
    JavaString runtimeDir = JavaNewStringUTF("/runtime/");
    JavaString apkPath    = JavaNewStringUTF("/com.acrodea.vividruntime.launcher.apk");
    JavaString rootDir    = JavaNewStringUTF("/RuntimeApps");
    JavaString appID      = JavaRetainObject(appid);
    jint       params     = 0;  // 2 reschk

    jint result = Runtime_launch(
        sdkInt,      // sdkInt
        surface,     // surface
        runtimeDir,  // runtimeDir
        apkPath,     // apkPath
        rootDir,     // rootDir
        appID,       // appID
        params,      // params
        NULL,        // fromManager
        NULL,        // args
        0);          // serverEnvId

    if (result != JNI_OK) {
        so_loge("[Launcher] launch error!");
        return;
    }

    runtimeIsRunning     = 1;
    SceUID controls_thid = sceKernelCreateThread("controls_thread", (SceKernelThreadEntry)controls_thread, 0x10000100, 128 * 1024, 0, 0, NULL);
    sceKernelStartThread(controls_thid, 0, NULL);
    sceKernelWaitThreadEnd(controls_thid, NULL, NULL);

    so_logd("[Launcher] launchRuntime(%s): %d", JavaGetStringUTFChars(appid), result);
}

void Launcher_appFinish() {
    runtimeIsRunning = 0;
    so_logd("[Launcher] appFinish()");
}