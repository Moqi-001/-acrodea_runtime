#include "so_jni_internal.h"

#define NEW_ARRAY(_jtype, _length, _jtype_enum)                    \
    ({                                                             \
        JArray        arr    = JavaNewArray(_length, _jtype_enum); \
        _jtype##Array result = JniNewLocalRefFromJava(arr);        \
        JavaReleaseObject(arr);                                    \
        result;                                                    \
    })

#define GET_ARRAY_ELEMENTS(_jtype, _array, _isCopy) \
    ({                                              \
        _jtype *elems = JniObjectToJava(_array);    \
        if (!elems) return NULL;                    \
        if (_isCopy) *_isCopy = JNI_FALSE;          \
        elems;                                      \
    })

#define RELEASE_ARRAY_ELEMENTS(_array, _elems, _mode)

#define GET_ARRAY_REGION(_array, _start, _len, _buf) \
    JavaGetArrayRegion(JniObjectToJava(_array), _start, _len, (void *)_buf)

#define SET_ARRAY_REGION(_array, _start, _len, _buf) \
    JavaSetArrayRegion(JniObjectToJava(_array), _start, _len, (const void *)_buf)

jlong JniGetArrayLength(jarray array) {
    return JavaGetArrayLength(JniObjectToJava(array));
}

// ============ Object Array ============
jobjectArray JniNewObjectArray(jsize length, jclass elementClass, jobject initialElement) {
    if (length <= 0 || !elementClass) {
        return NULL;
    }

    JObject initElem = JniObjectToJava(initialElement);

    if (initElem && !JavaIsInstanceOf(initElem, JniObjectToJava(elementClass))) {
        return NULL;
    }

    JObject *arr = (JObject *)JavaNewArray(length, JAVA_TYPE_OBJECT);
    if (!arr) {
        return NULL;
    }

    for (jint i = 0; i < length; i++) {
        arr[i] = JavaRetainObject(initElem);
    }

    jobjectArray result = JniNewLocalRefFromJava(arr);
    JavaReleaseObject(arr);  // 交由 LocalRef 托管

    return result;
}

jobject JniGetObjectArrayElement(jobjectArray array, jsize index) {
    JObject *arr    = JniObjectToJava(array);
    jsize    length = JavaGetArrayLength(arr);
    if (!arr || index < 0 || index >= length) {
        return NULL;
    }

    return JniNewLocalRefFromJava(arr[index]);
}

void JniSetObjectArrayElement(jobjectArray array, jsize index, jobject value) {
    JObject *arr    = JniObjectToJava(array);
    jsize    length = JavaGetArrayLength(arr);
    if (!arr || index < 0 || index >= length) {
        return;
    }

    JObject val = JniObjectToJava(value);
    if (arr[index] == val) return;

    if (arr[index]) {
        JavaReleaseObject(arr[index]);
    }
    arr[index] = JavaRetainObject(val);
}

// ============ New Array ============
jbooleanArray JniNewBooleanArray(jsize length) {
    return NEW_ARRAY(jboolean, length, JAVA_TYPE_BOOLEAN);
}

jbyteArray JniNewByteArray(jsize length) {
    return NEW_ARRAY(jbyte, length, JAVA_TYPE_BYTE);
}

jcharArray JniNewCharArray(jsize length) {
    return NEW_ARRAY(jchar, length, JAVA_TYPE_CHAR);
}

jshortArray JniNewShortArray(jsize length) {
    return NEW_ARRAY(jshort, length, JAVA_TYPE_SHORT);
}

jintArray JniNewIntArray(jsize length) {
    return NEW_ARRAY(jint, length, JAVA_TYPE_INT);
}

jlongArray JniNewLongArray(jsize length) {
    return NEW_ARRAY(jlong, length, JAVA_TYPE_LONG);
}

jfloatArray JniNewFloatArray(jsize length) {
    return NEW_ARRAY(jfloat, length, JAVA_TYPE_FLOAT);
}

jdoubleArray JniNewDoubleArray(jsize length) {
    return NEW_ARRAY(jdouble, length, JAVA_TYPE_DOUBLE);
}

// ============ Get Array Elements ============
jboolean *JniGetBooleanArrayElements(jbooleanArray array, jboolean *isCopy) {
    return GET_ARRAY_ELEMENTS(jboolean, array, isCopy);
}

jbyte *JniGetByteArrayElements(jbyteArray array, jboolean *isCopy) {
    return GET_ARRAY_ELEMENTS(jbyte, array, isCopy);
}

jchar *JniGetCharArrayElements(jcharArray array, jboolean *isCopy) {
    return GET_ARRAY_ELEMENTS(jchar, array, isCopy);
}

jshort *JniGetShortArrayElements(jshortArray array, jboolean *isCopy) {
    return GET_ARRAY_ELEMENTS(jshort, array, isCopy);
}

jint *JniGetIntArrayElements(jintArray array, jboolean *isCopy) {
    return GET_ARRAY_ELEMENTS(jint, array, isCopy);
}

