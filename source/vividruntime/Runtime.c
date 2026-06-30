#include <stdio.h>
#include <string.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "Runtime.h"

#include "Launcher.h"

static JClass sClass           = NULL;

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
                    JniNewLocalRefFromJavaArray(args),
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
    return JavaNewStringUTF(locale);
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
    const char *cstr = JavaGetStringUTFChars(str);
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
    return JavaNewStringUTF(commonDir);
}

JString Runtime_getCommonAppDir(JString str) {
    char        commonAppDir[1024];
    const char *cstr = JavaGetStringUTFChars(str);
    snprintf(commonAppDir, sizeof(commonAppDir), "%s%s", "/.ggee/a/", cstr);

    so_logd("[Runtime] getCommonAppDir(%s): '%s'", cstr, commonAppDir);
    return JavaNewStringUTF(commonAppDir);
}

JString Runtime_getEnv(JString key) {
    const char *key_cstr = JavaGetStringUTFChars(key);
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
    return JavaNewStringUTF(value);
}

void Runtime_jniTrackerSetPage(JString str) {
    const char *cstr = JavaGetStringUTFChars(str);
    so_logd("[Runtime] jniTrackerSetPage(%s)", cstr);
}

void Runtime_jniTrackerEvent(JString str1, JString str2) {
    const char *cstr1 = JavaGetStringUTFChars(str1);
    const char *cstr2 = JavaGetStringUTFChars(str2);
    so_logd("[Runtime] jniTrackerSetPage(%s,%s)", cstr1, cstr2);
}

void Runtime_jniTrackerAssignAttribute(jint id, JString attr) {
    const char *attr_cstr = JavaGetStringUTFChars(attr);
    so_logd("[Runtime] jniTrackerAssignAttribute(%d,%s)", id, attr_cstr);
}

void Runtime_jniTrackerRemoveAttribute(jint id) {
    so_logd("[Runtime] jniTrackerRemoveAttribute(%d)", id);
}

