#include <falso_jni/FalsoJNI_Impl.h>
#include <falso_jni/FalsoJNI.h>
#include "utils/logger.h"
#include "patch.h"
#include <dirent.h>
#include <malloc.h>
#include <psp2/kernel/threadmgr.h>
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
    return "en";
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

jobject getImei(jmethodID id, va_list args)
{
    debugPrintf("getImei()");
    return "MACW:0026e879f969";
}

void setMessage(jmethodID id, va_list args)
{
    const char *value = va_arg(args, char *);
    debugPrintf("setMessage(%s)",value);
}

typedef struct {
    int i;
    jstring str;
    int UserPtr;
    int kw;
} MyHttp;

MyHttp httpObj;

jobject MyHttp_constructor(jmethodID id, va_list args)
{
    int i = va_arg(args, int);

    debugPrintf("MyHttp_constructor(%p)", i);
    httpObj.i = i;
    return (jobject)&httpObj;
}

jint MyHttp_obj_init(jmethodID id, va_list args)
{
    int ERROR_INIT = -18;
    int NOT_HTTP_URL = -17;
    int REQUEST_OK = 0;

    httpObj.str = va_arg(args, jstring); // boot_from=1760app/1.0.0/top/topUrlInformation
    int i1 = va_arg(args, int); // 1
    int i2 = va_arg(args, int); // 0

    // 这个值要存起来, 实际通过 init 函数传递给了 k.kw
    httpObj.kw = va_arg(args, int); // 0
    
    debugPrintf("MyHttp_obj_init(%s, %d, %d, %d)", httpObj.str, i1, i2, httpObj.kw);
    // 判定 URL 是否以 "http" 开头
    if (httpObj.str == NULL || strncmp(httpObj.str, "http", 4) != 0) {
        // 正常 Java 层代码返回的 NOT_HTTP_URL
        return REQUEST_OK;
    }

    // 现在服务器不通, 正常应该返回 NOT_HTTP_URL
    return REQUEST_OK;
}

jint MyHttp_obj_setUserPtr(jmethodID id, va_list args)
{
    // 这个值继续存起来, 后面要用, 实际存到了 k.kv
    httpObj.UserPtr = va_arg(args, int);
    debugPrintf("MyHttp_obj_setUserPtr(%p)", httpObj.UserPtr);

    // Java 层永远返回 0
    return 0;
}

typedef struct {
    int arg1;
    int arg2;
    int arg3;
    int arg4;
    int userPtr;
    int kw;
    int i;
} ThreadArgs;

int http_response_thread(SceSize size, void* argp) {
    ThreadArgs* thread_params = (ThreadArgs*)argp;

    debugPrintf("[Vita] HTTP thread started: userPtr=%p", thread_params->userPtr);
    debugPrintf("[Vita] HTTP thread started: i=%p", thread_params->i);
    debugPrintf("[Vita] HTTP thread started: arg3=%d", thread_params->arg3);

    HttpResponse_soloader(thread_params->arg1, thread_params->arg2,
                          thread_params->arg3, thread_params->arg4,
                          thread_params->userPtr, thread_params->kw,
                          thread_params->i);

    debugPrintf("[Vita] HTTP thread complete for userPtr=%p", thread_params->userPtr);
    //SetRuntimeContext(124,0);
    //SetRuntimeContext(1112,0);
    return 0;
}
SceUID thread_id=-1;
jint MyHttp_obj_connect(jmethodID id, va_list args)
{
    int R_1 = -28;
    int R_2 = -1;
    int ERROR = -8;
    int ERROR_SERVER = 404;

    // HttpResponse_soloader(0,0,-18,0,httpObj.UserPtr,httpObj.kw,httpObj.i);

    ThreadArgs thread_args;

    thread_args.arg1 = 0;
    thread_args.arg2 = 0;
    thread_args.arg3 = -18;
    thread_args.arg4 = 0;
    thread_args.userPtr = httpObj.UserPtr;
    thread_args.kw = httpObj.kw;
    thread_args.i = httpObj.i;
    if(thread_id>=0)
     {
        return -28;
     }
    if(thread_id==-1)
    {
         thread_id = sceKernelCreateThread(
            "http_thread",
             http_response_thread,
             0x10000100,
             0x10000,
             0,
             0,
             NULL
    );
    }
     

    if (thread_id < 0) {
        debugPrintf("[Vita] Failed to create thread: 0x%08X", thread_id);
        return -8;
    }

    sceKernelStartThread(thread_id, sizeof(ThreadArgs), &thread_args);
    
    // Java 层正确处理返回 0
    return 0;
}

jint MyHttp_destroy(jmethodID id, va_list args)
{
    return 0;
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
    {17, "jniTrackerSetAttribute", METHOD_TYPE_VOID, jniTrackerSetAttribute},
    {18, "stereoIsAvailability", METHOD_TYPE_VOID, stereoIsAvailability},
    {19, "stereoSetView", METHOD_TYPE_VOID, stereoSetView},
    {20, "stereoGetDepth", METHOD_TYPE_VOID, stereoGetDepth},

    // Temp MyHttp fake hook
    {21, "com/acrodea/vividruntime/launcher/extension/MyHttp/<init>", METHOD_TYPE_OBJECT, MyHttp_constructor},
    {22, "init", METHOD_TYPE_INT, MyHttp_obj_init},
    {23, "setUserPtr", METHOD_TYPE_INT, MyHttp_obj_setUserPtr},
    {24, "connect", METHOD_TYPE_INT, MyHttp_obj_connect},
    {25, "destroy", METHOD_TYPE_INT, MyHttp_destroy},
    
    {26, "getImei", METHOD_TYPE_INT, getImei},
    {27, "setMessage", METHOD_TYPE_INT, setMessage},
};

MethodsBoolean methodsBoolean[] = {};
MethodsByte methodsByte[] = {};
MethodsChar methodsChar[] = {};
MethodsDouble methodsDouble[] = {

};
MethodsFloat methodsFloat[] = {
    {20, stereoGetDepth},
};
MethodsInt methodsInt[] = {};
MethodsLong methodsLong[] = {
    {30, GetCurrentTime},
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