jlong *JniGetLongArrayElements(jlongArray array, jboolean *isCopy) {
    return GET_ARRAY_ELEMENTS(jlong, array, isCopy);
}

jfloat *JniGetFloatArrayElements(jfloatArray array, jboolean *isCopy) {
    return GET_ARRAY_ELEMENTS(jfloat, array, isCopy);
}

jdouble *JniGetDoubleArrayElements(jdoubleArray array, jboolean *isCopy) {
    return GET_ARRAY_ELEMENTS(jdouble, array, isCopy);
}

// ============ Release Array Elements ============
void JniReleaseBooleanArrayElements(jbooleanArray array, jboolean *elems, jint mode) {
    RELEASE_ARRAY_ELEMENTS(array, elems, mode);
}

void JniReleaseByteArrayElements(jbyteArray array, jbyte *elems, jint mode) {
    RELEASE_ARRAY_ELEMENTS(array, elems, mode);
}

void JniReleaseCharArrayElements(jcharArray array, jchar *elems, jint mode) {
    RELEASE_ARRAY_ELEMENTS(array, elems, mode);
}

void JniReleaseShortArrayElements(jshortArray array, jshort *elems, jint mode) {
    RELEASE_ARRAY_ELEMENTS(array, elems, mode);
}

void JniReleaseIntArrayElements(jintArray array, jint *elems, jint mode) {
    RELEASE_ARRAY_ELEMENTS(array, elems, mode);
}

void JniReleaseLongArrayElements(jlongArray array, jlong *elems, jint mode) {
    RELEASE_ARRAY_ELEMENTS(array, elems, mode);
}

void JniReleaseFloatArrayElements(jfloatArray array, jfloat *elems, jint mode) {
    RELEASE_ARRAY_ELEMENTS(array, elems, mode);
}

void JniReleaseDoubleArrayElements(jdoubleArray array, jdouble *elems, jint mode) {
    RELEASE_ARRAY_ELEMENTS(array, elems, mode);
}

// ============ Get Array Region ============
void JniGetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len, jboolean *buf) {
    GET_ARRAY_REGION(array, start, len, buf);
}

void JniGetByteArrayRegion(jbyteArray array, jsize start, jsize len, jbyte *buf) {
    GET_ARRAY_REGION(array, start, len, buf);
}

void JniGetCharArrayRegion(jcharArray array, jsize start, jsize len, jchar *buf) {
    GET_ARRAY_REGION(array, start, len, buf);
}

void JniGetShortArrayRegion(jshortArray array, jsize start, jsize len, jshort *buf) {
    GET_ARRAY_REGION(array, start, len, buf);
}

void JniGetIntArrayRegion(jintArray array, jsize start, jsize len, jint *buf) {
    GET_ARRAY_REGION(array, start, len, buf);
}

void JniGetLongArrayRegion(jlongArray array, jsize start, jsize len, jlong *buf) {
    GET_ARRAY_REGION(array, start, len, buf);
}

void JniGetFloatArrayRegion(jfloatArray array, jsize start, jsize len, jfloat *buf) {
    GET_ARRAY_REGION(array, start, len, buf);
}

void JniGetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len, jdouble *buf) {
    GET_ARRAY_REGION(array, start, len, buf);
}

// ============ Set Array Region ============
void JniSetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len, const jboolean *buf) {
    SET_ARRAY_REGION(array, start, len, buf);
}

void JniSetByteArrayRegion(jbyteArray array, jsize start, jsize len, const jbyte *buf) {
    SET_ARRAY_REGION(array, start, len, buf);
}

void JniSetCharArrayRegion(jcharArray array, jsize start, jsize len, const jchar *buf) {
    SET_ARRAY_REGION(array, start, len, buf);
}

void JniSetShortArrayRegion(jshortArray array, jsize start, jsize len, const jshort *buf) {
    SET_ARRAY_REGION(array, start, len, buf);
}

void JniSetIntArrayRegion(jintArray array, jsize start, jsize len, const jint *buf) {
    SET_ARRAY_REGION(array, start, len, buf);
}

void JniSetLongArrayRegion(jlongArray array, jsize start, jsize len, const jlong *buf) {
    SET_ARRAY_REGION(array, start, len, buf);
}

void JniSetFloatArrayRegion(jfloatArray array, jsize start, jsize len, const jfloat *buf) {
    SET_ARRAY_REGION(array, start, len, buf);
}

void JniSetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len, const jdouble *buf) {
    SET_ARRAY_REGION(array, start, len, buf);
}

// ============ Primitive Array Critical ============
void *JniGetPrimitiveArrayCritical(jarray array, jboolean *isCopy) {
    return GET_ARRAY_ELEMENTS(void, array, isCopy);
}

void JniReleasePrimitiveArrayCritical(jarray array, void *carray, jint mode) {
    RELEASE_ARRAY_ELEMENTS(array, carray, mode);
}
