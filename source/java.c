#include <so_java/android/media/AudioTrack.h>
#include <so_java/android/os/Build.h>
#include <so_java/android/view/View.h>
#include <so_java/java/lang/System.h>

#include "java/extension/ExtensionManager.h"
#include "java/extension/MyAlarm.h"
#include "java/extension/MyAvatar.h"
#include "java/extension/MyBilling.h"
#include "java/extension/MyCamera.h"
#include "java/extension/MyDevice.h"
#include "java/extension/MyDeviceUtil.h"
#include "java/extension/MyDialog.h"
#include "java/extension/MyErrorDialog.h"
#include "java/extension/MyFacebook.h"
#include "java/extension/MyFont.h"
#include "java/extension/MyGgee.h"
#include "java/extension/MyHttp.h"
#include "java/extension/MyImagePicker.h"
#include "java/extension/MyLaunch.h"
#include "java/extension/MyLocation.h"
#include "java/extension/MyMediaManager.h"
#include "java/extension/MyMediaPlayer.h"
#include "java/extension/MyMovie.h"
#include "java/extension/MyMovieBridge.h"
#include "java/extension/MyPopup.h"
#include "java/extension/MyScreen.h"
#include "java/extension/MySensor.h"
#include "java/extension/MyTextInput.h"
#include "java/Runtime.h"

int soloader_setup_java() {
    setup_java_lang_System();
    setup_android_os_Build();
    setup_android_view_Surface();
    setup_android_media_AudioTrack();

    setup_ExtensionManager();
    setup_MyAlarm();
    setup_MyAvatar();
    setup_MyBilling();
    setup_MyCamera();
    setup_MyDevice();
    setup_MyDeviceUtil();
    setup_MyDialog();
    setup_MyErrorDialog();
    setup_MyFacebook();
    setup_MyFont();
    setup_MyGgee();
    setup_MyHttp();
    setup_MyImagePicker();
    setup_MyLaunch();
    setup_MyLocation();
    setup_MyMediaManager();
    setup_MyMediaPlayer();
    setup_MyMovie();
    setup_MyMovieBridge();
    setup_MyPopup();
    setup_MyScreen();
    setup_MySensor();
    setup_MyTextInput();
    setup_Runtime();

    return 0;
}
