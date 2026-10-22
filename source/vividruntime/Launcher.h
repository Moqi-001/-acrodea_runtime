#ifndef VIVIDRUNTIME_LAUNCHER_H
#define VIVIDRUNTIME_LAUNCHER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

void Launcher_launchRuntime(JavaString appid);
void Launcher_appFinish();

#ifdef __cplusplus
}
#endif

#endif