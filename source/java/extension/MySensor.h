#ifndef JAVA_MY_SENSOR_H
#define JAVA_MY_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

typedef struct _MySensor *MySensor;

void setup_MySensor(void);

#ifdef __cplusplus
}
#endif

#endif