#ifndef ANDROID_AUDIO_H
#define ANDROID_AUDIO_H

typedef struct AAudioTrack AAudioTrack;

void AAudioTrack_construct(AAudioTrack *self);
void AAudioTrack_destruct(AAudioTrack *self);
int  AAudioTrack_initCheck(AAudioTrack *self);
int  AAudioTrack_start(AAudioTrack *self);
int  AAudioTrack_pause(AAudioTrack *self);
int  AAudioTrack_stop(AAudioTrack *self);
int  AAudioTrack_set(AAudioTrack *self);

#endif