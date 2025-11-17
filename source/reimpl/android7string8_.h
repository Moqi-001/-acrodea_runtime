#include <stddef.h>
#include "utils/logger.h"
#define STRING8_MAX_DATA_SIZE 1024
typedef struct
{
    char data[STRING8_MAX_DATA_SIZE];
    size_t length;
} String8;

const char *android_String8_string(String8 *str8);
int android_String8_init(String8 **pstr8);
int android_String8_initFromCstring(String8 **pstr8, const char *s);
void android_String8_term(String8 **pstr8);
int android_String8_appendCstring(String8 **pstr8, const char *s);
int android_String8_appendPathCstring(String8 **pstr8, const char *s);
int android_String8_setToByCstring(String8 **pstr8, const char *s);
void android_String8_empty(String8 *str8);
size_t android_String8_length(String8 *str8);
int android_String8_isEmpty(String8 *str8);