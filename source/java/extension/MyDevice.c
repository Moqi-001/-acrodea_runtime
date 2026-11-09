#include <stdint.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "MyDevice.h"

struct _MyDevice {
    uint32_t dummy;
};

static JClass sClass = NULL;

JString MyDevice_getImei(MyDevice thiz) {
    const char *imei = "MACW:0026e879f969";  // 用于解密 .dat 的密钥

    so_logd("[MyDevice] getImei(): %s", imei);
    return String_newUTF(imei);
}

JString MyDevice_getImsi(MyDevice thiz) {
    const char *imsi = "460010123456789"; // 伪造值

    so_logd("[MyDevice] getImsi(): %s", imsi);
    return String_newUTF(imsi);
}

jint MyDevice_setNavigationHiddenEnable(jint enabled) {
    jint result = 0;
    so_logd("[MyDevice] setNavigationHiddenEnable(%d): %d", enabled, result);
    return result;
}

jint MyDevice_getNavigationHiddenState() {
    jint result = 0;
    so_logd("[MyDevice] getNavigationHiddenState(): %d", result);
    return result;
}

JString MyDevice_getPlatformInfo() {
    const char *result = "Psvita";
    so_logd("[MyDevice] getPlatformInfo(): %s", result);
    return String_newUTF(result);
}

jint MyDevice_setMutedEnable(jint enabled) {
    jint result = 0;
    so_logd("[MyDevice] setMutedEnable(%d): %d", enabled, result);
    return result;
}

jint MyDevice_getMutedState() {
    jint result = 0;
    so_logd("[MyDevice] getMutedState(): %d", result);
    return result;
}

static JSymbol class_symbols[] = {
    // JAVA
    JSYMBOL_METHOD("getImei", "()Ljava/lang/String;", &MyDevice_getImei),
    JSYMBOL_METHOD("getImsi", "()Ljava/lang/String;", &MyDevice_getImsi),
    JSYMBOL_METHOD("setNavigationHiddenEnable", "(I)I", &MyDevice_setNavigationHiddenEnable),
    JSYMBOL_METHOD("getNavigationHiddenState", "()I", &MyDevice_getNavigationHiddenState),
    JSYMBOL_METHOD("getPlatformInfo", "()Ljava/lang/String;", &MyDevice_getPlatformInfo),
    JSYMBOL_METHOD("setMutedEnable", "(I)I", &MyDevice_setMutedEnable),
    JSYMBOL_METHOD("getMutedState", "()I", &MyDevice_getMutedState),
};

static JClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/MyDevice",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _MyDevice),
    .dtor          = NULL,
};

void setup_MyDevice(void) {
    sClass = JavaCreateClass(&class_meta);
}
