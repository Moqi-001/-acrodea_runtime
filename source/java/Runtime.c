#include <stdio.h>
#include <string.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "Runtime.h"

#include "Launcher.h"

static JClass sClass = NULL;

// ====== JNI native fucntions ======
static void (*surfaceChanged)(JNIEnv *env, jclass clazz, jint width, jint height);
static void (*surfaceDestroyed)(JNIEnv *env, jclass clazz);
static void (*focusChanged)(JNIEnv *env, jclass clazz, jboolean hasFocus);
static void (*configurationChanged)(JNIEnv *env, jclass clazz, jint config, jint something);
static void (*keyDown)(JNIEnv *env, jclass clazz, jint keyCode, jint repeatCount, jint metaState);
static void (*keyUp)(JNIEnv *env, jclass clazz, jint keyCode, jint repeatCount, jint metaState);
static void (*touchMove)(JNIEnv *env, jclass clazz, jint pointerId, jint x, jint y);
static void (*touchDown)(JNIEnv *env, jclass clazz, jint pointerId, jint x, jint y, jboolean isPrimary);
static void (*touchUp)(JNIEnv *env, jclass clazz, jint pointerId, jint x, jint y, jboolean isPrimary);
static void (*pause)(JNIEnv *env, jclass clazz);
static void (*resume)(JNIEnv *env, jclass clazz);
static void (*quit)(JNIEnv *env, jclass clazz);
static jboolean (*waitFlip)(JNIEnv *env, jclass clazz, jint timeout);
static jboolean (*waitScreenshot)(JNIEnv *env, jclass clazz, jint timeout, jintArray pixels, jint format);
static void (*setSurfaceSize)(JNIEnv *env, jclass clazz, jboolean isReal, jint width, jint height);
// ====== JNI native fucntions 2 ======
static jint (*launch)(JNIEnv *env, jobject thiz, jint a, jobject surface, jstring b, jstring c, jstring d, jstring e, jint f, jstring g, jobjectArray h, jint i);
static jint (*marketlaunch)(JNIEnv *env, jobject thiz, jint a, jobject surface, jstring b, jstring c, jstring d, jstring e, jstring f, jstring g, jstring h, jstring i, jint j, jstring k, jobjectArray l);
static jstring (*rorequest)(JNIEnv *env, jobject thiz, jstring a, jstring b, jstring c, jstring d, jstring e, jstring f);
static jstring (*install)(JNIEnv *env, jobject thiz, jstring a, jstring b, jstring c, jstring d, jstring e, jstring f);
static jint (*roinstall)(JNIEnv *env, jobject thiz, jstring a, jstring b, jstring c, jstring d, jstring e);
static jint (*remove_)(JNIEnv *env, jobject thiz, jstring a, jstring b, jstring c, jstring d, jstring e);
static jint (*verify)(JNIEnv *env, jobject thiz, jstring a, jstring b, jstring c, jstring d, jstring e);
static void (*installed)(JNIEnv *env, jobject thiz);
static void (*uninstalled)(JNIEnv *env, jobject thiz);
static void (*install_error)(JNIEnv *env, jobject thiz);
static void (*MediaManagerInit)(JNIEnv *env, jobject thiz, jobject obj);
static jint (*getPackDataSize)(JNIEnv *env, jobject thiz, jstring str);
static jint (*getPackData)(JNIEnv *env, jobject thiz, jstring str, jbyteArray arr);
static void (*ExtensionInit)(JNIEnv *env, jobject thiz, jobject extensionManager);
static void (*DeviceCellChange)(JNIEnv *env, jobject thiz, jint value);
static void (*DeviceWifiChange)(JNIEnv *env, jobject thiz, jint value);
static void (*DeviceRoamingChange)(JNIEnv *env, jobject thiz, jint value);
static void (*DeviceMutedChange)(JNIEnv *env, jobject thiz, jint value);
static void (*DeviceKeyboardHiddenChange)(JNIEnv *env, jobject thiz, jint value);
static void (*DeviceNavigationHiddenChange)(JNIEnv *env, jobject thiz, jint value);
static void (*SensorChangeAccelerometer)(JNIEnv *env, jobject thiz, jfloat x, jfloat y, jfloat z);
static void (*SensorChangeGyro)(JNIEnv *env, jobject thiz, jfloat x, jfloat y, jfloat z);
static void (*SensorChangeMagnetic)(JNIEnv *env, jobject thiz, jfloat x, jfloat y, jfloat z);
static void (*LocationChange)(JNIEnv *env, jobject thiz, jint a, jdouble b, jdouble c, jdouble d, jdouble e, jdouble f, jdouble g, jdouble h);
static jint (*AvatarJavaScript)(JNIEnv *env, jobject thiz, jint a, jstring b, jstring c, jbyteArray d);
static jint (*CameraShotCompletionCallback)(JNIEnv *env, jobject thiz);
static jint (*CameraStartPreviewCompletionCallback)(JNIEnv *env, jobject thiz, jint value);
static jint (*MovieOnCompletionListenerCallback)(JNIEnv *env, jobject thiz);
static jint (*AdviewItemRewardCallback)(JNIEnv *env, jobject thiz, jstring str, jint value);
static jint (*AdviewCallback)(JNIEnv *env, jobject thiz, jstring str, jint a, jint b);
static jint (*EventNetworkCallback)(JNIEnv *env, jobject thiz, jstring str);
static void (*HttpResponse)(JNIEnv *env, jobject thiz, jint a, jint b, jint c, jint d, jint e);

