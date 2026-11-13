#include <stdint.h>

#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "MyExtensionInterface.h"

#include "ExtensionManager.h"

struct _ExtensionManager {
    uint32_t dummy;
};

static JClass sClass = NULL;

// ====== JAVA Impl ======
MyExtensionInterface ExtensionManager_getExtensonByName(ExtensionManager thiz, JString name) {
    const char          *name_cstr = String_getUTFChars(name);
    MyExtensionInterface inst      = Object_new(sClass);
    so_logd("[ExtensionManager] getExtensonByName(%p,%s): %p", thiz, name_cstr, inst);

    return inst;
}

static JSymbol class_symbols[] = {
    JAVA_SYMBOL_METHOD("getExtensonByName", "(Ljava/lang/String;)Lcom/acrodea/vividruntime/launcher/extension/MyExtensionInterface;", &ExtensionManager_getExtensonByName),
};

static JClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/ExtensionManager",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _ExtensionManager),
    .dtor          = NULL,
};

void setup_ExtensionManager(void) {
    sClass = JavaCreateClass(&class_meta);
}
