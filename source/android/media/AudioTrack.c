#include <stdlib.h>
#include <string.h>

#include <psp2/audioout.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/processmgr.h>
#include <so_common/so_logger.h>
#include <so_jni/so_jni.h>

#include "AudioTrack.h"

// Android 4.4 channel config
#define CHANNEL_OUT_FRONT_LEFT  0x4
#define CHANNEL_OUT_FRONT_RIGHT 0x8
#define CHANNEL_OUT_MONO        CHANNEL_OUT_FRONT_LEFT
#define CHANNEL_OUT_STEREO      (CHANNEL_OUT_FRONT_LEFT | CHANNEL_OUT_FRONT_RIGHT)

// Android 4.4 audio format
#define ENCODING_INVALID   0
#define ENCODING_DEFAULT   1
#define ENCODING_PCM_16BIT 2
#define ENCODING_PCM_8BIT  3

// Android 4.4 stream type
#define STREAM_MUSIC 3

// Android 4.4 audio data mode
#define MODE_STATIC 0
#define MODE_STREAM 1

#define OUTPUTS_NUM 3            // 输出缓冲区的缓存数量
#define OUTPUT_LEN  256          // 每次输出的样本数
#define TRACKS_NUM  8            // 音轨数量
#define TRACK_LEN   (1024 << 8)  // 音轨缓冲区的样本数

enum {
    PLAYSTATE_STOPPED = 0,
    PLAYSTATE_PAUSED  = 1,
    PLAYSTATE_PLAYING = 2,
};

typedef struct Track {
    jint                 slot;
    jint                 status;
    jboolean             has_new;
    jboolean             is_released;
    jboolean             is_stereo;
    jboolean             is_8bit;
    jfloat               left_volume;
    jfloat               right_volume;
    int16_t              buffer[TRACK_LEN];
    size_t               read_pos;
    size_t               write_pos;
    SceKernelLwMutexWork mutex;
} Track;

struct _AudioTrack {
    Track *track;
};

static JClass sClass = NULL;

static jboolean             audio_inited                           = JNI_FALSE;
static SceUID               audio_thid                             = -1;
static SceUID               audio_port                             = -1;
static jboolean             audio_pause                            = JNI_FALSE;
static jboolean             audio_stop                             = JNI_FALSE;
static Track               *audio_tracks[TRACKS_NUM]               = {0};
static jshort               audio_outputs[OUTPUTS_NUM][OUTPUT_LEN] = {0};
static SceKernelLwMutexWork audio_main_mutex;

static size_t mix_track(int16_t *output, size_t output_len, const int16_t *input, size_t input_len, float volume) {
    size_t mixed_len = output_len < input_len ? output_len : input_len;

    for (size_t i = 0; i < mixed_len; i++) {
        int32_t sum = (int32_t)output[i] + (int32_t)input[i] * volume;
        if (sum > 32767) sum = 32767;
        if (sum < -32768) sum = -32768;
        output[i] = (int16_t)sum;
    }

    return mixed_len;
}

static int audio_thread(SceSize args, void *argp) {
    so_logd("[AUDIO] Audio thread start run.\n");

    size_t   available_len;
    size_t   active_tracks;
    int16_t *output_buffer;
    size_t   buffer_idx = 0;
    size_t   output_len;

    while (!audio_stop) {
        active_tracks = 0;
        output_len    = 0;
        output_buffer = audio_outputs[buffer_idx];

        // 混音是累加操作，输出缓冲区必须初始化为 0，否则会有上一帧残留数据。
        memset(output_buffer, 0, OUTPUT_LEN * sizeof(int16_t));

        for (int i = 0; i < TRACKS_NUM; i++) {
            Track *track = audio_tracks[i];
            if (!track) continue;

            sceKernelLockLwMutex(&track->mutex, 1, NULL);
            if (!track->has_new) {
                if (track->is_released) {  // 无待播放音源时才释放
                    sceKernelLockLwMutex(&audio_main_mutex, 1, NULL);
                    audio_tracks[i] = NULL;
                    sceKernelUnlockLwMutex(&audio_main_mutex, 1);

                    sceKernelUnlockLwMutex(&track->mutex, 1);
                    sceKernelDeleteLwMutex(&track->mutex);
                    free(track);
                    continue;
                }
                sceKernelUnlockLwMutex(&track->mutex, 1);
                continue;
            }
            if (track->write_pos < track->read_pos) {
                available_len = track->write_pos + TRACK_LEN - track->read_pos;
            } else {
                available_len = track->write_pos - track->read_pos;
            }
            if (available_len <= 0) {
                track->has_new = 0;
                sceKernelUnlockLwMutex(&track->mutex, 1);
                continue;
            }
            sceKernelUnlockLwMutex(&track->mutex, 1);

            size_t mixed_len = available_len < OUTPUT_LEN ? available_len : OUTPUT_LEN;
            if (track->status != PLAYSTATE_PAUSED) {
                mixed_len = mix_track(output_buffer, OUTPUT_LEN, track->buffer + track->read_pos, mixed_len, track->left_volume);
            }
            track->read_pos = (track->read_pos + mixed_len) % TRACK_LEN;
            active_tracks++;

            if (output_len < mixed_len) {
                output_len = mixed_len;
            }
        }

        if (active_tracks <= 0) {
            sceKernelDelayThread(10 * 1000);
            continue;
        }

        sceAudioOutOutput(audio_port, output_buffer);
        buffer_idx = (buffer_idx + 1) % OUTPUTS_NUM;
    }

    so_logd("[AUDIO] Audio thread stop run.\n");
    sceKernelExitThread(0);
    return 0;
}

