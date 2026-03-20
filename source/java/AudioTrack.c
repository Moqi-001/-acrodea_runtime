#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <falso_jni/FalsoJNI.h>
#include <psp2/audioout.h>
#include <psp2/kernel/processmgr.h>

#include "reimpl/android/AudioTrack.h"
#include "utils/logger.h"

/*
 * 包名: android/media/AudioTrack
 */

#define AUDIO_OUT_SIZE     4096
#define AUDIO_BUFFER_COUNT 10

static SceKernelLwMutexWork audio_mutex;
SceUID                      audio_port = -1;
char                        audio_buffer[AUDIO_BUFFER_COUNT][AUDIO_OUT_SIZE];
int                         write_pos = 0;

// "<init>", "(IIIIII)V" (3,16000,4,2,4096,1)
jobject AudioTrack_init(jclass clazz, jmethodID id, va_list args) {
    const char *audioTrack        = "Vita_audio_obj";
    int         streamType        = va_arg(args, int);  // 3
    int         sampleRateInHz    = va_arg(args, int);  // 1600
    int         channelConfig     = va_arg(args, int);  // 4
    int         audioFormat       = va_arg(args, int);  // 2
    int         bufferSizeInBytes = va_arg(args, int);  // 4096
    int         mode              = va_arg(args, int);  // 1

    if (audio_port < 0) {
        int             channels         = (channelConfig == 4) ? 2 : 1;
        SceAudioOutMode mode             = (channels == 2) ? SCE_AUDIO_OUT_MODE_STEREO : SCE_AUDIO_OUT_MODE_MONO;
        int             sample_bits      = (audioFormat == 2) ? 16 : 8;
        int             bytes_per_sample = sample_bits / 8;
        int             len              = AUDIO_OUT_SIZE / (channels * bytes_per_sample);
        audio_port                       = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, len, sampleRateInHz, mode);
        if (audio_port < 0) {
            l_debug("[JNI] AudioTrack::<init>: sceAudioOutOpenPort failed!");
        } else {
            sceKernelCreateLwMutex(&audio_mutex, "audio_mutex", 2, 0, NULL);
        }
    } else {
        l_debug("[JNI] AudioTrack::<init>: Already initialized!");
    }

    void *res = (jobject)jni->NewStringUTF(&jni, audioTrack);
    l_debug("[JNI] AudioTrack::<init>(%d,%d,%d,%d,%d,%d): %p",
            streamType, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes, mode, res);
    return (jobject)res;
}

// "getMinBufferSize", "(III)I"
jint AudioTrack_getMinBufferSize(jobject obj, jmethodID id, va_list args) {
    int res            = AUDIO_OUT_SIZE;
    int sampleRateInHz = va_arg(args, int);
    int channelConfig  = va_arg(args, int);
    int audioFormat    = va_arg(args, int);
    l_debug("[JNI] AudioTrack::getMinBufferSize(%p,%d,%d,%d): %d", obj, sampleRateInHz, channelConfig, audioFormat, res);
    return (jint)res;
}

// "setStereoVolume", "(FF)I"
jint AudioTrack_setStereoVolume(jobject obj, jmethodID id, va_list args) {
    int   res         = 0;
    float leftVolume  = va_arg(args, double);
    float rightVolume = va_arg(args, double);

    if (audio_port < 0) {
        l_debug("[JNI] AudioTrack::setStereoVolume(%p,%f,%f): failed!", obj, leftVolume, rightVolume, res);
    } else {
        sceKernelLockLwMutex(&audio_mutex, 1, NULL);
        l_debug("[JNI] AudioTrack::setStereoVolume(%p,%f,%f): %d", obj, leftVolume, rightVolume, res);
        int vols[] = {32767 * leftVolume, 32767 * rightVolume};
        sceAudioOutSetVolume(audio_port, SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH, vols);
        sceKernelUnlockLwMutex(&audio_mutex, 1);
    }
    return (jint)res;
}

// "play", "()V"
void AudioTrack_play(jobject obj, jmethodID id, va_list args) {
    l_debug("[JNI] AudioTrack::play(%p)", obj);
}

// "stop", "()V"
void AudioTrack_stop(jobject obj, jmethodID id, va_list args) {
    l_debug("[JNI] AudioTrack::stop(%p)", obj);
}

// "pause", "()V"
void AudioTrack_pause(jobject obj, jmethodID id, va_list args) {
    l_debug("[JNI] AudioTrack::pause(%p)", obj);
}

// "flush", "()V"
void AudioTrack_flush(jobject obj, jmethodID id, va_list args) {
    l_debug("[JNI] AudioTrack::flush(%p)", obj);
}

// "release", "()V"
void AudioTrack_release(jobject obj, jmethodID id, va_list args) {
    l_debug("[JNI] AudioTrack::release(%p)", obj);
}

// "write", "([BII)I"
jint AudioTrack_write(jobject obj, jmethodID id, va_list args) {
    char *audioData     = va_arg(args, char *);
    int   offsetInBytes = va_arg(args, int);
    int   sizeInBytes   = va_arg(args, int);
    int   res           = sizeInBytes;

    if (audio_port < 0) {
        l_debug("[JNI] AudioTrack::write(%p,%p,%d,%d): failed!", obj, audioData, offsetInBytes, sizeInBytes);
    } else {
        sceKernelLockLwMutex(&audio_mutex, 1, NULL);
        l_debug("[JNI] AudioTrack::write(%p,%p,%d,%d): %d", obj, audioData, offsetInBytes, sizeInBytes, res);
        jbyte *bytes = jni->GetByteArrayElements(&jni, audioData, NULL);
        l_debug("[JNI] AudioTrack::write(%p,%p,%d,%d): %d", obj, audioData, offsetInBytes, sizeInBytes, res);
        l_debug("    GetByteArrayElements(env,%p,NULL): %p", audioData, bytes);
        if (bytes) {
            memset(audio_buffer[write_pos], 0, AUDIO_OUT_SIZE);
            memcpy(audio_buffer[write_pos], bytes + offsetInBytes, sizeInBytes);
            sceAudioOutOutput(audio_port, audio_buffer[write_pos]);
            write_pos = (write_pos + 1) % AUDIO_BUFFER_COUNT;
            jni->ReleaseByteArrayElements(&jni, audioData, bytes, 0);
        }
        sceKernelUnlockLwMutex(&audio_mutex, 1);
    }
    return (jint)res;
}
