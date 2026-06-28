#ifndef ANDROID_VIEW_SURFACE_H
#define ANDROID_VIEW_SURFACE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Surface *Surface;

void android_view_Surface_class_init(void);

Surface AndroidSurface_new();

#ifdef __cplusplus
}
#endif

#endif