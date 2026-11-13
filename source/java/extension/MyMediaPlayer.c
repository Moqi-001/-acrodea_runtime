// com.acrodea.vividruntime.launcher.extension.MyMediaPlayer
//
// Android 上这个类是 acrodea 自己写的 Java 包装：内部持有一个
// android.media.MediaPlayer，把 openFile/start/stop/pause/setVolume… 转发给它。
// runtime.so 的音频后端（__slBackendMP3OpenFile）通过 JNI 调它来播 BGM，
// 所以 Vita 上由我们提供等价实现。
//
// 实现分三层，从上到下：
//
//   Java 方法（openFile/start/stop/pause/setVolume…）
//       只做状态机和参数整理，真正的动作在下面两层。
//
//   资源层：openFile(path, offset, size)
//       runtime 传来的 offset/size 是「容器内绝对偏移」（如 res.pak 尾部目录）。
//       先把 path 当真实文件找（散装资源），找不到就打开容器按区间读。
//       「文件 + 区间」包成 SrcFile，挂到 mpg123 的 reader 回调上，
//       解码统一为 16bit 交错 PCM。
//
//   输出层：start() 起解码线程
//       mpg123_read 解出 PCM 后直接写平台 AudioTrack —— 重采样到 48kHz、
//       多轨混音、音频端口都由它负责，这里只喂 PCM，不碰端口。
//
// 线程模型：一把锁（lock）保护状态与 mpg123 句柄；解码线程持有锁的时间
// 只覆盖「取状态/执行 seek」，解码和写音轨都在锁外做，避免长时间占锁。
// 停止时先把音轨置 STOPPED（让卡在 AudioTrack_write 的线程解除阻塞）再 join。

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <unistd.h>

#include <mpg123.h>

#include <so_common/so_logger.h>
#include <so_java/android/media/AudioFormat.h>
#include <so_java/android/media/AudioTrack.h>
#include <so_jni/so_jni.h>

#include "MyMediaPlayer.h"

#include "config.h"

// 每次解码的帧数。AudioTrack 内部有环形缓冲（TRACK_BUFFER_FRAMES），
// 写入即返回、缓冲满时它自己阻塞，所以这里不需要额外的节流。
#define MMP_GRAIN 1024

// AudioTrack 的构造参数：MUSIC 流 + 流模式（非 STATIC）。
#define MMP_STREAM_MUSIC 3
#define MMP_MODE_STREAM  1

#define MMP_STATE_IDLE    0
#define MMP_STATE_PLAYING 1
#define MMP_STATE_PAUSED  2

// 容器候选：runtime 传来的 path 可能是逻辑名，也可能已经是容器路径。
static const char *const s_containers[] = {
    "res.pak",
    DATA_PATH "res.pak",
    "app0:res.pak",
    "ux0:app/" RUNTIME_APP_ID_STR "/res.pak",
};

// 逻辑路径的查找根目录。
static const char *const s_roots[] = {
    "",
    DATA_PATH,
};

// openFile 的错误码语义对齐 Android MediaPlayer 的 error 返回。
#define MMP_OPEN_FAILED (-4)

// openFile 的日志格式：先原样列出 Java 方法的全部入参，后面接结果。
// 这个方法在资源层有多条失败路径，统一用它，省得每处重复四个参数。
#define MMP_OPEN_LOG_ARGS(thiz, path, offset, size) \
    (void *)(thiz), (path), (long long)(offset), (long long)(size)
#define MMP_OPEN_LOG_FMT "[MyMediaPlayer] openFile(this=%p, path='%s', offset=%lld, size=%lld): "

// ---------------------------------------------------------------- 文件区间读取

// mpg123 的读取句柄：把「文件 + 区间」包成它的 reader 回调。
// pos 是区间内的逻辑位置，fpos 缓存 FILE* 的真实位置以省掉重复 fseek。
typedef struct {
    FILE   *fp;
    int64_t base;  // 数据在文件内的起点
    int64_t len;   // 有效长度
    int64_t pos;   // 当前逻辑位置（相对 base）
    int64_t fpos;  // FILE* 当前实际位置
} SrcFile;

