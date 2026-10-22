#include <stdint.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "MyExtensionInterface.h"

#include "ExtensionManager.h"

struct _ExtensionManager {
    uint32_t dummy;
};

static JavaClass sClass = NULL;

// ====== JAVA Impl ======
MyExtensionInterface ExtensionManager_getExtensonByName(ExtensionManager thiz, JavaString name) {
    const char *name_cstr = JavaGetStringUTFChars(name);
    so_logd("[ExtensionManager] getExtensonByName(%p,%s)", thiz, name_cstr);
    MyExtensionInterface inst = JavaAllocObjectNoClass(sizeof(struct _MyExtensionInterface), NULL);

    return inst;
}

static JavaSymbol class_symbols[] = {
    // static native
    JAVA_SYMBOL_METHOD("getExtensonByName", "(Ljava/lang/String;)Lcom/acrodea/vividruntime/launcher/extension/MyExtensionInterface;", &ExtensionManager_getExtensonByName),
};

static JavaClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/ExtensionManager",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _ExtensionManager),
    .dtor          = NULL,
};

void ExtensionManager_class_init(void) {
    sClass = JavaCreateClass(&class_meta);
}
