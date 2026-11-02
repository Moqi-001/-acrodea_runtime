#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <psp2/ctrl.h>
#include <psp2/ime_dialog.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/sysmodule.h>

#include <vitaGL.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "MyTextInput.h"

// com.acrodea.vividruntime.launcher.extension.MyTextInput：
//     setPropertycv(0, message)  -> 对话框正文
//     setPropertycv(1, title)    -> 对话框标题
//     setPropertycv(2, text)     -> 编辑框预填文本
//     setPropertycv(1000, len)   -> 最大输入长度
//     setPropertycv(1001, type)  -> 输入类型：password/multi/email/number/image
//     setPropertycv(1002, path)  -> image 模式的图片路径
//     setPropertycv(1003, bool)  -> 是否自动弹软键盘
//     doModal()                  -> 阻塞等用户输入，按 OK 返回 0，取消返回 1，重复打开返回 -8
//     getText()                  -> 确认后返回输入文本；未确认返回 null

#define MTI_TEXT_MAX 256

struct _MyTextInput {
    int  status;
    int  input_type;             // vg  输入类型 2000~2004
    int  confirmed;              // be  是否已确认
    char message[MTI_TEXT_MAX];  // bb  对话框正文
    char title[128];             // bc  对话框标题
    char text[MTI_TEXT_MAX];     // vc  预填文本
    char result[MTI_TEXT_MAX];   // bd  确认后的输入结果
    int  max_length;             // vf  最大输入长度
};

static JavaClass sClass = NULL;

extern void _utf16_to_utf8(const uint16_t *src, uint8_t *dst);
extern void _utf8_to_utf16(const uint8_t *src, uint16_t *dst);

// 原版会把 "&#44;" 还原为 ','（逗号在参数串里被转义了）
static void copy_unescape(char *dst, size_t dst_size, const char *src) {
    size_t o = 0;
    for (size_t i = 0; src[i] && o + 1 < dst_size; i++) {
        if (src[i] == '&' && strncmp(&src[i], "&#44;", 5) == 0) {
            dst[o++] = ',';
            i += 4;
        } else {
            dst[o++] = src[i];
        }
    }
    dst[o] = '\0';
}

static void reset_state(MyTextInput thiz) {
    thiz->message[0] = '\0';
    thiz->title[0]   = '\0';
    thiz->text[0]    = '\0';
    thiz->result[0]  = '\0';
    thiz->max_length = 0;
    thiz->input_type = 0;
}

jint MyTextInput_setPropertycv(MyTextInput thiz, jint prop, JavaString value) {
    const char *v = value ? JavaGetStringUTFChars(value) : "";

    so_loga("[MyTextInput] setPropertycv(%d, '%s')", prop, v ? v : "(null)");

    if (v) {
        switch (prop) {
            case 0:
                copy_unescape(thiz->message, sizeof(thiz->message), v);
                break;
            case 1:
                copy_unescape(thiz->title, sizeof(thiz->title), v);
                break;
            case 2:
                copy_unescape(thiz->text, sizeof(thiz->text), v);
                break;
            case 1000:
                thiz->max_length = atoi(v);
                break;
            case 1001:
                if (strcasecmp(v, "password") == 0)
                    thiz->input_type = 2000;
                else if (strcasecmp(v, "multi") == 0)
                    thiz->input_type = 2001;
                else if (strcasecmp(v, "email") == 0)
                    thiz->input_type = 2002;
                else if (strcasecmp(v, "number") == 0)
                    thiz->input_type = 2003;
                else if (strcasecmp(v, "image") == 0)
                    thiz->input_type = 2004;
                break;
            case 1002:
                break;  // image 路径，仅图片预览用
            case 1003:
                break;  // 自动弹键盘开关，系统键盘自带
            default:
                break;
        }
    }
    return 0;
}

