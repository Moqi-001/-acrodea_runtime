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

// Android 4.4 audio_channel_mask_t
#define Android_AUDIO_CHANNEL_OUT_MONO   0x1
#define Android_AUDIO_CHANNEL_OUT_STEREO 0x3

// Android 4.4 audio_format_t
#define Android_AUDIO_FORMAT_PCM_16_BIT 0x2
#define Android_AUDIO_FORMAT_PCM_8_BIT  0x3

#define AUDIO_MAX_VOLUME 32767

#define AUDIO_TRACKS_NUM         8                         // 音轨数量
#define AUDIO_OUTPUT_LEN         256                      // 每次输出的样本数
#define AUDIO_TRACK_BUFFER_LEN   (1024 << 10)  // 音轨缓冲区的样本数
#define AUDIO_OUTPUT_BUFFERS_NUM 3                         // 输出缓冲区的缓存个数

enum {
    AUDIO_STATUS_STOPPED = 0,
    AUDIO_STATUS_PAUSED,
    AUDIO_STATUS_PLAYYING,
};

typedef struct AudioTrack {
    int                  id;
    int                  is_inited;
    int                  is_using;
    int                  status;
    int                  has_new;
    int                  is_stereo;
    int                  is_8bit;
    float                left_volume;
    float                right_volume;
    int16_t             *buffer;
    size_t               output_len;
    size_t               buffer_len;
    size_t               read_pos;
    size_t               write_pos;
    SceKernelLwMutexWork mutex;
} AudioTrack;

static SceKernelLwMutexWork audio_main_mutex;
static SceUID               audio_thid                                                       = -1;
static int                  audio_inited                                                     = 0;
static int                  audio_pause                                                      = 0;
static int                  audio_stop                                                       = 0;
static SceUID               audio_port                                                       = -1;
static int16_t              audio_output_buffers[AUDIO_OUTPUT_BUFFERS_NUM][AUDIO_OUTPUT_LEN] = {0};
static AudioTrack           audio_tracks[AUDIO_TRACKS_NUM]                                   = {0};

static size_t mix_track(int16_t *output, size_t output_len, const int16_t *input, size_t input_len, float volume) {
    size_t mixed_len = output_len < input_len ? output_len : input_len;

    for (int i = 0; i < mixed_len; i++) {
        int32_t sum = (int32_t)output[i] + (int32_t)input[i] * volume;
        if (sum > 32767) sum = 32767;
        if (sum < -32768) sum = -32768;
        output[i] = (int16_t)sum;
    }

    return mixed_len;
}

static int audio_thread(SceSize args, void *argp) {
    l_debug("[AUDIO] Audio thread start run.\n");

    size_t   available_len;
    size_t   active_tracks;
    int16_t *output_buffer;
    size_t   buffer_idx = 0;

    while (!audio_stop) {
        active_tracks = 0;
        output_buffer = audio_output_buffers[buffer_idx];

        memset(output_buffer, 0, AUDIO_OUTPUT_LEN * sizeof(int16_t));

        for (int i = 0; i < AUDIO_TRACKS_NUM; i++) {
            AudioTrack *track = &audio_tracks[i];
            if (!track->is_inited)
                continue;

            sceKernelLockLwMutex(&track->mutex, 1, NULL);
            if (track->is_using && track->has_new) {
                if (track->write_pos >= track->read_pos) {
                    available_len = track->write_pos - track->read_pos;
                } else {
                    available_len = track->write_pos + track->buffer_len - track->read_pos;
                }
                if (available_len > 0) {
                    sceKernelUnlockLwMutex(&track->mutex, 1);
                    size_t input_len = available_len < AUDIO_OUTPUT_LEN ? available_len : AUDIO_OUTPUT_LEN;
                    size_t mixed_len = mix_track(output_buffer, AUDIO_OUTPUT_LEN, track->buffer + track->read_pos, input_len, track->left_volume);
                    track->read_pos  = (track->read_pos + mixed_len) % track->buffer_len;
                    active_tracks++;
                } else {
                    track->has_new = 0;
                    sceKernelUnlockLwMutex(&track->mutex, 1);
                }
            } else {
                sceKernelUnlockLwMutex(&track->mutex, 1);
            }
        }

        if (active_tracks > 0) {
            sceAudioOutOutput(audio_port, output_buffer);
            buffer_idx = (buffer_idx + 1) % AUDIO_OUTPUT_BUFFERS_NUM;
        } else {
            sceKernelDelayThread(10 * 1000);
        }
    }

    l_debug("[AUDIO] Audio thread stop run.\n");
    sceKernelExitThread(0);
    return 0;
}

