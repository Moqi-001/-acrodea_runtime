#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "utils/logger.h"
#include "reimpl/android/String8.h"

#define STRING8_MAX_DATA_SIZE 1024

typedef struct String8Base
{
    size_t length;
} String8Base;

struct String8
{
    char *string;
};

#define getBase(string) ((String8Base *)string - 1)
#define getString(base) ((char *)(base + 1))

static int real_init(String8 *str8, const char *s, size_t len)
{
    if (!str8)
        return -1;

    if (!s || len == 0) // 创建并写入空字符串
    {
        String8Base *base = (String8Base *)malloc(sizeof(String8Base) + 1);
        if (!base)
            return -1;
        str8->string = getString(base);
        base->length = 0;
        str8->string[base->length] = '\0';
    }
    else // 创建并复制字符串
    {
        String8Base *base = (String8Base *)malloc(sizeof(String8Base) + len + 1);
        if (!base)
            return -1;
        str8->string = getString(base);
        memcpy(str8->string, s, len);
        base->length = len;
        str8->string[base->length] = '\0';
    }

    return 0;
}

static int real_setTo(String8 *str8, const char *s, size_t len)
{
    if (!str8 || !str8->string)
        return -1;

    String8Base *base = getBase(str8->string);

    if (!s || len == 0) // 写入空字符串
    {
        base = (String8Base *)realloc(base, sizeof(String8Base) + 1);
        if (!base)
            return -1;
        str8->string = getString(base);
        base->length = 0;
        str8->string[base->length] = '\0';
    }
    else // 复制字符串
    {
        base = (String8Base *)realloc(base, sizeof(String8Base) + len + 1);
        if (!base)
            return -1;
        str8->string = getString(base);
        memcpy(str8->string, s, len);
        base->length = len;
        str8->string[base->length] = '\0';
    }

    return 0;
}

static int real_append(String8 *str8, const char *s, size_t len)
{
    if (!str8 || !str8->string)
        return -1;

    if (!s || len == 0)
        return 0;

    String8Base *base = getBase(str8->string);
    base = (String8Base *)realloc(base, sizeof(String8Base) + base->length + len + 1);
    if (!base)
        return -1;

    str8->string = getString(base);
    memcpy(str8->string + base->length, s, len);
    base->length += len;
    str8->string[base->length] = '\0';

    return 0;
}

static int real_appendPath(String8 *str8, const char *s, size_t len)
{
    if (!str8 || !str8->string)
        return -1;

    if (!s || len == 0)
        return 0;

    String8Base *base;

    if (s[0] != '/')
    {
        base = getBase(str8->string);
        if (str8->string[base->length] != '/')
        {
            base = (String8Base *)realloc(base, sizeof(String8Base) + base->length + len + 2);
            if (!base)
                return -1;
            str8->string = getString(base);
            str8->string[base->length++] = '/';
        }
        else
        {
            base = (String8Base *)realloc(base, sizeof(String8Base) + base->length + len + 1);
            if (!base)
                return -1;
            str8->string = getString(base);
        }

        memcpy(str8->string + base->length, s, len);
        base->length += len;
        str8->string[base->length] = '\0';
    }
    else
    {
        base = (String8Base *)realloc(base, sizeof(String8Base) + len + 1);
        if (!base)
            return -1;

        str8->string = getString(base);
        memcpy(str8->string, s, len);
        base->length = len;
        str8->string[base->length] = '\0';
    }

    return 0;
}

int android_String8_init(String8 *str8)
{
    l_debug("android_String8_init(%p)", str8);

    int res = real_init(str8, "", 0);

    l_debug("android_String8_init over: \"%s\"", str8 ? str8->string : NULL);
    return res;
}

int android_String8_initFromCstring(String8 *str8, const char *s)
{
    l_debug("android_String8_initFromCstring(%p,\"%s\")", str8, s);

    size_t len = s ? strlen(s) : 0;
    int res = real_init(str8, s, len);

    l_debug("android_String8_initFromCstring over: \"%s\"", str8 ? str8->string : NULL);
    return res;
}

int android_String8_initFromString8(String8 *str8, const String8 *src)
{
    l_debug("android_String8_initFromString8(%p,%p): \"%s\"", str8, src, src ? src->string : NULL);

    const char *s = src ? src->string : NULL;
    size_t len = s ? strlen(s) : 0;
    int res = real_init(str8, s, len);

    l_debug("android_String8_initFromString8 over: \"%s\"", str8 ? str8->string : NULL);
    return res;
}

void android_String8_term(String8 *str8)
{
    l_debug("android_String8_term(%p): \"%s\"", str8, str8 ? str8->string : NULL);

    if (str8 && str8->string)
    {
        String8Base *base = getBase(str8->string);
        free(base);
        str8->string = NULL;
    }

    l_debug("android_String8_term over");
}