jint MyTextInput_doModal(MyTextInput thiz) {
    static uint16_t osk_title_utf16[SCE_IME_DIALOG_MAX_TITLE_LENGTH];
    static uint16_t osk_initial_utf16[SCE_IME_DIALOG_MAX_TEXT_LENGTH];
    static uint16_t osk_input_utf16[SCE_IME_DIALOG_MAX_TEXT_LENGTH];

    // 已有对话框打开时返回 -8
    // if (thiz->status) return -8;

    _utf8_to_utf16((const uint8_t *)thiz->title, osk_title_utf16);
    _utf8_to_utf16((const uint8_t *)thiz->text, osk_initial_utf16);
    memset(osk_input_utf16, 0, sizeof(osk_input_utf16));

    SceImeDialogParam param;
    sceImeDialogParamInit(&param);
    param.supportedLanguages = 0x0001FFFF;
    param.languagesForced    = SCE_TRUE;
    param.type               = SCE_IME_TYPE_BASIC_LATIN;
    param.dialogMode         = SCE_IME_DIALOG_DIALOG_MODE_WITH_CANCEL;
    param.textBoxMode        = SCE_IME_DIALOG_TEXTBOX_MODE_DEFAULT;
    param.title              = osk_title_utf16;
    param.maxTextLength      = 64;
    param.initialText        = osk_initial_utf16;
    param.inputTextBuffer    = osk_input_utf16;

    int ret = sceImeDialogInit(&param);
    if (ret < 0) {
        so_loge("[MyTextInput] sceImeDialogInit failed: 0x%08X, treat as cancel", ret);
        snprintf(thiz->result, sizeof(thiz->result), "%s", thiz->text);
        return 1;
    }
    thiz->status = 1;
    so_loga("[MyTextInput] doModal: dialog opened");

    while (sceImeDialogGetStatus() != SCE_COMMON_DIALOG_STATUS_FINISHED) {
        vglSwapBuffers(GL_TRUE);
        sceKernelDelayThread(10000);
    }

    SceImeDialogResult result;
    memset(&result, 0, sizeof(result));
    sceImeDialogGetResult(&result);
    sceImeDialogTerm();
    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG_WIDE);
    thiz->status = 0;

    if (result.button == SCE_IME_DIALOG_BUTTON_ENTER) {
        // 确认：用键盘输入的文本（输入为空则保留预填文本）
        char utf8[MTI_TEXT_MAX];
        _utf16_to_utf8(osk_input_utf16, (uint8_t *)utf8);
        snprintf(thiz->result, sizeof(thiz->result), "%s", utf8[0] ? utf8 : thiz->text);
        so_loga("[MyTextInput] doModal(): ENTER, text='%s'", thiz->result);
        return 0;
    }

    // 取消：保留打开键盘时的预填文本
    snprintf(thiz->result, sizeof(thiz->result), "%s", thiz->text);
    so_loga("[MyTextInput] doModal(): CANCELLED, keep text='%s'", thiz->result);
    return 1;
}

JavaString MyTextInput_getText(MyTextInput thiz) {
    so_loga("[MyTextInput] getText(): '%s'", thiz->result);
    return JavaNewStringUTF(thiz->result);
}

jint MyTextInput_destroy(MyTextInput thiz) {
    so_loga("[MyTextInput] destroy()");
    reset_state(thiz);
    return 0;
}

static JavaSymbol class_symbols[] = {
    // instance methods
    JAVA_SYMBOL_METHOD("setPropertycv", "(ILjava/lang/String;)I", &MyTextInput_setPropertycv),
    JAVA_SYMBOL_METHOD("doModal", "()I", &MyTextInput_doModal),
    JAVA_SYMBOL_METHOD("getText", "()Ljava/lang/String;", &MyTextInput_getText),
    JAVA_SYMBOL_METHOD("destroy", "()I", &MyTextInput_destroy),
};

static JavaClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/MyTextInput",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _MyTextInput),
    .dtor          = NULL,
};

void MyTextInput_class_init(void) {
    sClass = JavaCreateClass(&class_meta);
}