// "<init>", "(IIIIII)V" (3,16000,4,2,4096,1)
jobject AudioTrack_init(jclass clazz, jmethodID id, va_list args) {
    int streamType        = va_arg(args, int);  // 3
    int sampleRateInHz    = va_arg(args, int);  // 1600
    int channelConfig     = va_arg(args, int);  // 4
    int audioFormat       = va_arg(args, int);  // 2
    int bufferSizeInBytes = va_arg(args, int);  // 4096
    int mode              = va_arg(args, int);  // 1

    int is_stereo = (channelConfig == Android_AUDIO_CHANNEL_OUT_STEREO) ? 1 : 0;
    int is_8bit   = (audioFormat == Android_AUDIO_FORMAT_PCM_8_BIT) ? 1 : 0;

    if (!audio_inited) {
        SceAudioOutMode audio_mode = is_stereo ? SCE_AUDIO_OUT_MODE_STEREO : SCE_AUDIO_OUT_MODE_MONO;
        audio_port                 = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, AUDIO_OUTPUT_LEN,
                                                         sampleRateInHz, audio_mode);
        if (audio_port < 0) {
            l_error("[JNI] AudioTrack::<init>: sceAudioOutOpenPort failed!");
            return NULL;
        }

        for (int i = 0; i < AUDIO_TRACKS_NUM; i++) {
            memset(&audio_tracks[i], 0, sizeof(AudioTrack));
            audio_tracks[i].id = i;
        }

        memset(&audio_output_buffers, 0, sizeof(audio_output_buffers));

        audio_thid = sceKernelCreateThread("audio_thread", audio_thread, 0x10000100, 0x40000, 0, 0, NULL);
        if (audio_thid < 0) {
            sceAudioOutReleasePort(audio_port);
            audio_port = -1;
            l_error("[JNI] AudioTrack::<init>: failed to create thread!");
            return NULL;
        }
        sceKernelCreateLwMutex(&audio_main_mutex, "audio_main_mutex", 2, 0, NULL);

        audio_stop = 0;
        if (sceKernelStartThread(audio_thid, 0, NULL) < 0) {
            sceKernelDeleteLwMutex(&audio_main_mutex);
            sceAudioOutReleasePort(audio_port);
            audio_port = -1;
            l_error("[JNI] AudioTrack::<init>: failed to start thread!");
            return NULL;
        }

        audio_inited = 1;
        l_debug("[JNI] AudioTrack::<init>: Audio context initialize OK!");
    } else {
        l_debug("[JNI] AudioTrack::<init>: Already initialized!");
    }

    // 加琐，防止异步同时调用
    sceKernelLockLwMutex(&audio_main_mutex, 1, NULL);

    int track_id = -1;
    for (int i = 0; i < AUDIO_TRACKS_NUM; i++) {
        if (!audio_tracks[i].is_using) {
            track_id = i;
            break;
        }
    }

    if (track_id == -1) {
        l_error("[JNI] AudioTrack::<init>: no free track!");
        return NULL;
    }

    AudioTrack *track = &audio_tracks[track_id];
    if (!track->is_inited) {
        track->buffer = malloc(AUDIO_TRACK_BUFFER_LEN * sizeof(int16_t));
        if (!track->buffer) {
            l_error("[JNI] AudioTrack::<init>: failed to alloc audio buffer!");
            return NULL;
        }
        track->buffer_len = AUDIO_TRACK_BUFFER_LEN;
        sceKernelCreateLwMutex(&track->mutex, "audio_track_mutex", 2, 0, NULL);
        track->is_inited = 1;
    }

    AudioTrack **res = malloc(sizeof(AudioTrack *));
    if (!res) {
        l_error("[JNI] AudioTrack::<init>: failed to alloc obj");
        return NULL;
    }
    *res = &audio_tracks[track_id];

    // 加锁，防止旧Track可能还在播放
    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    track->is_using   = 1;
    track->status     = AUDIO_STATUS_STOPPED;
    track->is_stereo  = is_stereo;
    track->is_8bit    = is_8bit;
    track->has_new    = 0;
    track->output_len = AUDIO_OUTPUT_LEN;
    track->read_pos = track->write_pos = 0;
    track->left_volume = track->right_volume = 1.0f;
    sceKernelUnlockLwMutex(&track->mutex, 1);

    sceKernelUnlockLwMutex(&audio_main_mutex, 1);

    l_debug("[JNI] [%d] AudioTrack::<init>(%d,%d,%d,%d,%d,%d): %p",
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
        l_error("[JNI] AudioTrack::write(%p,%p,%d,%d) failed: null obj!", obj, audioData, offsetInBytes, sizeInBytes);
        return (jint)res;
    }

    AudioTrack *track = *(AudioTrack **)obj;
    if (!track) {
        l_error("[JNI] AudioTrack::write(%p,%p,%d,%d) failed: error obj!", obj, audioData, offsetInBytes, sizeInBytes);
        return (jint)res;
    }

    if (!audio_inited) {
        l_error("[JNI] AudioTrack::write(%p,%p,%d,%d): failed!", obj, audioData, offsetInBytes, sizeInBytes);
        return (jint)res;
    }

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    if (track->status == AUDIO_STATUS_PLAYYING) {
        jbyte *bytes = jni->GetByteArrayElements(&jni, audioData, NULL);
        if (bytes) {
            int16_t *in  = (int16_t *)(bytes + offsetInBytes);
            int16_t *out = (int16_t *)(track->buffer);
            for (int i = 0; i < sizeInBytes / 2; i++) {
                out[track->write_pos++] = in[i];
                if (track->write_pos >= track->buffer_len)
                    track->write_pos = 0;
            }
            jni->ReleaseByteArrayElements(&jni, audioData, bytes, 0);
            track->has_new = 1;
        }
    }
    sceKernelUnlockLwMutex(&track->mutex, 1);

    // l_debug("[JNI] [%d] AudioTrack::write(%p,%p,%d,%d): %d", track->id, obj, audioData, offsetInBytes, sizeInBytes, res);
    return (jint)res;
}

