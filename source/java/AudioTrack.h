#ifndef SOLOADER_JAVA_MY_TEXT_INPUT_H
#define SOLOADER_JAVA_MY_TEXT_INPUT_H

#include <falso_jni/FalsoJNI.h>

jobject AudioTrack_init(jclass obj, jmethodID id, va_list args);
jint    AudioTrack_getMinBufferSize(jobject obj, jmethodID id, va_list args);
jint    AudioTrack_setStereoVolume(jobject obj, jmethodID id, va_list args);
void    AudioTrack_play(jobject obj, jmethodID id, va_list args);
void    AudioTrack_stop(jobject obj, jmethodID id, va_list args);
void    AudioTrack_pause(jobject obj, jmethodID id, va_list args);
void    AudioTrack_flush(jobject obj, jmethodID id, va_list args);
void    AudioTrack_release(jobject obj, jmethodID id, va_list args);
void    AudioTrack_release(jobject obj, jmethodID id, va_list args);
jint    AudioTrack_write(jobject obj, jmethodID id, va_list args);

#endif