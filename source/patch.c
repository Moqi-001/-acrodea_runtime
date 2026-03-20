// #include <string.h>
// #include <stdlib.h>
#include <stdio.h>

#include <kubridge.h>
#include <so_util/so_util.h>
#include "config.h"

#include "controls.h"
#include "reimpl/libGgee/ggee.h"
#include "reimpl/libKD/kd.h"
#include "reimpl/libLD/ld.h"
#include "reimpl/libLD/ld_patch.h"
#include "utils/logger.h"

#define _DWORD uintptr_t
extern so_module so_mod;

int (*bootstrap_start)(int argc, char *argv[], char *envp[]) = NULL;

int bootstrap_start_soloader(int argc, char *argv[], char *envp[]) {
    l_debug("bootstrap_start(%d,%p,%p)", argc, argv, envp);
    for (size_t i = 0; i < argc && argv[i]; i++) {
        l_debug("bootstrap_start: argv[%d]:(%s)", i, argv[i]);
    }

    // launch 参数
    char *launch_argv[] = {
        "/native/app/com.acrodea.vividruntime.launcher.apk",  // +0 apkPath  程序包路径
        "launch",                                             // +1 mode:    启动模式："launch", "navite_marketlaunch", "rorequest", "install", "roinstall", "remove", "verify"
        "/native/RuntimeApps",                                // +2 rootDir: RuntimeApps的路径
        "1760",                                               // +3 appID:   游戏ID，会将RuntimeApps和这ID拼接，鬼泣的ID为1760
        "debug",                                              // +4 params:  "debug", "reschk" // 无实际作用，函数空执行
        "",                                                   // +5 args:    额外参数传入子函数启动
    };

    // marketlaunch 参数
    char *marketlaunch_argv[] = {
        "/native/app/com.acrodea.vividruntime.launcher.apk",  // +0 apkPath  程序包路径
        "marketlaunch",                                       // +1 mode:    启动模式："launch", "navite_marketlaunch", "rorequest", "install", "roinstall", "remove", "verify"
        "/native/RuntimeApps/",                               // +2 rootDir: RuntimeApps的路径
        "1760",                                               // +3 appID:   游戏ID，会将RuntimeApps和这ID拼接，鬼泣的ID为1760
        "/lib/com.acrodea.gamecenter/app.exe",                // +4 exePath: 商店的exe路径
        "/native/RuntimeApps",                                // +5
        "/native/com.acrodea.gamecenter/res",                 // +6 resDir:  商店的res路径
        "/native/com.acrodea.gamecenter/.tmp",                // +7 tmpDir:  商店的tmp路径
        "from_manager:log_print",                             // +8 args:    额外参数传入子函数启动?
    };

    int ret = bootstrap_start(sizeof(launch_argv) / sizeof(launch_argv[0]), launch_argv, envp);
    // int ret = bootstrap_start(argc, argv, envp);
    l_debug("bootstrap_start(%d,%p,%p): %d", argc, argv, envp, ret);
    return ret;
}

int (*HttpResponse)(void *, int, int, int, void *, void *, void *);

void SetRuntimeContext(int index, int value) {
    char *(*Get)(void *)         = (void *)so_find_symbol(&so_mod, "_ZN14RuntimeContext3GetEv");
    char *RuntimeContextInstance = Get(NULL);
    char *F                      = RuntimeContextInstance + index;
    l_debug("SetRuntimeContext form(%d)", *F);
    *(F) = value;
    l_debug("SetRuntimeContext to(%d)", *F);
}

void plugin_gles_procaddress_terminate() {
    l_debug("plugin_gles_procaddress_terminate()");
}

void soloader_patch(void) {
    soloader_patch_controls(&so_mod);
    soloader_patch_ld(&so_mod);

    // hook_addr((uintptr_t)so_find_symbol(&so_mod, "kdExit"), (uintptr_t)&kdExit);
    hook_addr((uintptr_t)so_find_symbol(&so_mod, "__kdMapFilename"), (uintptr_t)&__kdMapFilename);
    hook_addr((uintptr_t)so_find_symbol(&so_mod, "__kdFlushICache"), (uintptr_t)&__kdFlushICache);

    // hook_addr((uintptr_t)so_find_symbol(&so_mod, "plugin_gles_procaddress_terminate"), (uintptr_t)&plugin_gles_procaddress_terminate);

    ggeeLibExpand = (void *)so_find_symbol(&so_mod, "ggeeLibExpand");
    HttpResponse  = (void *)(LOAD_ADDRESS + 0x40FB4 + 1);

    // ggeeLibExpandFile("ux0:data/Acrodea/lib4_5_0/libc.so", "ux0:data/Acrodea/libc.so");
}
