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
#include "loader.h"
#include "patch.h"
#include "utils/dialog.h"
#include "utils/utils.h"

extern so_library so_libnativehelper;
extern so_library so_liblog;
extern so_library so_libcutils;
extern so_library so_libutils;
extern so_library so_libGLESv1_CM;
extern so_library so_libGLESv2;
extern so_library so_libEGL;
extern so_library so_libui;
extern so_library so_libmedia;
extern so_library so_libsqlite;
extern so_library so_libdl;
extern so_library so_libbinder;
extern so_library so_libc;
extern so_library so_libstdcpp;
extern so_library so_libm;

so_module *runtime_mod = NULL;

static int is_unbound_stub(const void *fn) {
    if (!fn) return 1;

    const uint32_t *stub = (const uint32_t *)((uintptr_t)fn & ~(uintptr_t)1);
    return (stub[0] == 0xE24FC008u && stub[1] == 0xE3A0F000u);
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
        vglInitWithCustomThreshold(0, SCREEN_W, SCREEN_H,
                                   VITAGL_RAM_THRESHOLD_MB * 1024 * 1024, VITAGL_CDRAM_THRESHOLD_MB * 1024 * 1024,
                                   VITAGL_PHYCONT_THRESHOLD_MB * 1024 * 1024, VITAGL_CDIALOG_THRESHOLD_MB * 1024 * 1024,
                                   SCE_GXM_MULTISAMPLE_4X);
        opengl_inited = 1;
    }

    return 0;
}

static int soloader_create_modules() {
    so_module_create(&so_libnativehelper);
    so_module_create(&so_liblog);
    so_module_create(&so_libcutils);
    so_module_create(&so_libutils);
    so_module_create(&so_libGLESv1_CM);
    so_module_create(&so_libGLESv2);
    so_module_create(&so_libEGL);
    so_module_create(&so_libui);
    so_module_create(&so_libmedia);
    so_module_create(&so_libsqlite);
    so_module_create(&so_libdl);
    so_module_create(&so_libbinder);
    so_module_create(&so_libc);
    so_module_create(&so_libstdcpp);
    so_module_create(&so_libm);

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
    jint (*JNI_OnLoad)(JavaVM *, void *) = so_module_find_symbol(mod, "JNI_OnLoad");
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
    soloader_java_init();

    runtime_mod = soloader_load_library("acrodea_runtime");
    soloader_patch_runtime(runtime_mod);

    init_opengl();

    so_logd("[LOADER] soloader_init success!");

    return 0;
}

int soloader_run() {
    so_logd("[LOADER] soloader_run...");

    soloader_jni_onload(runtime_mod);

    JavaString appid = JavaNewStringUTF(RUNTIME_APP_ID);
    Runtime_launchRuntime(appid);
    JavaReleaseObject(appid);

    so_logd("[LOADER] soloader_run over.");
    return 0;
}

void soloader_term() {
    so_logd("[LOADER] soloader_term...");

    so_jni_fini();
    so_linker_fini();

    so_logd("[LOADER] soloader_term success!");
}
