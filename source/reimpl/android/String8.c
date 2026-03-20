#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "reimpl/android/SharedBuffer.h"
#include "reimpl/android/String8.h"
#include "utils/logger.h"

struct AString8 {
    char *string;
};

#define getSharedBuffer(self) (ASharedBuffer_bufferFromData(self->string))
#define getString(sb)         (ASharedBuffer_data(sb))
#define getLength(sb)         (ASharedBuffer_size(sb) - 1)

static int real_init(AString8 *self, const char *cstr, size_t len) {
    if (!self)
        return -1;

    ASharedBuffer *sb;
    size_t         self_len;

    if (!cstr || len == 0)  // 创建并写入空字符串
    {
        sb = ASharedBuffer_alloc(1);
        if (!sb)
            return -1;
        self->string           = getString(sb);
        self_len               = 0;
        self->string[self_len] = '\0';
    } else  // 创建并复制字符串
    {
        sb = ASharedBuffer_alloc(len + 1);
        if (!sb)
            return -1;
        self->string = getString(sb);
        memcpy(self->string, cstr, len);
        self_len               = len;
        self->string[self_len] = '\0';
    }

    return 0;
}

static int real_setTo(AString8 *self, const char *cstr, size_t len) {
    if (!self || !self->string)
        return -1;

    ASharedBuffer *sb       = getSharedBuffer(self);
    size_t         self_len = getLength(sb);

    if (!cstr || len == 0)  // 写入空字符串
    {
        sb = ASharedBuffer_editResize(sb, 1);
        if (!sb)
            return -1;
        self->string           = getString(sb);
        self_len               = 0;
        self->string[self_len] = '\0';
    } else  // 复制字符串
    {
        sb = ASharedBuffer_editResize(sb, len + 1);
        if (!sb)
            return -1;
        self->string = getString(sb);
        memcpy(self->string, cstr, len);
        self_len               = len;
        self->string[self_len] = '\0';
    }

    return 0;
}

static int real_append(AString8 *self, const char *cstr, size_t len) {
    if (!self || !self->string)
        return -1;

    if (!cstr || len == 0)
        return 0;

    ASharedBuffer *sb       = getSharedBuffer(self);
    size_t         self_len = getLength(sb);
    sb                      = ASharedBuffer_editResize(sb, self_len + len + 1);
    if (!sb)
        return -1;

    self->string = getString(sb);
    memcpy(self->string + self_len, cstr, len);
    self_len += len;
    self->string[self_len] = '\0';

    return 0;
}

static int real_appendPath(AString8 *self, const char *cstr, size_t len) {
    if (!self || !self->string)
        return -1;

    if (!cstr || len == 0)
        return 0;

    ASharedBuffer *sb;
    size_t         self_len;

    if (cstr[0] != '/') {
        sb       = getSharedBuffer(self);
        self_len = getLength(sb);
        if (self->string[self_len] != '/') {
            sb = ASharedBuffer_editResize(sb, self_len + len + 2);  // 多分配一个字节给'/'
            if (!sb)
                return -1;
            self->string             = getString(sb);
            self->string[self_len++] = '/';
        } else {
            sb = ASharedBuffer_editResize(sb, self_len + len + 1);
            if (!sb)
                return -1;
            self->string = getString(sb);
        }

        memcpy(self->string + self_len, cstr, len);
        self_len += len;
        self->string[self_len] = '\0';
    } else {
        sb = ASharedBuffer_editResize(sb, len + 1);
        if (!sb)
            return -1;
        self->string = getString(sb);
        memcpy(self->string, cstr, len);
        self_len               = len;
        self->string[self_len] = '\0';
    }

    return 0;
}

int AString8_construct(AString8 *self) {
    int res = real_init(self, NULL, 0);

    l_debug("[Android] AString8_construct(%p): '%s'", self, self ? self->string : NULL);
    return res;
}

int AString8_constructFromCstr(AString8 *self, const char *cstr) {
    size_t len = cstr ? strlen(cstr) : 0;
    int    res = real_init(self, cstr, len);

    l_debug("[Android] AString8_constructFromCstr(%p,%p): '%s'", self, cstr, self ? self->string : NULL);
    return res;
}

int AString8_constructFromOther(AString8 *self, const AString8 *other) {
    int res = -1;

    if (self) {
        if (other && other->string) {
            ASharedBuffer_acquire(getSharedBuffer(other));
            self->string = other->string;
            res          = 0;
        } else {
            self->string = NULL;
        }
    }

    l_debug("[Android] AString8_constructFromOther(%p,%p): '%s'", self, other, self ? self->string : NULL);
    return res;
}

