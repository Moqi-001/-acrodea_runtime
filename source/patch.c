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

#define _DWORD uintptr_t
extern so_module so_mod;

LdModule *bootstrap_mod                                      = NULL;
int (*bootstrap_start)(int argc, char *argv[], char *envp[]) = NULL;

int sub_52860(int a1)
{
    debugPrintf("sub_52860(%d)",a1);
    return 0;
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

    // hook_addr((uintptr_t)(0x101154+1+0x99000000), (uintptr_t)&sub_101154);
    // hook_addr((uintptr_t)(0x105D58+1+0x99000000), (uintptr_t)&sub_105D58);
    // hook_addr((uintptr_t)(0x10F07C+1+0x99000000), (uintptr_t)&sub_10F07C);
    hook_addr((uintptr_t)(0x52860+1+0x99000000), (uintptr_t)&sub_52860);
    

    //int ret = bootstrap_start(sizeof(marketlaunch_argv) / sizeof(marketlaunch_argv[0]), marketlaunch_argv, envp);
    int ret = bootstrap_start(argc, argv, envp);
    l_debug("bootstrap_start_soloader return %p", ret);
    return ret;
}

int (*HttpResponse)(void *, int, int, int, void *, void *, void *);

void HttpResponse_soloader(void *a1, int a2, int a3, int a4, void *a5, void *a6, void *a7)
{
    l_debug("HttpResponse(%p)", a1);
    int ret = HttpResponse(a1,a2,a3,a4,a5,a6,a7);
    l_debug("HttpResponse return %p", ret);

}

void plugin_gles_procaddress_terminate() {
    debugPrintf("plugin_gles_procaddress_terminate()");
}

void so_patch(void) {
    // Sample hook
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
    HttpResponse = (void *)(LOAD_ADDRESS + 0x40FB4+1);

    // ggeeLibExpandFile("ux0:data/Acrodea/lib4_5_0/libc.so", "ux0:data/Acrodea/libc.so");

    // int * UserConfirm = (int *) so_symbol(&so_mod, "_UserConfirm");
    // * UserConfirm = 1;

    // char ** GVfs = (char **) so_symbol(&so_mod, "GVfs");
    // *GVfs = malloc(60);
    // //**GVfs = 0;
    // memset(*GVfs,0,60);
    // debugPrintf("set GVfs over");
}
