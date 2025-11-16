//
// Created by Lisen on 2025/10/31.
//
#include "utils/logger.h"
#include "reimpl/android7string8.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

class SharedBuffer
{
public:
    enum {
        eKeepStorage = 0x00000001
    };

    static SharedBuffer* alloc(size_t size) {
        if (size >= (SIZE_MAX - sizeof(SharedBuffer))) {
            return nullptr;
        }
        SharedBuffer* sb = static_cast<SharedBuffer*>(malloc(sizeof(SharedBuffer) + size));
        if (sb) {
            sb->mRefs = 1;
            sb->mSize = size;
        }
        return sb;
    }

    static void dealloc(const SharedBuffer* released) {
        free(const_cast<SharedBuffer*>(released));
    }

    const void* data() const { return this + 1; }
    void* data() { return this + 1; }
    size_t size() const { return mSize; }

    static SharedBuffer* bufferFromData(void* data) {
        return data ? static_cast<SharedBuffer*>(data) - 1 : nullptr;
    }

    static const SharedBuffer* bufferFromData(const void* data) {
        return data ? static_cast<const SharedBuffer*>(data) - 1 : nullptr;
    }

    static size_t sizeFromData(const void* data) {
        return data ? bufferFromData(data)->mSize : 0;
    }

    SharedBuffer* edit() const {
        if (onlyOwner()) {
            return const_cast<SharedBuffer*>(this);
        }
        SharedBuffer* sb = alloc(mSize);
        if (sb) {
            memcpy(sb->data(), data(), size());
            release();
        }
        return sb;
    }

    SharedBuffer* editResize(size_t newSize) const {
        if (onlyOwner()) {
            SharedBuffer* buf = const_cast<SharedBuffer*>(this);
            if (buf->mSize == newSize) return buf;
            if ((newSize >= (SIZE_MAX - sizeof(SharedBuffer)))) {
                return nullptr;
            }
            buf = (SharedBuffer*)realloc(buf, sizeof(SharedBuffer) + newSize);
            if (buf != nullptr) {
                buf->mSize = newSize;
                return buf;
            }
        }
        SharedBuffer* sb = alloc(newSize);
        if (sb) {
            const size_t mySize = mSize;
            memcpy(sb->data(), data(), newSize < mySize ? newSize : mySize);
            release();
        }
        return sb;
    }

    SharedBuffer* attemptEdit() const {
        if (onlyOwner()) {
            return const_cast<SharedBuffer*>(this);
        }
        return nullptr;
    }

    SharedBuffer* reset(size_t new_size) const {
        SharedBuffer* sb = alloc(new_size);
        if (sb) {
            release();
        }
        return sb;
    }

    void acquire() const {
        mRefs++;
    }

    int32_t release(uint32_t flags = 0) const {
        const bool useDealloc = ((flags & eKeepStorage) == 0);
        if (onlyOwner()) {
            mRefs = 0;
            if (useDealloc) {
                dealloc(this);
            }
            return 1;
        }
        int32_t prevRefCount = mRefs--;
        if (prevRefCount == 1) {
            if (useDealloc) {
                dealloc(this);
            }
        }
        return prevRefCount;
    }

    bool onlyOwner() const { return (mRefs == 1); }

private:
    SharedBuffer() = default;
    ~SharedBuffer() = default;

    mutable int32_t mRefs;
    size_t mSize;
    uint32_t mReserved[2];
};

// String8 的简化版本，只包含必要的数据成员
class String8 {
private:
    const char *mString;

public:
    String8() : mString(getEmptyString()) {}

    String8(const char *other) : mString(allocFromUTF8(other, other ? strlen(other) : 0)) {}

    String8(const char* other, size_t len) : mString(allocFromUTF8(other, len)) {}

    String8(const String8 &other) : mString(other.mString) {
        SharedBuffer::bufferFromData(mString)->acquire();
    }

