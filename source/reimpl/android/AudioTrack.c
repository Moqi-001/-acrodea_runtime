/*

    _ZN7android10AudioTrackC1Ev
    _ZN7android10AudioTrackD1Ev
    _ZNK7android10AudioTrack9initCheckEv
    _ZN7android10AudioTrack5startEv
    _ZN7android10AudioTrack5pauseEv
    _ZN7android10AudioTrack4stopEv
    _ZN7android10AudioTrack9setVolumeEff
    _ZN7android10AudioTrack3setE19audio_stream_type_tj14audio_format_tji20audio_output_flags_tPFviPvS4_ES4_iRKNS_2spINS_7IMemoryEEEbi
    _ZN7android10AudioTrack3setE19audio_stream_type_tj14audio_format_tii20audio_output_flags_tPFviPvS4_ES4_iRKNS_2spINS_7IMemoryEEEbi
    _ZN7android10AudioTrack3setEijiiijPFviPvS1_ES1_iRKNS_2spINS_7IMemoryEEEbi
    _ZN7android10AudioTrack3setEijiiijPFviPvS1_ES1_iRKNS_2spINS_7IMemoryEEEb
    _ZN7android10AudioTrack3setE19audio_stream_type_tj14audio_format_tji20audio_output_flags_tPFviPvS4_ES4_iRKNS_2spINS_7IMemoryEEEbiNS0_13transfer_typeEPK20audio_offload_info_t
    _ZN7android10AudioTrack3setE19audio_stream_type_tj14audio_format_tji20audio_output_flags_tPFviPvS4_ES4_iRKNS_2spINS_7IMemoryEEEbiNS0_13transfer_typeEPK20audio_offload_info_ti

*/

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "reimpl/android/AudioTrack.h"
#include "reimpl/android/Errors.h"
#include "reimpl/android/SharedBuffer.h"
#include "utils/logger.h"

struct AAudioTrack {
    status_t             mStatus;
    audio_stream_type_t  mStreamType;
    uint32_t             mSampleRate;
    audio_format_t       mFormat;
    audio_channel_mask_t mChannelMask;
    size_t               mFrameCount;
    audio_output_flags_t mFlags;
    callback_t           mCbf;
    void                *mUserData;
    uint32_t             mNotificationFrames;
    const void          *mSharedBuffer;
    int                  mSessionId;

    transfer_type mTransfer;

    pthread_mutex_t mLock;
    bool            mIsTimed;
};  // +216

void AAudioTrack_construct(AAudioTrack *self) {
    l_debug("[Android] AAudioTrack_construct(%p)", self);
    self->mFrameCount         = 0;
    self->mFlags              = AUDIO_OUTPUT_FLAG_NONE;
    self->mCbf                = NULL;
    self->mUserData           = NULL;
    self->mNotificationFrames = 0;
    self->mSessionId          = 0;
}

void AAudioTrack_destruct(AAudioTrack *self) {
    l_debug("[Android] AAudioTrack_destruct(%p)", self);
}

status_t AAudioTrack_initCheck(AAudioTrack *self) {
    l_debug("[Android] AAudioTrack_initCheck(%p): %d", self, self->mStatus);
    return self->mStatus;
}

void AAudioTrack_start(AAudioTrack *self) {
    l_debug("[Android] AAudioTrack_start(%p)", self);
}

void AAudioTrack_pause(AAudioTrack *self) {
    l_debug("[Android] AAudioTrack_pause(%p)", self);
}

void AAudioTrack_stop(AAudioTrack *self) {
    l_debug("[Android] AAudioTrack_stop(%p)", self);
}

status_t AAudioTrack_setVolume(AAudioTrack *self, float left, float right) {
    l_debug("[Android] AAudioTrack_stop(%p)", self);
    return NO_ERROR;
}

/**
 * @param streamType 音频流类型
 * @param sampleRate 采样率
 * @param format 音频格式
 * @param channelMask 声道掩码
 * @param frameCount 帧数
 * @param flags 输出标志
 * @param callback 回调函数
 * @param user 用户数据
 * @param notificationFrames 通知帧数
 * @return status_t 状态码
 */
status_t AudioTrack_set(audio_stream_type_t         streamType,
                        uint32_t                    sampleRate,
                        audio_format_t              format,
                        uint32_t                    channelMask,
                        int                         frameCount,
                        audio_output_flags_t        flags,
                        callback_t                  cbf,
                        void                       *user,
                        int                         notificationFrames,
                        const void                 *sharedBuffer,  // IMemory
                        bool                        threadCanCallJava,
                        int                         sessionId,
                        transfer_type               transferType,
                        const audio_offload_info_t *offloadInfo,
                        int                         uid) {
    return NO_ERROR;
}

ssize_t AudioTrack_write(AAudioTrack *self, const void *buffer, size_t userSize) {
    /*
    if (self->mTransfer != TRANSFER_SYNC || self->mIsTimed) {
        return INVALID_OPERATION;
    }

    if ((ssize_t)userSize < 0 || (buffer == NULL && userSize != 0)) {
        // Sanity-check: user is most-likely passing an error code, and it would
        // make the return value ambiguous (actualSize vs error).
        l_debug("AudioTrack::write(buffer=%p, size=%u (%d)", buffer, userSize, userSize);
        return BAD_VALUE;
    }

    size_t written = 0;
    Buffer audioBuffer;

    while (userSize >= self->mFrameSize) {
        audioBuffer.frameCount = userSize / self->mFrameSize;

        status_t err = obtainBuffer(&audioBuffer, &ClientProxy::kForever);
        if (err < 0) {
            if (written > 0) {
                break;
            }
            return (ssize_t)err;
        }

        size_t toWrite;
        if (self->mFormat == AUDIO_FORMAT_PCM_8_BIT && !(self->mFlags & AUDIO_OUTPUT_FLAG_DIRECT)) {
            // Divide capacity by 2 to take expansion into account
            toWrite = audioBuffer.size >> 1;
            memcpy_to_i16_from_u8(audioBuffer.i16, (const uint8_t *)buffer, toWrite);
        } else {
            toWrite = audioBuffer.size;
            memcpy(audioBuffer.i8, buffer, toWrite);
        }
        buffer = ((const char *)buffer) + toWrite;
        userSize -= toWrite;
        written += toWrite;

        releaseBuffer(&audioBuffer);
    }

    return written;
    */
    return 0;
}