static int audio_init(jint sampleRateInHz, jint stereo) {
    SceAudioOutMode out_mode = stereo ? SCE_AUDIO_OUT_MODE_STEREO : SCE_AUDIO_OUT_MODE_MONO;

    audio_port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, OUTPUT_LEN, sampleRateInHz, out_mode);
    if (audio_port < 0) {
        so_logd("[AudioTrack] audio_init: sceAudioOutOpenPort failed!");
        return -1;
    }

    audio_thid = sceKernelCreateThread("audio_thread", audio_thread, 0x10000100, 0x40000, 0, 0, NULL);
    if (audio_thid < 0) {
        sceAudioOutReleasePort(audio_port);
        audio_port = -1;
        so_logd("[AudioTrack] audio_init: failed to create thread!");
        return JNI_ERR;
    }
    sceKernelCreateLwMutex(&audio_main_mutex, "audio_main_mutex", 2, 0, NULL);

    audio_stop = 0;
    if (sceKernelStartThread(audio_thid, 0, NULL) < 0) {
        sceKernelDeleteLwMutex(&audio_main_mutex);
        sceAudioOutReleasePort(audio_port);
        audio_port = -1;
        so_logd("[AudioTrack] audio_init: failed to start thread!");
        return JNI_ERR;
    }

    audio_inited = 1;
    so_logd("[AudioTrack] audio_init: initialize OK!");
    return JNI_OK;
}

void AudioTrack_ctor(AudioTrack thiz, jint streamType, jint sampleRateInHz, jint channelConfig,
                     jint audioFormat, int bufferSizeInBytes, int mode) {  // (3,16000,4,2,4096,1)
    thiz->track = NULL;

    jboolean is_stereo = (channelConfig == CHANNEL_OUT_STEREO) ? 1 : 0;
    jboolean is_8bit   = (audioFormat == ENCODING_PCM_8BIT) ? 1 : 0;

    if (!audio_inited && audio_init(sampleRateInHz, is_stereo) != JNI_OK) {
        return;
    }

    Track *track = malloc(sizeof(Track));
    if (!track) return;

    track->slot         = -1;
    track->status       = PLAYSTATE_STOPPED;
    track->has_new      = JNI_FALSE;
    track->is_released  = JNI_FALSE;
    track->is_stereo    = is_stereo;
    track->is_8bit      = is_8bit;
    track->left_volume  = 1.0f;
    track->right_volume = 1.0f;
    track->read_pos     = 0;
    track->write_pos    = 0;

    sceKernelCreateLwMutex(&track->mutex, "audio_track_mutex", 2, 0, NULL);

    sceKernelLockLwMutex(&audio_main_mutex, 1, NULL);
    for (jint i = 0; i < TRACKS_NUM; i++) {
        if (audio_tracks[i] == NULL) {
            track->slot = i;
            break;
        }
    }
    if (track->slot == -1) {
        so_logd("[AudioTrack] ctor: no free slot!");
        sceKernelDeleteLwMutex(&track->mutex);
        free(track);
        sceKernelUnlockLwMutex(&audio_main_mutex, 1);
        return;
    }
    thiz->track               = track;
    audio_tracks[track->slot] = track;
    sceKernelUnlockLwMutex(&audio_main_mutex, 1);

    so_logd("[AudioTrack][%d] ctor(%p,%d,%d,%d,%d,%d,%d)",
            track->slot, thiz, streamType, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes, mode);
}

jint AudioTrack_write(AudioTrack thiz, jbyte *audioData, int offsetInBytes, int sizeInBytes) {
    Track *track = thiz->track;
    if (!track) return -1;

    jint result = sizeInBytes;

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    if (track->status == PLAYSTATE_PLAYING) {
        int16_t *in_buf    = (int16_t *)(audioData + offsetInBytes);
        int16_t *out_buf   = (int16_t *)(track->buffer);
        size_t   write_pos = track->write_pos;
        size_t   in_len    = sizeInBytes / 2;

        if (write_pos + in_len > TRACK_LEN) {
            size_t first_part  = TRACK_LEN - write_pos;
            size_t second_part = in_len - first_part;

            memcpy(out_buf + write_pos, in_buf, first_part * sizeof(int16_t));
            memcpy(out_buf, in_buf + first_part, second_part * sizeof(int16_t));
        } else {
            memcpy(out_buf + write_pos, in_buf, sizeInBytes);
        }

        track->write_pos = (track->write_pos + in_len) % TRACK_LEN;
        track->has_new   = JNI_TRUE;
    }
    sceKernelUnlockLwMutex(&track->mutex, 1);

    so_logd("[AudioTrack][%d] write(%p,%p,%d,%d): %d", track->slot, thiz, audioData, offsetInBytes, sizeInBytes, result);
    return result;
}

