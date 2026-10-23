cmake_minimum_required(VERSION 3.14)

if (NOT DEFINED ENV{VITASDK} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR "Please define VITASDK env var to point to your SDK path or set CMAKE_TOOLCHAIN_FILE.")
endif ()

set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "Toolchain file")

project(AcrodeaRuntime C CXX)

include("${VITASDK}/share/vita.cmake" REQUIRED)

# Vita application configuration
set(VITA_APP_NAME "Devil May Cry 4 refrain")
set(VITA_TITLEID  "GGEE01760")
set(VITA_VPKNAME  "DevilMayCry4refrain")
set(VITA_VERSION  "01.00")
set(VITA_MKSFOEX_FLAGS "-d ATTRIBUTE2=12")

# Optional: used for "send" and "dump" targets
set(PSVITAIP "192.168.11.247" CACHE STRING "PSVita IP (for FTP access)")
set(PARSECOREPATH "$ENV{PARSECOREPATH}" CACHE STRING "vita-parse-core main.py")

# Game-specific definitions
set(DATA_PATH "ux0:data/Acrodea/" CACHE STRING "Path to data (with trailing /)")
set(LOG_PATH "${DATA_PATH}log.txt" CACHE STRING "Path to log.txt")
add_definitions(-DDATA_PATH="${DATA_PATH}" -DLOG_PATH="${LOG_PATH}")

# Unique debug definition for loader's logging to not interfere with anything
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG_SOLOADER)
  #add_definitions(-DDEBUG_LOGFILE) # 日志写入文件
endif()

# makes sincos, sincosf, etc. visible
add_definitions(-D_GNU_SOURCE -D__POSIX_VISIBLE=999999)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon -mfloat-abi=softfp -std=gnu11 -Wno-deprecated")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon -mfloat-abi=softfp -std=gnu++20 -Wno-write-strings -Wno-psabi")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O1 -g3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -g3")
else ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g0 -ffast-math")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g0 -ffast-math")
endif()

add_link_options(
    -Wl,-q
    -Wl,--wrap=chdir_soloader
    -Wl,--wrap=fopen_soloader
)

enable_language(ASM)