// "getMinBufferSize", "(III)I"
jint AudioTrack_getMinBufferSize(jclass clazz, jmethodID id, va_list args) {
    int sampleRateInHz = va_arg(args, int);
    int channelConfig  = va_arg(args, int);
    int audioFormat    = va_arg(args, int);

    int channels       = (channelConfig == Android_AUDIO_CHANNEL_OUT_STEREO) ? 2 : 1;
    int bytesPerSample = (audioFormat == Android_AUDIO_FORMAT_PCM_16_BIT) ? 2 : 1;  // 16-bit = 2 字节

    // 建议值：至少能容纳 10ms 的数据
    int minFrames = sampleRateInHz / 100;                   // 10ms 的帧数
    int minSize   = minFrames * channels * bytesPerSample;  // 10ms 的字节数

    int outputSize = AUDIO_OUTPUT_LEN * bytesPerSample;

    // 确保不小于硬件输出块大小
    if (minSize < outputSize) {
        minSize = outputSize;
    }

    int res = minSize;
    l_debug("[JNI] AudioTrack::getMinBufferSize(%p,%d,%d,%d): %d", clazz, sampleRateInHz, channelConfig, audioFormat, res);
    return (jint)res;
}

// "setStereoVolume", "(FF)I"
jint AudioTrack_setStereoVolume(jobject obj, jmethodID id, va_list args) {
    int   res         = 0;
    float leftVolume  = va_arg(args, double);
    float rightVolume = va_arg(args, double);

    if (!obj) {
        l_error("[JNI] AudioTrack::setStereoVolume(%p,%f,%f) failed: null obj!", obj, leftVolume, rightVolume);
        return (jint)res;
    }

    AudioTrack *track = *(AudioTrack **)obj;
    if (!track) {
        l_error("[JNI] AudioTrack::setStereoVolume(%p,%f,%f) failed: error obj!", obj, leftVolume, rightVolume);
        return (jint)res;
    }

    if (!audio_inited) {
        l_error("[JNI] AudioTrack::setStereoVolume(%p,%f,%f) failed: audio not initialized ", obj, leftVolume, rightVolume);
        return (jint)res;
    }

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    track->left_volume  = leftVolume;
    track->right_volume = rightVolume;
    sceKernelUnlockLwMutex(&track->mutex, 1);

    l_debug("[JNI] [%d] AudioTrack::setStereoVolume(%p,%f,%f): %d", track->id, obj, leftVolume, rightVolume, res);
    return (jint)res;
}