jint AudioTrack_setStereoVolume(AudioTrack thiz, jfloat leftVolume, jfloat rightVolume) {
    Track *track = thiz->track;
    if (!track) return -1;

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    track->left_volume  = leftVolume;
    track->right_volume = rightVolume;
    sceKernelUnlockLwMutex(&track->mutex, 1);
    so_logd("[AudioTrack][%d] setStereoVolume(%p,%f,%f): %d", track->slot, thiz, leftVolume, rightVolume, 0);
    return 0;
}

void AudioTrack_play(AudioTrack thiz) {
    Track *track = thiz->track;
    if (!track) return;

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    track->status = PLAYSTATE_PLAYING;
    sceKernelUnlockLwMutex(&track->mutex, 1);
    so_logd("[AudioTrack][%d] play(%p)", track->slot, thiz);
}

void AudioTrack_pause(AudioTrack thiz) {
    Track *track = thiz->track;
    if (!track) return;

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    track->status = PLAYSTATE_PAUSED;
    sceKernelUnlockLwMutex(&track->mutex, 1);
    so_logd("[AudioTrack][%d] pause(%p)", track->slot, thiz);
}

void AudioTrack_stop(AudioTrack thiz) {
    Track *track = thiz->track;
    if (!track) return;

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    track->status = PLAYSTATE_STOPPED;
    sceKernelUnlockLwMutex(&track->mutex, 1);
    so_logd("[AudioTrack][%d] stop(%p)", track->slot, thiz);
}

void AudioTrack_flush(AudioTrack thiz) {
    so_logd("[AudioTrack][%d] flush(%p)", thiz);
}

void AudioTrack_release(AudioTrack thiz) {
    Track *track = thiz->track;
    if (!track) return;

    sceKernelLockLwMutex(&track->mutex, 1, NULL);
    track->status      = PLAYSTATE_STOPPED;
    track->is_released = JNI_TRUE;
    thiz->track        = NULL;
    sceKernelUnlockLwMutex(&track->mutex, 1);
    so_logd("[AudioTrack][%d] release(%p)", track->slot, thiz);
}

jint AudioTrack_getMinBufferSize(jint sampleRateInHz, jint channelConfig, jint audioFormat) {
    jint channels       = (channelConfig == CHANNEL_OUT_STEREO) ? 2 : 1;
    jint bytesPerSample = (audioFormat == ENCODING_PCM_16BIT) ? 2 : 1;  // 16-bit = 2 字节

    // 建议值：至少能容纳 10ms 的数据
    jint minFrames = sampleRateInHz / 100;                   // 10ms 的帧数
    jint minSize   = minFrames * channels * bytesPerSample;  // 10ms 的字节数

    jint outputSize = OUTPUT_LEN * bytesPerSample;

    // 确保不小于硬件输出块大小
    if (minSize < outputSize) {
        minSize = outputSize;
    }

    so_logd("[AudioTrack] getMinBufferSize(%d,%d,%d): %d", sampleRateInHz, channelConfig, audioFormat, minSize);
    return minSize;
}

static JavaSymbol class_symbols[] = {
    // JAVA
    JAVA_SYMBOL_METHOD("<init>", "(IIIIII)V", &AudioTrack_ctor),
    JAVA_SYMBOL_METHOD("write", "([BII)I", &AudioTrack_write),
    JAVA_SYMBOL_METHOD("setStereoVolume", "(FF)I", &AudioTrack_setStereoVolume),
    JAVA_SYMBOL_METHOD("play", "()V", &AudioTrack_play),
    JAVA_SYMBOL_METHOD("pause", "()V", &AudioTrack_pause),
    JAVA_SYMBOL_METHOD("stop", "()V", &AudioTrack_stop),
    JAVA_SYMBOL_METHOD("flush", "()V", &AudioTrack_flush),
    JAVA_SYMBOL_METHOD("release", "()V", &AudioTrack_release),
    // static JAVA
    JAVA_SYMBOL_STATIC_METHOD("getMinBufferSize", "(III)I", &AudioTrack_getMinBufferSize),
};

static JavaClassMeta class_meta = {
    .name          = "android/media/AudioTrack",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _AudioTrack),
};

void android_media_AudioTrack_class_init(void) {
    sClass = JavaCreateClass(&class_meta);
}