    ~String8() {
        SharedBuffer::bufferFromData(mString)->release();
    }

    const char *string() const { return mString; }

    size_t length() const {
        const SharedBuffer* buf = SharedBuffer::bufferFromData(mString);
        return buf ? buf->size() - 1 : 0;
    }

    size_t size() const { return length(); }

    bool isEmpty() const { return length() == 0; }

    String8 &setTo(const char *other) {
        l_debug("setTo(%s)",other);
        const char *newString = allocFromUTF8(other, other ? strlen(other) : 0);
        l_debug("setTo newString(%s)",newString);
        SharedBuffer::bufferFromData(mString)->release();
        mString = newString ? newString : getEmptyString();
        return *this;
    }

    String8 &setTo(const String8 &other) {
        SharedBuffer::bufferFromData(other.mString)->acquire();
        SharedBuffer::bufferFromData(mString)->release();
        mString = other.mString;
        return *this;
    }

    String8 &append(const char *other) {
        if (!other || !*other) return *this;
        return real_append(other, strlen(other));
    }

    String8 &append(const String8 &other) {
        if (other.isEmpty()) return *this;
        return real_append(other.string(), other.length());
    }

    String8 &appendPath(const char *component) {
        if (!component || !*component) return *this;

        // 简单的路径追加实现
        if (isEmpty()) {
            setTo(component);
        } else {
            size_t len = length();
            if (mString[len - 1] != '/') {
                append("/");
            }
            append(component);
        }
        return *this;
    }

    String8 getPathDir() const {
        if (isEmpty()) return String8("");

        const char *str = mString;
        const char *lastSlash = strrchr(str, '/');
        if (!lastSlash) return String8("");

        return String8(str, lastSlash - str);
    }

private:
    static char *getEmptyString() {
        static SharedBuffer *gEmptyBuf = nullptr;
        if (!gEmptyBuf) {
            gEmptyBuf = SharedBuffer::alloc(1);
            char *str = static_cast<char *>(gEmptyBuf->data());
            *str = 0;
        }
        gEmptyBuf->acquire();
        return static_cast<char *>(gEmptyBuf->data());
    }

    static char *allocFromUTF8(const char *in, size_t len) {
        if (len == 0) return getEmptyString();

        SharedBuffer *buf = SharedBuffer::alloc(len + 1);
        if (!buf) return getEmptyString();

        char *str = static_cast<char *>(buf->data());
        if (in && len > 0) {
            memcpy(str, in, len);
        }
        str[len] = 0;
        return str;
    }

    String8 &real_append(const char *other, size_t otherLen) {
        if (otherLen == 0) return *this;

        size_t myLen = length();
        SharedBuffer *buf = SharedBuffer::bufferFromData(mString)->editResize(myLen + otherLen + 1);
        if (buf) {
            char *str = static_cast<char *>(buf->data());
            mString = str;
            memcpy(str + myLen, other, otherLen);
            str[myLen + otherLen] = 0;
        }
        return *this;
    }
};

// 构造函数
void *_ZN7android7String8C1Ev() {
    l_debug("_ZN7android7String8C1Ev");
    return new String8();
}

void *_ZN7android7String8C1Ev0(char *self) {
    //l_debug("_ZN7android7String8C1Ev0(%s)",((String8*)self)->string());
    l_debug("_ZN7android7String8C1Ev0(%s)",self);
    //memset(self,0,strlen(self));
    return self;
}

void *_ZN7android7String8C1EPKc0(const void **self,const char *other2) {
    l_debug("_ZN7android7String8C1EPKc0(%s)",other2);
    String8* path=new String8(DATA_PATH);
    path->append(other2);
    *self=path->string();
    return self;
}

void *_ZN7android7String8C1EPKc(const char *other) {
    l_debug("_ZN7android7String8C1EPKc");
    return new String8(other);
}

