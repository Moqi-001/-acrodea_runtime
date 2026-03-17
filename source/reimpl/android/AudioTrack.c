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

#include <stddef.h>
#include <stdint.h>

#include "reimpl/android/AudioTrack.h"
#include "utils/logger.h"

struct AAudioTrack {
    int inited;
};

void AAudioTrack_construct(AAudioTrack *self) {
}

void AAudioTrack_destruct(AAudioTrack *self) {
}

int AAudioTrack_initCheck(AAudioTrack *self) {
    return self->inited;
}

int AAudioTrack_start(AAudioTrack *self) {
}

int AAudioTrack_pause(AAudioTrack *self) {
}

int AAudioTrack_stop(AAudioTrack *self) {
}

int AAudioTrack_set(AAudioTrack *self) {
}
