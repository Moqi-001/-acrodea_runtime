#ifndef SO_JAVA_STRING_H
#define SO_JAVA_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../jni.h"
#include "JavaTypes.h"

JavaString   JavaNewString(const jchar *utf16, jsize len);
const jchar *JavaGetStringChars(JavaString string);
jsize        JavaGetStringLength(JavaString string);
jint         JavaGetStringRegion(JavaString string, jsize start, jsize length, jchar *dst);

JavaString  JavaNewStringUTF(const char *utf8);
const char *JavaGetStringUTFChars(JavaString string);
jsize       JavaGetStringUTFLength(JavaString string);
jint        JavaGetStringUTFRegion(JavaString string, jsize start, jsize length, char *dst);

#ifdef __cplusplus
}
#endif

#endif