void* _ZN7android7String8C1EPKc(const char* other, size_t len) {
    l_debug("_ZN7android7String8C1EPKcj - with length: %zu", len);

    // 创建新的 String8 对象
    String8* str = new String8();
    if (other && len > 0) {
        // 手动设置内容
        SharedBuffer* buf = SharedBuffer::alloc(len + 1);
        if (buf) {
            char* data = static_cast<char*>(buf->data());
            memcpy(data, other, len);
            data[len] = 0;

            // 释放原有的空字符串
            SharedBuffer::bufferFromData(str->string())->release();
            // 设置新的字符串
            *((const char**)str) = data;
        }
    }
    return str;
}

void *_ZN7android7String8C1ERKS0_(void *other) {
    l_debug("_ZN7android7String8C1ERKS0_");
    return new String8(*static_cast<String8 *>(other));
}

// 析构函数
void _ZN7android7String8D1Ev(void *self) {
    l_debug("_ZN7android7String8D1Ev");
    //delete static_cast<String8 *>(self);
}

// setTo 函数
void *_ZN7android7String85setToEPKc(void *self, const char *other) {
    l_debug("_ZN7android7String85setToEPKc");
    String8 *str = static_cast<String8 *>(self);
    return &str->setTo(other);
}

// append 函数
void *_ZN7android7String86appendEPKc(void *self, const char *other) {
    
    String8 *str = static_cast<String8 *>(self);
    l_debug("_ZN7android7String86appendEPKc(%s,%s)",str->string(),other);
    return &str->append(other);
}

void *_ZN7android7String86appendERKS0_(char **self,const char *other) {
   
    String8 *str = new String8(*self);
    l_debug("_ZN7android7String86appendERKS0_(%s)",str->string());
    String8 *otherStr = new String8(other);
    if(!otherStr->isEmpty())
       str->append(other);
    l_debug("_ZN7android7String86appendERKS0_end(%s)",str->string());
    return str;
}

// appendPath 函数
void *_ZN7android7String810appendPathEPKc(void *self, const char *component) {
    String8 *str = static_cast<String8 *>(self);
    l_debug("_ZN7android7String810appendPathEPKc(%s,%s)",str->string(),component);
    str->appendPath(component);
    l_debug("_ZN7android7String810appendPathEPKcend(%s)",str->string());
    return self;
}

// 获取函数
const char *_ZNK7android7String86stringEv(void *self) {
    l_debug("_ZNK7android7String86stringEv");
    const String8 *str = static_cast<const String8 *>(self);
    return str->string();
}

size_t _ZNK7android7String86lengthEv(void *self) {
    l_debug("_ZNK7android7String86lengthEv");
    const String8 *str = static_cast<const String8 *>(self);
    return str->length();
}

// 验证内存布局
void debug_string8_layout() {
    l_debug("=== String8 Memory Layout ===");
    l_debug("sizeof(String8): %zu", sizeof(String8));

    String8 test("test");
    l_debug("test object at: %p", &test);
    l_debug("test.mString at: %p", (void*)*((const char**)(&test)));
    l_debug("test.string(): %p", test.string());
    l_debug("=== End Memory Layout ===");
}

// 辅助函数：检查指针是否可读
bool isReadablePointer(const void* ptr) {
    if (!ptr) return false;

    // 1. 检查是否为常见的错误指针值
    uintptr_t addr = (uintptr_t)ptr;
    if (addr == 0xdeadbeef || addr == 0xbaadf00d ||
        addr == 0xffffffff || addr < 0x1000) {
        return false;
    }

    // 2. 检查是否在合理的用户空间地址范围内
    if (addr < 0x08000000 || addr > 0x7FFFFFFF) {
        return false;
    }

    return true;
}

