/*
 * This file is part of vitaGL
 * Copyright 2017, 2018, 2019, 2020 Rinnegatamante
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _VITAGL_VITAGL_GLPLATFORM_H_H_
#define _VITAGL_VITAGL_GLPLATFORM_H_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#ifndef HAVE_GL_HEADERS

#define GLbitfield uint32_t
#define GLboolean  uint8_t
#define GLbyte     int8_t
#define GLubyte    uint8_t
#define GLchar     char
#define GLshort    int16_t
#define GLushort   uint16_t
#define GLint      int32_t
#define GLuint     uint32_t
#define GLfixed    int32_t
#define GLint64    int64_t
#define GLuint64   uint64_t
#define GLsizei    int32_t
#define GLenum     uint32_t
#define GLintptr   int32_t
#define GLsizeiptr uint32_t
#define GLsync     int32_t
#define GLfloat    float
#define GLclampf   float
#define GLdouble   double
#define GLclampd   double
#define GLvoid     void
#define GLclampx   int32_t

#define EGLBoolean int32_t
#define EGLDisplay void *
#define EGLenum    uint32_t
#define EGLSurface void *
#define EGLContext void *
#define EGLConfig  void *
#define EGLint     int32_t

#define EGLint64  int64_t
#define EGLuint64 uint64_t

#define NativeDisplayType void *

#define GL_FALSE  0
#define GL_TRUE   1
#define EGL_FALSE 0
#define EGL_TRUE  1

#define GL_NO_ERROR 0

#define GL_ZERO 0
#define GL_ONE  1

#define GL_NONE 0

#define GL_INVALID_INDEX 0xFFFFFFFF

#endif  // HAVE_GL_HEADERS

#ifdef __cplusplus
}
#endif

#endif  // _VITAGL_VITAGL_GLPLATFORM_H_H_
