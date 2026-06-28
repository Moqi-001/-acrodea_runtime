#ifndef VIVIDRUNTIME_MY_ALARM_H
#define VIVIDRUNTIME_MY_ALARM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

typedef struct _MyAlarm *MyAlarm;

void MyAlarm_class_init(void);

#ifdef __cplusplus
}
#endif

#endif