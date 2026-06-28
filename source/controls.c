#include <math.h>
#include <stdint.h>

#include <psp2/ctrl.h>
#include <psp2/kernel/clib.h>
#include <psp2/touch.h>

#include "android/keycodes.h"
#include "vividruntime/Runtime.h"

#include "config.h"

typedef struct ButtonRemap {
    uint32_t native;
    uint32_t mapped;
    uint32_t repeat_count;
} ButtonRemap;

typedef struct TouchInfo {
    int   id;
    float currX;
    float currY;
    float prevX;
    float prevY;
} TouchInfo;

typedef struct ButtonToTouch {
    uint32_t  button;
    int       map_x;
    int       map_y;
    TouchInfo info;
} ButtonToTouch;

#define JOYSTICK_CENTER      127.0f
#define JOYSTICK_DEADZONE    24.0f
#define JOYSTICK_IS_MOVED(v) (((v) > JOYSTICK_CENTER + JOYSTICK_DEADZONE) || \
                              ((v) < JOYSTICK_CENTER - JOYSTICK_DEADZONE))

#define JOYSTICK_SCREEN_CENTER_X   152.0f
#define JOYSTICK_SCREEN_CENTER_Y   400.0f
#define JOYSTICK_SCREEN_MAX_RADIUS 70.0f

static ButtonRemap btn_remap[] = {
    {SCE_CTRL_UP,       AKEYCODE_DPAD_UP,       0},
    {SCE_CTRL_DOWN,     AKEYCODE_DPAD_DOWN,     0},
    {SCE_CTRL_LEFT,     AKEYCODE_DPAD_LEFT,     0},
    {SCE_CTRL_RIGHT,    AKEYCODE_DPAD_RIGHT,    0},
    {SCE_CTRL_CROSS,    AKEYCODE_BUTTON_B,      0},
    {SCE_CTRL_CIRCLE,   AKEYCODE_BUTTON_A,      0},
    {SCE_CTRL_SQUARE,   AKEYCODE_BUTTON_X,      0},
    {SCE_CTRL_TRIANGLE, AKEYCODE_BUTTON_Y,      0},
    {SCE_CTRL_LTRIGGER, AKEYCODE_BUTTON_L1,     0},
    {SCE_CTRL_RTRIGGER, AKEYCODE_BUTTON_R1,     0},
    {SCE_CTRL_START,    AKEYCODE_BUTTON_START,  0},
    {SCE_CTRL_SELECT,   AKEYCODE_BUTTON_SELECT, 0},
};

static ButtonToTouch btn2touch[] = {
    // {SCE_CTRL_UP,       152.0f, 330.0f, {0, -1, -1, -1, -1}},
    // {SCE_CTRL_DOWN,     152.0f, 470.0f, {0, -1, -1, -1, -1}},
    // {SCE_CTRL_LEFT,     82.0f,  400.0f, {0, -1, -1, -1, -1}},
    // {SCE_CTRL_RIGHT,    222.0f, 400.0f, {0, -1, -1, -1, -1}},
    {SCE_CTRL_CROSS,    866.0f, 240.0f, {0, -1, -1, -1, -1}}, // 切换为行走状态
    {SCE_CTRL_CIRCLE,   866.0f, 344.0f, {0, -1, -1, -1, -1}}, // 开枪
    {SCE_CTRL_SQUARE,   776.0f, 332.0f, {0, -1, -1, -1, -1}}, // 开门/拾取
    {SCE_CTRL_TRIANGLE, 814.0f, 450.0f, {0, -1, -1, -1, -1}}, // 菜刀
    {SCE_CTRL_LTRIGGER, 866.0f, 240.0f, {0, -1, -1, -1, -1}}, // 切换为行走状态
    {SCE_CTRL_RTRIGGER, 866.0f, 344.0f, {0, -1, -1, -1, -1}}, // 开枪
    {SCE_CTRL_START,    92.0f,  146.0f, {0, -1, -1, -1, -1}}, // 说明
    {SCE_CTRL_SELECT,   62.0f,  48.0f,  {0, -1, -1, -1, -1}}, // 菜单
};