// 辅助函数：安全提取字符串
void extractSafeString(const char* src, char* dest, size_t destSize) {
    if (!src || !dest || destSize == 0) {
        if (dest && destSize > 0) dest[0] = 0;
        return;
    }

    size_t i;
    for (i = 0; i < destSize - 1; i++) {
        // 检查字符是否可打印或常见控制字符
        if (src[i] == 0) break;
        if (src[i] < 0 || src[i] > 127) break; // 简单ASCII检查

        dest[i] = src[i];
    }
    dest[i] = 0;
}

// getPathDir 函数
void *_ZNK7android7String810getPathDirEv(void *self) {
    l_debug("_ZNK7android7String810getPathDirEv()");
    
    return NULL;
    if (!self) {
        l_debug("ERROR: self is null!");
        return nullptr;
    }

    // 直接检查内存内容，不依赖 String8 类
    const char** mStringPtr = (const char**)((uintptr_t)self);
    const char* strData = *mStringPtr;

    l_debug("Raw mString value: %p", strData);

    // 检查是否是有效的指针（不是 0xdeadbeef 等标记值）
    if ((uintptr_t)strData == 0xdeadbeef ||
        (uintptr_t)strData == 0xbaadf00d ||
        (uintptr_t)strData < 0x1000) {
        l_debug("ERROR: mString contains invalid pointer: %p", strData);
        // 返回一个空的 String8 对象而不是崩溃
        return new String8("");
    }

    // 检查指针是否可读
    if (!isReadablePointer(strData)) {
        l_debug("ERROR: mString points to unreadable memory: %p", strData);
        return new String8("");
    }

    // 安全地提取字符串内容
    char safeBuffer[256] = {0};
    extractSafeString(strData, safeBuffer, sizeof(safeBuffer));
    l_debug("Path string: '%s'", safeBuffer);

    // 手动实现 getPathDir 逻辑，不依赖 String8 的方法
    const char* lastSlash = strrchr(safeBuffer, '/');
    if (!lastSlash) {
        l_debug("No directory separator found, returning empty");
        return new String8("");
    }

    // 提取目录部分
    size_t dirLen = lastSlash - safeBuffer;
    char* dirPath = (char*)malloc(dirLen + 1);
    if (!dirPath) {
        l_debug("ERROR: malloc failed");
        return new String8("");
    }

    memcpy(dirPath, safeBuffer, dirLen);
    dirPath[dirLen] = 0;

    l_debug("Directory: '%s'", dirPath);

    String8* result = new String8(dirPath);
    free(dirPath);

    return result;
}

// walkPath
void* _ZNK7android7String88walkPathEPS0_(void* self, void* outRemains) {
    l_debug("_ZNK7android7String88walkPathEPS0_()");
    return NULL;
    if (!self) {
        l_debug("ERROR: self is null!");
        return _ZN7android7String8C1Ev(); // 返回空字符串
    }

    // 直接访问内存获取字符串
    const char** mStringPtr = (const char**)((uintptr_t)self);
    const char* strData = *mStringPtr;

    l_debug("Input path: %p -> '%s'", strData, strData ? strData : "NULL");

    if (!strData || !strData[0]) {
        l_debug("Empty path string");
        if (outRemains) {
            *(void**)outRemains = _ZN7android7String8C1Ev(); // 设置空的剩余部分
        }
        return _ZN7android7String8C1Ev(); // 返回空字符串
    }

    const char* buf = strData;

    // 跳过开头的路径分隔符
    if (*buf == '/') {
        buf++;
    }

    // 找到第一个路径分隔符
    const char* cp = strchr(buf, '/');

    if (cp == nullptr) {
        // 没有找到分隔符，返回整个字符串作为结果
        l_debug("No path separator found, returning entire path");
        void* result = _ZN7android7String8C1EPKc(buf);

        if (outRemains) {
            // 剩余部分为空
            *(void**)outRemains = _ZN7android7String8C1Ev();
        }

        l_debug("Result: '%s', Remains: ''",
                _ZNK7android7String86stringEv(result));
        return result;
    }

    // 提取第一个路径组件
    size_t componentLen = cp - buf;
    l_debug("Found separator at position %ld, component length: %zu",
            cp - strData, componentLen);

    void* result = _ZN7android7String8C1EPKc(buf, componentLen);

    if (outRemains) {
        // 设置剩余部分（跳过分隔符）
        *(void**)outRemains = _ZN7android7String8C1EPKc(cp + 1);
    }

    l_debug("Result: '%s', Remains: '%s'",
            _ZNK7android7String86stringEv(result),
            outRemains ? _ZNK7android7String86stringEv(*(void**)outRemains) : "NULL");

    return result;
}

