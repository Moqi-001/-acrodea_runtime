#ifndef AUDIO_RESAMPLER_H
#define AUDIO_RESAMPLER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct audio_resampler audio_resampler_t;

/* 创建重采样器
 * in_rate  : 输入采样率
 * out_rate : 输出采样率
 * 返回 NULL 表示失败。质量固定为 Kaiser taps=16（RetroArch NORMAL）。
 */
audio_resampler_t *audio_resampler_create(double in_rate, double out_rate);

/* 销毁 */
void audio_resampler_destroy(audio_resampler_t *re);

/* 重采样
 * in         : 输入 float 立体声交错缓冲（L,R,L,R,...）
 * in_frames  : 输入帧数（每帧 2 个 float）
 * out        : 输出缓冲，容量至少 in_frames * out_rate/in_rate + 2 帧
 * 返回实际输出的帧数。
 */
size_t audio_resampler_process(audio_resampler_t *re,
                                const float *in,
                                size_t in_frames,
                                float *out);

#ifdef __cplusplus
}
#endif

#endif