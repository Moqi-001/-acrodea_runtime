#include <stddef.h>
#define STRING8_MAX_DATA_SIZE 1024
typedef struct  {
    char   data[STRING8_MAX_DATA_SIZE];
    size_t length;
}String8;