// operator= 赋值操作符
void *_ZN7android7String8aSERKS0_(void *self, void *other) {
    l_debug("_ZN7android7String8aSERKS0_");
    String8 *str = static_cast<String8 *>(self);
    String8 *otherStr = static_cast<String8 *>(other);
    return &str->setTo(*otherStr);
}

// clear 函数
void _ZN7android7String85clearEv(void *self) {
    l_debug("_ZN7android7String85clearEv");
    String8 *str = static_cast<String8 *>(self);
    str->setTo("");
}

// isEmpty 函数
int _ZNK7android7String87isEmptyEv(void *self) {
    l_debug("_ZNK7android7String87isEmptyEv");
    const String8 *str = static_cast<const String8 *>(self);
    return str->isEmpty() ? 1 : 0;
}

// size 函数 (通常与 length 相同)
size_t _ZNK7android7String84sizeEv(void *self) {
    l_debug("_ZNK7android7String84sizeEv");
    const String8 *str = static_cast<const String8 *>(self);
    return str->size();
}

// format 函数
void *_ZN7android7String86formatEPKcz(const char *fmt, ...) {
    l_debug("_ZN7android7String86formatEPKcz");
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    return new String8(buffer);
}

// 获取共享缓冲区
const void* _ZNK7android7String812sharedBufferEv(void* self) {
    l_debug("_ZNK7android7String812sharedBufferEv");
    const String8* str = static_cast<const String8*>(self);
    return SharedBuffer::bufferFromData(str->string());
}

void* _ZN7android12SharedBuffer5allocEj(void* self,size_t size)
{
    l_debug("_ZN7android12SharedBuffer5allocEj(%p,%d)",self,size);
    SharedBuffer* s=static_cast<SharedBuffer*>(self);
    return s->alloc(size);
}

void* _ZNK7android12SharedBuffer10editResizeEj(void* self,size_t size)
{
    l_debug("_ZNK7android12SharedBuffer10editResizeEj(%p,%d)",self,size);
    SharedBuffer* s=static_cast<SharedBuffer*>(self);
    return s->editResize(size);
}

int _ZNK7android12SharedBuffer7releaseEj(void* self,size_t flags)
{
    l_debug("_ZNK7android12SharedBuffer7releaseEj(%p,%d)",self,flags);
    SharedBuffer* s=static_cast<SharedBuffer*>(self);
    return s->release(flags);
}

int _ZN7android12SharedBuffer7deallocEPKS0_(void* self,void* a2)
{
    l_debug("_ZN7android12SharedBuffer7deallocEPKS0_(%p,%p)",self,a2);
    SharedBuffer* s=static_cast<SharedBuffer*>(self);

    s->dealloc(static_cast<SharedBuffer*>(a2));
    return 0;
}

int _ZNK7android12SharedBuffer7acquireEv(void* self)
{
    l_debug("_ZNK7android12SharedBuffer7acquireEv(%p)",self);
    SharedBuffer* s=static_cast<SharedBuffer*>(self);
    s->acquire();
    return 0;
}

void* _ZNK7android12SharedBuffer11attemptEditEv(void* self)
{
    l_debug("_ZNK7android12SharedBuffer11attemptEditEv(%p)",self);
    SharedBuffer* s=static_cast<SharedBuffer*>(self);

    return s->attemptEdit();
}