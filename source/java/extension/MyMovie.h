#ifndef JAVA_MY_MOVIE_H
#define JAVA_MY_MOVIE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

typedef struct _MyMovie *MyMovie;

void setup_MyMovie(void);

#ifdef __cplusplus
}
#endif

#endif