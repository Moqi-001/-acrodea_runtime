#ifndef ANDROID_OS_BUILD_H
#define ANDROID_OS_BUILD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

typedef struct _Build *Build;

extern jint android_os_Build_VERSION_SDK_INT;

// 类初始化函数
void android_os_Build_class_init(void);

#ifdef __cplusplus
}
#endif

#endif