static mpg123_ssize_t src_read(void *handle, void *buf, size_t size) {
    SrcFile *s = (SrcFile *)handle;

    if (s->pos >= s->len) return 0;
    if ((int64_t)size > s->len - s->pos) size = (size_t)(s->len - s->pos);
    if (size == 0) return 0;

    int64_t want = s->base + s->pos;
    if (s->fpos != want) {
        if (fseek(s->fp, (long)want, SEEK_SET) != 0) return -1;
        s->fpos = want;
    }

    size_t got = fread(buf, 1, size, s->fp);
    s->pos += (int64_t)got;
    s->fpos += (int64_t)got;
    return (mpg123_ssize_t)got;
}

static off_t src_seek(void *handle, off_t offset, int whence) {
    SrcFile *s = (SrcFile *)handle;
    int64_t  np;

    switch (whence) {
        case SEEK_SET:
            np = offset;
            break;
        case SEEK_CUR:
            np = s->pos + offset;
            break;
        case SEEK_END:
            np = s->len + offset;
            break;
        default:
            return -1;
    }
    if (np < 0) return -1;

    s->pos = np;
    return (off_t)np;
}

// 找到资源所在文件：先按真实文件找（散装资源），找不到再开容器。
// *out_used 返回实际打开的路径（日志用）。
static FILE *mmp_open_source(const char *path, int64_t size, char *out_used, size_t used_size) {
    // 1) 逻辑路径当真实文件找
    for (unsigned i = 0; i < sizeof(s_roots) / sizeof(s_roots[0]); i++) {
        snprintf(out_used, used_size, "%s%s", s_roots[i], path);
        FILE *fp = fopen(out_used, "rb");
        if (fp) return fp;
    }

    // 2) 找不到就开容器：runtime 给的 size 就是对容器内的区间算的
    if (size > 0) {
        for (unsigned i = 0; i < sizeof(s_containers) / sizeof(s_containers[0]); i++) {
            FILE *fp = fopen(s_containers[i], "rb");
            if (fp) {
                snprintf(out_used, used_size, "%s", s_containers[i]);
                return fp;
            }
        }
    }

    out_used[0] = '\0';
    return NULL;
}

// ---------------------------------------------------------------- 对象与状态

struct _MyMediaPlayer {
    pthread_mutex_t lock;
    pthread_t       thread;
    int             thread_live;

    volatile int state;
    volatile int stop_req;

    jboolean is_looping;
    jboolean is_completed;
    int      error;

    int vol_l;  // 0..256 定点
    int vol_r;

    int rate;      // 采样率
    int channels;  // 1/2
    int duration_ms;
    int pos_ms;

    int seek_ms;
    int seek_req;

    AudioTrack     track;  // 平台音轨：重采样/混音/端口都由它负责
    mpg123_handle *mh;
    SrcFile        src;
    int            opened;
};

static JClass sClass = NULL;

static void mmp_close_locked(MyMediaPlayer thiz);

// 打开失败统一走这里：关掉已打开的资源，记错误码。
static int mmp_open_failed(MyMediaPlayer thiz, int code) {
    mmp_close_locked(thiz);
    thiz->error = code;
    return MMP_OPEN_FAILED;
}

// ---------------------------------------------------------------- 资源层：openFile

static void mmp_close_locked(MyMediaPlayer thiz) {
    if (thiz->mh) {
        mpg123_close(thiz->mh);
        mpg123_delete(thiz->mh);
        thiz->mh = NULL;
    }
    if (thiz->src.fp) {
        fclose(thiz->src.fp);
        thiz->src.fp = NULL;
    }
    thiz->opened      = 0;
    thiz->duration_ms = 0;
    thiz->pos_ms      = 0;
}