void AString8_destruct(AString8 *self) {
    l_debug("[Android] AString8_destruct(%p): '%s'", self, self ? self->string : NULL);

    if (self && self->string) {
        ASharedBuffer_release(getSharedBuffer(self), 0);
        self->string = NULL;
    }
}

int AString8_setTo(AString8 *self, const AString8 *other) {
    int res = -1;

    if (self) {
        if (self->string)
            ASharedBuffer_release(getSharedBuffer(self), 0);

        if (other && other->string) {
            ASharedBuffer_acquire(getSharedBuffer(other));
            self->string = other->string;
            res          = 0;
        } else {
            self->string = NULL;
        }
    }

    l_debug("[Android] AString8_setTo(%p,%p): '%s'", self, other, self ? self->string : NULL);
    return res;
}

int AString8_setToCstr(AString8 *self, const char *cstr) {
    size_t len = cstr ? strlen(cstr) : 0;
    int    res = real_setTo(self, cstr, len);

    l_debug("[Android] AString8_setToCstr(%p,%p): '%s", self, cstr, self ? self->string : NULL);
    return res;
}

int AString8_append(AString8 *self, const AString8 *other) {
    const char *cstr = other ? other->string : NULL;
    size_t      len  = other ? getLength(getSharedBuffer(other)) : 0;
    int         res  = real_append(self, cstr, len);

    l_debug("[Android] AString8_append(%p,%p): '%s'", self, other, self ? self->string : NULL);
    return res;
}

int AString8_appendCstr(AString8 *self, const char *cstr) {
    size_t len = cstr ? strlen(cstr) : 0;
    int    res = real_append(self, cstr, len);

    l_debug("[Android] AString8_appendCstr(%p,%p): '%s'", self, cstr, self ? self->string : NULL);
    return res;
}

int AString8_appendPath(AString8 *self, const AString8 *other) {
    const char *cstr = other ? other->string : NULL;
    size_t      len  = other ? getLength(getSharedBuffer(other)) : 0;
    int         res  = real_appendPath(self, cstr, len);

    l_debug("[Android] AString8_appendPath(%p,%p): '%s'", self, other, self ? self->string : NULL);
    return res;
}

int AString8_appendPathCstr(AString8 *self, const char *cstr) {
    size_t len = cstr ? strlen(cstr) : 0;
    int    res = real_appendPath(self, cstr, len);

    l_debug("[Android] AString8_appendPathCstr(%p,%p): '%s'", self, cstr, self ? self->string : NULL);
    return res;
}

int AString8_getPathDir(AString8 *self, const AString8 *other) {
    int res;

    if (!other || !other->string)  // 创建空字符串
    {
        res = real_init(self, NULL, 0);
    } else  // 创建并获取父路径
    {
        const char *cstr = other->string;
        const char *cp   = strrchr(cstr, '/');
        if (cp == NULL)
            cp = cstr;

        size_t len = cp - cstr;
        res        = real_init(self, cstr, len);
    }

    l_debug("[Android] AString8_getPathDir(%p,%p): '%s'", self, other, self ? self->string : NULL);
    return res;
}

int AString8_walkPath(AString8 *self, const AString8 *other, AString8 *out_remains) {
    int res;

    if (!other || !other->string)  // 创建空字符串
    {
        res = real_init(self, NULL, 0);
        if (out_remains && out_remains->string)
            real_setTo(out_remains, NULL, 0);
    } else {
        const char *cstr = other->string;
        const char *tail = cstr + getLength(getSharedBuffer(other));
        const char *buf  = cstr;
        const char *cp   = strchr(buf, '/');
        if (cp == buf) {
            // don't include a leading '/'.
            buf = buf + 1;
            cp  = strchr(buf, '/');
        }
        if (cp == NULL)
            cp = tail;

        size_t len = cp - buf;

        res = real_init(self, buf, len);
        if (out_remains && out_remains->string) {
            buf = (cp < tail) ? (cp + 1) : tail;
            len = tail - buf;
            real_setTo(out_remains, buf, len);
        }
    }

    l_debug("[Android] AString8_walkPath(%p,%p,%p): '%s','%s'", self, other, out_remains, self ? self->string : NULL, out_remains ? out_remains->string : NULL);
    return res;
}

int AString8_empty(AString8 *self) {
    return real_setTo(self, NULL, 0);
}

const char *AString8_string(AString8 *self) {
    return self ? self->string : NULL;
}

size_t AString8_length(AString8 *self) {
    return self ? getLength(getSharedBuffer(self)) : 0;
}

int AString8_isEmpty(AString8 *self) {
    return self ? (getLength(getSharedBuffer(self)) == 0) : 0;
}
