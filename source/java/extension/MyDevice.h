#ifndef JAVA_MY_DEVICE_H
#define JAVA_MY_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

typedef struct _MyDevice *MyDevice;

void setup_MyDevice(void);

#ifdef __cplusplus
}
#endif

#endif