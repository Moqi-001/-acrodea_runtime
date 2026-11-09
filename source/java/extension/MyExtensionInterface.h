#ifndef JAVA_MY_EXTENSION_INTERFACE_H
#define JAVA_MY_EXTENSION_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <so_java/android/content/Intent.h>
#include <so_java/android/view/View.h>
#include <so_jni/so_jni.h>

struct _MyExtensionInterface {
    void (*SetLocalView)(View view);
    void (*configurationChanged)();
    void (*extActivityResult)(jint i, jint i2, Intent intent);
    void (*focusChange)(jboolean z);
    void (*focusChanged)(jboolean z);
    jstring (*getExtensionName)();
    void (*pause)();
    void (*resume)();
    void (*runtime_stop)();
    void (*setExtraBoolean)(jint i, jboolean z);
    void (*setExtraString)(jint i, jstring str);
    void (*setOrientation)(jstring str);
    void (*stop)();
};

typedef struct _MyExtensionInterface *MyExtensionInterface;

#ifdef __cplusplus
}
#endif

#endif