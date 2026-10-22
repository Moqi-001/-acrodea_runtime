#ifndef VIVIDRUNTIME_EXTENSION_MANAGER_H
#define VIVIDRUNTIME_EXTENSION_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

#include "MyExtensionInterface.h"

typedef struct _ExtensionManager *ExtensionManager;

void ExtensionManager_class_init(void);

MyExtensionInterface ExtensionManager_getExtensonByName(ExtensionManager thiz, JavaString name);

#ifdef __cplusplus
}
#endif

#endif