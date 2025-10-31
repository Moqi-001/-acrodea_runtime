//
// Created by Lisen on 2025/10/31.
//
#include <string>
#include <stdio.h>
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

// 获取目录
// ex: /home/user/document.txt => /home/user/
void* _ZNK7android7String810getPathDirEv(void* self) {
    l_debug("_ZNK7android7String810getPathDirEv");

    String8Impl* str = static_cast<String8Impl*>(self);
    String8Impl* result = new String8Impl();

    if (str->data.empty()) {
        result->data = ".";
        return result;
    }

    // 找到最后一个路径分隔符
    size_t last_slash = str->data.find_last_of('/');

    if (last_slash == std::string::npos) {
        // 没有找到分隔符，返回当前目录
        result->data = ".";
    } else if (last_slash == 0) {
        // 根目录情况
        result->data = "/";
    } else {
        // 提取目录部分
        result->data = str->data.substr(0, last_slash);
    }

    return result;
}