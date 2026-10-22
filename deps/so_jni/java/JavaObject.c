#include <stdint.h>
#include <stdlib.h>

#include <so_common/so_shared_buffer.h>

#include "../jni.h"
#include "JavaClass.h"
#include "JavaTypes.h"

#include "JavaObject.h"

typedef struct JavaWeakRef {
    JavaObject         *ref_ptr;
    struct JavaWeakRef *next;
} JavaWeakRef;

typedef struct _JavaObject {
    uint32_t     magic;
    JavaClass    clazz;
    jsize        size;
    JavaDtor     dtor;
    JavaWeakRef *weak_refs;
    uint32_t     _padding;  // 8字节对齐
} _JavaObject;

#define JAVA_OBJECT_MAGIC    0x44957421  // "JavaObject".hashCode()
#define JAVA_OBJECT_CAST(_o) JAVA_BASE_CAST(_JavaObject *, _o, JAVA_OBJECT_MAGIC)

#define dataFromObject(_obj)  ((JavaObject)((_obj) + 1))
#define objectFromData(_data) JAVA_OBJECT_CAST(((_JavaObject *)(_data) - 1))

JavaObject _alloc_object(JavaClass clazz, jsize size, JavaDtor dtor) {
    if (size <= 0) return NULL;

    _JavaObject *obj = (_JavaObject *)so_shared_buffer_alloc(sizeof(_JavaObject) + size);
    if (!obj) {
        return NULL;
    }

    obj->magic     = JAVA_OBJECT_MAGIC;
    obj->clazz     = clazz;
    obj->size      = size;
    obj->dtor      = dtor;
    obj->weak_refs = NULL;

    return (JavaObject)dataFromObject(obj);
}

JavaObject JavaAllocObject(JavaClass clazz) {
    return _alloc_object(clazz, JavaGetClassInstanceSize(clazz), JavaGetClassDestructor(clazz));
}

JavaObject JavaAllocObjectNoClass(jsize size, JavaDtor dtor) {
    return _alloc_object(NULL, size, dtor);
}

JavaObject JavaRetainObject(JavaObject object) {
    if (!object) return NULL;

    _JavaObject *obj = objectFromData(object);
    if (!obj) return NULL;
    so_shared_buffer_retain(obj);

    return object;
}

int32_t JavaReleaseObject(JavaObject object) {
    if (!object) return -1;

    _JavaObject *obj  = objectFromData(object);
    int32_t      prev = so_shared_buffer_release_keep(obj);
    if (prev == 1) {
        // 调用析构函数
        if (obj->dtor) {
            obj->dtor(object);
        }
        // 注销弱引用
        JavaWeakRef *node = obj->weak_refs;
        while (node) {
            JavaWeakRef *next = node->next;
            if (node->ref_ptr) {
                *(node->ref_ptr) = NULL;
            }
            free(node);
            node = next;
        }
        // 释放内存
        so_shared_buffer_release_force(obj);
    }

    return prev;
}

int32_t JavaReleaseObjectKeep(JavaObject object) {
    if (!object) return -1;

    _JavaObject *obj  = objectFromData(object);
    int32_t      prev = so_shared_buffer_release_keep(obj);

    return prev;
}

void JavaReleaseObjectForce(JavaObject object) {
    if (!object) return;

    _JavaObject *obj = objectFromData(object);
    if (obj->dtor) {
        obj->dtor(object);
    }

    JavaWeakRef *node = obj->weak_refs;
    while (node) {
        JavaWeakRef *next = node->next;
        if (node->ref_ptr) {
            *(node->ref_ptr) = NULL;
        }
        free(node);
        node = next;
    }

    so_shared_buffer_release_force(obj);
}

jint JavaRegisterWeakRef(JavaObject object, JavaObject *ref_ptr) {
    if (!object || !ref_ptr) return JNI_ERR;

    _JavaObject *obj  = objectFromData(object);
    JavaWeakRef *node = (JavaWeakRef *)malloc(sizeof(JavaWeakRef));
    if (!node) return JNI_ERR;

    node->ref_ptr  = ref_ptr;
    node->next     = obj->weak_refs;
    obj->weak_refs = node;

    return JNI_OK;
}

void JavaUnregisterWeakRef(JavaObject object, JavaObject *ref_ptr) {
    if (!object || !ref_ptr) return;

    _JavaObject *obj  = objectFromData(object);
    JavaWeakRef *prev = NULL;
    JavaWeakRef *node = obj->weak_refs;
    while (node) {
        if (node->ref_ptr == ref_ptr) {
            if (prev) {
                prev->next = node->next;
            } else {
                obj->weak_refs = node->next;
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

    _JavaObject *obj = objectFromData(object);

    return obj->clazz;
}

jsize JavaGetObjectSize(JavaObject object) {
    if (!object) return 0;

    _JavaObject *obj = objectFromData(object);

    return obj->size;
}