static int mmp_open(MyMediaPlayer thiz, const char *path, int64_t offset, int64_t size) {
    char  used[512];
    FILE *fp = mmp_open_source(path, size, used, sizeof(used));

    if (!fp) {
        so_loge(MMP_OPEN_LOG_FMT "no such file", MMP_OPEN_LOG_ARGS(thiz, path, offset, size));
        return mmp_open_failed(thiz, 1);
    }

    // 容器模式下取「base + len」这段；散装文件则整段都算。
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);

    int64_t base = (offset > 0 && offset < file_size) ? offset : 0;
    int64_t len  = size;
    if (len <= 0 || base + len > file_size) len = (int64_t)file_size - base;

    // 记下文件头 4 字节：日志里用来确认是不是 MP3（FF Ex / ID3）。
    unsigned char magic[4] = {0};
    if (fseek(fp, (long)base, SEEK_SET) == 0) {
        if (fread(magic, 1, 4, fp) != 4) magic[0] = 0;
    }

    thiz->src.fp   = fp;
    thiz->src.base = base;
    thiz->src.len  = len;
    thiz->src.pos  = 0;
    thiz->src.fpos = -1;

    // --- mpg123 挂上 reader，读出格式，统一成 16bit ---
    int err  = MPG123_OK;
    thiz->mh = mpg123_new(NULL, &err);
    if (!thiz->mh) {
        so_loge(MMP_OPEN_LOG_FMT "mpg123_new failed (%d)", MMP_OPEN_LOG_ARGS(thiz, path, offset, size), err);
        return mmp_open_failed(thiz, 1);
    }

    mpg123_replace_reader_handle(thiz->mh, src_read, src_seek, NULL);

    if (mpg123_open_handle(thiz->mh, &thiz->src) != MPG123_OK) {
        so_loge(MMP_OPEN_LOG_FMT "mpg123_open_handle failed: %s (file='%s' magic=%02X%02X%02X%02X)",
                MMP_OPEN_LOG_ARGS(thiz, path, offset, size),
                mpg123_strerror(thiz->mh), used, magic[0], magic[1], magic[2], magic[3]);
        return mmp_open_failed(thiz, 1);
    }

    long rate     = 0;
    int  channels = 0, enc = 0;
    if (mpg123_getformat(thiz->mh, &rate, &channels, &enc) != MPG123_OK || rate <= 0 || channels <= 0) {
        so_loge(MMP_OPEN_LOG_FMT "mpg123_getformat failed: %s",
                MMP_OPEN_LOG_ARGS(thiz, path, offset, size), mpg123_strerror(thiz->mh));
        return mmp_open_failed(thiz, 1);
    }
    if (channels > 2) channels = 2;

    mpg123_format_none(thiz->mh);
    if (mpg123_format(thiz->mh, rate, channels, MPG123_ENC_SIGNED_16) != MPG123_OK) {
        so_loge(MMP_OPEN_LOG_FMT "mpg123_format(rate=%ld, ch=%d, S16) failed: %s",
                MMP_OPEN_LOG_ARGS(thiz, path, offset, size), rate, channels, mpg123_strerror(thiz->mh));
        return mmp_open_failed(thiz, 1);
    }

    // --- 时长：优先取头部信息，取不到就全文件扫描 ---
    off_t total = mpg123_length(thiz->mh);
    if (total <= 0) {
        mpg123_scan(thiz->mh);
        total = mpg123_length(thiz->mh);
    }

    thiz->rate        = (int)rate;
    thiz->channels    = channels;
    thiz->duration_ms = (total > 0) ? (int)((int64_t)total * 1000 / rate) : 0;
    thiz->pos_ms      = 0;
    thiz->opened      = 1;
    thiz->error       = 0;

    so_loga(MMP_OPEN_LOG_FMT "file='%s' base=0x%llX len=%lld magic=%02X%02X%02X%02X rate=%ld ch=%d dur=%dms",
            MMP_OPEN_LOG_ARGS(thiz, path, offset, size),
            used, (unsigned long long)base, (long long)len,
            magic[0], magic[1], magic[2], magic[3], rate, channels, thiz->duration_ms);

    return 0;
}

// ---------------------------------------------------------------- 输出层：音轨

