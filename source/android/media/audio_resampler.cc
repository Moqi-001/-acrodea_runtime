#include <malloc.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "audio_resampler.h"

static float sincf(float x) {
    if (x == 0.0f) return 1.0f;
    float px = (float)M_PI * x;
    return sinf(px) / px;
}

/* 第一类零阶修正贝塞尔函数 I0(x)，级数展开，足够精度 */
static double besseli0(double x) {
    double sum  = 1.0;
    double term = 1.0;
    double x2   = x * x * 0.25;
    int    k;
    for (k = 1; k < 50; k++) {
        term *= x2 / ((double)k * (double)k);
        sum += term;
        if (term < 1e-12 * sum) break;
    }
    return sum;
}

#define PHASE_BITS    8
#define SUBPHASE_BITS 16
#define TAPS          16 /* Kaiser, sidelobes=8, taps=16 */
#define KAISER_BETA   5.5f
#define CUTOFF        0.825f

typedef struct audio_resampler {
    float   *main_buffer; /* phase_table + buffer_l + buffer_r 一块分配 */
    float   *phase_table; /* 系数 + delta 表，stride=2 */
    float   *buffer_l;
    float   *buffer_r;
    unsigned phase_bits;
    unsigned subphase_bits;
    unsigned subphase_mask;
    unsigned taps;
    unsigned ptr;
    uint32_t time;
    float    subphase_mod;
    double   ratio; /* 输出/输入 */
    uint32_t phases;
} audio_resampler_t;

/* 生成 Kaiser 窗 sinc 系数表（含 delta 插值表） */
static void init_table_kaiser(audio_resampler_t *re) {
    const int    phases     = 1 << re->phase_bits;
    const int    taps       = (int)re->taps;
    const double sidelobes  = taps / 2.0;
    const double window_mod = besseli0(KAISER_BETA);
    int          i, j;

    for (i = 0; i < phases; i++) {
        for (j = 0; j < taps; j++) {
            int    n                          = j * phases + i;
            double window_phase               = (double)n / (phases * taps);
            window_phase                      = 2.0 * window_phase - 1.0;
            double sinc_phase                 = sidelobes * window_phase;
            double val                        = CUTOFF * sincf((float)(sinc_phase * CUTOFF)) *
                                                besseli0(KAISER_BETA * sqrt(1.0 - window_phase * window_phase)) /
                                                window_mod;
            re->phase_table[i * 2 * taps + j] = (float)val;
        }
    }

    /* 计算 delta 表（线性插值用） */
    {
        int p;
        for (p = 0; p < phases - 1; p++) {
            for (j = 0; j < taps; j++) {
                float delta                             = re->phase_table[(p + 1) * 2 * taps + j] -
                                                          re->phase_table[p * 2 * taps + j];
                re->phase_table[(p * 2 + 1) * taps + j] = delta;
            }
        }
        /* 最后一个相位，用 phase+1 的系数计算 delta */
        {
            int phase = phases - 1;
            for (j = 0; j < taps; j++) {
                int    n                                    = j * phases + (phase + 1);
                double window_phase                         = (double)n / (phases * taps);
                window_phase                                = 2.0 * window_phase - 1.0;
                double sinc_phase                           = sidelobes * window_phase;
                double val                                  = CUTOFF * sincf((float)(sinc_phase * CUTOFF)) *
                                                              besseli0(KAISER_BETA * sqrt(1.0 - window_phase * window_phase)) /
                                                              window_mod;
                float  delta                                = (float)val - re->phase_table[phase * 2 * taps + j];
                re->phase_table[(phase * 2 + 1) * taps + j] = delta;
            }
        }
    }
}

