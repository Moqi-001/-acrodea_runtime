#ifndef JAVA_LANG_SYSTEM_H
#define JAVA_LANG_SYSTEM_H

#include <so_jni/so_jni.h>

void java_lang_System_class_init();

jlong JavaSystem_currentTimeMillis();

#endif