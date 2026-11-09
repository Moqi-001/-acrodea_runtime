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

#ifndef _VITAGL_VITAGL_GLES2_GL2_H_H_
#define _VITAGL_VITAGL_GLES2_GL2_H_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <vitaGL/glplatform.h>
#ifndef HAVE_GL_HEADERS

#define GL_MAJOR_VERSION                      0x821B
#define GL_MINOR_VERSION                      0x821C
#define GL_NUM_EXTENSIONS                     0x821D
#define GL_QUERY_TARGET                       0x82EA
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED        0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE           0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE         0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE           0x8625
#define GL_CURRENT_VERTEX_ATTRIB              0x8626
#define GL_VERTEX_ATTRIB_ARRAY_POINTER        0x8645
#define GL_PROGRAM_ERROR_POSITION_ARB         0x864B
#define GL_PROGRAM_BINARY_LENGTH              0x8741
#define GL_NUM_PROGRAM_BINARY_FORMATS         0x87FE
#define GL_QUERY_RESULT                       0x8866
#define GL_QUERY_RESULT_AVAILABLE             0x8867
#define GL_MAX_VERTEX_ATTRIBS                 0x8869
#define GL_MAX_TEXTURE_IMAGE_UNITS            0x8872
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_CG_VERTEX_SHADER_EXT               0x890E
#define GL_CG_FRAGMENT_SHADER_EXT             0x890F
#define GL_SAMPLES_PASSED                     0x8914
#define GL_SAMPLER_BINDING                    0x8919
#define GL_UNIFORM_BUFFER                     0x8A11
#define GL_FRAGMENT_SHADER                    0x8B30
#define GL_VERTEX_SHADER                      0x8B31
#define GL_MAX_VARYING_FLOATS                 0x8B4B
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS    0x8B49
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS      0x8B4A
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS     0x8B4C
#define GL_SHADER_TYPE                        0x8B4F
#define GL_FLOAT_VEC2                         0x8B50
#define GL_FLOAT_VEC3                         0x8B51
#define GL_FLOAT_VEC4                         0x8B52
#define GL_INT_VEC2                           0x8B53
#define GL_INT_VEC3                           0x8B54
#define GL_INT_VEC4                           0x8B55
#define GL_FLOAT_MAT2                         0x8B5A
#define GL_FLOAT_MAT3                         0x8B5B
#define GL_FLOAT_MAT4                         0x8B5C
#define GL_SAMPLER_2D                         0x8B5E
#define GL_SAMPLER_CUBE                       0x8B60
#define GL_DELETE_STATUS                      0x8B80
#define GL_COMPILE_STATUS                     0x8B81
#define GL_LINK_STATUS                        0x8B82
#define GL_VALIDATE_STATUS                    0x8B83
#define GL_INFO_LOG_LENGTH                    0x8B84
#define GL_ATTACHED_SHADERS                   0x8B85
#define GL_ACTIVE_UNIFORMS                    0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH          0x8B87
#define GL_SHADER_SOURCE_LENGTH               0x8B88
#define GL_ACTIVE_ATTRIBUTES                  0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH        0x8B8A
#define GL_SHADING_LANGUAGE_VERSION           0x8B8C
#define GL_CURRENT_PROGRAM                    0x8B8D
#define GL_ANY_SAMPLES_PASSED                 0x8C2F
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS   0x8B4D
#define GL_ANY_SAMPLES_PASSED_CONSERVATIVE    0x8D6A
#define GL_SHADER_BINARY_FORMATS              0x8DF8
#define GL_NUM_SHADER_BINARY_FORMATS          0x8DF9
#define GL_SHADER_COMPILER                    0x8DFA
#define GL_MAX_VERTEX_UNIFORM_VECTORS         0x8DFB
#define GL_MAX_VARYING_VECTORS                0x8DFC
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS       0x8DFD
#define GL_QUERY_RESULT_NO_WAIT               0x9194

