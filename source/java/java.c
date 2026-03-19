#include <dirent.h>
#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <psp2/kernel/threadmgr.h>

#include <falso_jni/FalsoJNI.h>
#include <falso_jni/FalsoJNI_Impl.h>

#include "java/AudioTrack.h"
#include "java/extension/ExtensionManager.h"
#include "java/extension/MyDevice.h"
#include "java/extension/MyErrorDialog.h"
#include "java/Runtime.h"
#include "patch.h"
#include "utils/logger.h"
#include "utils/utils.h"

/*
 * JNI Methods
 */

// commonIsSLESCompatibility", "()I
jint commonIsSLESCompatibility(jclass clazz, jmethodID id, va_list args) {
    int res = 1;
    l_debug("[JNI] commonIsSLESCompatibility(): %d", res);
    return (jint)res;
}

NameToMethodID nameToMethodId[] = {
    // com/acrodea/vividruntime/launcher/Runtime
    {1, "vibrate", METHOD_TYPE_VOID},
    {2, "getLocale", METHOD_TYPE_OBJECT},
    {3, "launchRuntime", METHOD_TYPE_VOID},
    {4, "trialSetTime", METHOD_TYPE_VOID},
    {5, "appFinish", METHOD_TYPE_VOID},
    {6, "updateResourceCheck", METHOD_TYPE_VOID},
    {7, "invisibleRuntimeLogo", METHOD_TYPE_VOID},
    {8, "isUseRecord", METHOD_TYPE_BOOLEAN},
    {9, "getCommonDir", METHOD_TYPE_OBJECT},
    {10, "getCommonAppDir", METHOD_TYPE_OBJECT},
    {11, "getEnv", METHOD_TYPE_OBJECT},
    {12, "jniTrackerSetPage", METHOD_TYPE_VOID},
    {13, "jniTrackerEvent", METHOD_TYPE_VOID},
    {14, "jniTrackerAssignAttribute", METHOD_TYPE_VOID},
    {15, "jniTrackerRemoveAttribute", METHOD_TYPE_VOID},
    {16, "jniTrackerSetAttribute", METHOD_TYPE_VOID},
    {17, "stereoIsAvailability", METHOD_TYPE_INT},
    {18, "stereoSetView", METHOD_TYPE_VOID},
    {19, "stereoGetDepth", METHOD_TYPE_FLOAT},

    // com/acrodea/vividruntime/launcher/extension/ExtensionManager
    {20, "getExtensonByName", METHOD_TYPE_OBJECT},

    // com/acrodea/vividruntime/launcher/extension/MyDevice
    {21, "getImei", METHOD_TYPE_OBJECT},
    {22, "getImsi", METHOD_TYPE_OBJECT},
    {23, "setNavigationHiddenEnable", METHOD_TYPE_INT},
    {24, "getNavigationHiddenState", METHOD_TYPE_INT},
    {25, "getPlatformInfo", METHOD_TYPE_OBJECT},
    {26, "getMutedState", METHOD_TYPE_INT},

    // com/acrodea/vividruntime/launcher/extension/MyErrorDialog
    {27, "setMessage", METHOD_TYPE_VOID},
    {28, "setCode", METHOD_TYPE_VOID},

    // android/media/AudioTrack
    {29, "android/media/AudioTrack/<init>", METHOD_TYPE_OBJECT},
    {30, "getMinBufferSize", METHOD_TYPE_INT},
    {31, "setStereoVolume", METHOD_TYPE_INT},
    {32, "play", METHOD_TYPE_VOID},
    {33, "stop", METHOD_TYPE_VOID},
    {34, "release", METHOD_TYPE_VOID},
    {35, "write", METHOD_TYPE_INT},

    {36, "commonIsSLESCompatibility", METHOD_TYPE_INT},
};