// 按当前曲目的格式建一条平台音轨。切歌/停止都会关掉重建，不残留旧缓冲。
static int mmp_track_open(MyMediaPlayer thiz) {
    if (thiz->track) return 0;

    JClass clazz = JavaFindClass("android/media/AudioTrack");
    if (!clazz) {
        so_loge("[MyMediaPlayer] start(this=%p): android/media/AudioTrack is not registered", (void *)thiz);
        thiz->error = 2;
        return -1;
    }

    jint ch_cfg = (thiz->channels == 1) ? CHANNEL_OUT_MONO : CHANNEL_OUT_STEREO;
    jint buf    = AudioTrack_getMinBufferSize(thiz->rate, ch_cfg, ENCODING_PCM_16BIT);
    if (buf <= 0) buf = MMP_GRAIN * 2 * (jint)sizeof(int16_t) * 4;

    AudioTrack at = (AudioTrack)Object_new(clazz);
    if (!at) {
        so_loge("[MyMediaPlayer] start(this=%p): AudioTrack alloc failed", (void *)thiz);
        thiz->error = 2;
        return -1;
    }

    AudioTrack_ctor(at, MMP_STREAM_MUSIC, thiz->rate, ch_cfg, ENCODING_PCM_16BIT, buf, MMP_MODE_STREAM);
    AudioTrack_setStereoVolume(at, thiz->vol_l / 256.0f, thiz->vol_r / 256.0f);
    thiz->track = at;

    so_loga("[MyMediaPlayer] start(this=%p): track rate=%d ch=%d chCfg=0x%X buf=%d -> %p",
            (void *)thiz, thiz->rate, thiz->channels, ch_cfg, buf, (void *)at);
    return 0;
}

static void mmp_track_close(MyMediaPlayer thiz) {
    if (!thiz->track) return;

    AudioTrack_stop(thiz->track);
    Object_release(thiz->track);  // 析构 AudioTrack_dtor → release
    thiz->track = NULL;
}

// ---------------------------------------------------------------- 输出层：解码线程

static void *mmp_decode_thread(void *arg) {
    MyMediaPlayer thiz = (MyMediaPlayer)arg;
    // 双声道缓冲；单声道时只用到一半。
    int16_t *pcm = (int16_t *)malloc(sizeof(int16_t) * MMP_GRAIN * 2);

    if (!pcm) {
        so_loge("[MyMediaPlayer] decode(this=%p): pcm buffer alloc failed", (void *)thiz);
        thiz->error = 3;
        return NULL;
    }

    while (!thiz->stop_req) {
        // --- 持锁段：只处理 seek 和取快照，解码/写音轨在锁外 ---
        pthread_mutex_lock(&thiz->lock);

        if (thiz->state != MMP_STATE_PLAYING || !thiz->mh) {
            pthread_mutex_unlock(&thiz->lock);
            usleep(4000);
            continue;
        }

        if (thiz->seek_req) {
            off_t frame = (off_t)((int64_t)thiz->seek_ms * thiz->rate / 1000);
            off_t got   = mpg123_seek(thiz->mh, frame, SEEK_SET);
            if (got >= 0) thiz->pos_ms = (int)((int64_t)got * 1000 / thiz->rate);
            thiz->seek_req = 0;
        }

        size_t want = sizeof(int16_t) * MMP_GRAIN * (size_t)thiz->channels;
        size_t done = 0;
        int    rc   = mpg123_read(thiz->mh, (unsigned char *)pcm, want, &done);

        int        ch    = thiz->channels;
        int        rate  = thiz->rate;
        AudioTrack track = thiz->track;

        pthread_mutex_unlock(&thiz->lock);

        // --- 写平台音轨：重采样、混音、输出端口都由 AudioTrack 负责 ---
        if (done > 0) {
            if (!track) {
                so_loge("[MyMediaPlayer] decode(this=%p): track is gone", (void *)thiz);
                break;
            }

            jint written = AudioTrack_write(track, (jbyte *)pcm, 0, (jint)done);

            if (written <= 0) {
                so_loge("[MyMediaPlayer] decode(this=%p): AudioTrack_write(off=0, size=%u) -> %d",
                        (void *)thiz, (unsigned)done, written);
                pthread_mutex_lock(&thiz->lock);
                thiz->error = 5;
                thiz->state = MMP_STATE_IDLE;
                pthread_mutex_unlock(&thiz->lock);
                break;
            }

            pthread_mutex_lock(&thiz->lock);
            int frames = (int)((size_t)written / (sizeof(int16_t) * (size_t)ch));
            thiz->pos_ms += (int)((int64_t)frames * 1000 / rate);
            pthread_mutex_unlock(&thiz->lock);
        }

        // --- 收尾：播完 -> 循环或置完成；解码错 -> 记错误 ---
        if (rc == MPG123_DONE || (rc == MPG123_OK && done == 0)) {
            pthread_mutex_lock(&thiz->lock);
            if (thiz->is_looping) {
                mpg123_seek(thiz->mh, 0, SEEK_SET);
                thiz->pos_ms = 0;
            } else {
                thiz->state        = MMP_STATE_IDLE;
                thiz->is_completed = 1;
                thiz->pos_ms       = thiz->duration_ms;
            }
            pthread_mutex_unlock(&thiz->lock);
        } else if (rc != MPG123_OK && rc != MPG123_NEW_FORMAT) {
            pthread_mutex_lock(&thiz->lock);
            thiz->error = 4;
            thiz->state = MMP_STATE_IDLE;
            pthread_mutex_unlock(&thiz->lock);
            so_loge("[MyMediaPlayer] decode(this=%p): mpg123_read -> %d (%s)",
                    (void *)thiz, rc, mpg123_strerror(thiz->mh));
        }
    }

    free(pcm);
    return NULL;
}

