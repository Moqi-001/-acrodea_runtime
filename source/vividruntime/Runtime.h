#ifndef VIVIDRUNTIME_RUNTIME_H
#define VIVIDRUNTIME_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

#include "android/view/Surface.h"

#include "extension/ExtensionManager.h"
// #include "extension/MyMediaManager.h"

typedef struct _MyMediaManager *MyMediaManager;

void Runtime_class_init(void);

// ====== native ======
void     Runtime_surfaceChanged(jint width, jint height);
void     Runtime_surfaceDestroyed();
void     Runtime_focusChanged(jboolean hasFocus);
void     Runtime_configurationChanged(jint config, jint something);
void     Runtime_keyDown(jint keyCode, jint unicodeChar, jint repeatCount);
void     Runtime_keyUp(jint keyCode, jint unicodeChar, jint repeatCount);
void     Runtime_touchMove(jint action, jint x, jint y);
void     Runtime_touchDown(jint action, jint x, jint y, jboolean isPrimary);
void     Runtime_touchUp(jint action, jint x, jint y, jboolean isPrimary);
void     Runtime_pause();
void     Runtime_resume();
void     Runtime_quit();
jboolean Runtime_waitFlip(jint timeout);
void     Runtime_setSurfaceSize(jboolean isReal, jint width, jint height);

// ====== native 2 ======
jint Runtime_launch(jint sdkInt, Surface surface, JavaString runtimeDir, JavaString apkPath, JavaString rootDir, JavaString appId, jint params, JavaString fromManager, JavaString *args, jint serverEnvId);
void Runtime_MediaManagerInit(MyMediaManager mediaManager);
void Runtime_ExtensionInit(ExtensionManager obj);

// ====== JAVA Impl ======
void     Runtime_vibrate(jboolean enalbed);
JavaString  Runtime_getLocale();
void     Runtime_launchRuntime(JavaString appid);
void     Runtime_trialSetTime(jint t);
void     Runtime_appFinish();
void     Runtime_updateResourceCheck(JavaString str, jfloat *arr);
void     Runtime_invisibleRuntimeLogo();
jboolean Runtime_isUseRecord();
JavaString  Runtime_getCommonDir();
JavaString  Runtime_getCommonAppDir(JavaString str);
JavaString  Runtime_getEnv(JavaString key);
void     Runtime_jniTrackerSetPage(JavaString str);
void     Runtime_jniTrackerEvent(JavaString str1, JavaString str2);
void     Runtime_jniTrackerAssignAttribute(jint id, JavaString attr);
void     Runtime_jniTrackerRemoveAttribute(jint id);
void     Runtime_jniTrackerSetAttribute(jint id, JavaString attr);
jint     Runtime_stereoIsAvailability();
void     Runtime_stereoSetView(jboolean enalbed);
jfloat   Runtime_stereoGetDepth();

#ifdef __cplusplus
}
#endif

#endif