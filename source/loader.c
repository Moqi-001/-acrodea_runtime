#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kubridge.h>
#include <psp2/vshbridge.h>
#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>
#include <so_linker/so_linker.h>
#include <vitaGL.h>

#include "config.h"
#include "controls.h"
#include "java.h"
#include "java/Runtime.h"
#include "loader.h"
#include "patch.h"
#include "utils/dialog.h"
#include "utils/utils.h"

extern so_library libnativehelper_so;
extern so_library liblog_so;
extern so_library libcutils_so;
extern so_library libutils_so;
extern so_library libGLESv1_CM_so;
extern so_library libGLESv2_so;
extern so_library libEGL_so;
extern so_library libui_so;
extern so_library libmedia_so;
extern so_library libsqlite_so;
extern so_library libdl_so;
extern so_library libbinder_so;
extern so_library libc_so;
extern so_library libstdcxx_so;
extern so_library libm_so;

so_module *runtime_mod = NULL;

/* 为了兼容vita3k。vita3k不需要插件，但它有kuKernelMemProtect函数实现。
 * 当判断插件加载失败时再判断这个函数是否有绑定，这样的判断可实现对实机和vita3k都通用。*/
static int is_unbound_stub(const void *fn) {
    if (!fn) {
        return 1;
    }

    const uint32_t *stub = (const uint32_t *)((uintptr_t)fn & ~(uintptr_t)1);
    return (stub[0] == 0xE24FC008u && stub[1] == 0xE3A0F000u);  // 未绑定时的指令
}

static int check_kubridge() {
    int search_unk[2];
    if (_vshKernelSearchModuleByName("kubridge", search_unk) < 0 && is_unbound_stub(kuKernelMemProtect)) {
        fatal_error("[LOADER] kubridge.skprx is not installed.");
        return -1;
    }
    return 0;
}

static int check_opengl() {
    if (!file_exists("ur0:/data/libshacccg.suprx") && !file_exists("ur0:/data/external/libshacccg.suprx")) {
        fatal_error(
            "Error: libshacccg.suprx is not installed. "
            "Google \"ShaRKBR33D\" for quick installation.");
        return -1;
    }
    return 0;
}

static int init_opengl() {
    static int opengl_inited = 0;
    if (!opengl_inited) {
        vglInitParams params     = {0};
        params.width             = SCREEN_W;
        params.height            = SCREEN_H;
        params.ram_threshold     = VITAGL_RAM_THRESHOLD_MB * 1024 * 1024;
        params.cdram_threshold   = VITAGL_CDRAM_THRESHOLD_MB * 1024 * 1024;
        params.phycont_threshold = VITAGL_PHYCONT_THRESHOLD_MB * 1024 * 1024;
        params.cdlg_threshold    = VITAGL_CDIALOG_THRESHOLD_MB * 1024 * 1024;
#if RUNTIME_APP_ID == 1277
        // 4X MSAA produces 1px seams between adjacent background quads on hardware.
        params.msaa = VGL_MSAA_NONE;
#else
        params.msaa = VGL_MSAA_4X;
#endif
        vglInit(&params);
        opengl_inited = 1;
    }

    return 0;
}

static int soloader_create_modules() {
    so_module_create(&libnativehelper_so);
    so_module_create(&liblog_so);
    so_module_create(&libcutils_so);
    so_module_create(&libutils_so);
    so_module_create(&libGLESv1_CM_so);
    so_module_create(&libGLESv2_so);
    so_module_create(&libEGL_so);
    so_module_create(&libui_so);
    so_module_create(&libmedia_so);
    so_module_create(&libsqlite_so);
    so_module_create(&libdl_so);
    so_module_create(&libbinder_so);
    so_module_create(&libc_so);
    so_module_create(&libstdcxx_so);
    so_module_create(&libm_so);

    return 0;
}

so_module *soloader_load_library(const char *soname) {
    char path[1024];
    snprintf(path, 1024, "%slib%s.so", SO_LIB_DIR, soname);

    so_module *mod = so_module_open(path);
    if (!mod) {
        fatal_error("Error: could not load %s.", path);
        return NULL;
    }

    return mod;
}

int soloader_jni_onload(so_module *mod) {
    jint result = 0;

    ENTER_JNI_CALL(0)
    jint (*JNI_OnLoad)(JavaVM *, void *) = so_module_find_export(mod, "JNI_OnLoad");
    if (JNI_OnLoad) {
        result = JNI_OnLoad(&_java_vm, NULL);
    }
    EXIT_JNI_CALL(NULL)

    if (result < 0) {
        fatal_error("JNI_OnLoad: error!");
        return -1;
    }

    so_logd("[LOADER] JNI_OnLoad success! ('%s')", so_module_get_name(mod));
    return 0;
}

int soloader_init() {
    so_logd("[LOADER] soloader_init...");

    check_kubridge();
    check_opengl();
    so_linker_init();
    so_jni_init();
    soloader_create_modules();
    soloader_setup_java();
    soloader_init_controls();

    runtime_mod = soloader_load_library("acrodea_runtime");
    soloader_patch_runtime(runtime_mod);

    init_opengl();

    so_logd("[LOADER] soloader_init success!");

    return 0;
}

int soloader_run() {
    so_logd("[LOADER] soloader_run...");

    soloader_jni_onload(runtime_mod);

    JString appid = String_newUTF(RUNTIME_APP_ID_STR);
    Runtime_launchRuntime(appid);

    so_logd("[LOADER] soloader_run over.");
    return 0;
}

void soloader_term() {
    so_logd("[LOADER] soloader_term...");

    so_jni_fini();
    so_linker_fini();

    so_logd("[LOADER] soloader_term success!");
}
