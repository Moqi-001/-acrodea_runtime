#include <falso_jni/FalsoJNI_Impl.h>
#include <falso_jni/FalsoJNI.h>
#include "utils/logger.h"
#include <dirent.h>
/*
 * JNI Methods
*/

// jlong GetCurrentTime(jmethodID id, va_list args) {
//     int time=current_timestamp_ms();
//     debugPrintf("GetCurrentTime(%d)",time);
//     return (int64_t) time;
// }

jlong GetCurrentTime(jmethodID id, va_list args)
{
    return (int64_t)current_timestamp_ms();
}

void Native_Exit(jmethodID id, va_list args)
{
    debugPrintf("Native_Exit() requested via JNI.");
    exit(0);
}

void vibrate(jmethodID id, va_list args)
{

    debugPrintf("vibrate()");
}

jobject getLocale(jmethodID id, va_list args)
{

    debugPrintf("getLocale()");
    return "";
}

jobject getCommonDir(jmethodID id, va_list args)
{

    debugPrintf("getCommonDir()");
    return "ggee/";
}

void appFinish(jmethodID id, va_list args)
{
    debugPrintf("appFinish()");
}

void updateResourceCheck(jmethodID id, va_list args)
{
    const char *value = va_arg(args, char *);
    debugPrintf("updateResourceCheck(%s)", value);
}

void invisibleRuntimeLogo(jmethodID id, va_list args)
{
    debugPrintf("invisibleRuntimeLogo()");
}

jint stereoIsAvailability(jmethodID id, va_list args)
{
    debugPrintf("stereoIsAvailability()");
    return 1;
}

void stereoSetView(jmethodID id, va_list args)
{
    debugPrintf("stereoSetView()");
}

jint isUseRecord(jmethodID id, va_list args)
{
    debugPrintf("isUseRecord()");
    return 0;
}

jfloat stereoGetDepth(jmethodID id, va_list args)
{
    debugPrintf("stereoGetDepth()");
    return 1;
}

jobject getCommonAppDir(jmethodID id, va_list args)
{
    const char *value = va_arg(args, char *);
    debugPrintf("getCommonAppDir(%s)", value);
    return DATA_PATH;
}

void launchRuntime(jmethodID id, va_list args)
{
    const char *value = va_arg(args, char *);
    debugPrintf("launchRuntime(%s)", value);
}

jobject getEnv(jmethodID id, va_list args)
{
    const char *value = va_arg(args, char *);
    debugPrintf("getEnv(%s)", value);
    if (strcmp(value, "DEVICE_VENDOR") == 0)
    {
        return "Sony";
    }
    else if (strcmp(value, "DEVICE_MODEL") == 0)
    {
        return "Psvita";
    }
    else if (strcmp(value, "OS_VERSION") == 0)
    {
        return "2.1";
    }
    else if (strcmp(value, "OS") == 0)
    {
        return "Android";
    }
    else if (strcmp(value, "GL_RENDERER") == 0)
    {
        return "Adreno";
    }

    return "";
}

void trialSetTime(jmethodID id, va_list args)
{
    int time = va_arg(args, int);

    debugPrintf("trialSetTime(%d)", time);
}

void jniTrackerRemoveAttribute(jmethodID id, va_list args)
{
    int value = va_arg(args, int);

    debugPrintf("jniTrackerRemoveAttribute(%d)", value);
}

void jniTrackerSetPage(jmethodID id, va_list args)
{
    const char *value = va_arg(args, char *);
    debugPrintf("jniTrackerSetPage(%s)", value);
}

void jniTrackerSetAttribute(jmethodID id, va_list args)
{
    const char *value = va_arg(args, char *);
    debugPrintf("jniTrackerSetAttribute(%s)", value);
}

void jniTrackerEvent(jmethodID id, va_list args)
{
    const char *value = va_arg(args, char *);
    const char *value2 = va_arg(args, char *);
    debugPrintf("jniTrackerEvent(%s,%s)", value, value2);
}

void jniTrackerAssignAttribute(jmethodID id, va_list args)
{
    const char *value = va_arg(args, char *);
    debugPrintf("jniTrackerAssignAttribute(%s)", value);
}

void surfaceChanged(jmethodID id, va_list args)
{
    debugPrintf("surfaceChanged()");
}