// ====== native call ======
void Runtime_surfaceChanged(jint width, jint height) {
    ENTER_JNI_CALL(0)
    surfaceChanged(&_jni_env, sClass, width, height);
    EXIT_JNI_CALL(NULL)
}

void Runtime_surfaceDestroyed() {
    ENTER_JNI_CALL(0)
    surfaceDestroyed(&_jni_env, sClass);
    EXIT_JNI_CALL(NULL)
}

void Runtime_focusChanged(jboolean hasFocus) {
    ENTER_JNI_CALL(0)
    focusChanged(&_jni_env, sClass, hasFocus);
    EXIT_JNI_CALL(NULL)
}

void Runtime_configurationChanged(jint config, jint something) {
    ENTER_JNI_CALL(0)
    configurationChanged(&_jni_env, sClass, config, something);
    EXIT_JNI_CALL(NULL)
}

void Runtime_keyDown(jint keyCode, jint repeatCount, jint metaState) {
    ENTER_JNI_CALL(0)
    keyDown(&_jni_env, sClass, keyCode, repeatCount, metaState);
    EXIT_JNI_CALL(NULL)
}

void Runtime_keyUp(jint keyCode, jint repeatCount, jint metaState) {
    ENTER_JNI_CALL(0)
    keyUp(&_jni_env, sClass, keyCode, repeatCount, metaState);
    EXIT_JNI_CALL(NULL)
}

void Runtime_touchMove(jint pointerId, jint x, jint y) {
    ENTER_JNI_CALL(0)
    touchMove(&_jni_env, sClass, pointerId, x, y);
    EXIT_JNI_CALL(NULL)
}

void Runtime_touchDown(jint pointerId, jint x, jint y, jboolean isPrimary) {
    ENTER_JNI_CALL(0)
    touchDown(&_jni_env, sClass, pointerId, x, y, isPrimary);
    EXIT_JNI_CALL(NULL)
}

void Runtime_touchUp(jint pointerId, jint x, jint y, jboolean isPrimary) {
    ENTER_JNI_CALL(0)
    touchUp(&_jni_env, sClass, pointerId, x, y, isPrimary);
    EXIT_JNI_CALL(NULL)
}

void Runtime_pause() {
    ENTER_JNI_CALL(0)
    pause(&_jni_env, sClass);
    EXIT_JNI_CALL(NULL)
}

void Runtime_resume() {
    ENTER_JNI_CALL(0)
    resume(&_jni_env, sClass);
    EXIT_JNI_CALL(NULL)
}

void Runtime_quit() {
    ENTER_JNI_CALL(0)
    quit(&_jni_env, sClass);
    EXIT_JNI_CALL(NULL)
}

jboolean Runtime_waitFlip(jint timeout) {
    jboolean result;
    ENTER_JNI_CALL(0)
    result = waitFlip(&_jni_env, sClass, timeout);
    EXIT_JNI_CALL(NULL)
    return result;
}

void Runtime_setSurfaceSize(jboolean isReal, jint width, jint height) {
    ENTER_JNI_CALL(0)
    setSurfaceSize(&_jni_env, sClass, isReal, width, height);
    EXIT_JNI_CALL(NULL)
}