// "play", "()V"
void AudioTrack_play(jobject obj, jmethodID id, va_list args) {
    if (!obj) {
        l_error("[JNI] AudioTrack::play(%p) failed: null obj!", obj);
        return;
    }

    AudioTrack *track = *(AudioTrack **)obj;
    if (!track) {
        l_error("[JNI] AudioTrack::play(%p) failed: error obj!", obj);
        return;
    }

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    track->status = AUDIO_STATUS_PLAYYING;
    sceKernelUnlockLwMutex(&track->mutex, 1);

    l_debug("[JNI] [%d] AudioTrack::play(%p)", track->id, obj);
}

// "pause", "()V"
void AudioTrack_pause(jobject obj, jmethodID id, va_list args) {
    if (!obj) {
        l_error("[JNI] AudioTrack::pause(%p) failed: null obj!", obj);
        return;
    }

    AudioTrack *track = *(AudioTrack **)obj;
    if (!track) {
        l_error("[JNI] AudioTrack::pause(%p) failed: error obj!", obj);
        return;
    }

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    track->status = AUDIO_STATUS_PAUSED;
    sceKernelUnlockLwMutex(&track->mutex, 1);

    l_debug("[JNI] [%d] AudioTrack::pause(%p)", track->id, obj);
}

// "stop", "()V"
void AudioTrack_stop(jobject obj, jmethodID id, va_list args) {
    if (!obj) {
        l_error("[JNI] AudioTrack::stop(%p) failed: null obj!", obj);
        return;
    }

    AudioTrack *track = *(AudioTrack **)obj;
    if (!track) {
        l_error("[JNI] AudioTrack::stop(%p) failed: error obj!", obj);
        return;
    }

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    track->status = AUDIO_STATUS_STOPPED;
    sceKernelUnlockLwMutex(&track->mutex, 1);

    l_debug("[JNI] [%d] AudioTrack::stop(%p)", track->id, obj);
}

// "flush", "()V"
void AudioTrack_flush(jobject obj, jmethodID id, va_list args) {
    if (!obj) {
        l_error("[JNI] AudioTrack::flush(%p) failed: null obj!", obj);
        return;
    }

    AudioTrack *track = *(AudioTrack **)obj;
    if (!track) {
        l_error("[JNI] AudioTrack::flush(%p) failed: error obj!", obj);
        return;
    }

    l_debug("[JNI] [%d] AudioTrack::flush(%p)", track->id, obj);
}

// "release", "()V"
void AudioTrack_release(jobject obj, jmethodID id, va_list args) {
    if (!obj) {
        l_error("[JNI] AudioTrack::release(%p) failed: null obj!", obj);
        return;
    }

    AudioTrack *track = *(AudioTrack **)obj;
    if (!track) {
        l_error("[JNI] AudioTrack::release(%p) failed: error obj!", obj);
        return;
    }

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    track->status   = AUDIO_STATUS_STOPPED;
    track->is_using = 0;
    sceKernelUnlockLwMutex(&track->mutex, 1);

    l_debug("[JNI] [%d] AudioTrack::release(%p)", track->id, obj);
}