NameToMethodID nameToMethodId[] = {
    {1, "Native_Exit", METHOD_TYPE_VOID, Native_Exit},                              // ()V
    {2, "vibrate", METHOD_TYPE_VOID, vibrate},                                      // ()V
    {3, "launchRuntime", METHOD_TYPE_VOID, launchRuntime},                          // (I)V
    {4, "appFinish", METHOD_TYPE_VOID, appFinish},                                  // (I)V
    {5, "updateResourceCheck", METHOD_TYPE_LONG, updateResourceCheck},              // ()J
    {6, "invisibleRuntimeLogo", METHOD_TYPE_INT, invisibleRuntimeLogo},             // ()I
    {7, "isUseRecord", METHOD_TYPE_VOID, isUseRecord},                              // (Ljava/lang/String;)V
    {8, "getCommonAppDir", METHOD_TYPE_OBJECT, getCommonAppDir},                    // ()Ljava/lang/String;
    {9, "jniTrackerSetPage", METHOD_TYPE_INT, jniTrackerSetPage},                   // ([BII)I
    {10, "getEnv", METHOD_TYPE_INT, getEnv},                                        // ([BII)I
    {11, "getCommonDir", METHOD_TYPE_INT, getCommonDir},                            // ([BII)I
    {12, "jniTrackerEvent", METHOD_TYPE_INT, jniTrackerEvent},                      // ([BII)I
    {13, "jniTrackerAssignAttribute", METHOD_TYPE_INT, jniTrackerAssignAttribute},  // ([BII)I
    {14, "jniTrackerRemoveAttribute", METHOD_TYPE_LONG, jniTrackerRemoveAttribute}, // ([BII)I
    {15, "getLocale", METHOD_TYPE_INT, getLocale},                                  // ([BII)I
    {16, "trialSetTime", METHOD_TYPE_INT, trialSetTime},                            // ([BII)I
    {17, "surfaceChanged", METHOD_TYPE_VOID, surfaceChanged},
    {18, "jniTrackerSetAttribute", METHOD_TYPE_VOID, jniTrackerSetAttribute},
    {19, "stereoIsAvailability", METHOD_TYPE_VOID, stereoIsAvailability},
    {20, "stereoSetView", METHOD_TYPE_VOID, stereoSetView},
    {21, "stereoGetDepth", METHOD_TYPE_VOID, stereoGetDepth},
};

MethodsBoolean methodsBoolean[] = {};
MethodsByte methodsByte[] = {};
MethodsChar methodsChar[] = {};
MethodsDouble methodsDouble[] = {

};
MethodsFloat methodsFloat[] = {
    {21, stereoGetDepth},
};
MethodsInt methodsInt[] = {};
MethodsLong methodsLong[] = {
    {22, GetCurrentTime},
};
MethodsObject methodsObject[] = {};
MethodsShort methodsShort[] = {};
MethodsVoid methodsVoid[] = {};

/*
 * JNI Fields
*/

// System-wide constant that applications sometimes request
// https://developer.android.com/reference/android/content/Context.html#WINDOW_SERVICE
char WINDOW_SERVICE[] = "window";

// System-wide constant that's often used to determine Android version
// https://developer.android.com/reference/android/os/Build.VERSION.html#SDK_INT
// Possible values: https://developer.android.com/reference/android/os/Build.VERSION_CODES
const int SDK_INT = 19;     // Android 4.4 / KitKat
const int descriptor = 110; // Android 4.4 / KitKat

const int mSurface = 0;       // Android 4.4 / KitKat
const int mNativeSurface = 0; // Android 4.4 / KitKat
const int mNativeObject = 0;  // Android 4.4 / KitKat

NameToFieldID nameToFieldId[] = {
    {0, "WINDOW_SERVICE", FIELD_TYPE_OBJECT},
    {1, "SDK_INT", FIELD_TYPE_INT},
    {2, "descriptor", FIELD_TYPE_INT},
    {3, "mSurface", FIELD_TYPE_INT},
    {4, "mNativeSurface", FIELD_TYPE_INT},
    {5, "mNativeObject", FIELD_TYPE_INT},
};

FieldsBoolean fieldsBoolean[] = {};
FieldsByte fieldsByte[] = {};
FieldsChar fieldsChar[] = {};
FieldsDouble fieldsDouble[] = {};
FieldsFloat fieldsFloat[] = {};
FieldsInt fieldsInt[] = {
    {1, SDK_INT},
    {2, descriptor},
    {3, mSurface},
    {4, mNativeSurface},
    {5, mNativeObject},
};
FieldsObject fieldsObject[] = {
    {0, WINDOW_SERVICE},
};
FieldsLong fieldsLong[] = {};
FieldsShort fieldsShort[] = {};

__FALSOJNI_IMPL_CONTAINER_SIZES
