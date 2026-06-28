#ifndef SO_JAVA_STRING_H
#define SO_JAVA_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../jni.h"
#include "JavaTypes.h"

JString      JavaNewString(const jchar *utf16, jsize len);
const jchar *JavaGetStringChars(JString string);
jsize        JavaGetStringLength(JString string);
jint         JavaGetStringRegion(JString string, jsize start, jsize length, jchar *dst);

JString     JavaNewStringUTF(const char *utf8);
const char *JavaGetStringUTFChars(JString string);
jsize       JavaGetStringUTFLength(JString string);
jint        JavaGetStringUTFRegion(JString string, jsize start, jsize length, char *dst);

#ifdef __cplusplus
}
#endif

#endif