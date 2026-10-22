#ifndef SO_JAVA_CLASS_H
#define SO_JAVA_CLASS_H

#include "../jni.h"
#include "JavaTypes.h"

enum {
    JAVA_SYMBOL_FLAGS_UNDEF  = 0,       // 未定义
    JAVA_SYMBOL_FLAGS_STATIC = 1 << 0,  // static
    JAVA_SYMBOL_FLAGS_NATIVE = 1 << 1,  // native（仅方法）
    JAVA_SYMBOL_FLAGS_METHOD = 1 << 2,  // 方法
    JAVA_SYMBOL_FLAGS_FIELD  = 1 << 3,  // 字段
};

typedef struct JavaSymbol {
    char     *name;       // 符号名
    char     *signature;  // 符号签名
    uintptr_t addr;       // 符号地址
    uint32_t  flags;      // 符号标志
    jmethodID method;     // 方法 ID
} JavaSymbol;

typedef struct JavaClassMeta {
    char       *name;           // 类名
    char       *super_name;     // 父类名
    JavaSymbol *symbols;        // 符号数组
    jsize       num_symbols;    // 符号数量
    jsize       instance_size;  // 实例数据大小
    JavaDtor    dtor;           // 析构函数
} JavaClassMeta;

JavaClass JavaCreateClass(JavaClassMeta *meta);
void      JavaClearClassRegister(void);

const JavaSymbol *JavaFindMethodSymbol(const JavaClass clazz, const char *name, const char *sig, jboolean is_static);
const JavaSymbol *JavaFindFieldSymbol(const JavaClass clazz, const char *name, const char *sig, jboolean is_static);
const JavaSymbol *JavaFindNativeSymbol(const JavaClass clazz, const char *name, const char *sig);
JavaClass         JavaFindClass(const char *name);
JavaClass         JavaGetSuperclass(JavaClass clazz);
jboolean          JavaIsAssignableFrom(JavaClass thiz, JavaClass from);  // JNI 里这个参数应该是搞反了，这里调整了，使用时要注意参数顺序
jboolean          JavaIsInstanceOf(JavaObject obj, JavaClass clazz);
jint              JavaRegisterNatives(JavaClass clazz, const JNINativeMethod *methods, jint num_methods);
jint              JavaUnregisterNatives(JavaClass clazz);

JavaClassMeta *JavaGetClassMeta(JavaClass clazz);
const char    *JavaGetClassName(JavaClass clazz);
jsize          JavaGetClassInstanceSize(JavaClass clazz);
JavaDtor       JavaGetClassDestructor(JavaClass clazz);

// ============================================
// JAVA 符号表定义宏 (简化 JavaSymbol 结构体初始化，免去手动填写标志位等)
// ============================================
#define JAVA_SYMBOL_STATIC_METHOD(name, sig, p_func) \
    {name, sig, (uintptr_t)(p_func), (JAVA_SYMBOL_FLAGS_STATIC | JAVA_SYMBOL_FLAGS_METHOD), NULL}
#define JAVA_SYMBOL_STATIC_FIELD(name, sig, offset) \
    {name, sig, (uintptr_t)(offset), (JAVA_SYMBOL_FLAGS_STATIC | JAVA_SYMBOL_FLAGS_FIELD), NULL}
#define JAVA_SYMBOL_METHOD(name, sig, p_func) \
    {name, sig, (uintptr_t)(p_func), JAVA_SYMBOL_FLAGS_METHOD, NULL}
#define JAVA_SYMBOL_FIELD(name, sig, offset) \
    {name, sig, (uintptr_t)(offset), JAVA_SYMBOL_FLAGS_FIELD, NULL}
#define JAVA_SYMBOL_NATIVE(name, sig, pp_func) \
    {name, sig, (uintptr_t)(pp_func), JAVA_SYMBOL_FLAGS_NATIVE | JAVA_SYMBOL_FLAGS_METHOD, NULL}
#define JAVA_SYMBOL_STATIC_NATIVE(name, sig, pp_func) \
    JAVA_SYMBOL_NATIVE(name, sig, pp_func)

#endif