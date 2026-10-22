#ifndef SOLOADER_CONFIG_H
#define SOLOADER_CONFIG_H

#define SCREEN_W 960
#define SCREEN_H 544

#define MEMORY_NEWLIB_MB            256
#define VITAGL_RAM_THRESHOLD_MB     20
#define VITAGL_CDRAM_THRESHOLD_MB   0
#define VITAGL_PHYCONT_THRESHOLD_MB 8

#define SO_APK_PATH   DATA_PATH "base.apk"
#define SO_LIB_DIR    DATA_PATH
#define SO_ASSETS_DIR DATA_PATH "assets/"
#define SO_FILES_DIR  DATA_PATH "files/"
#define SO_CACHE_DIR  DATA_PATH "cache/"
#define SO_TTF_PATH   "app0:font.ttf"

#define RUNTIME_APP_ID "1760"  // "1760" "1761" "1277" "705"

#endif