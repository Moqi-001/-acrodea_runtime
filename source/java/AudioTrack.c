#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <falso_jni/FalsoJNI.h>
#include <psp2/audioout.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/processmgr.h>

#include "reimpl/android/AudioTrack.h"
#include "utils/dialog.h"
#include "utils/logger.h"

/*
 * 包名: android/media/AudioTrack
 */

#define AUDIO_TRACK_COUNT  16
#define AUDIO_OUTPUT_COUNT 512u
#define AUDIO_BUFFER_COUNT (AUDIO_OUTPUT_COUNT << 4)

typedef struct AudioTrack {
    int                  id;
    int                  is_inited;
    int                  is_using;
    int                  has_new;
    int                  pause;
    int                  stop;
    int16_t             *buffer;
    size_t               buffer_count;
    size_t               output_count;
    size_t               read_pos;
    size_t               write_pos;
    SceKernelLwMutexWork mutex;
    SceKernelLwCondWork  cond;
} AudioTrack;

static SceKernelLwMutexWork audio_main_mutex;
static SceUID               audio_thid         = -1;
static int                  audio_inited       = 0;
static int                  audio_stop         = 0;
static int                  audio_pause        = 0;
SceUID                      audio_port         = -1;
static int                  audio_output_count = 0;
static int                  audio_buffer_count = 0;
static AudioTrack           audio_tracks[AUDIO_TRACK_COUNT];

static int AudioThread(SceSize args, void *argp) {
    l_debug("[AUDIO] Audio thread start run.\n");

    size_t available;
    size_t playing_num;

    while (!audio_stop) {
        playing_num = 0;

        for (int i = 0; i < AUDIO_TRACK_COUNT; i++) {
            if (!audio_tracks[i].is_inited)
                continue;

            sceKernelLockLwMutex(&audio_tracks[i].mutex, 1, NULL);
            if (audio_tracks[i].is_using && audio_tracks[i].has_new) {
                if (audio_tracks[i].write_pos >= audio_tracks[i].read_pos) {
                    available = audio_tracks[i].write_pos - audio_tracks[i].read_pos;
                } else {
                    available = audio_tracks[i].write_pos + audio_buffer_count - audio_tracks[i].read_pos;
                }
                if (available >= audio_output_count) {
                    sceAudioOutOutput(audio_port, audio_tracks[i].buffer + audio_tracks[i].read_pos);
                    audio_tracks[i].read_pos += audio_output_count;
                    if (audio_tracks[i].read_pos >= audio_buffer_count)
                        audio_tracks[i].read_pos = 0;

                    playing_num++;
                } else {
                    audio_tracks[i].has_new = 0;
                }
            }
            sceKernelUnlockLwMutex(&audio_tracks[i].mutex, 1);
        }

        if (playing_num == 0) {
            sceKernelDelayThread(10 * 1000);
        }
    }

    l_debug("[AUDIO] Audio thread stop run.\n");
    sceKernelExitThread(0);
    return 0;
}

// "<init>", "(IIIIII)V" (3,16000,4,2,4096,1)
jobject AudioTrack_init(jclass clazz, jmethodID id, va_list args) {
    const char *audioTrack        = "AudioTrack";
    int         streamType        = va_arg(args, int);  // 3
    int         sampleRateInHz    = va_arg(args, int);  // 1600
    int         channelConfig     = va_arg(args, int);  // 4
    int         audioFormat       = va_arg(args, int);  // 2
    int         bufferSizeInBytes = va_arg(args, int);  // 4096
    int         mode              = va_arg(args, int);  // 1

    if (audio_port < 0) {
        int             audio_channels = (channelConfig == 4) ? 2 : 1;
        SceAudioOutMode audio_mode     = (audio_channels == 2) ? SCE_AUDIO_OUT_MODE_STEREO : SCE_AUDIO_OUT_MODE_MONO;
        audio_output_count             = AUDIO_OUTPUT_COUNT * audio_channels;
        audio_buffer_count             = AUDIO_BUFFER_COUNT * audio_channels;
        audio_port                     = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, audio_output_count,
                                                             sampleRateInHz, audio_mode);
        if (audio_port < 0) {
            l_error("[JNI] AudioTrack::<init>: sceAudioOutOpenPort failed!");
            return NULL;
        }

        for (int i = 0; i < AUDIO_TRACK_COUNT; i++) {
            memset(&audio_tracks[i], 0, sizeof(AudioTrack));
            audio_tracks[i].id = i;
        }

        audio_thid = sceKernelCreateThread("audio_thread", AudioThread, 0x10000100, 0x40000, 0, 0, NULL);
        if (audio_thid < 0) {
            l_error("[JNI] AudioTrack::<init>: failed to create thread!");
            return NULL;
        }
        sceKernelCreateLwMutex(&audio_main_mutex, "audio_main_mutex", 2, 0, NULL);

        audio_stop = 0;
        if (sceKernelStartThread(audio_thid, 0, NULL) < 0) {
            sceKernelDeleteLwMutex(&audio_main_mutex);
            l_error("[JNI] AudioTrack::<init>: failed to start thread!");
            return NULL;
        }

        l_debug("[JNI] AudioTrack::<init>: Audio context init OK!");
    } else {
        l_debug("[JNI] AudioTrack::<init>: Already initialized!");
    }

    // 加琐防止异步同时调用
    sceKernelLockLwMutex(&audio_main_mutex, 1, NULL);

    int track_id = -1;
    for (int i = 0; i < AUDIO_TRACK_COUNT; i++) {
        if (!audio_tracks[i].is_using) {
            track_id = i;
            break;
        }
    }

    if (track_id == -1) {
        l_error("[JNI] AudioTrack::<init>: no free track!");
        return NULL;
    }

    if (!audio_tracks[track_id].is_inited) {
        audio_tracks[track_id].buffer = malloc(audio_buffer_count * sizeof(int16_t));
        if (!audio_tracks[track_id].buffer) {
            l_error("[JNI] AudioTrack::<init>: failed to alloc audio buffer!");
            return NULL;
        }
        sceKernelCreateLwMutex(&audio_tracks[track_id].mutex, "audio_track_mutex", 2, 0, NULL);
        sceKernelCreateLwCond(&audio_tracks[track_id].cond, "audio_track_cond", 0, &audio_tracks[track_id].mutex, NULL);
        audio_tracks[track_id].is_inited = 1;
    }

    AudioTrack **res = malloc(sizeof(AudioTrack *));
    if (!res) {
        l_error("[JNI] AudioTrack::<init>: failed to alloc obj");
        return NULL;
    }
    *res = &audio_tracks[track_id];

    // 加锁，防止可能旧Track还在播放
    sceKernelLockLwMutex(&audio_tracks[track_id].mutex, 1, NULL);
    audio_tracks[track_id].is_using = 1;
    sceKernelUnlockLwMutex(&audio_tracks[track_id].mutex, 1);

    sceKernelUnlockLwMutex(&audio_main_mutex, 1);

    l_debug("[%d] [JNI] AudioTrack::<init>(%d,%d,%d,%d,%d,%d): %p",
            track_id, streamType, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes, mode, res);
    return (jobject)res;
}

