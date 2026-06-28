#ifndef SO_JAVA_OBJECT_H
#define SO_JAVA_OBJECT_H

#include "../jni.h"
#include "JavaTypes.h"

JObject JavaAllocObject(JClass clazz, JObjectDtor dtor);
JObject JavaAllocObjectNoClass(jsize size, JObjectDtor dtor);
JObject JavaRetainObject(JObject object);
int32_t JavaReleaseObject(JObject object);
int32_t JavaReleaseObjectKeep(JObject object);
void    JavaReleaseObjectForce(JObject object);

jint JavaRegisterWeakRef(JObject object, JObject *ref_ptr);
void JavaUnregisterWeakRef(JObject object, JObject *ref_ptr);

JClass JavaGetObjectClass(JObject object);
jsize  JavaGetObjectSize(JObject object);

#endif