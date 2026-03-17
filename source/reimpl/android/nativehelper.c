#include <falso_jni/jni.h>

#include "utils/logger.h"

// <nativehelper/JNIHelp.h>
/**
 * 抛出指定类型的异常
 * @param env       JNI环境指针
 * @param className Java异常类的全名（如 "java/lang/NullPointerException"）
 * @param msg       异常消息
 */
void jniThrowException(JNIEnv *env, const char *className, const char *msg) {
    l_debug("[JNI] jniThrowException(env,%s,%s)", className, msg);
}

// <nativehelper/JNINativeMethod.h>
/**
 * 注册本地方法到指定的Java类
 * @param env        JNI环境指针
 * @param className  Java类的全名（如 "com/example/MyClass"）
 * @param methods    JNINativeMethod数组
 * @param numMethods 方法数组的大小
 * @return 成功返回0，失败返回负数
 */
int jniRegisterNativeMethods(JNIEnv                *env,
                             const char            *className,
                             const JNINativeMethod *methods,
                             int                    numMethods) {
    l_debug("[JNI] jniRegisterNativeMethods(env,%s,%p,%d):", className, methods, numMethods);
    for (int i = 0; i < numMethods; i++) {
        l_debug("   methods[%d]: %s,%s,%p", i, methods[i].name, methods[i].signature, methods[i].fnPtr);
    }

    return 0;
}
