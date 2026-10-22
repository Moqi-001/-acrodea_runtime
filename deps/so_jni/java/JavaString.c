#include <stdlib.h>
#include <string.h>

#include "../jni.h"
#include "JavaObject.h"
#include "JavaTypes.h"

#include "JavaString.h"

typedef struct _JavaString {
    uint32_t magic;
    jchar   *utf16;
    jsize    utf16_len;
    char    *utf8;
    jsize    utf8_len;
} _JavaString;

#define JAVA_STRING_MAGIC    0x4C6A30F3  // "JavaString".hashCode()
#define JAVA_STRING_CAST(_o) JAVA_BASE_CAST(_JavaString *, _o, JAVA_STRING_MAGIC)

static jsize _utf8_to_utf16_length(const char *utf8, jsize utf8_len) {
    if (!utf8 || utf8_len <= 0) return 0;

    jsize                utf16_len = 0;
    const unsigned char *p         = (const unsigned char *)utf8;
    const unsigned char *end       = p + utf8_len;

    while (p < end) {
        if ((*p & 0x80) == 0) {  // 1字节 ASCII
            utf16_len++;
            p += 1;
        } else if ((*p & 0xE0) == 0xC0) {  // 2字节 UTF-8
            utf16_len++;
            p += 2;
        } else if ((*p & 0xF0) == 0xE0) {  // 3字节 UTF-8
            utf16_len++;
            p += 3;
        } else if ((*p & 0xF8) == 0xF0) {  // 4字节 UTF-8（代理对）
            utf16_len += 2;
            p += 4;
        } else {  // 非法 UTF-8
            utf16_len++;
            p += 1;
        }
    }

    return utf16_len;
}

static jsize _utf16_to_utf8_length(const jchar *utf16, jsize utf16_len) {
    if (!utf16 || utf16_len <= 0) return 0;

    jsize        utf8_len = 0;
    const jchar *p        = utf16;
    const jchar *end      = p + utf16_len;

    while (p < end) {
        if (*p < 0x80) {  // 1字节 ASCII
            utf8_len += 1;
            p += 1;
        } else if (*p < 0x800) {  // 2字节 UTF-8
            utf8_len += 2;
            p += 1;
        } else if (*p >= 0xD800 && *p <= 0xDBFF) {  // 4字节 UTF-8（代理对）
            utf8_len += 4;
            p += 2;
        } else {  // 3字节 UTF-8
            utf8_len += 3;
            p += 1;
        }
    }

    return utf8_len;
}

