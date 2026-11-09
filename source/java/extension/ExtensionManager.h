#ifndef JAVA_EXTENSION_MANAGER_H
#define JAVA_EXTENSION_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_jni/so_jni.h>

#include "MyExtensionInterface.h"

typedef struct _ExtensionManager *ExtensionManager;

void setup_ExtensionManager(void);

MyExtensionInterface ExtensionManager_getExtensonByName(ExtensionManager thiz, JString name);

#ifdef __cplusplus
}
#endif

#endif