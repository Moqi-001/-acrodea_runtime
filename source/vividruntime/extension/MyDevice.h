#ifndef VIVIDRUNTIME_MY_DEVICE_H
#define VIVIDRUNTIME_MY_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

typedef struct _MyDevice *MyDevice;

void MyDevice_class_init(void);

#ifdef __cplusplus
}
#endif

#endif