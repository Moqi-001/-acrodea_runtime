#ifndef ANDROID_MEDIA_AUDIO_TRACK_H
#define ANDROID_MEDIA_AUDIO_TRACK_H

#include <so_jni/so_jni.h>

typedef struct _AudioTrack *AudioTrack;

void android_media_AudioTrack_class_init(void);

void AudioTrack_ctor(AudioTrack thiz, jint streamType, jint sampleRateInHz, jint channelConfig,
                     jint audioFormat, jint bufferSizeInBytes, jint mode);
jint AudioTrack_write(AudioTrack thiz, jbyte *audioData, jint offsetInBytes, jint sizeInBytes);
jint AudioTrack_setStereoVolume(AudioTrack thiz, jfloat leftVolume, jfloat rightVolume);
void AudioTrack_play(AudioTrack thiz);
void AudioTrack_pause(AudioTrack thiz);
void AudioTrack_stop(AudioTrack thiz);
void AudioTrack_flush(AudioTrack thiz);
void AudioTrack_release(AudioTrack thiz);

jint AudioTrack_getMinBufferSize(jint sampleRateInHz, jint channelConfig, jint audioFormat);

#endif