/*
 *  Copyright (C) 2016 Simon Boyé
 *
 *  This file is part of lair.
 *
 *  lair is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  lair is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lair.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef _LAIR_RENDER_GL2_CONTEXT_H
#define _LAIR_RENDER_GL2_CONTEXT_H


#include <KHR/khrplatform.h>

#include <lair/core/lair.h>
#include <lair/core/log.h>


namespace lair {


typedef khronos_int8_t GLbyte;
typedef khronos_float_t GLclampf;
typedef khronos_int32_t GLfixed;
typedef short GLshort;
typedef unsigned short GLushort;
typedef void GLvoid;
typedef struct __GLsync *GLsync;
typedef khronos_int64_t GLint64;
typedef khronos_uint64_t GLuint64;
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef char GLchar;
typedef khronos_float_t GLfloat;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_intptr_t GLintptr;
typedef unsigned int GLbitfield;
typedef int GLint;
typedef unsigned char GLboolean;
typedef int GLsizei;
typedef khronos_uint8_t GLubyte;


namespace gl {
enum GL {
	FALSE                                              = 0,
	NO_ERROR                                           = 0,
	POINTS                                             = 0x0000,
	NONE                                               = 0,
	ZERO                                               = 0,
	TRUE                                               = 1,
	ONE                                                = 1,
	LINES                                              = 0x0001,
	LINE_LOOP                                          = 0x0002,
	LINE_STRIP                                         = 0x0003,
	TRIANGLES                                          = 0x0004,
	TRIANGLE_STRIP                                     = 0x0005,
	TRIANGLE_FAN                                       = 0x0006,
	DEPTH_BUFFER_BIT                                   = 0x00000100,
	NEVER                                              = 0x0200,
	LESS                                               = 0x0201,
	EQUAL                                              = 0x0202,
	LEQUAL                                             = 0x0203,
	GREATER                                            = 0x0204,
	NOTEQUAL                                           = 0x0205,
	GEQUAL                                             = 0x0206,
	ALWAYS                                             = 0x0207,
	SRC_COLOR                                          = 0x0300,
	ONE_MINUS_SRC_COLOR                                = 0x0301,
	SRC_ALPHA                                          = 0x0302,
	ONE_MINUS_SRC_ALPHA                                = 0x0303,
	DST_ALPHA                                          = 0x0304,
	ONE_MINUS_DST_ALPHA                                = 0x0305,
	DST_COLOR                                          = 0x0306,
	ONE_MINUS_DST_COLOR                                = 0x0307,
	SRC_ALPHA_SATURATE                                 = 0x0308,
	STENCIL_BUFFER_BIT                                 = 0x00000400,
	FRONT                                              = 0x0404,
	BACK                                               = 0x0405,
	FRONT_AND_BACK                                     = 0x0408,
	INVALID_ENUM                                       = 0x0500,
	INVALID_VALUE                                      = 0x0501,
	INVALID_OPERATION                                  = 0x0502,
	OUT_OF_MEMORY                                      = 0x0505,
	INVALID_FRAMEBUFFER_OPERATION                      = 0x0506,
	CW                                                 = 0x0900,
	CCW                                                = 0x0901,
	LINE_WIDTH                                         = 0x0B21,
	CULL_FACE                                          = 0x0B44,
	CULL_FACE_MODE                                     = 0x0B45,
	FRONT_FACE                                         = 0x0B46,
	DEPTH_RANGE                                        = 0x0B70,
	DEPTH_TEST                                         = 0x0B71,
	DEPTH_WRITEMASK                                    = 0x0B72,
	DEPTH_CLEAR_VALUE                                  = 0x0B73,
	DEPTH_FUNC                                         = 0x0B74,
	STENCIL_TEST                                       = 0x0B90,
	STENCIL_CLEAR_VALUE                                = 0x0B91,
	STENCIL_FUNC                                       = 0x0B92,
	STENCIL_VALUE_MASK                                 = 0x0B93,
	STENCIL_FAIL                                       = 0x0B94,
	STENCIL_PASS_DEPTH_FAIL                            = 0x0B95,
	STENCIL_PASS_DEPTH_PASS                            = 0x0B96,
	STENCIL_REF                                        = 0x0B97,
	STENCIL_WRITEMASK                                  = 0x0B98,
	VIEWPORT                                           = 0x0BA2,
	DITHER                                             = 0x0BD0,
	BLEND                                              = 0x0BE2,
	SCISSOR_BOX                                        = 0x0C10,
	SCISSOR_TEST                                       = 0x0C11,
	COLOR_CLEAR_VALUE                                  = 0x0C22,
	COLOR_WRITEMASK                                    = 0x0C23,
	UNPACK_ALIGNMENT                                   = 0x0CF5,
	PACK_ALIGNMENT                                     = 0x0D05,
	MAX_TEXTURE_SIZE                                   = 0x0D33,
	MAX_VIEWPORT_DIMS                                  = 0x0D3A,
	SUBPIXEL_BITS                                      = 0x0D50,
	RED_BITS                                           = 0x0D52,
	GREEN_BITS                                         = 0x0D53,
	BLUE_BITS                                          = 0x0D54,
	ALPHA_BITS                                         = 0x0D55,
	DEPTH_BITS                                         = 0x0D56,
	STENCIL_BITS                                       = 0x0D57,
	TEXTURE_2D                                         = 0x0DE1,
	DONT_CARE                                          = 0x1100,
	FASTEST                                            = 0x1101,
	NICEST                                             = 0x1102,
	BYTE                                               = 0x1400,
	UNSIGNED_BYTE                                      = 0x1401,
	SHORT                                              = 0x1402,
	UNSIGNED_SHORT                                     = 0x1403,
	INT                                                = 0x1404,
	UNSIGNED_INT                                       = 0x1405,
	FLOAT                                              = 0x1406,
	FIXED                                              = 0x140C,
	INVERT                                             = 0x150A,
	TEXTURE                                            = 0x1702,
	DEPTH_COMPONENT                                    = 0x1902,
	ALPHA                                              = 0x1906,
	RGB                                                = 0x1907,
	RGBA                                               = 0x1908,
	LUMINANCE                                          = 0x1909,
	LUMINANCE_ALPHA                                    = 0x190A,
	KEEP                                               = 0x1E00,
	REPLACE                                            = 0x1E01,
	INCR                                               = 0x1E02,
	DECR                                               = 0x1E03,
	VENDOR                                             = 0x1F00,
	RENDERER                                           = 0x1F01,
	VERSION                                            = 0x1F02,
	EXTENSIONS                                         = 0x1F03,
	NEAREST                                            = 0x2600,
	LINEAR                                             = 0x2601,
	NEAREST_MIPMAP_NEAREST                             = 0x2700,
	LINEAR_MIPMAP_NEAREST                              = 0x2701,
	NEAREST_MIPMAP_LINEAR                              = 0x2702,
	LINEAR_MIPMAP_LINEAR                               = 0x2703,
	TEXTURE_MAG_FILTER                                 = 0x2800,
	TEXTURE_MIN_FILTER                                 = 0x2801,
	TEXTURE_WRAP_S                                     = 0x2802,
	TEXTURE_WRAP_T                                     = 0x2803,
	REPEAT                                             = 0x2901,
	POLYGON_OFFSET_UNITS                               = 0x2A00,
	COLOR_BUFFER_BIT                                   = 0x00004000,
	CONSTANT_COLOR                                     = 0x8001,
	ONE_MINUS_CONSTANT_COLOR                           = 0x8002,
	CONSTANT_ALPHA                                     = 0x8003,
	ONE_MINUS_CONSTANT_ALPHA                           = 0x8004,
	BLEND_COLOR                                        = 0x8005,
	FUNC_ADD                                           = 0x8006,
	BLEND_EQUATION_RGB                                 = 0x8009,
	BLEND_EQUATION                                     = 0x8009,
	FUNC_SUBTRACT                                      = 0x800A,
	FUNC_REVERSE_SUBTRACT                              = 0x800B,
	UNSIGNED_SHORT_4_4_4_4                             = 0x8033,
	UNSIGNED_SHORT_5_5_5_1                             = 0x8034,
	POLYGON_OFFSET_FILL                                = 0x8037,
	POLYGON_OFFSET_FACTOR                              = 0x8038,
	RGBA4                                              = 0x8056,
	RGB5_A1                                            = 0x8057,
	TEXTURE_BINDING_2D                                 = 0x8069,
	SAMPLE_ALPHA_TO_COVERAGE                           = 0x809E,
	SAMPLE_COVERAGE                                    = 0x80A0,
	SAMPLE_BUFFERS                                     = 0x80A8,
	SAMPLES                                            = 0x80A9,
	SAMPLE_COVERAGE_VALUE                              = 0x80AA,
	SAMPLE_COVERAGE_INVERT                             = 0x80AB,
	BLEND_DST_RGB                                      = 0x80C8,
	BLEND_SRC_RGB                                      = 0x80C9,
	BLEND_DST_ALPHA                                    = 0x80CA,
	BLEND_SRC_ALPHA                                    = 0x80CB,
	CLAMP_TO_EDGE                                      = 0x812F,
	GENERATE_MIPMAP_HINT                               = 0x8192,
	DEPTH_COMPONENT16                                  = 0x81A5,
	UNSIGNED_SHORT_5_6_5                               = 0x8363,
	MIRRORED_REPEAT                                    = 0x8370,
	ALIASED_POINT_SIZE_RANGE                           = 0x846D,
	ALIASED_LINE_WIDTH_RANGE                           = 0x846E,
	TEXTURE0                                           = 0x84C0,
	TEXTURE1                                           = 0x84C1,
	TEXTURE2                                           = 0x84C2,
	TEXTURE3                                           = 0x84C3,
	TEXTURE4                                           = 0x84C4,
	TEXTURE5                                           = 0x84C5,
	TEXTURE6                                           = 0x84C6,
	TEXTURE7                                           = 0x84C7,
	TEXTURE8                                           = 0x84C8,
	TEXTURE9                                           = 0x84C9,
	TEXTURE10                                          = 0x84CA,
	TEXTURE11                                          = 0x84CB,
	TEXTURE12                                          = 0x84CC,
	TEXTURE13                                          = 0x84CD,
	TEXTURE14                                          = 0x84CE,
	TEXTURE15                                          = 0x84CF,
	TEXTURE16                                          = 0x84D0,
	TEXTURE17                                          = 0x84D1,
	TEXTURE18                                          = 0x84D2,
	TEXTURE19                                          = 0x84D3,
	TEXTURE20                                          = 0x84D4,
	TEXTURE21                                          = 0x84D5,
	TEXTURE22                                          = 0x84D6,
	TEXTURE23                                          = 0x84D7,
	TEXTURE24                                          = 0x84D8,
	TEXTURE25                                          = 0x84D9,
	TEXTURE26                                          = 0x84DA,
	TEXTURE27                                          = 0x84DB,
	TEXTURE28                                          = 0x84DC,
	TEXTURE29                                          = 0x84DD,
	TEXTURE30                                          = 0x84DE,
	TEXTURE31                                          = 0x84DF,
	ACTIVE_TEXTURE                                     = 0x84E0,
	MAX_RENDERBUFFER_SIZE                              = 0x84E8,
	INCR_WRAP                                          = 0x8507,
	DECR_WRAP                                          = 0x8508,
	TEXTURE_CUBE_MAP                                   = 0x8513,
	TEXTURE_BINDING_CUBE_MAP                           = 0x8514,
	TEXTURE_CUBE_MAP_POSITIVE_X                        = 0x8515,
	TEXTURE_CUBE_MAP_NEGATIVE_X                        = 0x8516,
	TEXTURE_CUBE_MAP_POSITIVE_Y                        = 0x8517,
	TEXTURE_CUBE_MAP_NEGATIVE_Y                        = 0x8518,
	TEXTURE_CUBE_MAP_POSITIVE_Z                        = 0x8519,
	TEXTURE_CUBE_MAP_NEGATIVE_Z                        = 0x851A,
	MAX_CUBE_MAP_TEXTURE_SIZE                          = 0x851C,
	VERTEX_ATTRIB_ARRAY_ENABLED                        = 0x8622,
	VERTEX_ATTRIB_ARRAY_SIZE                           = 0x8623,
	VERTEX_ATTRIB_ARRAY_STRIDE                         = 0x8624,
	VERTEX_ATTRIB_ARRAY_TYPE                           = 0x8625,
	CURRENT_VERTEX_ATTRIB                              = 0x8626,
	VERTEX_ATTRIB_ARRAY_POINTER                        = 0x8645,
	NUM_COMPRESSED_TEXTURE_FORMATS                     = 0x86A2,
	COMPRESSED_TEXTURE_FORMATS                         = 0x86A3,
	BUFFER_SIZE                                        = 0x8764,
	BUFFER_USAGE                                       = 0x8765,
	STENCIL_BACK_FUNC                                  = 0x8800,
	STENCIL_BACK_FAIL                                  = 0x8801,
	STENCIL_BACK_PASS_DEPTH_FAIL                       = 0x8802,
	STENCIL_BACK_PASS_DEPTH_PASS                       = 0x8803,
	BLEND_EQUATION_ALPHA                               = 0x883D,
	MAX_VERTEX_ATTRIBS                                 = 0x8869,
	VERTEX_ATTRIB_ARRAY_NORMALIZED                     = 0x886A,
	MAX_TEXTURE_IMAGE_UNITS                            = 0x8872,
	ARRAY_BUFFER                                       = 0x8892,
	ELEMENT_ARRAY_BUFFER                               = 0x8893,
	ARRAY_BUFFER_BINDING                               = 0x8894,
	ELEMENT_ARRAY_BUFFER_BINDING                       = 0x8895,
	VERTEX_ATTRIB_ARRAY_BUFFER_BINDING                 = 0x889F,
	STREAM_DRAW                                        = 0x88E0,
	STATIC_DRAW                                        = 0x88E4,
	DYNAMIC_DRAW                                       = 0x88E8,
	FRAGMENT_SHADER                                    = 0x8B30,
	VERTEX_SHADER                                      = 0x8B31,
	MAX_VERTEX_TEXTURE_IMAGE_UNITS                     = 0x8B4C,
	MAX_COMBINED_TEXTURE_IMAGE_UNITS                   = 0x8B4D,
	SHADER_TYPE                                        = 0x8B4F,
	FLOAT_VEC2                                         = 0x8B50,
	FLOAT_VEC3                                         = 0x8B51,
	FLOAT_VEC4                                         = 0x8B52,
	INT_VEC2                                           = 0x8B53,
	INT_VEC3                                           = 0x8B54,
	INT_VEC4                                           = 0x8B55,
	BOOL                                               = 0x8B56,
	BOOL_VEC2                                          = 0x8B57,
	BOOL_VEC3                                          = 0x8B58,
	BOOL_VEC4                                          = 0x8B59,
	FLOAT_MAT2                                         = 0x8B5A,
	FLOAT_MAT3                                         = 0x8B5B,
	FLOAT_MAT4                                         = 0x8B5C,
	SAMPLER_2D                                         = 0x8B5E,
	SAMPLER_CUBE                                       = 0x8B60,
	DELETE_STATUS                                      = 0x8B80,
	COMPILE_STATUS                                     = 0x8B81,
	LINK_STATUS                                        = 0x8B82,
	VALIDATE_STATUS                                    = 0x8B83,
	INFO_LOG_LENGTH                                    = 0x8B84,
	ATTACHED_SHADERS                                   = 0x8B85,
	ACTIVE_UNIFORMS                                    = 0x8B86,
	ACTIVE_UNIFORM_MAX_LENGTH                          = 0x8B87,
	SHADER_SOURCE_LENGTH                               = 0x8B88,
	ACTIVE_ATTRIBUTES                                  = 0x8B89,
	ACTIVE_ATTRIBUTE_MAX_LENGTH                        = 0x8B8A,
	SHADING_LANGUAGE_VERSION                           = 0x8B8C,
	CURRENT_PROGRAM                                    = 0x8B8D,
	IMPLEMENTATION_COLOR_READ_TYPE                     = 0x8B9A,
	IMPLEMENTATION_COLOR_READ_FORMAT                   = 0x8B9B,
	STENCIL_BACK_REF                                   = 0x8CA3,
	STENCIL_BACK_VALUE_MASK                            = 0x8CA4,
	STENCIL_BACK_WRITEMASK                             = 0x8CA5,
	FRAMEBUFFER_BINDING                                = 0x8CA6,
	RENDERBUFFER_BINDING                               = 0x8CA7,
	FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE                 = 0x8CD0,
	FRAMEBUFFER_ATTACHMENT_OBJECT_NAME                 = 0x8CD1,
	FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL               = 0x8CD2,
	FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE       = 0x8CD3,
	FRAMEBUFFER_COMPLETE                               = 0x8CD5,
	FRAMEBUFFER_INCOMPLETE_ATTACHMENT                  = 0x8CD6,
	FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT          = 0x8CD7,
	FRAMEBUFFER_INCOMPLETE_DIMENSIONS                  = 0x8CD9,
	FRAMEBUFFER_UNSUPPORTED                            = 0x8CDD,
	COLOR_ATTACHMENT0                                  = 0x8CE0,
	DEPTH_ATTACHMENT                                   = 0x8D00,
	STENCIL_ATTACHMENT                                 = 0x8D20,
	FRAMEBUFFER                                        = 0x8D40,
	RENDERBUFFER                                       = 0x8D41,
	RENDERBUFFER_WIDTH                                 = 0x8D42,
	RENDERBUFFER_HEIGHT                                = 0x8D43,
	RENDERBUFFER_INTERNAL_FORMAT                       = 0x8D44,
	STENCIL_INDEX8                                     = 0x8D48,
	RENDERBUFFER_RED_SIZE                              = 0x8D50,
	RENDERBUFFER_GREEN_SIZE                            = 0x8D51,
	RENDERBUFFER_BLUE_SIZE                             = 0x8D52,
	RENDERBUFFER_ALPHA_SIZE                            = 0x8D53,
	RENDERBUFFER_DEPTH_SIZE                            = 0x8D54,
	RENDERBUFFER_STENCIL_SIZE                          = 0x8D55,
	RGB565                                             = 0x8D62,
	LOW_FLOAT                                          = 0x8DF0,
	MEDIUM_FLOAT                                       = 0x8DF1,
	HIGH_FLOAT                                         = 0x8DF2,
	LOW_INT                                            = 0x8DF3,
	MEDIUM_INT                                         = 0x8DF4,
	HIGH_INT                                           = 0x8DF5,
	SHADER_BINARY_FORMATS                              = 0x8DF8,
	NUM_SHADER_BINARY_FORMATS                          = 0x8DF9,
	SHADER_COMPILER                                    = 0x8DFA,
	MAX_VERTEX_UNIFORM_VECTORS                         = 0x8DFB,
	MAX_VARYING_VECTORS                                = 0x8DFC,
	MAX_FRAGMENT_UNIFORM_VECTORS                       = 0x8DFD
};
}


class Context {
//public:
//	typedef Eigen::Vector2i       Vector2i;
//	typedef Eigen::Vector4i       Vector4i;
//	typedef std::vector<int>      IntArray;
//	typedef std::vector<Vector4>  Vector4Array;

public:
	Context(Logger* logger);

	Context(const Context&) = default;
	Context(Context&&)      = default;
	~Context() = default;

	Context& operator=(const Context&) = default;
	Context& operator=(Context&&)      = default;

	bool initialize();
	void shutdown();

	inline void setLogCalls(bool enable) {
		_logCalls = enable;
	}

	Logger& log();

//	void clear();
//	void setFromGl();

	// Custom methods
	void setEnabled(GLenum cap, bool enabled);


	// Errors
	GLenum getError();

	// Buffer Objects
	void genBuffers(GLsizei n, GLuint* buffers);
	void deleteBuffers(GLsizei n, GLuint* buffers);
	void bindBuffer(GLenum target, GLuint buffer);
	void bufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
	void bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
	GLboolean isBuffer(GLuint buffer);
	void getBufferParameteriv(GLenum target, GLenum pname, GLint* params);

	// Viewport and Clipping
	void depthRangef(GLfloat n, GLfloat f);
	void viewport(GLint x, GLint y, GLsizei width, GLsizei height);

	// Reading Pixels
	void readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels);

	// Texturing
	void activeTexture(GLenum texture);
	void texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
	void copyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
	void texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
	void copyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	void compressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
	void compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
	void texParameterf(GLenum target, GLenum pname, GLfloat param);
	void texParameterfv(GLenum target, GLenum pname, GLfloat* params);
	void texParameteri(GLenum target, GLenum pname, GLint param);
	void texParameteriv(GLenum target, GLenum pname, GLint* params);
	void generateMipmap(GLenum target);
	void bindTexture(GLenum target, GLuint texture);
	void deleteTextures(GLsizei n, GLuint* textures);
	void genTextures(GLsizei n, GLuint* textures);
	void getTexParameterfv(GLenum target, GLenum pname, GLfloat* params);
	void getTexParameteriv(GLenum target, GLenum pname, GLint* params);
	GLboolean isTexture(GLuint texture);

	// Vertices
	void vertexAttrib1f(GLuint index, GLfloat x);
	void vertexAttrib1fv(GLuint index, GLfloat* v);
	void vertexAttrib2f(GLuint index, GLfloat x, GLfloat y);
	void vertexAttrib2fv(GLuint index, GLfloat* v);
	void vertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z);
	void vertexAttrib3fv(GLuint index, GLfloat* v);
	void vertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void vertexAttrib4fv(GLuint index, GLfloat* v);
	void vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
	void enableVertexAttribArray(GLuint index);
	void disableVertexAttribArray(GLuint index);
	void drawArrays(GLenum mode, GLint first, GLsizei count);
	void drawElements(GLenum mode, GLsizei count, GLenum type, const void* indices);

	// Rasterization
	void lineWidth(GLfloat width);
	void frontFace(GLenum mode);
	void cullFace(GLenum mode);
	void polygonOffset(GLfloat factor, GLfloat units);

	// Pixel Rectangles
	void pixelStorei(GLenum pname, GLint param);

	// Shaders and Programs
	GLuint createShader(GLenum type);
	void shaderSource(GLuint shader, GLsizei count, GLchar* const* string, GLint* length);
	void compileShader(GLuint shader);
	void releaseShaderCompiler();
	void deleteShader(GLuint shader);
	void shaderBinary(GLsizei count, GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length);
	GLuint createProgram();
	void attachShader(GLuint program, GLuint shader);
	void detachShader(GLuint program, GLuint shader);
	void linkProgram(GLuint program);
	void useProgram(GLuint program);
	void deleteProgram(GLuint program);
	void getActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
	GLint getAttribLocation(GLuint program, GLchar* name);
	void bindAttribLocation(GLuint program, GLuint index, GLchar* name);
	GLint getUniformLocation(GLuint program, GLchar* name);
	void getActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
	void uniform1f(GLint location, GLfloat v0);
	void uniform1fv(GLint location, GLsizei count, GLfloat* value);
	void uniform1i(GLint location, GLint v0);
	void uniform1iv(GLint location, GLsizei count, GLint* value);
	void uniform2f(GLint location, GLfloat v0, GLfloat v1);
	void uniform2fv(GLint location, GLsizei count, GLfloat* value);
	void uniform2i(GLint location, GLint v0, GLint v1);
	void uniform2iv(GLint location, GLsizei count, GLint* value);
	void uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	void uniform3fv(GLint location, GLsizei count, GLfloat* value);
	void uniform3i(GLint location, GLint v0, GLint v1, GLint v2);
	void uniform3iv(GLint location, GLsizei count, GLint* value);
	void uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	void uniform4fv(GLint location, GLsizei count, GLfloat* value);
	void uniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
	void uniform4iv(GLint location, GLsizei count, GLint* value);
	void uniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, GLfloat* value);
	void uniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, GLfloat* value);
	void uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, GLfloat* value);
	void validateProgram(GLuint program);
	GLboolean isShader(GLuint shader);
	void getShaderiv(GLuint shader, GLenum pname, GLint* params);
	void getAttachedShaders(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders);
	void getShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
	void getShaderSource(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source);
	void getShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
	void getVertexAttribfv(GLuint index, GLenum pname, GLfloat* params);
	void getVertexAttribiv(GLuint index, GLenum pname, GLint* params);
	void getVertexAttribPointerv(GLuint index, GLenum pname, void** pointer);
	void getUniformfv(GLuint program, GLint location, GLfloat* params);
	void getUniformiv(GLuint program, GLint location, GLint* params);
	GLboolean isProgram(GLuint program);
	void getProgramiv(GLuint program, GLenum pname, GLint* params);
	void getProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);

	// Per-fragment Operations
	void scissor(GLint x, GLint y, GLsizei width, GLsizei height);
	void sampleCoverage(GLfloat value, GLboolean invert);
	void stencilFunc(GLenum func, GLint ref, GLuint mask);
	void stencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask);
	void stencilOp(GLenum fail, GLenum zfail, GLenum zpass);
	void stencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
	void depthFunc(GLenum func);
	void blendEquation(GLenum mode);
	void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
	void blendFunc(GLenum sfactor, GLenum dfactor);
	void blendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
	void blendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

	// Whole Framebuffer Operations
	void colorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
	void depthMask(GLboolean flag);
	void stencilMask(GLuint mask);
	void stencilMaskSeparate(GLenum face, GLuint mask);
	void clear(GLbitfield mask);
	void clearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void clearDepthf(GLfloat d);
	void clearStencil(GLint s);

	// Special Functions
	void flush();
	void finish();
	void hint(GLenum target, GLenum mode);

	// State and State Requests
	void enable(GLenum cap);
	void disable(GLenum cap);
	void getBooleanv(GLenum pname, GLboolean* data);
	void getIntegerv(GLenum pname, GLint* data);
	void getFloatv(GLenum pname, GLfloat* data);
	GLboolean isEnabled(GLenum cap);
	const GLubyte* getString(GLenum name);

	// Framebuffer Objects
	void bindFramebuffer(GLenum target, GLuint framebuffer);
	void deleteFramebuffers(GLsizei n, GLuint* framebuffers);
	void genFramebuffers(GLsizei n, GLuint* framebuffers);
	void bindRenderbuffer(GLenum target, GLuint renderbuffer);
	void deleteRenderbuffers(GLsizei n, GLuint* renderbuffers);
	void genRenderbuffers(GLsizei n, GLuint* renderbuffers);
	void renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	void framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	void framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	GLenum checkFramebufferStatus(GLenum target);
	GLboolean isFramebuffer(GLuint framebuffer);
	void getFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params);
	GLboolean isRenderbuffer(GLuint renderbuffer);
	void getRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params);

public:
//	// Vertex array data
//	IntArray  vertexAttribArrayEnabled;
//	IntArray  vertexAttribArraySize;
//	IntArray  vertexAttribArrayStride;
//	IntArray  vertexAttribArrayType;
//	IntArray  vertexAttribArrayNormalized;
//	//PtrArray  vertexAttribArrayPointer;
//	// TODO: pointers to buffers ?
//	int       arrayBufferBinding;
//	int       elementArrayBufferBinding;
//	IntArray  vertexAttribArrayBufferBinding;

//	// Buffer object state
//	// [see Buffer class]

//	// Transformation
//	Box2i     viewport;
//	Vector2   depthRange;

//	// Rasterization
//	Scalar    lineWidth;
//	bool      cullFace;
//	int       cullFaceMode;
//	int       frontFace;
//	Scalar    polygonOffsetFactor;
//	Scalar    polygonOffsetUnits;
//	bool      polygonOffsetFill;

//	// Multisampling
//	bool      sampleAlphaToCoverage;
//	bool      sampleCoverage;
//	Scalar    sampleCoverageValue;
//	bool      sampleCoverageInvert;

//	// Textures
//	// TODO: pointers to textures ?
//	IntArray  textureBinding2D;
//	IntArray  textureBindingCubeMap;
//	// [see Texture class]

//	// Texture environment and generation
//	int       activeTexture;

//	// Pixel operations
//	bool      scissorTest;
//	Box2i     scissorBox;
//	bool      stencilTest;
//	int       stencilFunc;
//	int       stencilValueMask;
//	int       stencilRef;
//	int       stencilFail;
//	int       stencilPassDepthFail;
//	int       stencilPassDepthPass;
//	int       stencilBackFunc;
//	int       stencilBackValueMask;
//	int       stencilBackRef;
//	int       stencilBackFail;
//	int       stencilBackPassDepthFail;
//	int       stencilBackPassDepthPass;
//	bool      depthTest;
//	int       depthFunc;
//	bool      blend;
//	int       blendSrcRgb;
//	int       blendSrcAlpha;
//	int       blendDstRgb;
//	int       blendDstAlpha;
//	int       blendEquationRgb;
//	int       blendEquationAlpha;
//	Vector4   blendColor;  // Missing from GLES2 state table ?
//	bool      dither;

//	// Framebuffer control
//	Vector4i  colorWritemask;
//	bool      depthWritemask;
//	int       stencilWritemask;
//	int       stencilBackWritemask;
//	Vector4   colorClearValue;
//	Scalar    depthClearValue;
//	int       stencilClearValue;

//	// Pixels
//	int       unpackAlignment;
//	int       packAlignment;

//	// Shader object states
//	// [see Shader class]

//	// Program object state
//	// TODO: pointer to shader ?
//	int       currentProgram;
//	// [see Program class]

//	// Vertex shader state
//	Vector4Array currentVertexAttrib;

//	// Hints
//	int       generateMipmapHint;

//	// Renderbuffer state
//	// TODO: pointer to renderbuffer ?
//	int       renderbufferBinding;
//	// [see Renderbuffer states]

//	// Framebuffer state
//	// TODO: pointer to framebuffer ?
//	int       framebufferBinding;
//	// [see Framebuffer class]

private:
	Logger*  _log;

	bool     _logCalls;
	LogLevel _logLevel;
};


}


#endif
