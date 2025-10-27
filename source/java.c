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

jlong GetCurrentTime(jmethodID id, va_list args) {
    return (int64_t) current_timestamp_ms();
}

jlong Native_GetHeapTotalMemory(jmethodID id, va_list args) {

    debugPrintf("Native_GetHeapTotalMemory(12000000)");
    return 12000000;
}

jlong Native_GetHeapFreeMemory(jmethodID id, va_list args) {

    debugPrintf("Native_GetHeapFreeMemory(12000000)");
    return 12000000;
}

jlong Native_GetExternalMemorySize(jmethodID id, va_list args) {

    debugPrintf("Native_GetExternalMemorySize(12000000000)");
    return 12000000000;
}

jlong Native_GetInternalMemorySize(jmethodID id, va_list args) {

    debugPrintf("Native_GetInternalMemorySize(12000000)");
    return 12000000;
}

void Native_Exit(jmethodID id, va_list args) {
    debugPrintf("Native_Exit() requested via JNI.");
    exit(0);
}

jobject Native_GetDeviceModel(jmethodID id, va_list args) {
   
    debugPrintf("Native_GetDeviceModel");
    return "PSV";
}

jobject Native_GetPhoneNumber(jmethodID id, va_list args) {
   
    debugPrintf("Native_GetPhoneNumber");
    return "110";
}

void Native_tryBuyItem(jmethodID id, va_list args) {

    debugPrintf("Native_tryBuyItem()");
}

void Native_InvalidData(jmethodID id, va_list args) {

    debugPrintf("Native_InvalidData()");
}

jint GetTextHeight(jmethodID id, va_list args) {
    debugPrintf("GetTextHeight(20)");
    return 20;
}

jint GetTextWidth(jmethodID id, va_list args) {
    const char * value = va_arg(args, char*);
    //debugPrintf("GetTextWidth(%s)",value);
    return 20*strlen(value);
}

jobject GetTextTexture(jmethodID id, va_list args) {
    const char* value = va_arg(args, char*);
    int style = va_arg(args, int);
    int color1 = va_arg(args, int);
    int color2 = va_arg(args, int);
    
    debugPrintf("GetTextTexture(%s,%d,%d,%d)",value,style,color1,color2);
    return NULL;
}

jint GetAppTypeVersion()
{
    debugPrintf("GetAppTypeVersion(3)");
    return 3;
}

int fps=0;
void Native_SetFps(jmethodID id, va_list args) {
    fps = va_arg(args, int);
    debugPrintf("Native_SetFps(%d)",fps);
}

void BGMVolume(jmethodID id, va_list args) {
     
    debugPrintf("BGMVolume()");
}

void SoundPlay(jmethodID id, va_list args) {
    debugPrintf("SoundPlay()");
}

void SoundRelease(jmethodID id, va_list args) {
    debugPrintf("SoundRelease()");
}

void SoundStop(jmethodID id, va_list args) {
    debugPrintf("SoundStop()");
}
void SoundLoad(jmethodID id, va_list args) {
    debugPrintf("SoundLoad()");
}

void SoundVolume(jmethodID id, va_list args) {
    debugPrintf("SoundVolume()");
}

void Native_RunInKeyPad(jmethodID id, va_list args) {
    debugPrintf("Native_RunInKeyPad()");
}
void Native_StartAdBanner(jmethodID id, va_list args) {
    debugPrintf("Native_StartAdBanner()");
}
void Native_StopAdBanner(jmethodID id, va_list args) {
    debugPrintf("Native_StopAdBanner()");
}
void BGMLoad(jmethodID id, va_list args) {
    debugPrintf("BGMLoad()");
}
void BGMRelease(jmethodID id, va_list args) {
    debugPrintf("BGMRelease()");
}

void BGMPlay(jmethodID id, va_list args) {
    debugPrintf("BGMPlay()");
}

void Native_ExitPopup(jmethodID id, va_list args) {
    debugPrintf("Native_ExitPopup()");
}

jobject GetRealDeviceMacAddress(jmethodID id, va_list args) {
    debugPrintf("GetRealDeviceMacAddress(02:00:00:00:00:00)");
    return "02:00:00:00:00:00";
}

void BGMStop(jmethodID id, va_list args) {
    debugPrintf("BGMStop()");
}

void Native_DrawTip(jmethodID id, va_list args) {
    debugPrintf("Native_DrawTip()");
}

void Native_SetPauseBar(jmethodID id, va_list args) {
    int state = va_arg(args, int);

    debugPrintf("Native_SetPauseBar(%d)",state);
}

void OnVibrator(jmethodID id, va_list args)
{
    debugPrintf("OnVibrator()");
}

void Native_ShowOffer(jmethodID id, va_list args)
{
    debugPrintf("Native_ShowOffer()");
}

void Native_callNxPlay(jmethodID id, va_list args)
{
    int type = va_arg(args, int);
    Login();
    debugPrintf("Native_callNxPlay(%d)",type);
}

void surfaceChanged(jmethodID id, va_list args)
{
    debugPrintf("surfaceChanged()");
}

