#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <psp2/ctrl.h>
#include <psp2/ime_dialog.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>

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

// vitaGL 内部帧计数（libvitaGL.a 导出），用来判断是否还有别的线程在 swap
extern uint32_t vgl_framecount;

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

static JClass sClass = NULL;

// ---------------------------------------------------------------- UTF 转换
// 不用 dialog.c 里的版本：那份实现把 4 字节 UTF-8（emoji 等）当 3 字节处理，
// 会读到字符串结尾之后。这里的实现带边界与合法性检查。
static void mti_utf8_to_utf16(const char *src, uint16_t *dst, size_t cap) {
    size_t o = 0;
    if (cap == 0) return;

    for (size_t i = 0; src[i] && o + 1 < cap;) {
        uint8_t  c = (uint8_t)src[i];
        uint32_t cp;
        int      n;

        if (c < 0x80) {
            cp = c;
            n  = 1;
        } else if ((c & 0xE0) == 0xC0) {
            cp = c & 0x1F;
            n  = 2;
        } else if ((c & 0xF0) == 0xE0) {
            cp = c & 0x0F;
            n  = 3;
        } else if ((c & 0xF8) == 0xF0) {
            cp = c & 0x07;
            n  = 4;
        } else {
            i++;
            continue;
        }

        int ok = 1;
        for (int k = 1; k < n; k++) {
            if ((src[i + k] & 0xC0) != 0x80) {
                ok = 0;
                break;
            }
            cp = (cp << 6) | (src[i + k] & 0x3F);
        }
        if (!ok) {
            i++;
            continue;
        }
        i += n;

        if (cp < 0x10000) {
            dst[o++] = (uint16_t)cp;
        } else if (o + 2 < cap) {
            cp -= 0x10000;
            dst[o++] = (uint16_t)(0xD800 + (cp >> 10));
            dst[o++] = (uint16_t)(0xDC00 + (cp & 0x3FF));
        } else {
            break;
        }
    }
    dst[o] = 0;
}

static void mti_utf16_to_utf8(const uint16_t *src, char *dst, size_t cap) {
    size_t o = 0;
    if (cap == 0) return;

    for (size_t i = 0; src[i] && o + 1 < cap; i++) {
        uint32_t cp = src[i];

        if (cp >= 0xD800 && cp <= 0xDBFF && src[i + 1] >= 0xDC00 && src[i + 1] <= 0xDFFF) {
            cp = 0x10000 + ((cp - 0xD800) << 10) + (src[i + 1] - 0xDC00);
            i++;
        }

        if (cp < 0x80) {
            dst[o++] = (char)cp;
        } else if (cp < 0x800) {
            if (o + 2 >= cap) break;
            dst[o++] = (char)(0xC0 | (cp >> 6));
            dst[o++] = (char)(0x80 | (cp & 0x3F));
        } else if (cp < 0x10000) {
            if (o + 3 >= cap) break;
            dst[o++] = (char)(0xE0 | (cp >> 12));
            dst[o++] = (char)(0x80 | ((cp >> 6) & 0x3F));
            dst[o++] = (char)(0x80 | (cp & 0x3F));
        } else {
            if (o + 4 >= cap) break;
            dst[o++] = (char)(0xF0 | (cp >> 18));
            dst[o++] = (char)(0x80 | ((cp >> 12) & 0x3F));
            dst[o++] = (char)(0x80 | ((cp >> 6) & 0x3F));
            dst[o++] = (char)(0x80 | (cp & 0x3F));
        }
    }
    dst[o] = 0;
}

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
    thiz->confirmed  = 0;
    thiz->status     = 0;
}

