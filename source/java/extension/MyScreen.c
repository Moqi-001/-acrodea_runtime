#include <stdarg.h>

#include <falso_jni/FalsoJNI.h>

#include "utils/logger.h"

/*
 * 包名: com/acrodea/vividruntime/launcher/extension/MyScreen
 */

#define WIDTH_PIXELS  960
#define HEIGHT_PIXELS 544

#define DENSITY       0
#define DENSITY_DPI   0
#define X_DENSITY_DPI 0
#define Y_DENSITY_DPI 0

// "getWidthPixels", "()I"
jint MyScreen_getWidthPixels(jobject obj, jmethodID id, va_list args) {
    int res = WIDTH_PIXELS;
    l_debug("[JNI] MyScreen::getWidthPixels: %f", res);
    return (jint)res;
}

// "getHeightPixels", "()I"
jint MyScreen_getHeightPixels(jobject obj, jmethodID id, va_list args) {
    int res = WIDTH_PIXELS;
    l_debug("[JNI] MyScreen::getHeightPixels: %d", res);
    return (jint)res;
}

// "getDensity", "()F"
jfloat MyScreen_getDensity(jobject obj, jmethodID id, va_list args) {
    float res = DENSITY;
    l_debug("[JNI] MyScreen::getDensity: %f", res);
    return (jfloat)res;
}

// "getDensityDpi", "()I"
jint MyScreen_getDensityDpi(jobject obj, jmethodID id, va_list args) {
    int res = DENSITY_DPI;
    l_debug("[JNI] MyScreen::getDensityDpi: %d", res);
    return (jint)res;
}

// "getXDensityDpi", "()F"
jfloat MyScreen_getXDensityDpi(jobject obj, jmethodID id, va_list args) {
    float res = X_DENSITY_DPI;
    l_debug("[JNI] MyScreen::getXDensityDpi: %f", res);
    return (jfloat)res;
}

// "getYDensityDpi", "()F"
jfloat MyScreen_getYDensityDpi(jobject obj, jmethodID id, va_list args) {
    float res = Y_DENSITY_DPI;
    l_debug("[JNI] MyScreen::getYDensityDpi: %f", res);
    return (jfloat)res;
}