MethodsBoolean methodsBoolean[] = {
    {8, Runtime_isUseRecord},
};
MethodsByte   methodsByte[]   = {};
MethodsChar   methodsChar[]   = {};
MethodsDouble methodsDouble[] = {};
MethodsFloat  methodsFloat[]  = {
    {19, Runtime_stereoGetDepth},
};
MethodsInt methodsInt[] = {
    {17, Runtime_stereoIsAvailability},

    {23, MyDevice_setNavigationHiddenEnable},
    {24, MyDevice_getNavigationHiddenState},
    {26, MyDevice_getMutedState},

    {30, AudioTrack_getMinBufferSize},
    {31, AudioTrack_setStereoVolume},
    {35, AudioTrack_write},

    {36, commonIsSLESCompatibility},
};
MethodsLong   methodsLong[]   = {};
MethodsObject methodsObject[] = {
    {2, Runtime_getLocale},
    {9, Runtime_getCommonDir},
    {10, Runtime_getCommonAppDir},
    {11, Runtime_getEnv},

    {20, ExtensionManager_getExtensonByName},
    {21, MyDevice_getImei},
    {22, MyDevice_getImsi},
    {25, MyDevice_getPlatformInfo},

    {29, AudioTrack_init},
};
MethodsShort methodsShort[] = {};
MethodsVoid  methodsVoid[]  = {
    {1, Runtime_vibrate},
    {3, Runtime_launchRuntime},
    {4, Runtime_trialSetTime},
    {5, Runtime_appFinish},
    {6, Runtime_updateResourceCheck},
    {7, Runtime_invisibleRuntimeLogo},
    {12, Runtime_jniTrackerSetPage},
    {13, Runtime_jniTrackerEvent},
    {14, Runtime_jniTrackerAssignAttribute},
    {15, Runtime_jniTrackerRemoveAttribute},
    {16, Runtime_jniTrackerSetAttribute},
    {18, Runtime_stereoSetView},

    {27, MyErrorDialog_setMessage},
    {28, MyErrorDialog_setCode},

    {32, AudioTrack_play},
    {33, AudioTrack_stop},
    {34, AudioTrack_release},
};

/*
 * JNI Fields
 */

// System-wide constant that applications sometimes request
// https://developer.android.com/reference/android/content/Context.html#WINDOW_SERVICE
char WINDOW_SERVICE[] = "window";

// System-wide constant that's often used to determine Android version
// https://developer.android.com/reference/android/os/Build.VERSION.html#SDK_INT
// Possible values: https://developer.android.com/reference/android/os/Build.VERSION_CODES
const int SDK_INT    = 19;   // Android 4.4 / KitKat
const int descriptor = 110;  // Android 4.4 / KitKat

const int mSurface       = 0;  // Android 4.4 / KitKat
const int mNativeSurface = 0;  // Android 4.4 / KitKat
const int mNativeObject  = 0;  // Android 4.4 / KitKat

NameToFieldID nameToFieldId[] = {
    {0, "WINDOW_SERVICE", FIELD_TYPE_OBJECT},
    {1, "SDK_INT", FIELD_TYPE_INT},
    {2, "descriptor", FIELD_TYPE_INT},
    {3, "mSurface", FIELD_TYPE_INT},
    {4, "mNativeSurface", FIELD_TYPE_INT},
    {5, "mNativeObject", FIELD_TYPE_INT},
};

FieldsBoolean fieldsBoolean[] = {};
FieldsByte    fieldsByte[]    = {};
FieldsChar    fieldsChar[]    = {};
FieldsDouble  fieldsDouble[]  = {};
FieldsFloat   fieldsFloat[]   = {};
FieldsInt     fieldsInt[]     = {
    {1, SDK_INT},
    {2, descriptor},
    {3, mSurface},
    {4, mNativeSurface},
    {5, mNativeObject},
};
FieldsObject fieldsObject[] = {
    {0, WINDOW_SERVICE},
};
FieldsLong  fieldsLong[]  = {};
FieldsShort fieldsShort[] = {};

__FALSOJNI_IMPL_CONTAINER_SIZES
