#ifndef JAVA_RUNTIME_H
#define JAVA_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_java/android/view/Surface.h>
#include <so_jni/so_jni.h>

#include "extension/ExtensionManager.h"
#include "extension/MyMediaManager.h"

void setup_Runtime(void);

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
jint Runtime_launch(jint sdkInt, Surface surface, JString runtimeDir, JString apkPath, JString rootDir,
                    JString appId, jint params, JString fromManager, JString *args, jint serverEnvId);
void Runtime_MediaManagerInit(MyMediaManager mediaManager);
void Runtime_ExtensionInit(ExtensionManager obj);

// ====== JAVA Impl ======
void     Runtime_vibrate(jboolean enalbed);
JString  Runtime_getLocale();
void     Runtime_launchRuntime(JString appid);
void     Runtime_trialSetTime(jint t);
void     Runtime_appFinish();
void     Runtime_updateResourceCheck(JString str, jfloat *arr);
void     Runtime_invisibleRuntimeLogo();
jboolean Runtime_isUseRecord();
JString  Runtime_getCommonDir();
JString  Runtime_getCommonAppDir(JString str);
JString  Runtime_getEnv(JString key);
void     Runtime_jniTrackerSetPage(JString str);
void     Runtime_jniTrackerEvent(JString str1, JString str2);
void     Runtime_jniTrackerAssignAttribute(jint id, JString attr);
void     Runtime_jniTrackerRemoveAttribute(jint id);
void     Runtime_jniTrackerSetAttribute(jint id, JString attr);
jint     Runtime_stereoIsAvailability();
void     Runtime_stereoSetView(jboolean enalbed);
jfloat   Runtime_stereoGetDepth();

#ifdef __cplusplus
}
#endif

#endif