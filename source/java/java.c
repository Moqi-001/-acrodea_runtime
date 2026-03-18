#include <dirent.h>
#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <psp2/kernel/threadmgr.h>

#include <falso_jni/FalsoJNI.h>
#include <falso_jni/FalsoJNI_Impl.h>

#include "java/extension/ExtensionManager.h"
#include "java/extension/MyDevice.h"
#include "java/Runtime.h"
#include "patch.h"
#include "utils/logger.h"
#include "utils/utils.h"

/*
 * JNI Methods
 */

// "setMessage", "(Ljava/lang/String;)V"
void setMessage(jmethodID id, va_list args) {
    const char *value = va_arg(args, char *);
    l_debug("[JNI] setMessage(%s)", value);
}

jlong GetCurrentTime(jmethodID id, va_list args) {
    int64_t res = current_timestamp_ms();
    l_debug("[JNI] GetCurrentTime(): %lld", res);
    return (jlong)res;
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
    {22, "setNavigationHiddenEnable", METHOD_TYPE_INT},
    {23, "getNavigationHiddenState", METHOD_TYPE_INT},
    {24, "getPlatformInfo", METHOD_TYPE_OBJECT},
    {25, "getMutedState", METHOD_TYPE_INT},

    {26, "setMessage", METHOD_TYPE_VOID},

    {27, "GetCurrentTime", METHOD_TYPE_LONG},
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

    {22, MyDevice_setNavigationHiddenEnable},
    {23, MyDevice_getNavigationHiddenState},
    {25, MyDevice_getMutedState},
};
MethodsLong methodsLong[] = {
    {27, GetCurrentTime},
};
MethodsObject methodsObject[] = {
    {2, Runtime_getLocale},
    {9, Runtime_getCommonDir},
    {10, Runtime_getCommonAppDir},
    {11, Runtime_getEnv},

    {20, ExtensionManager_getExtensonByName},
    {21, MyDevice_getImei},
    {24, MyDevice_getPlatformInfo},

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

    {26, setMessage},
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