// ====== native call 2======
jint Runtime_launch(jint sdkInt, Surface surface, JString runtimeDir, JString apkPath, JString rootDir, JString appId, jint params, JString fromManager, JString *args, jint serverEnvId) {
    jint result;
    ENTER_JNI_CALL(0)
    result = launch(&_jni_env, sClass, sdkInt,
                    JniNewLocalRefFromJava(surface),
                    JniNewLocalRefFromJava(runtimeDir),
                    JniNewLocalRefFromJava(apkPath),
                    JniNewLocalRefFromJava(rootDir),
                    JniNewLocalRefFromJava(appId),
                    params,
                    JniNewLocalRefFromJava(fromManager),
                    JniNewLocalRefFromJava(args),
                    serverEnvId);
    EXIT_JNI_CALL(NULL)
    return result;
}

void Runtime_MediaManagerInit(MyMediaManager mediaManager) {
    ENTER_JNI_CALL(0)
    MediaManagerInit(&_jni_env, sClass, JniNewLocalRefFromJava(mediaManager));
    EXIT_JNI_CALL(NULL)
}

void Runtime_ExtensionInit(ExtensionManager extensionManager) {
    ENTER_JNI_CALL(0)
    ExtensionInit(&_jni_env, sClass, JniNewLocalRefFromJava(extensionManager));
    EXIT_JNI_CALL(NULL)
}

// ====== JAVA Impl ======
void Runtime_vibrate(jboolean enalbed) {
    so_logd("[Runtime] vibrate(%d)", enalbed);
}

JString Runtime_getLocale() {
    const char *locale = "zh_CN";
    so_logd("[Runtime] getLocale(): '%s'", locale);
    return String_newUTF(locale);
}

void Runtime_launchRuntime(JString appid) {
    Launcher_launchRuntime(appid);
}

void Runtime_trialSetTime(jint t) {
    so_logd("[Runtime] trialSetTime(%d)", t);
}

void Runtime_appFinish() {
    Launcher_appFinish();
}

void Runtime_updateResourceCheck(JString str, jfloat *arr) {
    const char *cstr = String_getUTFChars(str);
    so_logd("[Runtime] updateResourceCheck(%s,%p)", cstr, arr);
}

void Runtime_invisibleRuntimeLogo() {
    so_logd("[Runtime] invisibleRuntimeLogo()");
}

jboolean Runtime_isUseRecord() {
    jboolean result = JNI_FALSE;
    so_logd("[Runtime] isUseRecord(): %u", result);
    return result;
}

JString Runtime_getCommonDir() {
    const char *commonDir = "/.ggee/";
    so_logd("[Runtime] getCommonDir(): '%s'", commonDir);
    return String_newUTF(commonDir);
}

JString Runtime_getCommonAppDir(JString str) {
    char        commonAppDir[1024];
    const char *cstr = String_getUTFChars(str);
    snprintf(commonAppDir, sizeof(commonAppDir), "%s%s", "/.ggee/a/", cstr);

    so_logd("[Runtime] getCommonAppDir(%s): '%s'", cstr, commonAppDir);
    return String_newUTF(commonAppDir);
}

JString Runtime_getEnv(JString key) {
    const char *key_cstr = String_getUTFChars(key);
    const char *value;

    if (strcmp(key_cstr, "DEVICE_VENDOR") == 0) {
        value = "Sony";
    } else if (strcmp(key_cstr, "DEVICE_MODEL") == 0) {
        value = "Psvita";
    } else if (strcmp(key_cstr, "OS_VERSION") == 0) {
        value = "2.1";
    } else if (strcmp(key_cstr, "OS") == 0) {
        value = "Android";
    } else if (strcmp(key_cstr, "GL_RENDERER") == 0) {
        value = "Adreno";  // "PowerVR SGX543MP4+"
    } else if (strcmp(key_cstr, "GL_VERSION_SUPPORTED") == 0) {
        value = "2.0";
    } else if (strcmp(key_cstr, "GL_VENDOR") == 0) {
        value = "Qualcomm";  // "Imagination Technologies"
    } else {
        value = "unknown";
    }

    so_logd("[Runtime] getEnv(%s): %s", key_cstr, value);
    return String_newUTF(value);
}

void Runtime_jniTrackerSetPage(JString str) {
    const char *cstr = String_getUTFChars(str);
    so_logd("[Runtime] jniTrackerSetPage(%s)", cstr);
}

void Runtime_jniTrackerEvent(JString str1, JString str2) {
    const char *cstr1 = String_getUTFChars(str1);
    const char *cstr2 = String_getUTFChars(str2);
    so_logd("[Runtime] jniTrackerSetPage(%s,%s)", cstr1, cstr2);
}