audio_resampler_t *audio_resampler_create(double in_rate, double out_rate) {
    audio_resampler_t *re = (audio_resampler_t *)calloc(1, sizeof(*re));
    if (!re) return NULL;

    re->phase_bits    = PHASE_BITS;
    re->subphase_bits = SUBPHASE_BITS;
    re->subphase_mask = (1u << SUBPHASE_BITS) - 1u;
    re->subphase_mod  = 1.0f / (float)(1u << SUBPHASE_BITS);
    re->taps          = TAPS;
    re->ratio         = out_rate / in_rate;
    re->phases        = 1u << (PHASE_BITS + SUBPHASE_BITS);

    /* 计算 bandwith_mod：升采样时 1.0，降采样时按比例 */
    double   bandwidth_mod = (out_rate >= in_rate) ? 1.0 : (out_rate / in_rate);
    unsigned taps          = TAPS;
    double   cutoff        = CUTOFF;
    if (bandwidth_mod < 1.0) {
        cutoff *= bandwidth_mod;
        taps = (unsigned)ceil(taps / bandwidth_mod);
    }
    /* NEON 对齐：taps 取 8 的倍数 */
    taps     = (taps + 7u) & ~7u;
    re->taps = taps;

    size_t phase_elems = (size_t)(1u << PHASE_BITS) * taps * 2;
    size_t elems       = phase_elems + 4 * taps;

    re->main_buffer = (float *)memalign(128, sizeof(float) * elems);
    if (!re->main_buffer) {
        free(re);
        return NULL;
    }
    memset(re->main_buffer, 0, sizeof(float) * elems);

    re->phase_table = re->main_buffer;
    re->buffer_l    = re->main_buffer + phase_elems;
    re->buffer_r    = re->buffer_l + 2 * taps;

    init_table_kaiser(re);
    return re;
}

void audio_resampler_destroy(audio_resampler_t *re) {
    if (!re) return;
    free(re->main_buffer);
    free(re);
}

#if defined(__ARM_NEON__)
#include <arm_neon.h>

static size_t process_neon_kaiser(audio_resampler_t *re,
                                  const float       *input,
                                  size_t             frames,
                                  float             *output) {
    const unsigned phases        = re->phases;
    const uint32_t ratio         = phases / re->ratio;
    const unsigned taps          = re->taps;
    const unsigned subphase_bits = re->subphase_bits;
    const unsigned subphase_mask = re->subphase_mask;
    const float    subphase_mod  = re->subphase_mod;
    float         *out           = output;
    size_t         out_frames    = 0;

    while (frames) {
        while (frames && re->time >= phases) {
            if (!re->ptr) re->ptr = taps;
            re->ptr--;

            re->buffer_l[re->ptr + taps] = re->buffer_l[re->ptr] = *input++;
            re->buffer_r[re->ptr + taps] = re->buffer_r[re->ptr] = *input++;

            re->time -= phases;
            frames--;
        }

        {
            const float *buffer_l = re->buffer_l + re->ptr;
            const float *buffer_r = re->buffer_r + re->ptr;

            while (re->time < phases) {
                unsigned     phase       = re->time >> subphase_bits;
                const float *phase_table = re->phase_table + phase * taps * 2;
                const float *delta_table = phase_table + taps;
                float32x4_t  delta       = vdupq_n_f32(
                    (float)(re->time & subphase_mask) * subphase_mod);
                float32x4_t p1 = vdupq_n_f32(0.0f);
                float32x4_t p2 = vdupq_n_f32(0.0f);
                int         i;

                for (i = 0; i < (int)taps; i += 8) {
                    float32x4x2_t coeff8 = vld2q_f32(&phase_table[i]);
                    float32x4x2_t delta8 = vld2q_f32(&delta_table[i]);
                    float32x4x2_t left8  = vld2q_f32(&buffer_l[i]);
                    float32x4x2_t right8 = vld2q_f32(&buffer_r[i]);

                    coeff8.val[0] = vmlaq_f32(coeff8.val[0], delta8.val[0], delta);
                    coeff8.val[1] = vmlaq_f32(coeff8.val[1], delta8.val[1], delta);

                    p1 = vmlaq_f32(p1, left8.val[0], coeff8.val[0]);
                    p2 = vmlaq_f32(p2, right8.val[0], coeff8.val[0]);
                    p1 = vmlaq_f32(p1, left8.val[1], coeff8.val[1]);
                    p2 = vmlaq_f32(p2, right8.val[1], coeff8.val[1]);
                }

                {
                    float32x2_t p3 = vadd_f32(vget_low_f32(p1), vget_high_f32(p1));
                    float32x2_t p4 = vadd_f32(vget_low_f32(p2), vget_high_f32(p2));
                    vst1_f32(out, vpadd_f32(p3, p4));
                }

                out += 2;
                out_frames++;
                re->time += ratio;
            }
        }
    }

    return out_frames;
}

#else

static size_t process_c_kaiser(audio_resampler_t *re,
                               const float       *input,
                               size_t             frames,
                               float             *output) {
}

#endif

size_t audio_resampler_process(audio_resampler_t *re,
                               const float       *in,
                               size_t             in_frames,
                               float             *out) {
#if defined(__ARM_NEON__)
    return process_neon_kaiser(re, in, in_frames, out);
#else
    return process_c_kaiser(re, in, in_frames, out);
#endif
}