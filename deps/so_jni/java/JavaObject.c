#include <stdint.h>
#include <stdlib.h>

#include <so_common/so_shared_buffer.h>

#include "../jni.h"
#include "JavaClass.h"
#include "JavaTypes.h"

#include "JavaObject.h"

typedef struct JavaWeakRef {
    JObject            *ref_ptr;
    struct JavaWeakRef *next;
} JavaWeakRef;

typedef struct JavaObject {
    uint32_t     magic;
    JClass       clazz;
    jsize        size;
    JObjectDtor  dtor;
    JavaWeakRef *weak_refs;
} JavaObject;

#define JAVA_OBJECT_MAGIC    0x44957421  // "JavaObject".hashCode()
#define JAVA_OBJECT_CAST(_o) JAVA_BASE_CAST(JavaObject *, _o, JAVA_OBJECT_MAGIC)

#define dataFromObject(_obj)  ((JObject)((_obj) + 1))
#define objectFromData(_data) JAVA_OBJECT_CAST(((JavaObject *)(_data) - 1))

JObject _alloc_object(JClass clazz, jsize size, JObjectDtor dtor) {
    if (size <= 0) return NULL;

    JavaObject *obj = (JavaObject *)so_shared_buffer_alloc(sizeof(JavaObject) + size);
    if (!obj) {
        return NULL;
    }

    obj->magic     = JAVA_OBJECT_MAGIC;
    obj->clazz     = clazz;
    obj->size      = size;
    obj->dtor      = dtor;
    obj->weak_refs = NULL;

    return (JObject)dataFromObject(obj);
}

JObject JavaAllocObject(JClass clazz, JObjectDtor dtor) {
    return _alloc_object(clazz, JavaGetClassInstanceSize(clazz), dtor);
}

JObject JavaAllocObjectNoClass(jsize size, JObjectDtor dtor) {
    return _alloc_object(NULL, size, dtor);
}

JObject JavaRetainObject(JObject object) {
    if (!object) return NULL;

    JavaObject *obj = objectFromData(object);
    if (!obj) return NULL;
    so_shared_buffer_retain(obj);

    return object;
}

int32_t JavaReleaseObject(JObject object) {
    if (!object) return -1;

    JavaObject *obj  = objectFromData(object);
    int32_t     prev = so_shared_buffer_release_keep(obj);
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

int32_t JavaReleaseObjectKeep(JObject object) {
    if (!object) return -1;

    JavaObject *obj  = objectFromData(object);
    int32_t     prev = so_shared_buffer_release_keep(obj);

    return prev;
}

void JavaReleaseObjectForce(JObject object) {
    if (!object) return;

    JavaObject *obj = objectFromData(object);
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

jint JavaRegisterWeakRef(JObject object, JObject *ref_ptr) {
    if (!object || !ref_ptr) return JNI_ERR;

    JavaObject  *obj  = objectFromData(object);
    JavaWeakRef *node = (JavaWeakRef *)malloc(sizeof(JavaWeakRef));
    if (!node) return JNI_ERR;

    node->ref_ptr  = ref_ptr;
    node->next     = obj->weak_refs;
    obj->weak_refs = node;

    return JNI_OK;
}

void JavaUnregisterWeakRef(JObject object, JObject *ref_ptr) {
    if (!object || !ref_ptr) return;

    JavaObject  *obj  = objectFromData(object);
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

JClass JavaGetObjectClass(JObject object) {
    if (!object) return 0;

    JavaObject *obj = objectFromData(object);

    return obj->clazz;
}

jsize JavaGetObjectSize(JObject object) {
    if (!object) return 0;

    JavaObject *obj = objectFromData(object);

    return obj->size;
}
