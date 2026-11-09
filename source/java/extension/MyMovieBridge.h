#ifndef JAVA_MY_MOVIE_BRIDGE_H
#define JAVA_MY_MOVIE_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

typedef struct _MyMovieBridge *MyMovieBridge;

void setup_MyMovieBridge(void);

#ifdef __cplusplus
}
#endif

#endif