jint MyTextInput_setPropertycv(MyTextInput thiz, jint prop, JString value) {
    const char *v = value ? String_getUTFChars(value) : "";

    so_loga("[MyTextInput] setPropertycv(prop=%d, value='%s')", prop, v ? v : "(null)");

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

static uint64_t mti_now_us(void) {
    return sceKernelGetProcessTimeWide();
}

// 推进一帧对话框。
//
// 游戏的主循环（main → InputName → myTextInputHandler → doModal）就是渲染线程
// 本身，卡在这里时游戏一帧都不会画。而 sceCommonDialogUpdate 只负责把对话框
// “叠”到后缓冲上，它不会清屏；没有背景重绘时，会动的对话框（尤其是退场下滑）
// 会在每个缓冲里留下它两帧前的位置，看起来就是一串拖尾残影。
// 所以由本线程驱动时必须先自己铺一帧干净背景，再让系统合成对话框。
static void mti_present_dialog_frame(void) {
    // 用完把游戏的清屏色还回去，避免黑色渗到之后的正常渲染里
    GLfloat saved_clear[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, saved_clear);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(saved_clear[0], saved_clear[1], saved_clear[2], saved_clear[3]);

    vglSwapBuffers(GL_TRUE);
}

jint MyTextInput_doModal(MyTextInput thiz) {
    static uint16_t title_u16[SCE_IME_DIALOG_MAX_TITLE_LENGTH];
    static uint16_t initial_u16[SCE_IME_DIALOG_MAX_TEXT_LENGTH];
    static uint16_t input_u16[SCE_IME_DIALOG_MAX_TEXT_LENGTH + 1];

    static int in_modal = 0;
    if (in_modal) {
        so_loge("[MyTextInput] doModal(): already open, return -8");
        return -8;
    }

    memset(title_u16, 0, sizeof(title_u16));
    memset(initial_u16, 0, sizeof(initial_u16));
    memset(input_u16, 0, sizeof(input_u16));

    mti_utf8_to_utf16(thiz->title, title_u16, SCE_IME_DIALOG_MAX_TITLE_LENGTH);
    mti_utf8_to_utf16(thiz->text, initial_u16, SCE_IME_DIALOG_MAX_TEXT_LENGTH);

    // 最大长度：用游戏给的值（1000），无效时退回 64
    SceUInt32 max_len = thiz->max_length;
    if (max_len <= 0 || max_len >= SCE_IME_DIALOG_MAX_TEXT_LENGTH)
        max_len = SCE_IME_DIALOG_MAX_TEXT_LENGTH - 1;

    SceUInt32 type     = SCE_IME_TYPE_DEFAULT;
    SceUInt32 text_box = SCE_IME_DIALOG_TEXTBOX_MODE_DEFAULT;
    SceUInt32 option   = 0;

    switch (thiz->input_type) {
        case 2000:  // password
            text_box = SCE_IME_DIALOG_TEXTBOX_MODE_PASSWORD;
            break;
        case 2001:  // multi
            option = SCE_IME_OPTION_MULTILINE;
            break;
        case 2002:  // email
            type = SCE_IME_TYPE_MAIL;
            break;
        case 2003:  // number
            type = SCE_IME_TYPE_NUMBER;
            break;
        default:
            break;
    }

    SceImeDialogParam param;
    sceImeDialogParamInit(&param);
    param.supportedLanguages = 0x0001FFFF;
    param.languagesForced    = SCE_TRUE;
    param.type               = type;
    param.option             = option;
    param.dialogMode         = SCE_IME_DIALOG_DIALOG_MODE_WITH_CANCEL;
    param.textBoxMode        = text_box;
    param.title              = title_u16;
    param.maxTextLength      = max_len;
    param.initialText        = initial_u16;
    param.inputTextBuffer    = input_u16;

    so_loga("[MyTextInput] doModal(): type=%u textbox=%u max=%u title='%s' initial='%s'",
            type, text_box, max_len, thiz->title, thiz->text);
    so_loga("[MyTextInput] doModal(): initial u16: %04X %04X %04X %04X %04X %04X %04X %04X",
            initial_u16[0], initial_u16[1], initial_u16[2], initial_u16[3],
            initial_u16[4], initial_u16[5], initial_u16[6], initial_u16[7]);

    sceImeDialogInit(&param);

    for (;;) {
        int status = sceImeDialogGetStatus();
        if (status == SCE_COMMON_DIALOG_STATUS_FINISHED) {
            break;
        }
        mti_present_dialog_frame();
        sceKernelDelayThread(10000);
    }

    SceImeDialogResult result;
    memset(&result, 0, sizeof(result));
    sceImeDialogGetResult(&result);
    sceImeDialogTerm();
    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG_WIDE);

    if (result.button == SCE_IME_DIALOG_BUTTON_ENTER) {
        char utf8[MTI_TEXT_MAX];
        memset(utf8, 0, sizeof(utf8));
        mti_utf16_to_utf8(input_u16, utf8, sizeof(utf8));
        // 输入为空则保留预填文本
        snprintf(thiz->result, sizeof(thiz->result), "%s", utf8[0] ? utf8 : thiz->text);
        thiz->confirmed = 1;
        so_loga("[MyTextInput] doModal(): ENTER, text='%s'", thiz->result);
        return 0;
    }

    // 取消：保留打开键盘时的预填文本
    snprintf(thiz->result, sizeof(thiz->result), "%s", thiz->text);
    thiz->confirmed = 0;
    so_loga("[MyTextInput] doModal(): CANCELLED, keep text='%s'", thiz->result);
    return 1;
}

JString MyTextInput_getText(MyTextInput thiz) {
    so_loga("[MyTextInput] getText(): '%s'", thiz->result);
    return String_newUTF(thiz->result);
}

jint MyTextInput_destroy(MyTextInput thiz) {
    so_loga("[MyTextInput] destroy()");
    reset_state(thiz);
    return 0;
}

static JSymbol class_symbols[] = {
    // instance methods
    JSYMBOL_METHOD("setPropertycv", "(ILjava/lang/String;)I", &MyTextInput_setPropertycv),
    JSYMBOL_METHOD("doModal", "()I", &MyTextInput_doModal),
    JSYMBOL_METHOD("getText", "()Ljava/lang/String;", &MyTextInput_getText),
    JSYMBOL_METHOD("destroy", "()I", &MyTextInput_destroy),
};

static JClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/MyTextInput",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _MyTextInput),
    .dtor          = NULL,
};

void setup_MyTextInput(void) {
    sClass = JavaCreateClass(&class_meta);
}