void Runtime_jniTrackerSetAttribute(jint id, JString attr) {
    const char *attr_cstr = JavaGetStringUTFChars(attr);
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

static JavaSymbol class_symbols[] = {
    // static native
    JAVA_SYMBOL_STATIC_NATIVE("surfaceChanged", "(II)V", &surfaceChanged),
    JAVA_SYMBOL_STATIC_NATIVE("surfaceDestroyed", "()V", &surfaceDestroyed),
    JAVA_SYMBOL_STATIC_NATIVE("focusChanged", "(Z)V", &focusChanged),
    JAVA_SYMBOL_STATIC_NATIVE("configurationChanged", "(II)V", &configurationChanged),
    JAVA_SYMBOL_STATIC_NATIVE("keyDown", "(III)V", &keyDown),
    JAVA_SYMBOL_STATIC_NATIVE("keyUp", "(III)V", &keyUp),
    JAVA_SYMBOL_STATIC_NATIVE("touchMove", "(III)V", &touchMove),
    JAVA_SYMBOL_STATIC_NATIVE("touchDown", "(IIIZ)V", &touchDown),
    JAVA_SYMBOL_STATIC_NATIVE("touchUp", "(IIIZ)V", &touchUp),
    JAVA_SYMBOL_STATIC_NATIVE("pause", "()V", &pause),
    JAVA_SYMBOL_STATIC_NATIVE("resume", "()V", &resume),
    JAVA_SYMBOL_STATIC_NATIVE("quit", "()V", &quit),
    JAVA_SYMBOL_STATIC_NATIVE("waitFlip", "(I)Z", &waitFlip),
    JAVA_SYMBOL_STATIC_NATIVE("waitScreenshot", "(I[II)Z", &waitScreenshot),
    JAVA_SYMBOL_STATIC_NATIVE("setSurfaceSize", "(ZII)V", &setSurfaceSize),

    // static native 2
    JAVA_SYMBOL_STATIC_NATIVE("launch", "(ILandroid/view/Surface;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;[Ljava/lang/String;I)I", &launch),
    JAVA_SYMBOL_STATIC_NATIVE("marketlaunch", "(ILandroid/view/Surface;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;[Ljava/lang/String;)I", &marketlaunch),
    JAVA_SYMBOL_STATIC_NATIVE("rorequest", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", &rorequest),
    JAVA_SYMBOL_STATIC_NATIVE("install", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", &install),
    JAVA_SYMBOL_STATIC_NATIVE("roinstall", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I", &roinstall),
    JAVA_SYMBOL_STATIC_NATIVE("remove", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I", &remove_),
    JAVA_SYMBOL_STATIC_NATIVE("verify", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I", &verify),
    JAVA_SYMBOL_STATIC_NATIVE("installed", "()V", &installed),
    JAVA_SYMBOL_STATIC_NATIVE("uninstalled", "()V", &uninstalled),
    JAVA_SYMBOL_STATIC_NATIVE("install_error", "()V", &install_error),
    JAVA_SYMBOL_STATIC_NATIVE("MediaManagerInit", "(Ljava/lang/Object;)V", &MediaManagerInit),
    JAVA_SYMBOL_STATIC_NATIVE("getPackDataSize", "(Ljava/lang/String;)I", &getPackDataSize),
    JAVA_SYMBOL_STATIC_NATIVE("getPackData", "(Ljava/lang/String;[B)I", &getPackData),
    JAVA_SYMBOL_STATIC_NATIVE("ExtensionInit", "(Ljava/lang/Object;)V", &ExtensionInit),
    JAVA_SYMBOL_STATIC_NATIVE("DeviceCellChange", "(I)V", &DeviceCellChange),
    JAVA_SYMBOL_STATIC_NATIVE("DeviceWifiChange", "(I)V", &DeviceWifiChange),
    JAVA_SYMBOL_STATIC_NATIVE("DeviceRoamingChange", "(I)V", &DeviceRoamingChange),
    JAVA_SYMBOL_STATIC_NATIVE("DeviceMutedChange", "(I)V", &DeviceMutedChange),
    JAVA_SYMBOL_STATIC_NATIVE("DeviceKeyboardHiddenChange", "(I)V", &DeviceKeyboardHiddenChange),
    JAVA_SYMBOL_STATIC_NATIVE("DeviceNavigationHiddenChange", "(I)V", &DeviceNavigationHiddenChange),
    JAVA_SYMBOL_STATIC_NATIVE("SensorChangeAccelerometer", "(FFF)V", &SensorChangeAccelerometer),
    JAVA_SYMBOL_STATIC_NATIVE("SensorChangeGyro", "(FFF)V", &SensorChangeGyro),
    JAVA_SYMBOL_STATIC_NATIVE("SensorChangeMagnetic", "(FFF)V", &SensorChangeMagnetic),
    JAVA_SYMBOL_STATIC_NATIVE("LocationChange", "(IDDDDDDD)V", &LocationChange),
    JAVA_SYMBOL_STATIC_NATIVE("AvatarJavaScript", "(ILjava/lang/String;Ljava/lang/String;[B)I", &AvatarJavaScript),
    JAVA_SYMBOL_STATIC_NATIVE("CameraShotCompletionCallback", "()I", &CameraShotCompletionCallback),
    JAVA_SYMBOL_STATIC_NATIVE("CameraStartPreviewCompletionCallback", "(I)I", &CameraStartPreviewCompletionCallback),
    JAVA_SYMBOL_STATIC_NATIVE("MovieOnCompletionListenerCallback", "()I", &MovieOnCompletionListenerCallback),
    JAVA_SYMBOL_STATIC_NATIVE("AdviewItemRewardCallback", "(Ljava/lang/String;I)I", &AdviewItemRewardCallback),
    JAVA_SYMBOL_STATIC_NATIVE("AdviewCallback", "(Ljava/lang/String;II)I", &AdviewCallback),
    JAVA_SYMBOL_STATIC_NATIVE("EventNetworkCallback", "(Ljava/lang/String;)I", &EventNetworkCallback),
    JAVA_SYMBOL_STATIC_NATIVE("HttpResponse", "(IIIII)V", &HttpResponse),

    // static java
    JAVA_SYMBOL_STATIC_METHOD("vibrate", "(Z)V", &Runtime_vibrate),
    JAVA_SYMBOL_STATIC_METHOD("getLocale", "()Ljava/lang/String;", &Runtime_getLocale),
    JAVA_SYMBOL_STATIC_METHOD("launchRuntime", "(Ljava/lang/String;)V", &Runtime_launchRuntime),
    JAVA_SYMBOL_STATIC_METHOD("trialSetTime", "(I)V", &Runtime_trialSetTime),
    JAVA_SYMBOL_STATIC_METHOD("appFinish", "()V", &Runtime_appFinish),
    JAVA_SYMBOL_STATIC_METHOD("updateResourceCheck", "(Ljava/lang/String;[F)V", &Runtime_updateResourceCheck),
    JAVA_SYMBOL_STATIC_METHOD("invisibleRuntimeLogo", "()V", &Runtime_invisibleRuntimeLogo),
    JAVA_SYMBOL_STATIC_METHOD("isUseRecord", "()Z", &Runtime_isUseRecord),
    JAVA_SYMBOL_STATIC_METHOD("getCommonDir", "()Ljava/lang/String;", &Runtime_getCommonDir),
    JAVA_SYMBOL_STATIC_METHOD("getCommonAppDir", "(Ljava/lang/String;)Ljava/lang/String;", &Runtime_getCommonAppDir),
    JAVA_SYMBOL_STATIC_METHOD("getEnv", "(Ljava/lang/String;)Ljava/lang/String;", &Runtime_getEnv),
    JAVA_SYMBOL_STATIC_METHOD("jniTrackerSetPage", "(Ljava/lang/String;)V", &Runtime_jniTrackerSetPage),
    JAVA_SYMBOL_STATIC_METHOD("jniTrackerEvent", "(Ljava/lang/String;Ljava/lang/String;)V", &Runtime_jniTrackerEvent),
    JAVA_SYMBOL_STATIC_METHOD("jniTrackerAssignAttribute", "(ILjava/lang/String;)V", &Runtime_jniTrackerAssignAttribute),
    JAVA_SYMBOL_STATIC_METHOD("jniTrackerRemoveAttribute", "(I)V", &Runtime_jniTrackerRemoveAttribute),
    JAVA_SYMBOL_STATIC_METHOD("jniTrackerSetAttribute", "(ILjava/lang/String;)V", &Runtime_jniTrackerSetAttribute),
    JAVA_SYMBOL_STATIC_METHOD("stereoIsAvailability", "()I", &Runtime_stereoIsAvailability),
    JAVA_SYMBOL_STATIC_METHOD("stereoSetView", "(Z)V", &Runtime_stereoSetView),
    JAVA_SYMBOL_STATIC_METHOD("stereoGetDepth", "()F", &Runtime_stereoGetDepth),
};

static JavaClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/Runtime",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = 0,
};

void Runtime_class_init(void) {
    sClass = JavaCreateClass(&class_meta);
}
