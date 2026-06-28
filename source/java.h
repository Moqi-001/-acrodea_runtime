#ifndef SOLOADED_JAVA_H
#define SOLOADED_JAVA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

#include "vividruntime/Runtime.h"

int soloader_java_init();

#ifdef __cplusplus
};
#endif

#endif