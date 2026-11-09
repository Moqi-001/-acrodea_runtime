#ifndef JAVA_MY_LOCATION_H
#define JAVA_MY_LOCATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

typedef struct _MyLocation *MyLocation;

void setup_MyLocation(void);

#ifdef __cplusplus
}
#endif

#endif