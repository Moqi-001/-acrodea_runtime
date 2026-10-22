#include <string.h>

#include "../jni.h"
#include "JavaObject.h"
#include "JavaTypes.h"

#include "JavaArray.h"

typedef struct _JavaArray {
    uint32_t magic;
    jint     type;
    jsize    length;
    jsize    elem_size;
    void    *elems;
} _JavaArray;

#define JAVA_ARRAY_MAGIC    0x4388FD37  // "JavaArray".hashCode()
#define JAVA_ARRAY_CAST(_o) JAVA_BASE_CAST(_JavaArray *, _o, JAVA_ARRAY_MAGIC)

#define headerFromArray(_elems) (_JavaArray *)(((char *)(_elems) + JavaGetObjectSize(_elems)) - sizeof(_JavaArray))

#define ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))

static jsize _type_size(jint type) {
    switch (type) {
        case JAVA_TYPE_BOOLEAN:
            return sizeof(jboolean);
        case JAVA_TYPE_BYTE:
            return sizeof(jbyte);
        case JAVA_TYPE_CHAR:
            return sizeof(jchar);
        case JAVA_TYPE_SHORT:
            return sizeof(jshort);
        case JAVA_TYPE_INT:
            return sizeof(jint);
        case JAVA_TYPE_LONG:
            return sizeof(jlong);
        case JAVA_TYPE_FLOAT:
            return sizeof(jfloat);
        case JAVA_TYPE_DOUBLE:
            return sizeof(jdouble);
        case JAVA_TYPE_OBJECT:
            return sizeof(jobject);
        default:
            return 0;
    }
}

void _array_dtor(JavaArray arr) {
    _JavaArray *header = headerFromArray(arr);
    if (header->type == JAVA_TYPE_OBJECT) {
        JavaObject *elems = (JavaObject *)header->elems;
        for (int i = 0; i < header->length; i++) {
            if (elems[i]) {
                JavaReleaseObject(elems[i]);
            }
        }
    }
}

JavaArray JavaNewArray(jsize length, JavaType type) {
    jsize elem_size = _type_size(type);
    if (length <= 0 || elem_size <= 0) return NULL;

    jsize elems_size = elem_size * length;
    elems_size       = ALIGN_UP(elems_size, sizeof(uintptr_t));
    JavaArray array  = JavaAllocObjectNoClass(elems_size + sizeof(_JavaArray), (JavaDtor)_array_dtor);
    if (!array) return NULL;

    if (type == JAVA_TYPE_OBJECT) {
        memset(array, 0, elems_size);
    }

    _JavaArray *header = headerFromArray(array);
    header->magic      = JAVA_ARRAY_MAGIC;
    header->type       = type;
    header->length     = length;
    header->elem_size  = elem_size;
    header->elems      = array;

    return array;
}

JavaType JavaGetArrayType(JavaArray array) {
    if (!array) return JAVA_TYPE_UNKNOWN;

    _JavaArray *header = JAVA_ARRAY_CAST(headerFromArray(array));
    if (!header) return JAVA_TYPE_UNKNOWN;
    return header->type;
}

jsize JavaGetArrayLength(JavaArray array) {
    if (!array) return 0;

    _JavaArray *header = JAVA_ARRAY_CAST(headerFromArray(array));
    if (!header) return 0;
    return header->length;
}

jint JavaGetArrayRegion(JavaArray array, jsize start, jsize length, void *dst) {
    if (!array || !dst || start < 0 || length <= 0) return JNI_EINVAL;

    _JavaArray *header = JAVA_ARRAY_CAST(headerFromArray(array));
    if (!header) return JNI_ERR;
    if (start + length > header->length) return JNI_ERR;

    void *src = (char *)array + start * header->elem_size;
    memcpy(dst, src, length * header->elem_size);

    return JNI_OK;
}

jint JavaSetArrayRegion(JavaArray array, jsize start, jsize length, const void *src) {
    if (!array || !src || start < 0 || length <= 0) return JNI_EINVAL;

    _JavaArray *header = JAVA_ARRAY_CAST(headerFromArray(array));
    if (!header) return JNI_ERR;
    if (start + length > header->length) return JNI_ERR;

    void *dst = (char *)array + start * header->elem_size;
    if (src == dst) return JNI_OK;  // 无需自拷贝
    memcpy(dst, src, length * header->elem_size);

    return JNI_OK;
}