NameToMethodID nameToMethodId[] = {
    { 1, "Native_Exit", METHOD_TYPE_VOID ,Native_Exit}, // ()V
    { 2, "Native_tryBuyItem", METHOD_TYPE_VOID ,Native_tryBuyItem}, // ()V
    { 3, "BGMPlay", METHOD_TYPE_VOID ,BGMPlay}, // (I)V
    { 4, "SoundPlay", METHOD_TYPE_VOID ,SoundPlay}, // (I)V
    { 5, "SoundRelease", METHOD_TYPE_LONG ,SoundRelease}, // ()J
    { 6, "SoundStop", METHOD_TYPE_INT,SoundStop }, // ()I
    { 7, "SoundLoad", METHOD_TYPE_VOID ,SoundLoad}, // (Ljava/lang/String;)V
    { 8, "SoundVolume", METHOD_TYPE_OBJECT ,SoundVolume}, // ()Ljava/lang/String;
    { 9, "Native_StartAdBanner", METHOD_TYPE_OBJECT,Native_StartAdBanner }, // ()Ljava/lang/String;
    { 10, "Native_RunInKeyPad", METHOD_TYPE_OBJECT ,Native_RunInKeyPad}, // ()Ljava/lang/String;
    { 11, "Native_StopAdBanner", METHOD_TYPE_FLOAT ,Native_StopAdBanner}, // ()F
    { 12, "BGMVolume", METHOD_TYPE_OBJECT ,BGMVolume}, // ()Ljava/lang/String;
    { 13, "BGMStop", METHOD_TYPE_VOID,BGMStop }, // (I)V
    { 14, "Native_ExitPopup", METHOD_TYPE_INT,Native_ExitPopup }, // ()I
    { 15, "OnVibrator", METHOD_TYPE_INT ,OnVibrator}, // ([BII)I
    { 16, "GetRealDeviceMacAddress", METHOD_TYPE_INT,GetRealDeviceMacAddress }, // ([BII)I
    { 17, "BGMRelease", METHOD_TYPE_INT ,BGMRelease}, // ([BII)I
    { 18, "GetTextTexture", METHOD_TYPE_INT ,GetTextTexture}, // ([BII)I
    { 19, "GetTextHeight", METHOD_TYPE_INT ,GetTextHeight}, // ([BII)I
    { 20, "GetTextWidth", METHOD_TYPE_INT ,GetTextWidth}, // ([BII)I
    { 21, "Native_SetFps", METHOD_TYPE_INT ,Native_SetFps}, // ([BII)I
    { 22, "GetCurrentTime", METHOD_TYPE_LONG ,NULL}, // ([BII)I
    { 23, "BGMLoad", METHOD_TYPE_INT ,BGMLoad}, // ([BII)I
    { 24, "GetAppTypeVersion", METHOD_TYPE_INT ,GetAppTypeVersion}, // ([BII)I
    { 25, "Native_GetHeapTotalMemory", METHOD_TYPE_INT ,NULL}, // ([BII)I
    { 26, "Native_GetHeapFreeMemory", METHOD_TYPE_INT ,Native_GetHeapFreeMemory}, // ([BII)I
    { 27, "Native_InvalidData", METHOD_TYPE_INT ,Native_InvalidData}, // ([BII)I
    { 28, "Native_ShowOffer", METHOD_TYPE_INT ,Native_ShowOffer}, // ([BII)I
    { 29, "Native_callNxPlay", METHOD_TYPE_INT ,Native_callNxPlay}, // ([BII)I
    { 30, "Native_DrawTip", METHOD_TYPE_INT ,Native_DrawTip}, // ([BII)I
    { 31, "Native_GetPhoneNumber", METHOD_TYPE_INT ,Native_GetPhoneNumber}, // ([BII)I
    { 32, "Native_SetPauseBar", METHOD_TYPE_INT ,Native_SetPauseBar}, // ([BII)I
    { 33, "surfaceChanged", METHOD_TYPE_VOID ,surfaceChanged},
};

MethodsBoolean methodsBoolean[] = {};
MethodsByte methodsByte[] = {};
MethodsChar methodsChar[] = {};
MethodsDouble methodsDouble[] = {

};
MethodsFloat methodsFloat[] = {

};
MethodsInt methodsInt[] = {};
MethodsLong methodsLong[] = {
    {22,GetCurrentTime},
    {25,Native_GetHeapTotalMemory}
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
const int SDK_INT = 19; // Android 4.4 / KitKat
const int descriptor = 110; // Android 4.4 / KitKat
NameToFieldID nameToFieldId[] = {
		{ 0, "WINDOW_SERVICE", FIELD_TYPE_OBJECT }, 
		{ 1, "SDK_INT", FIELD_TYPE_INT },
        { 2, "descriptor", FIELD_TYPE_INT },
};

FieldsBoolean fieldsBoolean[] = {};
FieldsByte fieldsByte[] = {};
FieldsChar fieldsChar[] = {};
FieldsDouble fieldsDouble[] = {};
FieldsFloat fieldsFloat[] = {};
FieldsInt fieldsInt[] = {
		{ 1, SDK_INT },
        { 2, descriptor },
};
FieldsObject fieldsObject[] = {
		{ 0, WINDOW_SERVICE },
};
FieldsLong fieldsLong[] = {};
FieldsShort fieldsShort[] = {};

__FALSOJNI_IMPL_CONTAINER_SIZES
