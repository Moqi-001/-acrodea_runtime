#include <falso_jni/FalsoJNI.h>
#include <math.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/clib.h>
#include <psp2/touch.h>
#include <so_util/so_util.h>
#include <stdint.h>
#include "config.h"
#include "utils/logger.h"
#include "utils/settings.h"

#define SCREEN_W 960
#define SCREEN_H 544

#define AKEYCODE_DPAD_UP       19
#define AKEYCODE_DPAD_DOWN     20
#define AKEYCODE_DPAD_LEFT     21
#define AKEYCODE_DPAD_RIGHT    22
#define AKEYCODE_BUTTON_A      96
#define AKEYCODE_BUTTON_B      97
#define AKEYCODE_BUTTON_X      99
#define AKEYCODE_BUTTON_Y      100
#define AKEYCODE_BUTTON_L1     102
#define AKEYCODE_BUTTON_R1     103
#define AKEYCODE_BUTTON_START  108
#define AKEYCODE_BUTTON_SELECT 109

typedef struct {
    uint32_t sce_button;
    uint32_t android_button;
    int      repeat_count;
} ButtonMapping;

typedef struct {
    uint32_t sce_button;
    uint32_t x;
    uint32_t y;
} ButtonToTouchMapping;

static ButtonMapping mapping[] = {
    {SCE_CTRL_UP, AKEYCODE_DPAD_UP, 0},
    {SCE_CTRL_DOWN, AKEYCODE_DPAD_DOWN, 0},
    {SCE_CTRL_LEFT, AKEYCODE_DPAD_LEFT, 0},
    {SCE_CTRL_RIGHT, AKEYCODE_DPAD_RIGHT, 0},
    {SCE_CTRL_CROSS, AKEYCODE_BUTTON_B, 0},
    {SCE_CTRL_CIRCLE, AKEYCODE_BUTTON_A, 0},
    {SCE_CTRL_SQUARE, AKEYCODE_BUTTON_X, 0},
    {SCE_CTRL_TRIANGLE, AKEYCODE_BUTTON_Y, 0},
    {SCE_CTRL_LTRIGGER, AKEYCODE_BUTTON_L1, 0},
    {SCE_CTRL_RTRIGGER, AKEYCODE_BUTTON_R1, 0},
    {SCE_CTRL_START, AKEYCODE_BUTTON_START, 0},
    {SCE_CTRL_SELECT, AKEYCODE_BUTTON_SELECT, 0},
};

int (*navite_touchDown)(void *env, void *obj, int action, int x, int y, int index);
int (*navite_touchMove)(void *env, void *obj, int action, int x, int y);
int (*navite_touchUp)(void *env, void *obj, int action, int x, int y, int index);
int (*navite_keyDown)(void *env, void *obj, int keyCode, int unicodeChar, int repeatCount);
int (*navite_keyUp)(void *env, void *obj, int keyCode, int unicodeChar, int repeatCount);

void controls_init() {
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);
}

void controls_poll() {
    static uint32_t old_buttons = 0;
    static int      lastX[2]    = {-1, -1};
    static int      lastY[2]    = {-1, -1};
    SceCtrlData     pad;
    SceTouchData    touch;

    sceCtrlPeekBufferPositive(0, &pad, 1);
    sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);

    // Button
    if (pad.lx > 162) {
        pad.buttons |= SCE_CTRL_RIGHT;
    } else if (pad.lx < 92) {
        pad.buttons |= SCE_CTRL_LEFT;
    }
    if (pad.ly > 162) {
        pad.buttons |= SCE_CTRL_DOWN;
    } else if (pad.ly < 92) {
        pad.buttons |= SCE_CTRL_UP;
    }
    for (int i = 0; i < sizeof(mapping) / sizeof(ButtonMapping); i++) {
        int cur_pressed = ((pad.buttons & mapping[i].sce_button) == mapping[i].sce_button);
        int old_pressed = ((old_buttons & mapping[i].sce_button) == mapping[i].sce_button);
        if (cur_pressed) {
            if (!old_pressed)
                navite_keyDown(0, 0, mapping[i].android_button, 0, mapping[i].repeat_count);
            mapping[i].repeat_count++;
        } else {
            if (old_pressed)
                navite_keyUp(0, 0, mapping[i].android_button, 0, mapping[i].repeat_count);
            mapping[i].repeat_count = 0;
        }
    }
    old_buttons = pad.buttons;

    // Touch
    for (int i = 0; i < 2; i++) {
        if (i < touch.reportNum) {
            int x = (int)((float)touch.report[i].x * (float)SCREEN_W / 1920.0f);
            int y = (int)((float)touch.report[i].y * (float)SCREEN_H / 1088.0f);

            if (lastX[i] == -1 || lastY[i] == -1) {
                navite_touchDown(0, 0, 0, x, y, i);
            } else if (lastX[i] != x || lastY[i] != y) {
                navite_touchMove(0, 0, 0, x, y);
            }
            lastX[i] = x;
            lastY[i] = y;
        } else {
            if (lastX[i] != -1 || lastY[i] != -1) {
                navite_touchUp(0, 0, 0, lastX[i], lastY[i], i);
            }
            lastX[i] = -1;
            lastY[i] = -1;
        }
    }
}

int so_patch_controls(so_module *mod) {
    navite_touchDown = (void *)(mod->text_base + 0x000401D0 + 1);
    navite_touchMove = (void *)(mod->text_base + 0x000401FC + 1);
    navite_touchUp   = (void *)(mod->text_base + 0x000401A4 + 1);
    navite_keyDown   = (void *)(mod->text_base + 0x00040234 + 1);
    navite_keyUp     = (void *)(mod->text_base + 0x00040218 + 1);

    return 0;
}