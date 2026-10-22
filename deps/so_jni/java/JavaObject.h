#ifndef SO_JAVA_OBJECT_H
#define SO_JAVA_OBJECT_H

#include "../jni.h"
#include "JavaTypes.h"

JavaObject JavaAllocObject(JavaClass clazz);
JavaObject JavaAllocObjectNoClass(jsize size, JavaDtor dtor);
JavaObject JavaRetainObject(JavaObject object);
int32_t    JavaReleaseObject(JavaObject object);
int32_t    JavaReleaseObjectKeep(JavaObject object);
void       JavaReleaseObjectForce(JavaObject object);

jint JavaRegisterWeakRef(JavaObject object, JavaObject *ref_ptr);
void JavaUnregisterWeakRef(JavaObject object, JavaObject *ref_ptr);

JavaClass JavaGetObjectClass(JavaObject object);
jsize     JavaGetObjectSize(JavaObject object);

#endif