file(GLOB_RECURSE SO_COMMON_SRC  deps/so_common/*.c)
file(GLOB_RECURSE SO_LINKER_SRC  deps/so_linker/*.c)
file(GLOB_RECURSE SO_JNI_SRC     deps/so_jni/*.c deps/so_jni/*.S)
file(GLOB_RECURSE SO_BIONIC_SRC  deps/so_libs/reimpl/bionic/*.c)
file(GLOB_RECURSE SO_ANDROID_SRC deps/so_libs/reimpl/android/*.c)

add_executable(${CMAKE_PROJECT_NAME}
  ${SO_COMMON_SRC}
  ${SO_LINKER_SRC}
  ${SO_JNI_SRC}
  ${SO_BIONIC_SRC}
  ${SO_ANDROID_SRC}

  deps/so_libs/libnativehelper.c
  deps/so_libs/liblog.c
  deps/so_libs/libcutils.c
  deps/so_libs/libutils.c
  deps/so_libs/libGLESv1_CM.c
  deps/so_libs/libGLESv2.c
  deps/so_libs/libEGL.c
  deps/so_libs/libui.c
  deps/so_libs/libmedia.c
  deps/so_libs/libsqlite.c
  deps/so_libs/libdl.c
  deps/so_libs/libbinder.c
  deps/so_libs/libc.c
  deps/so_libs/libstdcpp.c
  deps/so_libs/libm.c

  source/utils/dialog.c
  source/utils/utils.c

  source/android/os/Build.c
  source/android/os/Build_VERSION.c
  source/android/view/Surface.c
  source/android/media/AudioTrack.c
  source/java/lang/System.c

  source/vividruntime/extension/MyAlarm.c
  source/vividruntime/extension/MyAvatar.c
  source/vividruntime/extension/MyBilling.c
  source/vividruntime/extension/MyCamera.c
  source/vividruntime/extension/MyDeviceUtil.c
  source/vividruntime/extension/MyDialog.c
  source/vividruntime/extension/MyErrorDialog.c
  source/vividruntime/extension/MyFacebook.c
  source/vividruntime/extension/MyFont.c
  source/vividruntime/extension/MyHttp.c
  source/vividruntime/extension/MyImagePicker.c
  source/vividruntime/extension/MyLaunch.c
  source/vividruntime/extension/MyLocation.c
  source/vividruntime/extension/MyMediaManager.c
  source/vividruntime/extension/MyMediaPlayer.c
  source/vividruntime/extension/MyMovie.c
  source/vividruntime/extension/MyMovieBridge.c
  source/vividruntime/extension/MyPopup.c
  source/vividruntime/extension/MyScreen.c
  source/vividruntime/extension/MySensor.c
  source/vividruntime/extension/MyTextInput.c

  source/vividruntime/extension/ExtensionManager.c
  source/vividruntime/extension/MyDevice.c
  source/vividruntime/extension/MyErrorDialog.c
  source/vividruntime/extension/MyGgee.c
  source/vividruntime/Launcher.c
  source/vividruntime/Runtime.c

  source/controls.c
  source/java.c
  source/loader.c
  source/main.c
  source/patch.c
  source/patch/libGgee/ggee.c
  source/patch/libKD/kd.c
  source/patch/libLD/ld.c
  source/patch/libLD/ld_hook.c
  source/patch/libLD/ld_libc.c
  source/patch/libLD/ld_libcpp.c
  source/patch/libLD/ld_bootstrap.c
)

target_include_directories(${CMAKE_PROJECT_NAME} SYSTEM
  PUBLIC ${CMAKE_SOURCE_DIR}/libs/include
  PUBLIC ${CMAKE_SOURCE_DIR}/deps
)

target_include_directories(${CMAKE_PROJECT_NAME}
  PUBLIC ${CMAKE_SOURCE_DIR}/source
)

target_link_directories (${CMAKE_PROJECT_NAME}
  PUBLIC ${CMAKE_SOURCE_DIR}/libs/lib
)

target_link_libraries(${CMAKE_PROJECT_NAME}
  -Wl,--whole-archive pthread -Wl,--no-whole-archive
  speexdsp
  vitaGL
  vitashark
  SceShaccCgExt
  taihen_stub
  SceShaccCg_stub
  kubridge_stub
  mathneon
  sqlite3
  png
  z
  m
  stdc++
  SceHid_stub
  SceCtrl_stub
  SceTouch_stub
  SceMotion_stub
  SceAppMgr_stub
  SceAppUtil_stub
  SceAudio_stub
  SceAudioIn_stub
  SceCommonDialog_stub
  SceDisplay_stub
  SceKernelDmacMgr_stub
  SceGxm_stub
  ScePower_stub
  SceVshBridge_stub
  SceAvPlayer_stub
)

vita_create_self(eboot.bin ${CMAKE_PROJECT_NAME} UNSAFE NOASLR)
vita_create_vpk(${VITA_VPKNAME}.vpk ${VITA_TITLEID} eboot.bin
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE ${CMAKE_SOURCE_DIR}/resources/cpuinfo cpuinfo
  FILE ${CMAKE_SOURCE_DIR}/resources/meminfo meminfo
  FILE ${CMAKE_SOURCE_DIR}/resources/urandom urandom
  FILE ${CMAKE_SOURCE_DIR}/resources/livearea/${VITA_VPKNAME}/icon0.png sce_sys/icon0.png
  FILE ${CMAKE_SOURCE_DIR}/resources/livearea/${VITA_VPKNAME}/pic0.png sce_sys/pic0.png
  FILE ${CMAKE_SOURCE_DIR}/resources/livearea/${VITA_VPKNAME}/startup.png sce_sys/livearea/contents/startup.png
  FILE ${CMAKE_SOURCE_DIR}/resources/livearea/${VITA_VPKNAME}/bg0.png sce_sys/livearea/contents/bg0.png
  FILE ${CMAKE_SOURCE_DIR}/resources/livearea/${VITA_VPKNAME}/template.xml sce_sys/livearea/contents/template.xml
)

add_custom_target(send
  COMMAND echo destroy | nc ${PSVITAIP} 1338
  COMMAND echo destroy | nc ${PSVITAIP} 1338
  COMMAND curl -T eboot.bin ftp://${PSVITAIP}:1337/ux0:/app/${VITA_TITLEID}/
  COMMAND echo launch ${VITA_TITLEID} | nc ${PSVITAIP} 1338
  DEPENDS ${VITA_VPKNAME}.vpk-vpk
)

add_custom_target(send_kvdb
  COMMAND echo destroy | nc ${PSVITAIP} 1338
  COMMAND curl -T eboot.bin ftp://${PSVITAIP}:1337/ux0:/app/${VITA_TITLEID}/
  COMMAND echo launch ux0:/app/${VITA_TITLEID}/eboot.bin | nc ${PSVITAIP} 1338
  DEPENDS ${VITA_VPKNAME}.vpk-vpk
)
