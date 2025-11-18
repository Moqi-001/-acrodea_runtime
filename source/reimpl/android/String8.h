#ifndef ANDROID_STRING8_H
#define ANDROID_STRING8_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct String8 String8;

    int android_String8_init(String8 **pstr8);
    int android_String8_initFromCstring(String8 **pstr8, const char *s);
    int android_String8_initFromString8(String8 **pstr8, const String8 **psrc);
    void android_String8_term(String8 **pstr8);

    int android_String8_setToCstring(String8 **pstr8, const char *s);
    int android_String8_appendCstring(String8 **pstr8, const char *s);
    int android_String8_append(String8 **pstr8, const String8 **psrc);
    int android_String8_appendPathCstring(String8 **pstr8, const char *s);
    int android_String8_getPathDir(String8 **pstr8, const String8 **psrc);
    int android_String8_walkPath(String8 **pstr8, const String8 **psrc, String8 **outRemains);
    void android_String8_empty(String8 *str8);

    const char *android_String8_string(String8 *str8);
    size_t android_String8_length(String8 *str8);
    int android_String8_isEmpty(String8 *str8);

#ifdef __cplusplus
}
#endif

#endif