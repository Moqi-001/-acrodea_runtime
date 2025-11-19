#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "utils/logger.h"
#include "reimpl/android/String8.h"

#define STRING8_MAX_DATA_SIZE 1024
struct String8
{
    char data[STRING8_MAX_DATA_SIZE];
    size_t length;
};

int android_String8_init(String8 **pstr8)
{
    l_debug("android_String8_init(%p)", pstr8);

    if (!pstr8)
    {
        l_debug("android_String8_init failed: null ptr");
        return -1;
    }

    *pstr8 = (String8 *)malloc(sizeof(String8));
    if (!*pstr8)
        return -1;

    (*pstr8)->data[0] = '\0';
    (*pstr8)->length = 0;

    l_debug("android_String8_init over: %p", *pstr8);
    return 0;
}

int android_String8_initFromCstring(String8 **pstr8, const char *s)
{
    l_debug("android_String8_initFromCstring(%p,\"%s\")", pstr8, s);

    if (android_String8_init(pstr8) < 0)
        return -1;

    if (!s)
        return 0;

    size_t len = strlen(s);
    if (len > STRING8_MAX_DATA_SIZE - 1)
        return -1;

    memcpy((*pstr8)->data, s, len);
    (*pstr8)->length = len;
    (*pstr8)->data[len] = '\0';

    l_debug("android_String8_initFromCstring over: \"%s\"", (*pstr8)->data);
    return 0;
}

int android_String8_initFromString8(String8 **pstr8, const String8 **psrc)
{
    l_debug("android_String8_initFromString8(%p,%p)", pstr8, psrc);

    if (android_String8_init(pstr8) < 0)
        return -1;

    if (!psrc || !*psrc)
        return 0;

    const char *s = (*psrc)->data;
    size_t len = strlen(s);
    if (len > STRING8_MAX_DATA_SIZE - 1)
        return -1;

    memcpy((*pstr8)->data, s, len);
    (*pstr8)->length = len;
    (*pstr8)->data[len] = '\0';

    l_debug("android_String8_initFromString8 over: \"%s\"", (*pstr8)->data);
    return 0;
}

void android_String8_term(String8 **pstr8)
{
    l_debug("android_String8_term(%p)", pstr8);

    if (pstr8 && *pstr8)
    {
        l_debug("android_String8_term begin: \"%s\"", (*pstr8)->data);
        free(*pstr8);
        *pstr8 = NULL;
        l_debug("android_String8_term ok");
    }
}

int android_String8_setToCstring(String8 **pstr8, const char *s)
{
    l_debug("android_String8_setToCstring(%p,\"%s\")", pstr8, s);

    if (!pstr8 || !*pstr8 || !s)
    {
        l_debug("android_String8_setToCstring failed: null ptr");
        return -1;
    }

    size_t len = strlen(s);
    if (len > STRING8_MAX_DATA_SIZE - 1)
        return -1;

    memcpy((*pstr8)->data, s, len);
    (*pstr8)->length = len;
    (*pstr8)->data[len] = '\0';

    l_debug("android_String8_setToCstring over: \"%s\"", (*pstr8)->data);
    return 0;
}

int android_String8_appendCstring(String8 **pstr8, const char *s)
{
    l_debug("android_String8_appendCstring(%p,\"%s\")", pstr8, s);

    if (!pstr8 || !*pstr8 || !s)
    {
        l_debug("android_String8_appendCstring failed: null ptr");
        return -1;
    }

    size_t len = strlen(s);
    if (len == 0)
        return 0;
    if ((*pstr8)->length + len >= STRING8_MAX_DATA_SIZE)
        return -1;

    l_debug("android_String8_appendCstring begin: \"%s\",\"%s\"", (*pstr8)->data, s);

    char *buf = (*pstr8)->data + (*pstr8)->length;
    memcpy(buf, s, len);
    (*pstr8)->length += len;
    (*pstr8)->data[(*pstr8)->length] = '\0';

    l_debug("android_String8_appendCstring over: \"%s\"", (*pstr8)->data);
}

