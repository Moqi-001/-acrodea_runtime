#include <stddef.h>

#include <so_jni/so_jni.h>

#include "android/os/Build.h"
#include "android/view/View.h"
#include "android/media/AudioTrack.h"
#include "java/lang/System.h"

#include "vividruntime/extension/ExtensionManager.h"
#include "vividruntime/extension/MyDevice.h"
#include "vividruntime/extension/MyErrorDialog.h"
#include "vividruntime/extension/MyGgee.h"
#include "vividruntime/Runtime.h"

int soloader_java_init() {
    java_lang_System_class_init();
    android_os_Build_class_init();
    android_view_Surface_class_init();
    android_media_AudioTrack_class_init();

    Runtime_class_init();
    ExtensionManager_class_init();
    MyDevice_class_init();
    MyErrorDialog_class_init();
    MyGgee_class_init();

    return 0;
}