// "write", "([BII)I"
jint AudioTrack_write(jobject obj, jmethodID id, va_list args) {
    jbyteArray audioData     = va_arg(args, jbyteArray);
    int        offsetInBytes = va_arg(args, int);
    int        sizeInBytes   = va_arg(args, int);
    int        res           = sizeInBytes;

    if (!obj) {
        l_error("[JNI] AudioTrack::write(%p,%p,%d,%d) failed: null obj!");
        return (jint)res;
    }

    AudioTrack *track = *(AudioTrack **)obj;
    if (!track) {
        l_error("[JNI] AudioTrack::write(%p) failed: null obj!", obj);
        return (jint)res;
    }

    if (audio_port < 0) {
        l_error("[JNI] AudioTrack::write(%p,%p,%d,%d): failed!", obj, audioData, offsetInBytes, sizeInBytes);
        return (jint)res;
    }

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    jbyte *bytes = jni->GetByteArrayElements(&jni, audioData, NULL);
    if (bytes) {
        int16_t *in  = (int16_t *)(bytes + offsetInBytes);
        int16_t *out = (int16_t *)track->buffer;
        for (int i = 0; i < sizeInBytes / 2; i++) {
            out[track->write_pos++] = in[i];
            if (track->write_pos >= audio_buffer_count)
                track->write_pos = 0;
        }
        jni->ReleaseByteArrayElements(&jni, audioData, bytes, 0);
        track->has_new = 1;
    }
    sceKernelUnlockLwMutex(&track->mutex, 1);

    l_debug("[%d] [JNI] AudioTrack::write(%p,%p,%d,%d): %d", track->id, obj, audioData, offsetInBytes, sizeInBytes, res);
    return (jint)res;
}

// "release", "()V"
void AudioTrack_release(jobject obj, jmethodID id, va_list args) {
    if (!obj) {
        l_error("[JNI] AudioTrack::release(%p,%p,%d,%d) failed: null obj!");
        return;
    }

    AudioTrack *track = *(AudioTrack **)obj;
    if (!track) {
        l_error("[JNI] AudioTrack::release(%p) failed: null obj!", obj);
        return;
    }

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    track->is_using = 0;
    sceKernelUnlockLwMutex(&track->mutex, 1);

    l_debug("[%d] [JNI] AudioTrack::release(%p)", track->id, obj);
}

// "getMinBufferSize", "(III)I"
jint AudioTrack_getMinBufferSize(jobject obj, jmethodID id, va_list args) {
    int sampleRateInHz = va_arg(args, int);
    int channelConfig  = va_arg(args, int);
    int audioFormat    = va_arg(args, int);

    int audio_channels = (channelConfig == 4) ? 2 : 1;
    int res            = AUDIO_OUTPUT_COUNT * audio_channels * sizeof(int16_t);
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
        return (jint)res;
    }

    sceKernelLockLwMutex(&audio_main_mutex, 1, NULL);
    l_debug("[JNI] AudioTrack::setStereoVolume(%p,%f,%f): %d", obj, leftVolume, rightVolume, res);
    int vols[] = {32767 * leftVolume, 32767 * rightVolume};
    sceAudioOutSetVolume(audio_port, SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH, vols);
    sceKernelUnlockLwMutex(&audio_main_mutex, 1);

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
