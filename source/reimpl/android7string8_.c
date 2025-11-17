#include "reimpl/android7string8_.h"

int android_String8_init(String8 **pstr8)
{
    l_debug("android_String8_init(%p)", pstr8);

    if (!pstr8)
    {
        l_debug("android_String8_init null(%p)", *pstr8);
        return -1;
    }

    *pstr8 = (String8 *)malloc(sizeof(String8));
    if (!*pstr8)
        return -1;

    (*pstr8)->data[0] = '\0';
    (*pstr8)->length = 0;
    l_debug("android_String8_init return(%s)", (*pstr8)->data);

    return 0;
}

int android_String8_initFromCstring(String8 **pstr8, const char *s)
{
    l_debug("android_String8_initFromCstring(%p,%s)", pstr8, s);

    if (!pstr8 || !s)
    {
        l_debug("android_String8_initFromCstring null(%p,%s)", *pstr8, s);
        return -1;
    }

    *pstr8 = (String8 *)malloc(sizeof(String8));
    if (!*pstr8)
        return -1;

    size_t len = strlen(s);
    if (len > STRING8_MAX_DATA_SIZE - 1)
        len = STRING8_MAX_DATA_SIZE - 1;
    memcpy((*pstr8)->data, s, len);
    (*pstr8)->length = len;
    (*pstr8)->data[len] = '\0';
    l_debug("android_String8_initFromCstring return(%p,%s)", pstr8, (*pstr8)->data);

    return 0;
}

void android_String8_term(String8 **pstr8)
{
    l_debug("android_String8_term(%p)", pstr8);

    if (pstr8 && *pstr8)
    {
        l_debug("android_String8_term free(%s)", (*pstr8)->data);
        free(*pstr8);
        *pstr8 = NULL;
    }
}

int android_String8_appendCstring(String8 **pstr8, const char *s)
{
    l_debug("android_String8_appendCstring(%p,%s)", pstr8, s);

    if (!pstr8 || !*pstr8 || !s)
    {
        l_debug("android_String8_appendCstring null(%p,%s)", *pstr8, s);
        return -1;
    }

    size_t len = strlen(s);
    if (len == 0)
        return 0;
    if ((*pstr8)->length + len >= STRING8_MAX_DATA_SIZE)
    {
        return -1;
    }

    char *buf = (*pstr8)->data + (*pstr8)->length;
    memcpy(buf, s, len);
    (*pstr8)->length += len;
    (*pstr8)->data[(*pstr8)->length] = '\0';
    l_debug("android_String8_appendCstring return(%s)", (*pstr8)->data);

    return 0;
}

int android_String8_appendPathCstring(String8 **pstr8, const char *s)
{
    l_debug("android_String8_appendPathCstring(%p,%s)", pstr8, s);

    if (!pstr8 || !*pstr8 || !s)
    {
        l_debug("android_String8_appendPathCstring null(%p,%s)", *pstr8, s);
        return -1;
    }

    size_t len = strlen(s);
    if (len == 0)
        return 0;

    char *buf;
    if (s[0] != '/')
    {
        if ((*pstr8)->length > 0 && (*pstr8)->data[(*pstr8)->length - 1] != '/')
        {
            if ((*pstr8)->length + 1 + len > STRING8_MAX_DATA_SIZE - 1)
                return -1;
            (*pstr8)->data[(*pstr8)->length++] = '/';
        }
        else if ((*pstr8)->length + len > STRING8_MAX_DATA_SIZE - 1)
        {
            return -1;
        }
        buf = (*pstr8)->data + (*pstr8)->length;
    }
    else
    {
        if (len > STRING8_MAX_DATA_SIZE - 1)
            return -1;
        buf = (*pstr8)->data;
        (*pstr8)->length = 0;
    }

    memcpy(buf, s, len);
    (*pstr8)->length += len;
    (*pstr8)->data[(*pstr8)->length] = '\0';
    l_debug("android_String8_appendPathCstring return(%s)", (*pstr8)->data);

    return 0;
}

int android_String8_setToByCstring(String8 **pstr8, const char *s)
{
    l_debug("android_String8_setToByCstring(%p,%s)", pstr8, s);
    if (!pstr8 || !*pstr8 || !s)
    {
        l_debug("android_String8_setToByCstring null(%p,%s)", *pstr8, s);
        return -1;
    }
    size_t len = strlen(s);
    if (len > STRING8_MAX_DATA_SIZE - 1)
        len = STRING8_MAX_DATA_SIZE - 1;
    memcpy((*pstr8)->data, s, len);
    (*pstr8)->length = len;
    (*pstr8)->data[len] = '\0';
    l_debug("android_String8_setToByCstring reture(%s)", (*pstr8)->data);

    return 0;
}

void android_String8_empty(String8 *str8)
{
    l_debug("android_String8_empty(%p)", str8);

    if (str8)
    {
        str8->data[0] = '\0';
        str8->length = 0;
    }
}

const char *android_String8_string(String8 *str8)
{
    if (str8)
        l_debug("android_String8_string(%s)", str8->data);
    return str8 ? str8->data : NULL;
}

size_t android_String8_length(String8 *str8)
{
    if (str8)
        l_debug("android_String8_length(%d)", str8->length);
    return str8 ? str8->length : 0;
}

int android_String8_isEmpty(String8 *str8)
{
    return str8 ? str8->length == 0 : 1;
}