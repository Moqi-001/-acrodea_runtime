#ifndef VIVIDRUNTIME_MY_LOCATION_H
#define VIVIDRUNTIME_MY_LOCATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

typedef struct _MyLocation *MyLocation;

void MyLocation_class_init(void);

#ifdef __cplusplus
}
#endif

#endif