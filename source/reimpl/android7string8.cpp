//
// Created by Lisen on 2025/10/31.
//
#include <string>
#include <stdint.h>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "utils/logger.h"
#include "reimpl/android7string8.h"

// 使用std::string包装的完整实现
struct String8Impl {
    std::string data;
};


// 构造函数
void* _ZN7android7String8C1Ev() {
    l_debug("_ZN7android7String8C1Ev");

    return new String8Impl();
}

void* _ZN7android7String8C1EPKc(const char* other) {
    l_debug("_ZN7android7String8C1EPKc");

    String8Impl* str = new String8Impl();
    if (other) {
        str->data = other;
    }
    return str;
}

void* _ZN7android7String8C1ERKS0_(void* other) {
    l_debug("_ZN7android7String8C1ERKS0_");

    String8Impl* other_str = static_cast<String8Impl*>(other);
    String8Impl* str = new String8Impl();
    str->data = other_str->data;  // 使用std::string的拷贝构造函数
    return str;
}

// 析构函数
void _ZN7android7String8D1Ev(void* self) {
    l_debug("_ZN7android7String8D1Ev");

    delete static_cast<String8Impl*>(self);
}

// setTo 方法
void* _ZN7android7String85setToEPKc(void* self, const char* other) {
    l_debug("_ZN7android7String85setToEPKc");

    String8Impl* str = static_cast<String8Impl*>(self);
    if (other) {
        str->data = other;
    } else {
        str->data.clear();
    }
    return self;
}

// append 方法
void* _ZN7android7String86appendEPKc(void* self, const char* other) {
    l_debug("_ZN7android7String86appendEPKc");

    String8Impl* str = static_cast<String8Impl*>(self);
    if (other) {
        str->data.append(other);
    }
    return self;
}

void* _ZN7android7String86appendERKS0_(void* self, void* other) {
    l_debug("_ZN7android7String86appendERKS0_");

    String8Impl* str = static_cast<String8Impl*>(self);
    String8Impl* other_str = static_cast<String8Impl*>(other);
    str->data.append(other_str->data);
    return self;
}

// appendPath 方法
void* _ZN7android7String810appendPathEPKc(void* self, const char* component) {
    l_debug("_ZN7android7String810appendPathEPKc");

    String8Impl* str = static_cast<String8Impl*>(self);
    if (!component) return self;

    // 确保路径分隔符
    if (!str->data.empty() && str->data.back() != '/') {
        str->data.append("/");
    }

    // 添加组件
    str->data.append(component);
    return self;
}

// string() 方法
const char* _ZNK7android7String86stringEv(void* self) {
    l_debug("_ZNK7android7String86stringEv");

    String8Impl* str = static_cast<String8Impl*>(self);
    return str->data.c_str();
}

// length() 方法
size_t _ZNK7android7String86lengthEv(void* self) {
    l_debug("_ZNK7android7String86lengthEv");

    String8Impl* str = static_cast<String8Impl*>(self);
    return str->data.length();
}

void* _ZN7android7String8pLEPKc(void* self, const char* other) {
    l_debug("_ZN7android7String8pLEPKc");

    return _ZN7android7String86appendEPKc(self, other);
}

void* _ZN7android7String8pLERKS0_(void* self, void* other) {
    l_debug("_ZN7android7String8pLERKS0_");

    return _ZN7android7String86appendERKS0_(self, other);
}

// clear 方法
void _ZN7android7String85clearEv(void* self) {
    l_debug("_ZN7android7String85clearEv");

    String8Impl* str = static_cast<String8Impl*>(self);
    str->data.clear();
}

// 赋值操作符
void* _ZN7android7String8aSERKS0_(void* self, void* other) {
    l_debug("_ZN7android7String8aSERKS0_");

    String8Impl* str = static_cast<String8Impl*>(self);
    String8Impl* other_str = static_cast<String8Impl*>(other);
    str->data = other_str->data;
    return self;
}

// 最大允许字符串长度
static const size_t MAX_ALLOWED_STRING = 1 << 20; // 1MB

