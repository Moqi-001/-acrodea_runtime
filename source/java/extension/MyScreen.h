#ifndef JAVA_MY_SCREEN_H
#define JAVA_MY_SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

typedef struct _MyScreen *MyScreen;

void setup_MyScreen(void);

#ifdef __cplusplus
}
#endif

#endif