int android_String8_append(String8 **pstr8, const String8 **psrc)
{
    l_debug("android_String8_append(%p,%p)", pstr8, psrc);

    if (!pstr8 || !*pstr8 || !psrc || !*psrc)
    {
        l_debug("android_String8_append failed: null ptr");
        return -1;
    }

    const char *s = (*psrc)->data;
    size_t len = strlen(s);
    if (len == 0)
        return 0;
    if ((*pstr8)->length + len >= STRING8_MAX_DATA_SIZE)
        return -1;

    l_debug("android_String8_append begin: \"%s\",\"%s\"", (*pstr8)->data, s);

    char *buf = (*pstr8)->data + (*pstr8)->length;
    memcpy(buf, s, len);
    (*pstr8)->length += len;
    (*pstr8)->data[(*pstr8)->length] = '\0';

    l_debug("android_String8_append over: \"%s\"", (*pstr8)->data);
    return 0;
}

int android_String8_appendPathCstring(String8 **pstr8, const char *s)
{
    l_debug("android_String8_appendPathCstring(%p,\"%s\")", pstr8, s);

    if (!pstr8 || !*pstr8 || !s)
    {
        l_debug("android_String8_appendPathCstring failed: null ptr");
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

    l_debug("android_String8_appendPathCstring begin: \"%s\",\"%s\"", (*pstr8)->data, s);

    memcpy(buf, s, len);
    (*pstr8)->length += len;
    (*pstr8)->data[(*pstr8)->length] = '\0';

    l_debug("android_String8_appendPathCstring over: \"%s\"", (*pstr8)->data);
    return 0;
}

int android_String8_getPathDir(String8 **pstr8, const String8 **psrc)
{
    l_debug("android_String8_getPathDir(%p,%p)", pstr8, psrc);

    if (android_String8_init(pstr8) < 0)
        return -1;

    if (!psrc || !*psrc)
        return 0;

    const char *s = (*psrc)->data;
    const char *cp = strrchr(s, '/');
    if (cp == NULL)
        return 0;

    size_t len = cp - s;
    if (len > STRING8_MAX_DATA_SIZE - 1)
        return -1;

    l_debug("android_String8_getPathDir begin: \"%s\"\n", (*psrc)->data);

    memcpy((*pstr8)->data, s, len);
    (*pstr8)->length = len;
    (*pstr8)->data[len] = '\0';

    l_debug("android_String8_getPathDir over: \"%s\"\n", (*pstr8)->data);
    return 0;
}

int android_String8_walkPath(String8 **pstr8, const String8 **psrc, String8 **outRemains)
{
    l_debug("android_String8_walkPath(%p, %p, %p)\n", pstr8, psrc, outRemains);

    if (android_String8_init(pstr8) < 0)
        return -1;

    if (!psrc || !*psrc)
    {
        if (outRemains && *outRemains)
        {
            (*outRemains)->data[0] = '\0';
            (*outRemains)->length = 0;
        }
        return 0;
    }

    const char *s = (*psrc)->data;
    const char *tail = s + strlen(s);
    const char *buf = s;
    const char *cp = strchr(buf, '/');
    if (cp == buf)
    {
        // don't include a leading '/'.
        buf = buf + 1;
        cp = strchr(buf, '/');
    }
    if (cp == NULL)
        cp = tail;

    size_t len = cp - buf;
    if (len > STRING8_MAX_DATA_SIZE - 1)
        return -1;

    l_debug("android_String8_walkPath begin: \"%s\"\n", (*psrc)->data);

    memcpy((*pstr8)->data, buf, len);
    (*pstr8)->length = len;
    (*pstr8)->data[len] = '\0';

    if (outRemains && *outRemains)
    {
        buf = cp < tail ? cp + 1 : tail;
        len = tail - buf;
        if (len > STRING8_MAX_DATA_SIZE - 1)
            return -1;
        memcpy((*outRemains)->data, buf, len);
        (*outRemains)->length = len;
        (*outRemains)->data[len] = '\0';

        l_debug("android_String8_walkPath over: remains=\"%s\"\n", (*outRemains)->data);
    }

    l_debug("android_String8_walkPath over: walk=\"%s\"\n", (*pstr8)->data);
    return 0;
}

void android_String8_empty(String8 *str8)
{
    if (str8)
    {
        str8->data[0] = '\0';
        str8->length = 0;
    }
}

const char *android_String8_string(String8 *str8)
{
    return str8 ? str8->data : NULL;
}

size_t android_String8_length(String8 *str8)
{
    return str8 ? str8->length : 0;
}

int android_String8_isEmpty(String8 *str8)
{
    return str8 ? str8->length == 0 : 1;
}