void Runtime_jniTrackerAssignAttribute(jint id, JString attr) {
    const char *attr_cstr = String_getUTFChars(attr);
    so_logd("[Runtime] jniTrackerAssignAttribute(%d,%s)", id, attr_cstr);
}

void Runtime_jniTrackerRemoveAttribute(jint id) {
    so_logd("[Runtime] jniTrackerRemoveAttribute(%d)", id);
}

void Runtime_jniTrackerSetAttribute(jint id, JString attr) {
    const char *attr_cstr = String_getUTFChars(attr);
    so_logd("[Runtime] jniTrackerSetAttribute(%d,%s)", id, attr_cstr);
}

jint Runtime_stereoIsAvailability() {
    jint result = 1;
    so_logd("[Runtime] stereoIsAvailability(): %d", result);
    return result;
}

void Runtime_stereoSetView(jboolean enalbed) {
    so_logd("[Runtime] stereoSetView(%u)", enalbed);
}

jfloat Runtime_stereoGetDepth() {
    jfloat result = 1.0f;
    so_logd("[Runtime] stereoGetDepth(): %f", result);
    return result;
}

static JSymbol class_symbols[] = {
    // static native
    JSYMBOL_NATIVE("surfaceChanged", "(II)V", &surfaceChanged),
    JSYMBOL_NATIVE("surfaceDestroyed", "()V", &surfaceDestroyed),
    JSYMBOL_NATIVE("focusChanged", "(Z)V", &focusChanged),
    JSYMBOL_NATIVE("configurationChanged", "(II)V", &configurationChanged),
    JSYMBOL_NATIVE("keyDown", "(III)V", &keyDown),
    JSYMBOL_NATIVE("keyUp", "(III)V", &keyUp),
    JSYMBOL_NATIVE("touchMove", "(III)V", &touchMove),
    JSYMBOL_NATIVE("touchDown", "(IIIZ)V", &touchDown),
    JSYMBOL_NATIVE("touchUp", "(IIIZ)V", &touchUp),
    JSYMBOL_NATIVE("pause", "()V", &pause),
    JSYMBOL_NATIVE("resume", "()V", &resume),
    JSYMBOL_NATIVE("quit", "()V", &quit),
    JSYMBOL_NATIVE("waitFlip", "(I)Z", &waitFlip),
    JSYMBOL_NATIVE("waitScreenshot", "(I[II)Z", &waitScreenshot),
    JSYMBOL_NATIVE("setSurfaceSize", "(ZII)V", &setSurfaceSize),

    // static native 2
    JSYMBOL_NATIVE("launch", "(ILandroid/view/Surface;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;[Ljava/lang/String;I)I", &launch),
    JSYMBOL_NATIVE("marketlaunch", "(ILandroid/view/Surface;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;[Ljava/lang/String;)I", &marketlaunch),
    JSYMBOL_NATIVE("rorequest", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", &rorequest),
    JSYMBOL_NATIVE("install", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", &install),
    JSYMBOL_NATIVE("roinstall", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I", &roinstall),
    JSYMBOL_NATIVE("remove", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I", &remove_),
    JSYMBOL_NATIVE("verify", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I", &verify),
    JSYMBOL_NATIVE("installed", "()V", &installed),
    JSYMBOL_NATIVE("uninstalled", "()V", &uninstalled),
    JSYMBOL_NATIVE("install_error", "()V", &install_error),
    JSYMBOL_NATIVE("MediaManagerInit", "(Ljava/lang/Object;)V", &MediaManagerInit),
    JSYMBOL_NATIVE("getPackDataSize", "(Ljava/lang/String;)I", &getPackDataSize),
    JSYMBOL_NATIVE("getPackData", "(Ljava/lang/String;[B)I", &getPackData),
    JSYMBOL_NATIVE("ExtensionInit", "(Ljava/lang/Object;)V", &ExtensionInit),
    JSYMBOL_NATIVE("DeviceCellChange", "(I)V", &DeviceCellChange),
    JSYMBOL_NATIVE("DeviceWifiChange", "(I)V", &DeviceWifiChange),
    JSYMBOL_NATIVE("DeviceRoamingChange", "(I)V", &DeviceRoamingChange),
    JSYMBOL_NATIVE("DeviceMutedChange", "(I)V", &DeviceMutedChange),
    JSYMBOL_NATIVE("DeviceKeyboardHiddenChange", "(I)V", &DeviceKeyboardHiddenChange),
    JSYMBOL_NATIVE("DeviceNavigationHiddenChange", "(I)V", &DeviceNavigationHiddenChange),
    JSYMBOL_NATIVE("SensorChangeAccelerometer", "(FFF)V", &SensorChangeAccelerometer),
    JSYMBOL_NATIVE("SensorChangeGyro", "(FFF)V", &SensorChangeGyro),
    JSYMBOL_NATIVE("SensorChangeMagnetic", "(FFF)V", &SensorChangeMagnetic),
    JSYMBOL_NATIVE("LocationChange", "(IDDDDDDD)V", &LocationChange),
    JSYMBOL_NATIVE("AvatarJavaScript", "(ILjava/lang/String;Ljava/lang/String;[B)I", &AvatarJavaScript),
    JSYMBOL_NATIVE("CameraShotCompletionCallback", "()I", &CameraShotCompletionCallback),
    JSYMBOL_NATIVE("CameraStartPreviewCompletionCallback", "(I)I", &CameraStartPreviewCompletionCallback),
    JSYMBOL_NATIVE("MovieOnCompletionListenerCallback", "()I", &MovieOnCompletionListenerCallback),
    JSYMBOL_NATIVE("AdviewItemRewardCallback", "(Ljava/lang/String;I)I", &AdviewItemRewardCallback),
    JSYMBOL_NATIVE("AdviewCallback", "(Ljava/lang/String;II)I", &AdviewCallback),
    JSYMBOL_NATIVE("EventNetworkCallback", "(Ljava/lang/String;)I", &EventNetworkCallback),
    JSYMBOL_NATIVE("HttpResponse", "(IIIII)V", &HttpResponse),

    // static method
    JSYMBOL_STATIC_METHOD("vibrate", "(Z)V", &Runtime_vibrate),
    JSYMBOL_STATIC_METHOD("getLocale", "()Ljava/lang/String;", &Runtime_getLocale),
    JSYMBOL_STATIC_METHOD("launchRuntime", "(Ljava/lang/String;)V", &Runtime_launchRuntime),
    JSYMBOL_STATIC_METHOD("trialSetTime", "(I)V", &Runtime_trialSetTime),
    JSYMBOL_STATIC_METHOD("appFinish", "()V", &Runtime_appFinish),
    JSYMBOL_STATIC_METHOD("updateResourceCheck", "(Ljava/lang/String;[F)V", &Runtime_updateResourceCheck),
    JSYMBOL_STATIC_METHOD("invisibleRuntimeLogo", "()V", &Runtime_invisibleRuntimeLogo),
    JSYMBOL_STATIC_METHOD("isUseRecord", "()Z", &Runtime_isUseRecord),
    JSYMBOL_STATIC_METHOD("getCommonDir", "()Ljava/lang/String;", &Runtime_getCommonDir),
    JSYMBOL_STATIC_METHOD("getCommonAppDir", "(Ljava/lang/String;)Ljava/lang/String;", &Runtime_getCommonAppDir),
    JSYMBOL_STATIC_METHOD("getEnv", "(Ljava/lang/String;)Ljava/lang/String;", &Runtime_getEnv),
    JSYMBOL_STATIC_METHOD("jniTrackerSetPage", "(Ljava/lang/String;)V", &Runtime_jniTrackerSetPage),
    JSYMBOL_STATIC_METHOD("jniTrackerEvent", "(Ljava/lang/String;Ljava/lang/String;)V", &Runtime_jniTrackerEvent),
    JSYMBOL_STATIC_METHOD("jniTrackerAssignAttribute", "(ILjava/lang/String;)V", &Runtime_jniTrackerAssignAttribute),
    JSYMBOL_STATIC_METHOD("jniTrackerRemoveAttribute", "(I)V", &Runtime_jniTrackerRemoveAttribute),
    JSYMBOL_STATIC_METHOD("jniTrackerSetAttribute", "(ILjava/lang/String;)V", &Runtime_jniTrackerSetAttribute),
    JSYMBOL_STATIC_METHOD("stereoIsAvailability", "()I", &Runtime_stereoIsAvailability),
    JSYMBOL_STATIC_METHOD("stereoSetView", "(Z)V", &Runtime_stereoSetView),
    JSYMBOL_STATIC_METHOD("stereoGetDepth", "()F", &Runtime_stereoGetDepth),
};

static JClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/Runtime",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = 0,
    .dtor          = NULL,
};

void setup_Runtime(void) {
    sClass = JavaCreateClass(&class_meta);
}
