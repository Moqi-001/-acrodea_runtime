/*
 * FalsoJNI_ImplBridge.h
 *
 * Fake Java Native Interface, providing JavaVM and JNIEnv objects.
 *
 * Copyright (C) 2021 Andy Nguyen
 * Copyright (C) 2021 Rinnegatamante
 * Copyright (C) 2022 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#ifndef FALSOJNI_ARRAY
#define FALSOJNI_ARRAY

#include "FalsoJNI_ImplBridge.h"
#include "jni.h"

jarray    *jarray_alloc(jsize len, FIELD_TYPE type);
jboolean   jarray_acquire(jarray *array);
jboolean   jarray_release(jarray *array);
FIELD_TYPE jarray_type(jarray *array);
void      *jarray_data(jarray *array);
jsize      jarray_length(jarray *array);

#endif