#ifndef SOLOADER_JAVA_MY_SENSOR_H
#define SOLOADER_JAVA_MY_SENSOR_H

#include <falso_jni/FalsoJNI.h>

jint MySensor_setAccelerometer(jobject obj, jmethodID id, va_list args);
jint MySensor_checkType(jobject obj, jmethodID id, va_list args);
jint MySensor_setGyro(jobject obj, jmethodID id, va_list args);

#endif