// 停止并回收解码线程。
// 顺序很重要：解码线程可能正卡在 AudioTrack_write（缓冲满等空间），
// 先把音轨置 STOPPED，混音线程把已写数据读干净，write 才能返回、线程才能退出。
static void mmp_stop_thread(MyMediaPlayer thiz) {
    if (!thiz->thread_live) return;

    thiz->stop_req = 1;
    if (thiz->track) AudioTrack_stop(thiz->track);

    pthread_join(thiz->thread, NULL);
    thiz->thread_live = 0;
    thiz->stop_req    = 0;
}

// ---------------------------------------------------------------- Java 方法

void MyMediaPlayer_ctor(MyMediaPlayer thiz) {
    memset(thiz, 0, sizeof(*thiz));
    pthread_mutex_init(&thiz->lock, NULL);
    thiz->state = MMP_STATE_IDLE;
    thiz->vol_l = 256;
    thiz->vol_r = 256;
}

void MyMediaPlayer_destroy(MyMediaPlayer thiz) {
    mmp_stop_thread(thiz);

    pthread_mutex_lock(&thiz->lock);
    mmp_close_locked(thiz);
    pthread_mutex_unlock(&thiz->lock);

    mmp_track_close(thiz);
}

void MyMediaPlayer_createPlayer(MyMediaPlayer thiz) {
    so_loga("[MyMediaPlayer] createPlayer(this=%p)", (void *)thiz);
}

void MyMediaPlayer_destroyPlayer(MyMediaPlayer thiz) {
    so_loga("[MyMediaPlayer] destroyPlayer(this=%p)", (void *)thiz);
    mmp_stop_thread(thiz);
    mmp_track_close(thiz);
}

void MyMediaPlayer_done(MyMediaPlayer thiz) {
    mmp_stop_thread(thiz);
    pthread_mutex_lock(&thiz->lock);
    thiz->state        = MMP_STATE_IDLE;
    thiz->is_completed = 1;
    pthread_mutex_unlock(&thiz->lock);
    so_loga("[MyMediaPlayer] done(this=%p)", (void *)thiz);
}

jint MyMediaPlayer_openFile(MyMediaPlayer thiz, JString path, jlong offset, jlong size) {
    const char *path_cstr = path ? String_getUTFChars(path) : NULL;

    if (!path_cstr) {
        so_loge(MMP_OPEN_LOG_FMT "null path", MMP_OPEN_LOG_ARGS(thiz, "(null)", offset, size));
        pthread_mutex_lock(&thiz->lock);
        int rc = mmp_open_failed(thiz, 1);
        pthread_mutex_unlock(&thiz->lock);
        return rc;
    }

    mmp_stop_thread(thiz);
    mmp_track_close(thiz);

    pthread_mutex_lock(&thiz->lock);
    mmp_close_locked(thiz);
    int rc = mmp_open(thiz, path_cstr, offset, size);
    pthread_mutex_unlock(&thiz->lock);

    return rc;
}

void MyMediaPlayer_closeFile(MyMediaPlayer thiz) {
    mmp_stop_thread(thiz);
    mmp_track_close(thiz);
    pthread_mutex_lock(&thiz->lock);
    mmp_close_locked(thiz);
    thiz->state = MMP_STATE_IDLE;
    pthread_mutex_unlock(&thiz->lock);
    so_loga("[MyMediaPlayer] closeFile(this=%p)", (void *)thiz);
}