#define touchDown(action, x, y, isPrimary)         Runtime_touchDown(action, x, y, isPrimary)
#define touchMove(action, x, y)                    Runtime_touchMove(action, x, y)
#define touchUp(action, x, y, isPrimary)           Runtime_touchUp(action, x, y, isPrimary)
#define keyDown(keyCode, unicodeChar, repeatCount) Runtime_keyDown(keyCode, unicodeChar, repeatCount)
#define keyUp(keyCode, unicodeChar, repeatCount)   Runtime_keyUp(keyCode, unicodeChar, repeatCount)

static int joystick_screen_offset(int v) {
    float off = v - JOYSTICK_CENTER;
    if (off > JOYSTICK_DEADZONE) {
        return (off - JOYSTICK_DEADZONE) / (JOYSTICK_CENTER - JOYSTICK_DEADZONE) * JOYSTICK_SCREEN_MAX_RADIUS;
    }
    if (off < -JOYSTICK_DEADZONE) {
        return (off + JOYSTICK_DEADZONE) / (JOYSTICK_CENTER - JOYSTICK_DEADZONE) * JOYSTICK_SCREEN_MAX_RADIUS;
    }

    return 0;
}

void controls_poll() {
    static TouchInfo touchInfo[2] = {
        {0, -1, -1, -1, -1},
        {1, -1, -1, -1, -1}
    };
    static TouchInfo joyInfo = {0, -1, -1, -1, -1};

    SceCtrlData  pad;
    SceTouchData touch;
    sceCtrlPeekBufferPositive(0, &pad, 1);
    sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);

    int map_port = 0;

    // JoystickToButton
    if (pad.lx < JOYSTICK_CENTER - JOYSTICK_DEADZONE) {
        pad.buttons |= SCE_CTRL_LEFT;
    } else if (pad.lx > JOYSTICK_CENTER + JOYSTICK_DEADZONE) {
        pad.buttons |= SCE_CTRL_RIGHT;
    }
    if (pad.ly < JOYSTICK_CENTER - JOYSTICK_DEADZONE) {
        pad.buttons |= SCE_CTRL_UP;
    } else if (pad.ly > JOYSTICK_CENTER + JOYSTICK_DEADZONE) {
        pad.buttons |= SCE_CTRL_DOWN;
    }

    // ButtonRemap
    for (int i = 0; i < sizeof(btn_remap) / sizeof(btn_remap[0]); i++) {
        int curr_pressed = ((pad.buttons & btn_remap[i].native) == btn_remap[i].native);
        int prev_pressed = (btn_remap[i].repeat_count > 0);

        if (curr_pressed) {
            if (!prev_pressed)
                keyDown(btn_remap[i].mapped, 0, btn_remap[i].repeat_count);
            btn_remap[i].repeat_count++;
        } else {
            if (prev_pressed)
                keyUp(btn_remap[i].mapped, 0, btn_remap[i].repeat_count);
            btn_remap[i].repeat_count = 0;
        }
    }

    // Touch
    for (int i = 0; i < 2; i++) {
        TouchInfo *info       = &touchInfo[i];
        jboolean   is_primary = (map_port == 0);

        if (i < touch.reportNum) {
            info->id    = map_port++;
            info->currX = (float)touch.report[i].x * (float)SCREEN_W / 1920.0f;
            info->currY = (float)touch.report[i].y * (float)SCREEN_H / 1088.0f;

            if (info->prevX == -1 || info->prevY == -1) {
                touchDown(0, info->currX, info->currY, is_primary);
            } else if (info->currX != info->prevX || info->currY != info->prevY) {
                touchMove(0, info->currX, info->currY);
            }
        } else {
            if (info->prevX != -1 || info->prevY != -1) {
                info->id = map_port++;
                touchUp(0, info->prevX, info->prevY, is_primary);
            }
            info->currX = -1;
            info->currY = -1;
        }
        info->prevX = info->currX;
        info->prevY = info->currY;
    }
}
