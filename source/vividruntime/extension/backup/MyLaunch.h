#ifndef VIVIDRUNTIME_MY_LAUNCH_H
#define VIVIDRUNTIME_MY_LAUNCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

typedef struct _MyLaunch *MyLaunch;

void MyLaunch_class_init(void);

#ifdef __cplusplus
}
#endif

#endif