// 检查地址是否落在可读映射区（/proc/self/maps）
static bool addr_is_readable(const void* addr) {
    if (!addr) return false;
    uintptr_t a = (uintptr_t)addr;
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) return false;
    char line[512];
    bool ok = false;
    while (fgets(line, sizeof(line), f)) {
        uintptr_t start = 0, end = 0;
        char perms[8] = {0};
        // 解析如 "00400000-0040b000 r-xp ..."
        if (sscanf(line, "%" PRIxPTR "-%" PRIxPTR " %7s", &start, &end, perms) >= 2) {
            if (a >= start && a < end && perms[0] == 'r') { ok = true; break; }
        }
    }
    fclose(f);
    return ok;
}

static bool safe_read(void* out, const void* src, size_t len) {
    if (!out || !src || len == 0) return false;
    if (!addr_is_readable(src)) return false;
    memcpy(out, src, len);
    return true;
}

// 获取目录
// ex: /home/user/document.txt => /home/user/
void* _ZNK7android7String810getPathDirEv(void* self) {
    l_debug("_ZNK7android7String810getPathDirEv self=%p", self);

    if (!self) {
        l_debug("self is NULL -> return '.'");
        return _ZN7android7String8C1EPKc(".");
    }

    // 先安全读取 self 中的第一个指针 (mBuffer)
    void* mBuffer = nullptr;
    if (!safe_read(&mBuffer, self, sizeof(void*))) {
        l_debug("failed to read mBuffer from self -> return '.'");
        return _ZN7android7String8C1EPKc(".");
    }
    if (!mBuffer) {
        l_debug("mBuffer is NULL -> return '.'");
        return _ZN7android7String8C1EPKc(".");
    }

    // 验证 mBuffer 可读
    if (!addr_is_readable(mBuffer)) {
        l_debug("mBuffer %p not readable -> return '.'", mBuffer);
        return _ZN7android7String8C1EPKc(".");
    }

    // SharedBuffer 前两个字段通常是 size_t mRefs, size_t mSize
    // 先读取这两个 size_t，注意 32/64 位差异由 sizeof(size_t) 自动处理
    size_t header_vals[2] = {0, 0};
    if (!safe_read(&header_vals, mBuffer, sizeof(header_vals))) {
        l_debug("cannot read SharedBuffer header -> return '.'");
        return _ZN7android7String8C1EPKc(".");
    }
    size_t mRefs = header_vals[0];
    size_t mSize = header_vals[1];

    // 合理性检查
    if (mRefs > 1000000000) { // 非常大的引用计数不合理
        l_debug("weird mRefs=%zu -> abort", mRefs);
        return _ZN7android7String8C1EPKc(".");
    }
    if (mSize == 0 || mSize > MAX_ALLOWED_STRING) {
        l_debug("weird mSize=%zu -> abort", mSize);
        return _ZN7android7String8C1EPKc(".");
    }

    // 计算字符串数据起始地址: mBuffer + sizeof(size_t)*2
    size_t header_bytes = sizeof(size_t) * 2;
    const char* data_ptr = reinterpret_cast<const char*>((uintptr_t)mBuffer + header_bytes);

    // 确保 data_ptr 在可读映射中
    if (!addr_is_readable(data_ptr)) {
        l_debug("data_ptr %p not readable -> return '.'", data_ptr);
        return _ZN7android7String8C1EPKc(".");
    }

    // 读取字符串（受限于 mSize）
    size_t to_read = (mSize < MAX_ALLOWED_STRING) ? mSize : MAX_ALLOWED_STRING;
    // 临时缓冲，多分配一字节以保证以 '\0' 结束
    char* tmp = (char*)malloc(to_read + 1);
    if (!tmp) {
        l_debug("malloc fail -> return '.'");
        return _ZN7android7String8C1EPKc(".");
    }
    if (!safe_read(tmp, data_ptr, to_read)) {
        l_debug("failed to read string bytes -> free & return '.'");
        free(tmp);
        return _ZN7android7String8C1EPKc(".");
    }
    tmp[to_read] = '\0';

    size_t actual_len = strnlen(tmp, to_read);

    std::string input(tmp, actual_len);
    free(tmp);

    l_debug("got string (len=%zu): %s", input.length(), input.c_str());

    size_t last_slash = input.find_last_of('/');
    std::string output;
    if (last_slash == std::string::npos) output = ".";
    else if (last_slash == 0) output = "/";
    else output = input.substr(0, last_slash);

    l_debug("getPathDir -> '%s'", output.c_str());
    // 返回一个新构造的 String8
    return _ZN7android7String8C1EPKc(output.c_str());
}