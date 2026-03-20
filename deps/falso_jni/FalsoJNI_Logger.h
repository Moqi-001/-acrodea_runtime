/*
 * FalsoJNI_Logger.h
 *
 * Fake Java Native Interface, providing JavaVM and JNIEnv objects.
 *
 * Copyright (C) 2022-2023 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"

#ifndef FALSOJNI_LOGGER_H
#define FALSOJNI_LOGGER_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define fjni_log_dbg(...)  _fjni_log_debug(__VA_ARGS__)
#define fjni_log_info(...) _fjni_log_info(__VA_ARGS__)
#define fjni_log_warn(...) _fjni_log_warn(__VA_ARGS__)
#define fjni_log_err(...)  _fjni_log_error(__VA_ARGS__)

void _fjni_log_debug(const char *fmt, ...);
void _fjni_log_info(const char *fmt, ...);
void _fjni_log_warn(const char *fmt, ...);
void _fjni_log_error(const char *fmt, ...);

#ifdef __cplusplus
};
#endif

#endif  // FALSOJNI_LOGGER_H