void MyMediaPlayer_start(MyMediaPlayer thiz) {
    pthread_mutex_lock(&thiz->lock);
    int opened = thiz->opened;
    pthread_mutex_unlock(&thiz->lock);

    if (!opened) {
        so_loge("[MyMediaPlayer] start(this=%p): openFile was not called", (void *)thiz);
        return;
    }

    if (mmp_track_open(thiz) < 0) return;

    pthread_mutex_lock(&thiz->lock);
    thiz->state        = MMP_STATE_PLAYING;
    thiz->is_completed = 0;
    pthread_mutex_unlock(&thiz->lock);

    if (!thiz->thread_live) {
        thiz->stop_req = 0;
        if (pthread_create(&thiz->thread, NULL, mmp_decode_thread, thiz) != 0) {
            so_loge("[MyMediaPlayer] start(this=%p): pthread_create failed", (void *)thiz);
            thiz->error = 3;
            return;
        }
        thiz->thread_live = 1;
    }

    AudioTrack_play(thiz->track);

    so_loga("[MyMediaPlayer] start(this=%p): dur=%dms, rate=%d, ch=%d",
            (void *)thiz, thiz->duration_ms, thiz->rate, thiz->channels);
}

void MyMediaPlayer_stop(MyMediaPlayer thiz) {
    mmp_stop_thread(thiz);

    pthread_mutex_lock(&thiz->lock);
    thiz->state  = MMP_STATE_IDLE;
    thiz->pos_ms = 0;
    if (thiz->mh) mpg123_seek(thiz->mh, 0, SEEK_SET);
    pthread_mutex_unlock(&thiz->lock);

    mmp_track_close(thiz);  // 释放音轨，缓冲里剩下的采样不会再播出来
    so_loga("[MyMediaPlayer] stop(this=%p)", (void *)thiz);
}

void MyMediaPlayer_pause(MyMediaPlayer thiz) {
    pthread_mutex_lock(&thiz->lock);
    if (thiz->state == MMP_STATE_PLAYING) thiz->state = MMP_STATE_PAUSED;
    if (thiz->track) AudioTrack_pause(thiz->track);
    pthread_mutex_unlock(&thiz->lock);

    so_loga("[MyMediaPlayer] pause(this=%p): pos=%dms", (void *)thiz, thiz->pos_ms);
}

void MyMediaPlayer_setVolume(MyMediaPlayer thiz, jfloat left, jfloat right) {
    if (left < 0.0f) left = 0.0f;
    if (left > 1.0f) left = 1.0f;
    if (right < 0.0f) right = 0.0f;
    if (right > 1.0f) right = 1.0f;

    pthread_mutex_lock(&thiz->lock);
    thiz->vol_l = (int)(left * 256.0f);
    thiz->vol_r = (int)(right * 256.0f);
    if (thiz->track) AudioTrack_setStereoVolume(thiz->track, left, right);
    pthread_mutex_unlock(&thiz->lock);
}

void MyMediaPlayer_setLooping(MyMediaPlayer thiz, jboolean is_looping) {
    pthread_mutex_lock(&thiz->lock);
    thiz->is_looping = is_looping;
    pthread_mutex_unlock(&thiz->lock);
}

void MyMediaPlayer_setPosition(MyMediaPlayer thiz, jint ms) {
    if (ms < 0) ms = 0;

    pthread_mutex_lock(&thiz->lock);
    if (thiz->mh && !thiz->thread_live) {
        // 线程不在跑，直接 seek
        off_t frame = (off_t)((int64_t)ms * thiz->rate / 1000);
        off_t got   = mpg123_seek(thiz->mh, frame, SEEK_SET);
        if (got >= 0) thiz->pos_ms = (int)((int64_t)got * 1000 / thiz->rate);
    } else {
        // 线程在跑，交给它在下一轮处理
        thiz->seek_ms  = ms;
        thiz->seek_req = 1;
    }
    pthread_mutex_unlock(&thiz->lock);
}

jint MyMediaPlayer_getPosition(MyMediaPlayer thiz) {
    pthread_mutex_lock(&thiz->lock);
    jint pos = thiz->pos_ms;
    pthread_mutex_unlock(&thiz->lock);
    return pos;
}

jint MyMediaPlayer_getDuration(MyMediaPlayer thiz) {
    pthread_mutex_lock(&thiz->lock);
    jint dur = thiz->duration_ms;
    pthread_mutex_unlock(&thiz->lock);
    return dur;
}

