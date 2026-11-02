#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <so_common/so_shared_buffer.h>

#include "../jni.h"
#include "JavaClass.h"
#include "JavaTypes.h"

#include "JavaObject.h"

typedef struct JavaWeakRef {
    JavaObject         *ref_ptr;
    struct JavaWeakRef *next;
} JavaWeakRef;

typedef struct JavaObjectHeader {
    uint32_t     magic;
    JavaClass    clazz;
    jsize        size;
    JavaDtor     dtor;
    JavaWeakRef *weak_refs;
    uint32_t     _padding;  // 8字节对齐
} JavaObjectHeader;

#define JAVA_OBJECT_MAGIC    0x44957421  // "JavaObject".hashCode()
#define JAVA_OBJECT_CAST(_o) JAVA_BASE_CAST(JavaObjectHeader *, _o, JAVA_OBJECT_MAGIC)

#define objectFromHeader(_header) ((JavaObject)((_header) + 1))
#define headerFromObject(_obj)    JAVA_OBJECT_CAST(((JavaObjectHeader *)(_obj) - 1))

JavaObject _alloc_object(JavaClass clazz, jsize size, JavaDtor dtor) {
    if (size <= 0) return NULL;

    JavaObjectHeader *header = (JavaObjectHeader *)so_shared_buffer_alloc(sizeof(JavaObjectHeader) + size);
    if (!header) {
        return NULL;
    }

    JavaObject object = objectFromHeader(header);
    memset(object, 0, size);

    header->magic     = JAVA_OBJECT_MAGIC;
    header->clazz     = clazz;
    header->size      = size;
    header->dtor      = dtor;
    header->weak_refs = NULL;

    return object;
}

JavaObject JavaAllocObject(JavaClass clazz) {
    return _alloc_object(clazz, JavaGetClassInstanceSize(clazz), JavaGetClassDestructor(clazz));
}

JavaObject JavaAllocObjectNoClass(jsize size, JavaDtor dtor) {
    return _alloc_object(NULL, size, dtor);
}

JavaObject JavaRetainObject(JavaObject object) {
    if (!object) return NULL;

    JavaObjectHeader *header = headerFromObject(object);
    if (!header) return NULL;
    so_shared_buffer_retain(header);

    return object;
}

int32_t JavaReleaseObject(JavaObject object) {
    if (!object) return -1;

    JavaObjectHeader *header = headerFromObject(object);
    int32_t           prev   = so_shared_buffer_release_keep(header);
    if (prev == 1) {
        // 调用析构函数
        if (header->dtor) {
            header->dtor(object);
        }
        // 注销弱引用
        JavaWeakRef *node = header->weak_refs;
        while (node) {
            JavaWeakRef *next = node->next;
            if (node->ref_ptr) {
                *(node->ref_ptr) = NULL;
            }
            free(node);
            node = next;
        }
        // 释放内存
        so_shared_buffer_release_force(header);
    }

    return prev;
}

int32_t JavaReleaseObjectKeep(JavaObject object) {
    if (!object) return -1;

    JavaObjectHeader *header = headerFromObject(object);
    int32_t           prev   = so_shared_buffer_release_keep(header);

    return prev;
}

void JavaReleaseObjectForce(JavaObject object) {
    if (!object) return;

    JavaObjectHeader *header = headerFromObject(object);
    if (header->dtor) {
        header->dtor(object);
    }

    JavaWeakRef *node = header->weak_refs;
    while (node) {
        JavaWeakRef *next = node->next;
        if (node->ref_ptr) {
            *(node->ref_ptr) = NULL;
        }
        free(node);
        node = next;
    }

    so_shared_buffer_release_force(header);
}

jint JavaRegisterWeakRef(JavaObject object, JavaObject *ref_ptr) {
    if (!object || !ref_ptr) return JNI_ERR;

    JavaObjectHeader *header = headerFromObject(object);
    JavaWeakRef      *node   = (JavaWeakRef *)malloc(sizeof(JavaWeakRef));
    if (!node) return JNI_ERR;

    node->ref_ptr     = ref_ptr;
    node->next        = header->weak_refs;
    header->weak_refs = node;

    return JNI_OK;
}

void JavaUnregisterWeakRef(JavaObject object, JavaObject *ref_ptr) {
    if (!object || !ref_ptr) return;

    JavaObjectHeader *header = headerFromObject(object);
    JavaWeakRef      *prev   = NULL;
    JavaWeakRef      *node   = header->weak_refs;
    while (node) {
        if (node->ref_ptr == ref_ptr) {
            if (prev) {
                prev->next = node->next;
            } else {
                header->weak_refs = node->next;
            }
            free(node);
            return;
        }
        prev = node;
        node = node->next;
    }
}

JavaClass JavaGetObjectClass(JavaObject object) {
    if (!object) return 0;

    JavaObjectHeader *header = headerFromObject(object);

    return header->clazz;
}

jsize JavaGetObjectSize(JavaObject object) {
    if (!object) return 0;

    JavaObjectHeader *header = headerFromObject(object);

    return header->size;
}
