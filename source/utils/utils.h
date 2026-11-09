/*
 * Copyright (C) 2021      Rinnegatamante
 * Copyright (C) 2022-2024 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

/**
 * @file  utils.h
 * @brief Common helper utilities.
 */

#ifndef SOLOADER_UTILS_H
#define SOLOADER_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

bool   file_copy(const char *path, const char *destination);
bool   file_exists(const char *path);
bool   file_load(const char *path, uint8_t **buffer, size_t *size);
bool   file_mkpath(const char *path, mode_t mode);
bool   file_save(const char *path, const uint8_t *buffer, size_t size);
size_t file_size(const char *path);

void str_replace(char **str, const char *needle, const char *replacement);
void str_remove(char *str, const char *needle);
bool str_starts_with(const char *str, const char *prefix);
bool str_ends_with(const char *str, const char *suffix);

#ifdef __cplusplus
};
#endif

#endif  // SOLOADER_UTILS_H
