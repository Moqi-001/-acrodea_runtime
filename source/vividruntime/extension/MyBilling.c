#include <stdint.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "MyBilling.h"

struct _MyBilling {
    uint32_t dummy;
};

static JavaClass sClass = NULL;

// 安卓原版 MyBilling.getUserId()：
//   1) 读本地 SharedPreferences 缓存的 "userid"（密钥 "adlr3ve0bke92jfa"），有则直接返回；
//   2) 没有则 HTTP 请求 G-gee 平台 API，解析 JSON {stat:"ok", userNo:"..."}，
//      成功后缓存到本地并返回 userNo，失败返回 ""（此时上层报 "Couldn't obtain User ID"）。
// 模拟器环境无 G-gee 平台服务，模拟"本地已缓存"分支，返回固定 userid。
// 保持固定值：若游戏用 userid 参与存档加密，固定 ID 可保证存档跨运行一致。
JavaString MyBilling_getUserId(MyBilling thiz) {
    const char *uid = "1234567890";

    so_logd("[MyBilling] getUserId(): %s", uid);
    return JavaNewStringUTF(uid);
}

// 安卓原版 MyBilling.start(type, accessToken, paymentId)：
//   弹出 BillingActivity WebView 完成内购。模拟器无计费 UI，
//   返回 -27（与原版 "SDK 版本不支持计费" 同款错误码，游戏按计费不可用处理）。
jint MyBilling_start(MyBilling thiz, jint type, JavaString access_token, JavaString payment_id) {
    so_logd("[MyBilling] start(type=%d): billing unsupported, return -27", type);
    return -27;
}

static JavaSymbol class_symbols[] = {
    // instance methods（对应安卓原版 Java 实现）
    JAVA_SYMBOL_METHOD("getUserId", "()Ljava/lang/String;", &MyBilling_getUserId),
    JAVA_SYMBOL_METHOD("start", "(ILjava/lang/String;Ljava/lang/String;)I", &MyBilling_start),
};

static JavaClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/MyBilling",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _MyBilling),
    .dtor          = NULL,
};

void MyBilling_class_init(void) {
    sClass = JavaCreateClass(&class_meta);
}
