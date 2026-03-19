
#include <falso_jni/FalsoJNI.h>

typedef void *View;
typedef void *Intent;

typedef struct MyExtensionInterface {
    void (*SetLocalView)(View view);
    void (*configurationChanged)();
    void (*extActivityResult)(int i, int i2, Intent intent);
    void (*focusChange)(jboolean z);
    void (*focusChanged)(jboolean z);
    jstring (*getExtensionName)();
    void (*pause)();
    void (*resume)();
    void (*runtime_stop)();
    void (*setExtraBoolean)(int i, jboolean z);
    void (*setExtraString)(int i, jstring str);
    void (*setOrientation)(jstring str);
    void (*stop)();
} MyExtensionInterface;