jboolean MyMediaPlayer_isPlaying(MyMediaPlayer thiz) {
    return (thiz->state == MMP_STATE_PLAYING) ? JNI_TRUE : JNI_FALSE;
}

jboolean MyMediaPlayer_isCompleted(MyMediaPlayer thiz) {
    return thiz->is_completed;
}

jboolean MyMediaPlayer_isLooping(MyMediaPlayer thiz) {
    return thiz->is_looping;
}

jint MyMediaPlayer_getErrorState(MyMediaPlayer thiz) {
    return thiz->error;
}

// MediaPlayer 的监听回调：Android 上是 Java 侧自己注册给 MediaPlayer 的，
// native 不会调它们；保留空实现是为了让 runtime 的 GetMethodID 能成功。
void MyMediaPlayer_onCompletion(MyMediaPlayer thiz, JObject player) {
    (void)thiz;
    (void)player;
}

jboolean MyMediaPlayer_onError(MyMediaPlayer thiz, JObject player, jint what, jint extra) {
    (void)thiz;
    (void)player;
    (void)what;
    (void)extra;
    return JNI_FALSE;
}

// ---------------------------------------------------------------- 类注册

static JSymbol class_symbols[] = {
    // method
    JAVA_SYMBOL_METHOD("<init>", "()V", &MyMediaPlayer_ctor),
    JAVA_SYMBOL_METHOD("destroy", "()V", &MyMediaPlayer_destroy),
    JAVA_SYMBOL_METHOD("createPlayer", "()V", &MyMediaPlayer_createPlayer),
    JAVA_SYMBOL_METHOD("destroyPlayer", "()V", &MyMediaPlayer_destroyPlayer),
    JAVA_SYMBOL_METHOD("done", "()V", &MyMediaPlayer_done),
    JAVA_SYMBOL_METHOD("openFile", "(Ljava/lang/String;JJ)I", &MyMediaPlayer_openFile),
    JAVA_SYMBOL_METHOD("closeFile", "()V", &MyMediaPlayer_closeFile),
    JAVA_SYMBOL_METHOD("start", "()V", &MyMediaPlayer_start),
    JAVA_SYMBOL_METHOD("stop", "()V", &MyMediaPlayer_stop),
    JAVA_SYMBOL_METHOD("pause", "()V", &MyMediaPlayer_pause),
    JAVA_SYMBOL_METHOD("setVolume", "(FF)V", &MyMediaPlayer_setVolume),
    JAVA_SYMBOL_METHOD("setLooping", "(Z)V", &MyMediaPlayer_setLooping),
    JAVA_SYMBOL_METHOD("setPosition", "(I)V", &MyMediaPlayer_setPosition),
    JAVA_SYMBOL_METHOD("getPosition", "()I", &MyMediaPlayer_getPosition),
    JAVA_SYMBOL_METHOD("getDuration", "()I", &MyMediaPlayer_getDuration),
    JAVA_SYMBOL_METHOD("isPlaying", "()Z", &MyMediaPlayer_isPlaying),
    JAVA_SYMBOL_METHOD("isCompleted", "()Z", &MyMediaPlayer_isCompleted),
    JAVA_SYMBOL_METHOD("isLooping", "()Z", &MyMediaPlayer_isLooping),
    JAVA_SYMBOL_METHOD("getErrorState", "()I", &MyMediaPlayer_getErrorState),
    JAVA_SYMBOL_METHOD("onCompletion", "(Landroid/media/MediaPlayer;)V", &MyMediaPlayer_onCompletion),
    JAVA_SYMBOL_METHOD("onError", "(Landroid/media/MediaPlayer;II)Z", &MyMediaPlayer_onError),
};

static JClassMeta class_meta = {
    .name          = "com/acrodea/vividruntime/launcher/extension/MyMediaPlayer",
    .super_name    = NULL,
    .symbols       = class_symbols,
    .num_symbols   = sizeof(class_symbols) / sizeof(class_symbols[0]),
    .instance_size = sizeof(struct _MyMediaPlayer),
    .dtor          = (JDtor)&MyMediaPlayer_destroy,
};

void setup_MyMediaPlayer(void) {
    mpg123_init();
    sClass = JavaCreateClass(&class_meta);
}