int android_String8_setTo(String8 *str8, const String8 *src)
{
    l_debug("android_String8_setToCstring(%p,%p): \"%s\"", str8, src, src ? src->string : NULL);

    const char *s = src ? src->string : NULL;
    size_t len = s ? strlen(s) : 0;
    int res = real_setTo(str8, s, len);

    l_debug("android_String8_setToCstring over: \"%s\"", str8 ? str8->string : NULL);
    return 0;
}

int android_String8_setToCstring(String8 *str8, const char *s)
{
    l_debug("android_String8_setToCstring(%p,\"%s\")", str8, s);

    size_t len = s ? strlen(s) : 0;
    int res = real_setTo(str8, s, len);

    l_debug("android_String8_setToCstring over: \"%s\"", str8 ? str8->string : NULL);
    return 0;
}

int android_String8_append(String8 *str8, const String8 *src)
{
    l_debug("android_String8_append(%p,%p): \"%s\",\"%s\"", str8, src, str8 ? str8->string : NULL, src ? src->string : NULL);

    const char *s = src ? src->string : NULL;
    size_t len = s ? getBase(s)->length : 0;
    int res = real_append(str8, s, len);

    l_debug("android_String8_append over: \"%s\"", str8 ? str8->string : NULL);
    return res;
}

int android_String8_appendCstring(String8 *str8, const char *s)
{
    l_debug("android_String8_appendCstring(%p): \"%s\",\"%s\"", str8, str8 ? str8->string : NULL, s);

    size_t len = s ? strlen(s) : 0;
    int res = real_append(str8, s, len);

    l_debug("android_String8_appendCstring over: \"%s\"", str8 ? str8->string : NULL);
    return res;
}

int android_String8_appendPath(String8 *str8, const String8 *src)
{
    l_debug("android_String8_appendPath(%p,%p): \"%s\",\"%s\"", str8, src, str8 ? str8->string : NULL, src ? src->string : NULL);

    const char *s = src ? src->string : NULL;
    size_t len = s ? strlen(s) : 0;
    int res = real_appendPath(str8, s, len);

    l_debug("android_String8_appendPath over: \"%s\"", str8 ? str8->string : NULL);
    return res;
}

int android_String8_appendPathCstring(String8 *str8, const char *s)
{
    l_debug("android_String8_appendPathCstring(%p): \"%s\",\"%s\"", str8, str8 ? str8->string : NULL, s);

    size_t len = s ? strlen(s) : 0;
    int res = real_appendPath(str8, s, len);

    l_debug("android_String8_appendPathCstring over: \"%s\"", str8 ? str8->string : NULL);
    return res;
}

int android_String8_getPathDir(String8 *str8, const String8 *src)
{
    l_debug("android_String8_getPathDir(%p,%p): \"%s\"", str8, src, src ? src->string : NULL);

    int res;

    if (!src || !src->string) // 创建空字符串
    {
        res = real_init(str8, NULL, 0);
    }
    else // 创建并获取父路径
    {
        const char *s = src->string;
        const char *cp = strrchr(s, '/');
        if (cp == NULL)
            cp = s;

        size_t len = cp - s;
        res = real_init(str8, s, len);
    }

    l_debug("android_String8_getPathDir over: \"%s\"", str8 ? str8->string : NULL);
    return res;
}

int android_String8_walkPath(String8 *str8, const String8 *src, String8 *out_remains)
{
    l_debug("android_String8_walkPath(%p,%p,%p): \"%s\"", str8, src, out_remains, src ? src->string : NULL);

    int res;

    if (!src || !src->string) // 创建空字符串
    {
        res = real_init(str8, NULL, 0);
        if (out_remains && out_remains->string)
            real_setTo(out_remains, NULL, 0);
    }
    else
    {
        const char *s = src->string;
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

        res = real_init(str8, buf, len);
        if (out_remains && out_remains->string)
        {
            buf = cp < tail ? cp + 1 : tail;
            len = tail - buf;
            real_setTo(str8, buf, len);
        }
    }

    l_debug("android_String8_walkPath over: \"%s\",\"%s\"", str8 ? str8->string : NULL, out_remains ? out_remains->string : NULL);
    return res;
}

int android_String8_empty(String8 *str8)
{
    return real_setTo(str8, NULL, 0);
}

const char *android_String8_string(String8 *str8)
{
    return str8 ? str8->string : NULL;
}

size_t android_String8_length(String8 *str8)
{
    return str8 && str8->string ? getBase(str8->string)->length : 0;
}

int android_String8_isEmpty(String8 *str8)
{
    return str8 && str8->string ? getBase(str8->string)->length == 1 : 0;
}
