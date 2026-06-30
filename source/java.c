#include <stddef.h>

#include <so_jni/so_jni.h>

#include "android/media/AudioTrack.h"
#include "android/os/Build.h"
#include "android/view/View.h"
#include "java/lang/System.h"

#include "vividruntime/extension/MyAlarm.h"
#include "vividruntime/extension/MyAvatar.h"
#include "vividruntime/extension/MyBilling.h"
#include "vividruntime/extension/MyCamera.h"
#include "vividruntime/extension/MyDeviceUtil.h"
#include "vividruntime/extension/MyDialog.h"
#include "vividruntime/extension/MyErrorDialog.h"
#include "vividruntime/extension/MyFacebook.h"
#include "vividruntime/extension/MyFont.h"
#include "vividruntime/extension/MyHttp.h"
#include "vividruntime/extension/MyImagePicker.h"
#include "vividruntime/extension/MyLaunch.h"
#include "vividruntime/extension/MyLocation.h"
#include "vividruntime/extension/MyMediaManager.h"
#include "vividruntime/extension/MyMediaPlayer.h"
#include "vividruntime/extension/MyMovie.h"
#include "vividruntime/extension/MyMovieBridge.h"
#include "vividruntime/extension/MyPopup.h"
#include "vividruntime/extension/MyScreen.h"
#include "vividruntime/extension/MySensor.h"
#include "vividruntime/extension/MyTextInput.h"

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

    MyHttp_class_init();
    MyMediaManager_class_init();
    MyMediaPlayer_class_init();
    MySensor_class_init();
    MyLaunch_class_init();
    MyLocation_class_init();
    MyMovieBridge_class_init();
    MyImagePicker_class_init();
    MyBilling_class_init();
    MyAvatar_class_init();
    MyErrorDialog_class_init();
    MyCamera_class_init();
    MyMovie_class_init();
    MyScreen_class_init();
    MyDialog_class_init();
    MyFacebook_class_init();
    MyAlarm_class_init();
    MyFont_class_init();
    MyPopup_class_init();
    MyDeviceUtil_class_init();
    MyTextInput_class_init();

    return 0;
}