static void _utf8_to_utf16(const char *utf8, jsize utf8_len, jchar *utf16_out) {
    if (!utf8 || utf8_len <= 0 || !utf16_out) return;

    const unsigned char *p   = (const unsigned char *)utf8;
    const unsigned char *end = p + utf8_len;

    while (p < end) {
        if ((*p & 0x80) == 0) {
            *utf16_out++ = *p++;
        } else if ((*p & 0xE0) == 0xC0) {
            if (p + 1 < end) {
                unsigned char c1 = *p++;
                unsigned char c2 = *p++;
                if ((c2 & 0xC0) == 0x80) {
                    *utf16_out++ = ((c1 & 0x1F) << 6) | (c2 & 0x3F);
                } else {
                    *utf16_out++ = 0xFFFD;  // 非法：输出'�'
                }
            } else {
                *utf16_out++ = 0xFFFD;  // 非法：输出'�'
                p            = end;
            }
        } else if ((*p & 0xF0) == 0xE0) {
            if (p + 2 < end) {
                unsigned char c1 = *p++;
                unsigned char c2 = *p++;
                unsigned char c3 = *p++;
                if ((c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80) {
                    *utf16_out++ = ((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
                } else {
                    *utf16_out++ = 0xFFFD;  // 非法：输出'�'
                }
            } else {
                *utf16_out++ = 0xFFFD;  // 非法：输出'�'
                p            = end;
            }
        } else if ((*p & 0xF8) == 0xF0) {
            if (p + 3 < end) {
                unsigned char c1 = *p++;
                unsigned char c2 = *p++;
                unsigned char c3 = *p++;
                unsigned char c4 = *p++;
                if ((c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80 && (c4 & 0xC0) == 0x80) {
                    uint32_t codepoint = ((c1 & 0x07) << 18) | ((c2 & 0x3F) << 12) |
                                         ((c3 & 0x3F) << 6) | (c4 & 0x3F);
                    if (codepoint >= 0x10000 && codepoint <= 0x10FFFF) {
                        *utf16_out++ = 0xD800 + ((codepoint - 0x10000) >> 10);
                        *utf16_out++ = 0xDC00 + ((codepoint - 0x10000) & 0x3FF);
                    } else {
                        *utf16_out++ = 0xFFFD;  // 非法：输出'�'
                    }
                } else {
                    *utf16_out++ = 0xFFFD;  // 非法：输出'�'
                }
            } else {
                *utf16_out++ = 0xFFFD;  // 非法：输出'�'
                p            = end;
            }
        } else {
            *utf16_out++ = 0xFFFD;  // 非法：输出'�'
            p++;
        }
    }
}

static void _utf16_to_utf8(const jchar *utf16, jsize utf16_len, char *utf8_out) {
    if (!utf16 || utf16_len <= 0 || !utf8_out) return;

    const jchar *p   = utf16;
    const jchar *end = p + utf16_len;

    while (p < end) {
        jchar ch = *p++;

        if (ch < 0x80) {
            // 1字节 ASCII
            *utf8_out++ = (char)ch;
        } else if (ch < 0x800) {
            // 2字节 UTF-8: 110xxxxx 10xxxxxx
            *utf8_out++ = (char)(0xC0 | ((ch >> 6) & 0x1F));
            *utf8_out++ = (char)(0x80 | (ch & 0x3F));
        } else if (ch >= 0xD800 && ch <= 0xDBFF) {
            // 高代理项，需要组合成 4 字节 UTF-8
            if (p + 1 < end) {
                jchar high = ch;
                jchar low  = *p++;

                if (low >= 0xDC00 && low <= 0xDFFF) {
                    // 计算 Unicode 码点
                    uint32_t codepoint = 0x10000 + ((high - 0xD800) << 10) + (low - 0xDC00);

                    // 4 字节 UTF-8: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                    *utf8_out++ = (char)(0xF0 | ((codepoint >> 18) & 0x07));
                    *utf8_out++ = (char)(0x80 | ((codepoint >> 12) & 0x3F));
                    *utf8_out++ = (char)(0x80 | ((codepoint >> 6) & 0x3F));
                    *utf8_out++ = (char)(0x80 | (codepoint & 0x3F));
                } else {
                    // 非法：高代理项后不是低代理项，输出 '�'
                    *utf8_out++ = (char)0xEF;
                    *utf8_out++ = (char)0xBF;
                    *utf8_out++ = (char)0xBD;
                }
            } else {
                // 非法：高代理项后无字符，输出 '�'
                *utf8_out++ = (char)0xEF;
                *utf8_out++ = (char)0xBF;
                *utf8_out++ = (char)0xBD;
            }
        } else if (ch >= 0xDC00 && ch <= 0xDFFF) {
            // 非法：孤立的低代理项，输出 '�'
            *utf8_out++ = (char)0xEF;
            *utf8_out++ = (char)0xBF;
            *utf8_out++ = (char)0xBD;
        } else {
            // 3 字节 UTF-8（普通 BMP 字符）: 11100xxx 10xxxxxx 10xxxxxx
            *utf8_out++ = (char)(0xE0 | ((ch >> 12) & 0x0F));
            *utf8_out++ = (char)(0x80 | ((ch >> 6) & 0x3F));
            *utf8_out++ = (char)(0x80 | (ch & 0x3F));
        }
    }

    *utf8_out = '\0';
}

static jint _string_utf16_ensure(_JavaString *str) {
    if (!str) return JNI_ERR;

    if (str->utf16) return JNI_OK;

    if (!str->utf8 || str->utf8_len <= 0) {
        return JNI_ERR;
    }

    jsize  utf16_len = _utf8_to_utf16_length(str->utf8, str->utf8_len);
    jchar *utf16     = (jchar *)malloc(utf16_len * sizeof(jchar));
    if (!utf16) {
        return JNI_ERR;
    }

    _utf8_to_utf16(str->utf8, str->utf8_len, utf16);
    str->utf16     = utf16;
    str->utf16_len = utf16_len;

    return JNI_OK;
}

static jint _string_utf8_ensure(_JavaString *str) {
    if (!str) return JNI_ERR;

    if (str->utf8) return JNI_OK;

    if (!str->utf16 || str->utf16_len <= 0) {
        return JNI_ERR;
    }

    jsize utf8_len = _utf16_to_utf8_length(str->utf16, str->utf16_len);
    char *utf8     = (char *)malloc(utf8_len + 1);
    if (!utf8) {
        return JNI_ERR;
    }

    _utf16_to_utf8(str->utf16, str->utf16_len, utf8);
    str->utf8     = utf8;
    str->utf8_len = utf8_len;

    return JNI_OK;
}

static jint _string_utf16_ctor(_JavaString *str, const jchar *utf16, jsize utf16_len) {
    if (!str || !utf16 || utf16_len <= 0) return JNI_EINVAL;

    str->magic       = JAVA_STRING_MAGIC;
    str->utf8        = NULL;
    str->utf8_len    = 0;
    jsize utf16_size = utf16_len * sizeof(jchar);
    str->utf16       = malloc(utf16_size);
    if (!str->utf16) {
        return JNI_ENOMEM;
    }
    memcpy(str->utf16, utf16, utf16_size);
    str->utf16_len = utf16_len;

    return JNI_OK;
}

static jint _string_utf8_ctor(_JavaString *str, const char *utf8) {
    if (!str || !utf8) return JNI_EINVAL;

    str->magic      = JAVA_STRING_MAGIC;
    str->utf16      = NULL;
    str->utf16_len  = 0;
    jsize utf8_len  = strlen(utf8);
    jsize utf8_size = utf8_len + 1;
    str->utf8       = malloc(utf8_size);
    if (!str->utf8) {
        return JNI_ENOMEM;
    }
    memcpy(str->utf8, utf8, utf8_size);
    str->utf8_len = utf8_len;

    return JNI_OK;
}

static void _string_dtor(_JavaString *str) {
    if (!str) return;

    if (str->utf16) {
        free(str->utf16);
    }
    if (str->utf8) {
        free(str->utf8);
    }
}

JavaString JavaNewString(const jchar *utf16, jsize len) {
    _JavaString *str = (_JavaString *)JavaAllocObjectNoClass(sizeof(_JavaString), (JavaDtor)_string_dtor);
    if (!str) {
        return NULL;
    }

    if (_string_utf16_ctor(str, utf16, len) != JNI_OK) {
        JavaReleaseObject(str);
        return NULL;
    }

    return (JavaString)str;
}

const jchar *JavaGetStringChars(JavaString string) {
    _JavaString *str = JAVA_STRING_CAST(string);
    if (!str) return NULL;

    if (_string_utf16_ensure(str) != JNI_OK) return NULL;

    return str->utf16;
}

jsize JavaGetStringLength(JavaString string) {
    _JavaString *str = JAVA_STRING_CAST(string);
    if (!str) return 0;

    if (_string_utf16_ensure(str) != JNI_OK) return 0;

    return str->utf16_len;
}

jint JavaGetStringRegion(JavaString string, jsize start, jsize length, jchar *dst) {
    _JavaString *str = JAVA_STRING_CAST(string);
    if (!str || !dst || start < 0 || length <= 0) return JNI_EINVAL;

    if (_string_utf16_ensure(str) != JNI_OK) return JNI_ERR;

    if (!str->utf16 || start + length > str->utf16_len) return JNI_ERR;

    memcpy(dst, str->utf16 + start, length * sizeof(jchar));

    return JNI_OK;
}

JavaString JavaNewStringUTF(const char *utf8) {
    _JavaString *str = (_JavaString *)JavaAllocObjectNoClass(sizeof(_JavaString), (JavaDtor)_string_dtor);
    if (!str) {
        return NULL;
    }

    if (_string_utf8_ctor(str, utf8) != JNI_OK) {
        JavaReleaseObject(str);
        return NULL;
    }

    return (JavaString)str;
}

const char *JavaGetStringUTFChars(JavaString string) {
    _JavaString *str = JAVA_STRING_CAST(string);
    if (!str) return NULL;

    if (_string_utf8_ensure(str) != JNI_OK) return NULL;

    return str->utf8;
}

jsize JavaGetStringUTFLength(JavaString string) {
    _JavaString *str = JAVA_STRING_CAST(string);
    if (!str) return 0;

    if (_string_utf8_ensure(str) != JNI_OK) return 0;

    return str->utf8_len;
}

jint JavaGetStringUTFRegion(JavaString string, jsize start, jsize length, char *dst) {
    _JavaString *str = JAVA_STRING_CAST(string);
    if (!str || !dst || start < 0 || length <= 0) return JNI_EINVAL;

    if (_string_utf8_ensure(str) != JNI_OK) return JNI_ERR;

    if (!str->utf8 || start + length > str->utf8_len) return JNI_ERR;

    memcpy(dst, str->utf8 + start, length);

    return JNI_OK;
}