void      glAttachShader(GLuint prog, GLuint shad);
void      glBindAttribLocation(GLuint program, GLuint index, const GLchar *name);
void      glCompileShader(GLuint shader);
GLuint    glCreateProgram(void);
GLuint    glCreateShader(GLenum shaderType);
void      glDeleteProgram(GLuint prog);
void      glDeleteShader(GLuint shad);
void      glDisableVertexAttribArray(GLuint index);
void      glEnableVertexAttribArray(GLuint index);
void      glGetActiveAttrib(GLuint prog, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
void      glGetActiveUniform(GLuint prog, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
void      glGetAttachedShaders(GLuint prog, GLsizei maxCount, GLsizei *count, GLuint *shads);
GLint     glGetAttribLocation(GLuint prog, const GLchar *name);
void      glGetProgramBinary(GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
void      glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
void      glGetProgramiv(GLuint program, GLenum pname, GLint *params);
void      glGetShaderInfoLog(GLuint handle, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
void      glGetShaderiv(GLuint handle, GLenum pname, GLint *params);
void      glGetShaderSource(GLuint handle, GLsizei bufSize, GLsizei *length, GLchar *source);
GLuint    glGetUniformBlockIndex(GLuint prog, const GLchar *uniformBlockName);
GLint     glGetUniformLocation(GLuint prog, const GLchar *name);
void      glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat *params);
void      glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params);
void      glGetVertexAttribPointerv(GLuint index, GLenum pname, void **pointer);
GLboolean glIsProgram(GLuint program);
void      glLinkProgram(GLuint progr);
void      glProgramUniform1f(GLuint program, GLint location, GLfloat v0);
void      glProgramUniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
void      glProgramUniform1i(GLuint program, GLint location, GLint v0);
void      glProgramUniform1iv(GLuint program, GLint location, GLsizei count, const GLint *value);
void      glProgramUniform2f(GLuint program, GLint location, GLfloat v0, GLfloat v1);
void      glProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
void      glProgramUniform2i(GLuint program, GLint location, GLint v0, GLint v1);
void      glProgramUniform2iv(GLuint program, GLint location, GLsizei count, const GLint *value);
void      glProgramUniform3f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
void      glProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
void      glProgramUniform3i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
void      glProgramUniform3iv(GLuint program, GLint location, GLsizei count, const GLint *value);
void      glProgramUniform4f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
void      glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
void      glProgramUniform4i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
void      glProgramUniform4iv(GLuint program, GLint location, GLsizei count, const GLint *value);
void      glProgramUniformMatrix2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void      glProgramUniformMatrix3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void      glProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void      glReleaseShaderCompiler(void);
void      glShaderBinary(GLsizei count, const GLuint *handles, GLenum binaryFormat, const void *binary, GLsizei length);
void      glShaderSource(GLuint handle, GLsizei count, const GLchar *const *string, const GLint *length);
void      glUniform1f(GLint location, GLfloat v0);
void      glUniform1fv(GLint location, GLsizei count, const GLfloat *value);
void      glUniform1i(GLint location, GLint v0);
void      glUniform1iv(GLint location, GLsizei count, const GLint *value);
void      glUniform2f(GLint location, GLfloat v0, GLfloat v1);
void      glUniform2fv(GLint location, GLsizei count, const GLfloat *value);
void      glUniform2i(GLint location, GLint v0, GLint v1);
void      glUniform2iv(GLint location, GLsizei count, const GLint *value);
void      glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
void      glUniform3fv(GLint location, GLsizei count, const GLfloat *value);
void      glUniform3i(GLint location, GLint v0, GLint v1, GLint v2);
void      glUniform3iv(GLint location, GLsizei count, const GLint *value);
void      glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
void      glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
void      glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
void      glUniform4iv(GLint location, GLsizei count, const GLint *value);
void      glUniformBlockBinding(GLuint prog, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
void      glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void      glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void      glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void      glUseProgram(GLuint program);
void      glVertexAttribDivisor(GLuint index, GLuint divisor);
void      glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

#endif  // HAVE_GL_HEADERS

#ifdef __cplusplus
}
#endif

#endif  // _VITAGL_VITAGL_GLES2_GL2_H_H_
