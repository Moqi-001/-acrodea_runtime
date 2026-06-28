#ifndef VIVIDRUNTIME_MY_MOVIE_H
#define VIVIDRUNTIME_MY_MOVIE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

typedef struct _MyMovie *MyMovie;

void MyMovie_class_init(void);

#ifdef __cplusplus
}
#endif

#endif