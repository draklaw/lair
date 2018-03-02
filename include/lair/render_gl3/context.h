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

/*
 *  This file is generated automatically by gl_context.py.
 */


#ifndef _LAIR_RENDER_GL3_CONTEXT_H
#define _LAIR_RENDER_GL3_CONTEXT_H


#include <lair/core/lair.h>
#include <lair/core/log.h>


#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif


namespace lair {

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef char GLchar;
typedef unsigned short GLhalf;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
typedef int64_t GLint64;
typedef uint64_t GLuint64;
typedef struct __GLsync *GLsync;
typedef void ( *GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);

namespace gl {
enum GL {

	// GL_VERSION_1_0

	DEPTH_BUFFER_BIT                              = 0x00000100,
	STENCIL_BUFFER_BIT                            = 0x00000400,
	COLOR_BUFFER_BIT                              = 0x00004000,
	FALSE                                         = 0,
	TRUE                                          = 1,
	POINTS                                        = 0x0000,
	LINES                                         = 0x0001,
	LINE_LOOP                                     = 0x0002,
	LINE_STRIP                                    = 0x0003,
	TRIANGLES                                     = 0x0004,
	TRIANGLE_STRIP                                = 0x0005,
	TRIANGLE_FAN                                  = 0x0006,
	NEVER                                         = 0x0200,
	LESS                                          = 0x0201,
	EQUAL                                         = 0x0202,
	LEQUAL                                        = 0x0203,
	GREATER                                       = 0x0204,
	NOTEQUAL                                      = 0x0205,
	GEQUAL                                        = 0x0206,
	ALWAYS                                        = 0x0207,
	ZERO                                          = 0,
	ONE                                           = 1,
	SRC_COLOR                                     = 0x0300,
	ONE_MINUS_SRC_COLOR                           = 0x0301,
	SRC_ALPHA                                     = 0x0302,
	ONE_MINUS_SRC_ALPHA                           = 0x0303,
	DST_ALPHA                                     = 0x0304,
	ONE_MINUS_DST_ALPHA                           = 0x0305,
	DST_COLOR                                     = 0x0306,
	ONE_MINUS_DST_COLOR                           = 0x0307,
	SRC_ALPHA_SATURATE                            = 0x0308,
	NONE                                          = 0,
	FRONT_LEFT                                    = 0x0400,
	FRONT_RIGHT                                   = 0x0401,
	BACK_LEFT                                     = 0x0402,
	BACK_RIGHT                                    = 0x0403,
	FRONT                                         = 0x0404,
	BACK                                          = 0x0405,
	LEFT                                          = 0x0406,
	RIGHT                                         = 0x0407,
	FRONT_AND_BACK                                = 0x0408,
	NO_ERROR                                      = 0,
	INVALID_ENUM                                  = 0x0500,
	INVALID_VALUE                                 = 0x0501,
	INVALID_OPERATION                             = 0x0502,
	OUT_OF_MEMORY                                 = 0x0505,
	CW                                            = 0x0900,
	CCW                                           = 0x0901,
	POINT_SIZE                                    = 0x0B11,
	POINT_SIZE_RANGE                              = 0x0B12,
	POINT_SIZE_GRANULARITY                        = 0x0B13,
	LINE_SMOOTH                                   = 0x0B20,
	LINE_WIDTH                                    = 0x0B21,
	LINE_WIDTH_RANGE                              = 0x0B22,
	LINE_WIDTH_GRANULARITY                        = 0x0B23,
	POLYGON_MODE                                  = 0x0B40,
	POLYGON_SMOOTH                                = 0x0B41,
	CULL_FACE                                     = 0x0B44,
	CULL_FACE_MODE                                = 0x0B45,
	FRONT_FACE                                    = 0x0B46,
	DEPTH_RANGE                                   = 0x0B70,
	DEPTH_TEST                                    = 0x0B71,
	DEPTH_WRITEMASK                               = 0x0B72,
	DEPTH_CLEAR_VALUE                             = 0x0B73,
	DEPTH_FUNC                                    = 0x0B74,
	STENCIL_TEST                                  = 0x0B90,
	STENCIL_CLEAR_VALUE                           = 0x0B91,
	STENCIL_FUNC                                  = 0x0B92,
	STENCIL_VALUE_MASK                            = 0x0B93,
	STENCIL_FAIL                                  = 0x0B94,
	STENCIL_PASS_DEPTH_FAIL                       = 0x0B95,
	STENCIL_PASS_DEPTH_PASS                       = 0x0B96,
	STENCIL_REF                                   = 0x0B97,
	STENCIL_WRITEMASK                             = 0x0B98,
	VIEWPORT                                      = 0x0BA2,
	DITHER                                        = 0x0BD0,
	BLEND_DST                                     = 0x0BE0,
	BLEND_SRC                                     = 0x0BE1,
	BLEND                                         = 0x0BE2,
	LOGIC_OP_MODE                                 = 0x0BF0,
	DRAW_BUFFER                                   = 0x0C01,
	READ_BUFFER                                   = 0x0C02,
	SCISSOR_BOX                                   = 0x0C10,
	SCISSOR_TEST                                  = 0x0C11,
	COLOR_CLEAR_VALUE                             = 0x0C22,
	COLOR_WRITEMASK                               = 0x0C23,
	DOUBLEBUFFER                                  = 0x0C32,
	STEREO                                        = 0x0C33,
	LINE_SMOOTH_HINT                              = 0x0C52,
	POLYGON_SMOOTH_HINT                           = 0x0C53,
	UNPACK_SWAP_BYTES                             = 0x0CF0,
	UNPACK_LSB_FIRST                              = 0x0CF1,
	UNPACK_ROW_LENGTH                             = 0x0CF2,
	UNPACK_SKIP_ROWS                              = 0x0CF3,
	UNPACK_SKIP_PIXELS                            = 0x0CF4,
	UNPACK_ALIGNMENT                              = 0x0CF5,
	PACK_SWAP_BYTES                               = 0x0D00,
	PACK_LSB_FIRST                                = 0x0D01,
	PACK_ROW_LENGTH                               = 0x0D02,
	PACK_SKIP_ROWS                                = 0x0D03,
	PACK_SKIP_PIXELS                              = 0x0D04,
	PACK_ALIGNMENT                                = 0x0D05,
	MAX_TEXTURE_SIZE                              = 0x0D33,
	MAX_VIEWPORT_DIMS                             = 0x0D3A,
	SUBPIXEL_BITS                                 = 0x0D50,
	TEXTURE_1D                                    = 0x0DE0,
	TEXTURE_2D                                    = 0x0DE1,
	TEXTURE_WIDTH                                 = 0x1000,
	TEXTURE_HEIGHT                                = 0x1001,
	TEXTURE_BORDER_COLOR                          = 0x1004,
	DONT_CARE                                     = 0x1100,
	FASTEST                                       = 0x1101,
	NICEST                                        = 0x1102,
	BYTE                                          = 0x1400,
	UNSIGNED_BYTE                                 = 0x1401,
	SHORT                                         = 0x1402,
	UNSIGNED_SHORT                                = 0x1403,
	INT                                           = 0x1404,
	UNSIGNED_INT                                  = 0x1405,
	FLOAT                                         = 0x1406,
	CLEAR                                         = 0x1500,
	AND                                           = 0x1501,
	AND_REVERSE                                   = 0x1502,
	COPY                                          = 0x1503,
	AND_INVERTED                                  = 0x1504,
	NOOP                                          = 0x1505,
	XOR                                           = 0x1506,
	OR                                            = 0x1507,
	NOR                                           = 0x1508,
	EQUIV                                         = 0x1509,
	INVERT                                        = 0x150A,
	OR_REVERSE                                    = 0x150B,
	COPY_INVERTED                                 = 0x150C,
	OR_INVERTED                                   = 0x150D,
	NAND                                          = 0x150E,
	SET                                           = 0x150F,
	TEXTURE                                       = 0x1702,
	COLOR                                         = 0x1800,
	DEPTH                                         = 0x1801,
	STENCIL                                       = 0x1802,
	STENCIL_INDEX                                 = 0x1901,
	DEPTH_COMPONENT                               = 0x1902,
	RED                                           = 0x1903,
	GREEN                                         = 0x1904,
	BLUE                                          = 0x1905,
	ALPHA                                         = 0x1906,
	RGB                                           = 0x1907,
	RGBA                                          = 0x1908,
	POINT                                         = 0x1B00,
	LINE                                          = 0x1B01,
	FILL                                          = 0x1B02,
	KEEP                                          = 0x1E00,
	REPLACE                                       = 0x1E01,
	INCR                                          = 0x1E02,
	DECR                                          = 0x1E03,
	VENDOR                                        = 0x1F00,
	RENDERER                                      = 0x1F01,
	VERSION                                       = 0x1F02,
	EXTENSIONS                                    = 0x1F03,
	NEAREST                                       = 0x2600,
	LINEAR                                        = 0x2601,
	NEAREST_MIPMAP_NEAREST                        = 0x2700,
	LINEAR_MIPMAP_NEAREST                         = 0x2701,
	NEAREST_MIPMAP_LINEAR                         = 0x2702,
	LINEAR_MIPMAP_LINEAR                          = 0x2703,
	TEXTURE_MAG_FILTER                            = 0x2800,
	TEXTURE_MIN_FILTER                            = 0x2801,
	TEXTURE_WRAP_S                                = 0x2802,
	TEXTURE_WRAP_T                                = 0x2803,
	REPEAT                                        = 0x2901,

	// GL_VERSION_1_1

	COLOR_LOGIC_OP                                = 0x0BF2,
	POLYGON_OFFSET_UNITS                          = 0x2A00,
	POLYGON_OFFSET_POINT                          = 0x2A01,
	POLYGON_OFFSET_LINE                           = 0x2A02,
	POLYGON_OFFSET_FILL                           = 0x8037,
	POLYGON_OFFSET_FACTOR                         = 0x8038,
	TEXTURE_BINDING_1D                            = 0x8068,
	TEXTURE_BINDING_2D                            = 0x8069,
	TEXTURE_INTERNAL_FORMAT                       = 0x1003,
	TEXTURE_RED_SIZE                              = 0x805C,
	TEXTURE_GREEN_SIZE                            = 0x805D,
	TEXTURE_BLUE_SIZE                             = 0x805E,
	TEXTURE_ALPHA_SIZE                            = 0x805F,
	DOUBLE                                        = 0x140A,
	PROXY_TEXTURE_1D                              = 0x8063,
	PROXY_TEXTURE_2D                              = 0x8064,
	R3_G3_B2                                      = 0x2A10,
	RGB4                                          = 0x804F,
	RGB5                                          = 0x8050,
	RGB8                                          = 0x8051,
	RGB10                                         = 0x8052,
	RGB12                                         = 0x8053,
	RGB16                                         = 0x8054,
	RGBA2                                         = 0x8055,
	RGBA4                                         = 0x8056,
	RGB5_A1                                       = 0x8057,
	RGBA8                                         = 0x8058,
	RGB10_A2                                      = 0x8059,
	RGBA12                                        = 0x805A,
	RGBA16                                        = 0x805B,

	// GL_VERSION_1_2

	UNSIGNED_BYTE_3_3_2                           = 0x8032,
	UNSIGNED_SHORT_4_4_4_4                        = 0x8033,
	UNSIGNED_SHORT_5_5_5_1                        = 0x8034,
	UNSIGNED_INT_8_8_8_8                          = 0x8035,
	UNSIGNED_INT_10_10_10_2                       = 0x8036,
	TEXTURE_BINDING_3D                            = 0x806A,
	PACK_SKIP_IMAGES                              = 0x806B,
	PACK_IMAGE_HEIGHT                             = 0x806C,
	UNPACK_SKIP_IMAGES                            = 0x806D,
	UNPACK_IMAGE_HEIGHT                           = 0x806E,
	TEXTURE_3D                                    = 0x806F,
	PROXY_TEXTURE_3D                              = 0x8070,
	TEXTURE_DEPTH                                 = 0x8071,
	TEXTURE_WRAP_R                                = 0x8072,
	MAX_3D_TEXTURE_SIZE                           = 0x8073,
	UNSIGNED_BYTE_2_3_3_REV                       = 0x8362,
	UNSIGNED_SHORT_5_6_5                          = 0x8363,
	UNSIGNED_SHORT_5_6_5_REV                      = 0x8364,
	UNSIGNED_SHORT_4_4_4_4_REV                    = 0x8365,
	UNSIGNED_SHORT_1_5_5_5_REV                    = 0x8366,
	UNSIGNED_INT_8_8_8_8_REV                      = 0x8367,
	UNSIGNED_INT_2_10_10_10_REV                   = 0x8368,
	BGR                                           = 0x80E0,
	BGRA                                          = 0x80E1,
	MAX_ELEMENTS_VERTICES                         = 0x80E8,
	MAX_ELEMENTS_INDICES                          = 0x80E9,
	CLAMP_TO_EDGE                                 = 0x812F,
	TEXTURE_MIN_LOD                               = 0x813A,
	TEXTURE_MAX_LOD                               = 0x813B,
	TEXTURE_BASE_LEVEL                            = 0x813C,
	TEXTURE_MAX_LEVEL                             = 0x813D,
	SMOOTH_POINT_SIZE_RANGE                       = 0x0B12,
	SMOOTH_POINT_SIZE_GRANULARITY                 = 0x0B13,
	SMOOTH_LINE_WIDTH_RANGE                       = 0x0B22,
	SMOOTH_LINE_WIDTH_GRANULARITY                 = 0x0B23,
	ALIASED_LINE_WIDTH_RANGE                      = 0x846E,

	// GL_VERSION_1_3

	TEXTURE0                                      = 0x84C0,
	TEXTURE1                                      = 0x84C1,
	TEXTURE2                                      = 0x84C2,
	TEXTURE3                                      = 0x84C3,
	TEXTURE4                                      = 0x84C4,
	TEXTURE5                                      = 0x84C5,
	TEXTURE6                                      = 0x84C6,
	TEXTURE7                                      = 0x84C7,
	TEXTURE8                                      = 0x84C8,
	TEXTURE9                                      = 0x84C9,
	TEXTURE10                                     = 0x84CA,
	TEXTURE11                                     = 0x84CB,
	TEXTURE12                                     = 0x84CC,
	TEXTURE13                                     = 0x84CD,
	TEXTURE14                                     = 0x84CE,
	TEXTURE15                                     = 0x84CF,
	TEXTURE16                                     = 0x84D0,
	TEXTURE17                                     = 0x84D1,
	TEXTURE18                                     = 0x84D2,
	TEXTURE19                                     = 0x84D3,
	TEXTURE20                                     = 0x84D4,
	TEXTURE21                                     = 0x84D5,
	TEXTURE22                                     = 0x84D6,
	TEXTURE23                                     = 0x84D7,
	TEXTURE24                                     = 0x84D8,
	TEXTURE25                                     = 0x84D9,
	TEXTURE26                                     = 0x84DA,
	TEXTURE27                                     = 0x84DB,
	TEXTURE28                                     = 0x84DC,
	TEXTURE29                                     = 0x84DD,
	TEXTURE30                                     = 0x84DE,
	TEXTURE31                                     = 0x84DF,
	ACTIVE_TEXTURE                                = 0x84E0,
	MULTISAMPLE                                   = 0x809D,
	SAMPLE_ALPHA_TO_COVERAGE                      = 0x809E,
	SAMPLE_ALPHA_TO_ONE                           = 0x809F,
	SAMPLE_COVERAGE                               = 0x80A0,
	SAMPLE_BUFFERS                                = 0x80A8,
	SAMPLES                                       = 0x80A9,
	SAMPLE_COVERAGE_VALUE                         = 0x80AA,
	SAMPLE_COVERAGE_INVERT                        = 0x80AB,
	TEXTURE_CUBE_MAP                              = 0x8513,
	TEXTURE_BINDING_CUBE_MAP                      = 0x8514,
	TEXTURE_CUBE_MAP_POSITIVE_X                   = 0x8515,
	TEXTURE_CUBE_MAP_NEGATIVE_X                   = 0x8516,
	TEXTURE_CUBE_MAP_POSITIVE_Y                   = 0x8517,
	TEXTURE_CUBE_MAP_NEGATIVE_Y                   = 0x8518,
	TEXTURE_CUBE_MAP_POSITIVE_Z                   = 0x8519,
	TEXTURE_CUBE_MAP_NEGATIVE_Z                   = 0x851A,
	PROXY_TEXTURE_CUBE_MAP                        = 0x851B,
	MAX_CUBE_MAP_TEXTURE_SIZE                     = 0x851C,
	COMPRESSED_RGB                                = 0x84ED,
	COMPRESSED_RGBA                               = 0x84EE,
	TEXTURE_COMPRESSION_HINT                      = 0x84EF,
	TEXTURE_COMPRESSED_IMAGE_SIZE                 = 0x86A0,
	TEXTURE_COMPRESSED                            = 0x86A1,
	NUM_COMPRESSED_TEXTURE_FORMATS                = 0x86A2,
	COMPRESSED_TEXTURE_FORMATS                    = 0x86A3,
	CLAMP_TO_BORDER                               = 0x812D,

	// GL_VERSION_1_4

	BLEND_DST_RGB                                 = 0x80C8,
	BLEND_SRC_RGB                                 = 0x80C9,
	BLEND_DST_ALPHA                               = 0x80CA,
	BLEND_SRC_ALPHA                               = 0x80CB,
	POINT_FADE_THRESHOLD_SIZE                     = 0x8128,
	DEPTH_COMPONENT16                             = 0x81A5,
	DEPTH_COMPONENT24                             = 0x81A6,
	DEPTH_COMPONENT32                             = 0x81A7,
	MIRRORED_REPEAT                               = 0x8370,
	MAX_TEXTURE_LOD_BIAS                          = 0x84FD,
	TEXTURE_LOD_BIAS                              = 0x8501,
	INCR_WRAP                                     = 0x8507,
	DECR_WRAP                                     = 0x8508,
	TEXTURE_DEPTH_SIZE                            = 0x884A,
	TEXTURE_COMPARE_MODE                          = 0x884C,
	TEXTURE_COMPARE_FUNC                          = 0x884D,
	BLEND_COLOR                                   = 0x8005,
	BLEND_EQUATION                                = 0x8009,
	CONSTANT_COLOR                                = 0x8001,
	ONE_MINUS_CONSTANT_COLOR                      = 0x8002,
	CONSTANT_ALPHA                                = 0x8003,
	ONE_MINUS_CONSTANT_ALPHA                      = 0x8004,
	FUNC_ADD                                      = 0x8006,
	FUNC_REVERSE_SUBTRACT                         = 0x800B,
	FUNC_SUBTRACT                                 = 0x800A,
	MIN                                           = 0x8007,
	MAX                                           = 0x8008,

	// GL_VERSION_1_5

	BUFFER_SIZE                                   = 0x8764,
	BUFFER_USAGE                                  = 0x8765,
	QUERY_COUNTER_BITS                            = 0x8864,
	CURRENT_QUERY                                 = 0x8865,
	QUERY_RESULT                                  = 0x8866,
	QUERY_RESULT_AVAILABLE                        = 0x8867,
	ARRAY_BUFFER                                  = 0x8892,
	ELEMENT_ARRAY_BUFFER                          = 0x8893,
	ARRAY_BUFFER_BINDING                          = 0x8894,
	ELEMENT_ARRAY_BUFFER_BINDING                  = 0x8895,
	VERTEX_ATTRIB_ARRAY_BUFFER_BINDING            = 0x889F,
	READ_ONLY                                     = 0x88B8,
	WRITE_ONLY                                    = 0x88B9,
	READ_WRITE                                    = 0x88BA,
	BUFFER_ACCESS                                 = 0x88BB,
	BUFFER_MAPPED                                 = 0x88BC,
	BUFFER_MAP_POINTER                            = 0x88BD,
	STREAM_DRAW                                   = 0x88E0,
	STREAM_READ                                   = 0x88E1,
	STREAM_COPY                                   = 0x88E2,
	STATIC_DRAW                                   = 0x88E4,
	STATIC_READ                                   = 0x88E5,
	STATIC_COPY                                   = 0x88E6,
	DYNAMIC_DRAW                                  = 0x88E8,
	DYNAMIC_READ                                  = 0x88E9,
	DYNAMIC_COPY                                  = 0x88EA,
	SAMPLES_PASSED                                = 0x8914,
	SRC1_ALPHA                                    = 0x8589,

	// GL_VERSION_2_0

	BLEND_EQUATION_RGB                            = 0x8009,
	VERTEX_ATTRIB_ARRAY_ENABLED                   = 0x8622,
	VERTEX_ATTRIB_ARRAY_SIZE                      = 0x8623,
	VERTEX_ATTRIB_ARRAY_STRIDE                    = 0x8624,
	VERTEX_ATTRIB_ARRAY_TYPE                      = 0x8625,
	CURRENT_VERTEX_ATTRIB                         = 0x8626,
	VERTEX_PROGRAM_POINT_SIZE                     = 0x8642,
	VERTEX_ATTRIB_ARRAY_POINTER                   = 0x8645,
	STENCIL_BACK_FUNC                             = 0x8800,
	STENCIL_BACK_FAIL                             = 0x8801,
	STENCIL_BACK_PASS_DEPTH_FAIL                  = 0x8802,
	STENCIL_BACK_PASS_DEPTH_PASS                  = 0x8803,
	MAX_DRAW_BUFFERS                              = 0x8824,
	DRAW_BUFFER0                                  = 0x8825,
	DRAW_BUFFER1                                  = 0x8826,
	DRAW_BUFFER2                                  = 0x8827,
	DRAW_BUFFER3                                  = 0x8828,
	DRAW_BUFFER4                                  = 0x8829,
	DRAW_BUFFER5                                  = 0x882A,
	DRAW_BUFFER6                                  = 0x882B,
	DRAW_BUFFER7                                  = 0x882C,
	DRAW_BUFFER8                                  = 0x882D,
	DRAW_BUFFER9                                  = 0x882E,
	DRAW_BUFFER10                                 = 0x882F,
	DRAW_BUFFER11                                 = 0x8830,
	DRAW_BUFFER12                                 = 0x8831,
	DRAW_BUFFER13                                 = 0x8832,
	DRAW_BUFFER14                                 = 0x8833,
	DRAW_BUFFER15                                 = 0x8834,
	BLEND_EQUATION_ALPHA                          = 0x883D,
	MAX_VERTEX_ATTRIBS                            = 0x8869,
	VERTEX_ATTRIB_ARRAY_NORMALIZED                = 0x886A,
	MAX_TEXTURE_IMAGE_UNITS                       = 0x8872,
	FRAGMENT_SHADER                               = 0x8B30,
	VERTEX_SHADER                                 = 0x8B31,
	MAX_FRAGMENT_UNIFORM_COMPONENTS               = 0x8B49,
	MAX_VERTEX_UNIFORM_COMPONENTS                 = 0x8B4A,
	MAX_VARYING_FLOATS                            = 0x8B4B,
	MAX_VERTEX_TEXTURE_IMAGE_UNITS                = 0x8B4C,
	MAX_COMBINED_TEXTURE_IMAGE_UNITS              = 0x8B4D,
	SHADER_TYPE                                   = 0x8B4F,
	FLOAT_VEC2                                    = 0x8B50,
	FLOAT_VEC3                                    = 0x8B51,
	FLOAT_VEC4                                    = 0x8B52,
	INT_VEC2                                      = 0x8B53,
	INT_VEC3                                      = 0x8B54,
	INT_VEC4                                      = 0x8B55,
	BOOL                                          = 0x8B56,
	BOOL_VEC2                                     = 0x8B57,
	BOOL_VEC3                                     = 0x8B58,
	BOOL_VEC4                                     = 0x8B59,
	FLOAT_MAT2                                    = 0x8B5A,
	FLOAT_MAT3                                    = 0x8B5B,
	FLOAT_MAT4                                    = 0x8B5C,
	SAMPLER_1D                                    = 0x8B5D,
	SAMPLER_2D                                    = 0x8B5E,
	SAMPLER_3D                                    = 0x8B5F,
	SAMPLER_CUBE                                  = 0x8B60,
	SAMPLER_1D_SHADOW                             = 0x8B61,
	SAMPLER_2D_SHADOW                             = 0x8B62,
	DELETE_STATUS                                 = 0x8B80,
	COMPILE_STATUS                                = 0x8B81,
	LINK_STATUS                                   = 0x8B82,
	VALIDATE_STATUS                               = 0x8B83,
	INFO_LOG_LENGTH                               = 0x8B84,
	ATTACHED_SHADERS                              = 0x8B85,
	ACTIVE_UNIFORMS                               = 0x8B86,
	ACTIVE_UNIFORM_MAX_LENGTH                     = 0x8B87,
	SHADER_SOURCE_LENGTH                          = 0x8B88,
	ACTIVE_ATTRIBUTES                             = 0x8B89,
	ACTIVE_ATTRIBUTE_MAX_LENGTH                   = 0x8B8A,
	FRAGMENT_SHADER_DERIVATIVE_HINT               = 0x8B8B,
	SHADING_LANGUAGE_VERSION                      = 0x8B8C,
	CURRENT_PROGRAM                               = 0x8B8D,
	POINT_SPRITE_COORD_ORIGIN                     = 0x8CA0,
	LOWER_LEFT                                    = 0x8CA1,
	UPPER_LEFT                                    = 0x8CA2,
	STENCIL_BACK_REF                              = 0x8CA3,
	STENCIL_BACK_VALUE_MASK                       = 0x8CA4,
	STENCIL_BACK_WRITEMASK                        = 0x8CA5,

	// GL_VERSION_2_1

	PIXEL_PACK_BUFFER                             = 0x88EB,
	PIXEL_UNPACK_BUFFER                           = 0x88EC,
	PIXEL_PACK_BUFFER_BINDING                     = 0x88ED,
	PIXEL_UNPACK_BUFFER_BINDING                   = 0x88EF,
	FLOAT_MAT2x3                                  = 0x8B65,
	FLOAT_MAT2x4                                  = 0x8B66,
	FLOAT_MAT3x2                                  = 0x8B67,
	FLOAT_MAT3x4                                  = 0x8B68,
	FLOAT_MAT4x2                                  = 0x8B69,
	FLOAT_MAT4x3                                  = 0x8B6A,
	SRGB                                          = 0x8C40,
	SRGB8                                         = 0x8C41,
	SRGB_ALPHA                                    = 0x8C42,
	SRGB8_ALPHA8                                  = 0x8C43,
	COMPRESSED_SRGB                               = 0x8C48,
	COMPRESSED_SRGB_ALPHA                         = 0x8C49,

	// GL_VERSION_3_0

	COMPARE_REF_TO_TEXTURE                        = 0x884E,
	CLIP_DISTANCE0                                = 0x3000,
	CLIP_DISTANCE1                                = 0x3001,
	CLIP_DISTANCE2                                = 0x3002,
	CLIP_DISTANCE3                                = 0x3003,
	CLIP_DISTANCE4                                = 0x3004,
	CLIP_DISTANCE5                                = 0x3005,
	CLIP_DISTANCE6                                = 0x3006,
	CLIP_DISTANCE7                                = 0x3007,
	MAX_CLIP_DISTANCES                            = 0x0D32,
	MAJOR_VERSION                                 = 0x821B,
	MINOR_VERSION                                 = 0x821C,
	NUM_EXTENSIONS                                = 0x821D,
	CONTEXT_FLAGS                                 = 0x821E,
	COMPRESSED_RED                                = 0x8225,
	COMPRESSED_RG                                 = 0x8226,
	CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT           = 0x00000001,
	RGBA32F                                       = 0x8814,
	RGB32F                                        = 0x8815,
	RGBA16F                                       = 0x881A,
	RGB16F                                        = 0x881B,
	VERTEX_ATTRIB_ARRAY_INTEGER                   = 0x88FD,
	MAX_ARRAY_TEXTURE_LAYERS                      = 0x88FF,
	MIN_PROGRAM_TEXEL_OFFSET                      = 0x8904,
	MAX_PROGRAM_TEXEL_OFFSET                      = 0x8905,
	CLAMP_READ_COLOR                              = 0x891C,
	FIXED_ONLY                                    = 0x891D,
	MAX_VARYING_COMPONENTS                        = 0x8B4B,
	TEXTURE_1D_ARRAY                              = 0x8C18,
	PROXY_TEXTURE_1D_ARRAY                        = 0x8C19,
	TEXTURE_2D_ARRAY                              = 0x8C1A,
	PROXY_TEXTURE_2D_ARRAY                        = 0x8C1B,
	TEXTURE_BINDING_1D_ARRAY                      = 0x8C1C,
	TEXTURE_BINDING_2D_ARRAY                      = 0x8C1D,
	R11F_G11F_B10F                                = 0x8C3A,
	UNSIGNED_INT_10F_11F_11F_REV                  = 0x8C3B,
	RGB9_E5                                       = 0x8C3D,
	UNSIGNED_INT_5_9_9_9_REV                      = 0x8C3E,
	TEXTURE_SHARED_SIZE                           = 0x8C3F,
	TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH         = 0x8C76,
	TRANSFORM_FEEDBACK_BUFFER_MODE                = 0x8C7F,
	MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS    = 0x8C80,
	TRANSFORM_FEEDBACK_VARYINGS                   = 0x8C83,
	TRANSFORM_FEEDBACK_BUFFER_START               = 0x8C84,
	TRANSFORM_FEEDBACK_BUFFER_SIZE                = 0x8C85,
	PRIMITIVES_GENERATED                          = 0x8C87,
	TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN         = 0x8C88,
	RASTERIZER_DISCARD                            = 0x8C89,
	MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS = 0x8C8A,
	MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS       = 0x8C8B,
	INTERLEAVED_ATTRIBS                           = 0x8C8C,
	SEPARATE_ATTRIBS                              = 0x8C8D,
	TRANSFORM_FEEDBACK_BUFFER                     = 0x8C8E,
	TRANSFORM_FEEDBACK_BUFFER_BINDING             = 0x8C8F,
	RGBA32UI                                      = 0x8D70,
	RGB32UI                                       = 0x8D71,
	RGBA16UI                                      = 0x8D76,
	RGB16UI                                       = 0x8D77,
	RGBA8UI                                       = 0x8D7C,
	RGB8UI                                        = 0x8D7D,
	RGBA32I                                       = 0x8D82,
	RGB32I                                        = 0x8D83,
	RGBA16I                                       = 0x8D88,
	RGB16I                                        = 0x8D89,
	RGBA8I                                        = 0x8D8E,
	RGB8I                                         = 0x8D8F,
	RED_INTEGER                                   = 0x8D94,
	GREEN_INTEGER                                 = 0x8D95,
	BLUE_INTEGER                                  = 0x8D96,
	RGB_INTEGER                                   = 0x8D98,
	RGBA_INTEGER                                  = 0x8D99,
	BGR_INTEGER                                   = 0x8D9A,
	BGRA_INTEGER                                  = 0x8D9B,
	SAMPLER_1D_ARRAY                              = 0x8DC0,
	SAMPLER_2D_ARRAY                              = 0x8DC1,
	SAMPLER_1D_ARRAY_SHADOW                       = 0x8DC3,
	SAMPLER_2D_ARRAY_SHADOW                       = 0x8DC4,
	SAMPLER_CUBE_SHADOW                           = 0x8DC5,
	UNSIGNED_INT_VEC2                             = 0x8DC6,
	UNSIGNED_INT_VEC3                             = 0x8DC7,
	UNSIGNED_INT_VEC4                             = 0x8DC8,
	INT_SAMPLER_1D                                = 0x8DC9,
	INT_SAMPLER_2D                                = 0x8DCA,
	INT_SAMPLER_3D                                = 0x8DCB,
	INT_SAMPLER_CUBE                              = 0x8DCC,
	INT_SAMPLER_1D_ARRAY                          = 0x8DCE,
	INT_SAMPLER_2D_ARRAY                          = 0x8DCF,
	UNSIGNED_INT_SAMPLER_1D                       = 0x8DD1,
	UNSIGNED_INT_SAMPLER_2D                       = 0x8DD2,
	UNSIGNED_INT_SAMPLER_3D                       = 0x8DD3,
	UNSIGNED_INT_SAMPLER_CUBE                     = 0x8DD4,
	UNSIGNED_INT_SAMPLER_1D_ARRAY                 = 0x8DD6,
	UNSIGNED_INT_SAMPLER_2D_ARRAY                 = 0x8DD7,
	QUERY_WAIT                                    = 0x8E13,
	QUERY_NO_WAIT                                 = 0x8E14,
	QUERY_BY_REGION_WAIT                          = 0x8E15,
	QUERY_BY_REGION_NO_WAIT                       = 0x8E16,
	BUFFER_ACCESS_FLAGS                           = 0x911F,
	BUFFER_MAP_LENGTH                             = 0x9120,
	BUFFER_MAP_OFFSET                             = 0x9121,
	DEPTH_COMPONENT32F                            = 0x8CAC,
	DEPTH32F_STENCIL8                             = 0x8CAD,
	FLOAT_32_UNSIGNED_INT_24_8_REV                = 0x8DAD,
	INVALID_FRAMEBUFFER_OPERATION                 = 0x0506,
	FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING         = 0x8210,
	FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE         = 0x8211,
	FRAMEBUFFER_ATTACHMENT_RED_SIZE               = 0x8212,
	FRAMEBUFFER_ATTACHMENT_GREEN_SIZE             = 0x8213,
	FRAMEBUFFER_ATTACHMENT_BLUE_SIZE              = 0x8214,
	FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE             = 0x8215,
	FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE             = 0x8216,
	FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE           = 0x8217,
	FRAMEBUFFER_DEFAULT                           = 0x8218,
	FRAMEBUFFER_UNDEFINED                         = 0x8219,
	DEPTH_STENCIL_ATTACHMENT                      = 0x821A,
	MAX_RENDERBUFFER_SIZE                         = 0x84E8,
	DEPTH_STENCIL                                 = 0x84F9,
	UNSIGNED_INT_24_8                             = 0x84FA,
	DEPTH24_STENCIL8                              = 0x88F0,
	TEXTURE_STENCIL_SIZE                          = 0x88F1,
	TEXTURE_RED_TYPE                              = 0x8C10,
	TEXTURE_GREEN_TYPE                            = 0x8C11,
	TEXTURE_BLUE_TYPE                             = 0x8C12,
	TEXTURE_ALPHA_TYPE                            = 0x8C13,
	TEXTURE_DEPTH_TYPE                            = 0x8C16,
	UNSIGNED_NORMALIZED                           = 0x8C17,
	FRAMEBUFFER_BINDING                           = 0x8CA6,
	DRAW_FRAMEBUFFER_BINDING                      = 0x8CA6,
	RENDERBUFFER_BINDING                          = 0x8CA7,
	READ_FRAMEBUFFER                              = 0x8CA8,
	DRAW_FRAMEBUFFER                              = 0x8CA9,
	READ_FRAMEBUFFER_BINDING                      = 0x8CAA,
	RENDERBUFFER_SAMPLES                          = 0x8CAB,
	FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE            = 0x8CD0,
	FRAMEBUFFER_ATTACHMENT_OBJECT_NAME            = 0x8CD1,
	FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL          = 0x8CD2,
	FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE  = 0x8CD3,
	FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER          = 0x8CD4,
	FRAMEBUFFER_COMPLETE                          = 0x8CD5,
	FRAMEBUFFER_INCOMPLETE_ATTACHMENT             = 0x8CD6,
	FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT     = 0x8CD7,
	FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER            = 0x8CDB,
	FRAMEBUFFER_INCOMPLETE_READ_BUFFER            = 0x8CDC,
	FRAMEBUFFER_UNSUPPORTED                       = 0x8CDD,
	MAX_COLOR_ATTACHMENTS                         = 0x8CDF,
	COLOR_ATTACHMENT0                             = 0x8CE0,
	COLOR_ATTACHMENT1                             = 0x8CE1,
	COLOR_ATTACHMENT2                             = 0x8CE2,
	COLOR_ATTACHMENT3                             = 0x8CE3,
	COLOR_ATTACHMENT4                             = 0x8CE4,
	COLOR_ATTACHMENT5                             = 0x8CE5,
	COLOR_ATTACHMENT6                             = 0x8CE6,
	COLOR_ATTACHMENT7                             = 0x8CE7,
	COLOR_ATTACHMENT8                             = 0x8CE8,
	COLOR_ATTACHMENT9                             = 0x8CE9,
	COLOR_ATTACHMENT10                            = 0x8CEA,
	COLOR_ATTACHMENT11                            = 0x8CEB,
	COLOR_ATTACHMENT12                            = 0x8CEC,
	COLOR_ATTACHMENT13                            = 0x8CED,
	COLOR_ATTACHMENT14                            = 0x8CEE,
	COLOR_ATTACHMENT15                            = 0x8CEF,
	COLOR_ATTACHMENT16                            = 0x8CF0,
	COLOR_ATTACHMENT17                            = 0x8CF1,
	COLOR_ATTACHMENT18                            = 0x8CF2,
	COLOR_ATTACHMENT19                            = 0x8CF3,
	COLOR_ATTACHMENT20                            = 0x8CF4,
	COLOR_ATTACHMENT21                            = 0x8CF5,
	COLOR_ATTACHMENT22                            = 0x8CF6,
	COLOR_ATTACHMENT23                            = 0x8CF7,
	COLOR_ATTACHMENT24                            = 0x8CF8,
	COLOR_ATTACHMENT25                            = 0x8CF9,
	COLOR_ATTACHMENT26                            = 0x8CFA,
	COLOR_ATTACHMENT27                            = 0x8CFB,
	COLOR_ATTACHMENT28                            = 0x8CFC,
	COLOR_ATTACHMENT29                            = 0x8CFD,
	COLOR_ATTACHMENT30                            = 0x8CFE,
	COLOR_ATTACHMENT31                            = 0x8CFF,
	DEPTH_ATTACHMENT                              = 0x8D00,
	STENCIL_ATTACHMENT                            = 0x8D20,
	FRAMEBUFFER                                   = 0x8D40,
	RENDERBUFFER                                  = 0x8D41,
	RENDERBUFFER_WIDTH                            = 0x8D42,
	RENDERBUFFER_HEIGHT                           = 0x8D43,
	RENDERBUFFER_INTERNAL_FORMAT                  = 0x8D44,
	STENCIL_INDEX1                                = 0x8D46,
	STENCIL_INDEX4                                = 0x8D47,
	STENCIL_INDEX8                                = 0x8D48,
	STENCIL_INDEX16                               = 0x8D49,
	RENDERBUFFER_RED_SIZE                         = 0x8D50,
	RENDERBUFFER_GREEN_SIZE                       = 0x8D51,
	RENDERBUFFER_BLUE_SIZE                        = 0x8D52,
	RENDERBUFFER_ALPHA_SIZE                       = 0x8D53,
	RENDERBUFFER_DEPTH_SIZE                       = 0x8D54,
	RENDERBUFFER_STENCIL_SIZE                     = 0x8D55,
	FRAMEBUFFER_INCOMPLETE_MULTISAMPLE            = 0x8D56,
	MAX_SAMPLES                                   = 0x8D57,
	FRAMEBUFFER_SRGB                              = 0x8DB9,
	HALF_FLOAT                                    = 0x140B,
	MAP_READ_BIT                                  = 0x0001,
	MAP_WRITE_BIT                                 = 0x0002,
	MAP_INVALIDATE_RANGE_BIT                      = 0x0004,
	MAP_INVALIDATE_BUFFER_BIT                     = 0x0008,
	MAP_FLUSH_EXPLICIT_BIT                        = 0x0010,
	MAP_UNSYNCHRONIZED_BIT                        = 0x0020,
	COMPRESSED_RED_RGTC1                          = 0x8DBB,
	COMPRESSED_SIGNED_RED_RGTC1                   = 0x8DBC,
	COMPRESSED_RG_RGTC2                           = 0x8DBD,
	COMPRESSED_SIGNED_RG_RGTC2                    = 0x8DBE,
	RG                                            = 0x8227,
	RG_INTEGER                                    = 0x8228,
	R8                                            = 0x8229,
	R16                                           = 0x822A,
	RG8                                           = 0x822B,
	RG16                                          = 0x822C,
	R16F                                          = 0x822D,
	R32F                                          = 0x822E,
	RG16F                                         = 0x822F,
	RG32F                                         = 0x8230,
	R8I                                           = 0x8231,
	R8UI                                          = 0x8232,
	R16I                                          = 0x8233,
	R16UI                                         = 0x8234,
	R32I                                          = 0x8235,
	R32UI                                         = 0x8236,
	RG8I                                          = 0x8237,
	RG8UI                                         = 0x8238,
	RG16I                                         = 0x8239,
	RG16UI                                        = 0x823A,
	RG32I                                         = 0x823B,
	RG32UI                                        = 0x823C,
	VERTEX_ARRAY_BINDING                          = 0x85B5,

	// GL_VERSION_3_1

	SAMPLER_2D_RECT                               = 0x8B63,
	SAMPLER_2D_RECT_SHADOW                        = 0x8B64,
	SAMPLER_BUFFER                                = 0x8DC2,
	INT_SAMPLER_2D_RECT                           = 0x8DCD,
	INT_SAMPLER_BUFFER                            = 0x8DD0,
	UNSIGNED_INT_SAMPLER_2D_RECT                  = 0x8DD5,
	UNSIGNED_INT_SAMPLER_BUFFER                   = 0x8DD8,
	TEXTURE_BUFFER                                = 0x8C2A,
	MAX_TEXTURE_BUFFER_SIZE                       = 0x8C2B,
	TEXTURE_BINDING_BUFFER                        = 0x8C2C,
	TEXTURE_BUFFER_DATA_STORE_BINDING             = 0x8C2D,
	TEXTURE_RECTANGLE                             = 0x84F5,
	TEXTURE_BINDING_RECTANGLE                     = 0x84F6,
	PROXY_TEXTURE_RECTANGLE                       = 0x84F7,
	MAX_RECTANGLE_TEXTURE_SIZE                    = 0x84F8,
	R8_SNORM                                      = 0x8F94,
	RG8_SNORM                                     = 0x8F95,
	RGB8_SNORM                                    = 0x8F96,
	RGBA8_SNORM                                   = 0x8F97,
	R16_SNORM                                     = 0x8F98,
	RG16_SNORM                                    = 0x8F99,
	RGB16_SNORM                                   = 0x8F9A,
	RGBA16_SNORM                                  = 0x8F9B,
	SIGNED_NORMALIZED                             = 0x8F9C,
	PRIMITIVE_RESTART                             = 0x8F9D,
	PRIMITIVE_RESTART_INDEX                       = 0x8F9E,
	COPY_READ_BUFFER                              = 0x8F36,
	COPY_WRITE_BUFFER                             = 0x8F37,
	UNIFORM_BUFFER                                = 0x8A11,
	UNIFORM_BUFFER_BINDING                        = 0x8A28,
	UNIFORM_BUFFER_START                          = 0x8A29,
	UNIFORM_BUFFER_SIZE                           = 0x8A2A,
	MAX_VERTEX_UNIFORM_BLOCKS                     = 0x8A2B,
	MAX_GEOMETRY_UNIFORM_BLOCKS                   = 0x8A2C,
	MAX_FRAGMENT_UNIFORM_BLOCKS                   = 0x8A2D,
	MAX_COMBINED_UNIFORM_BLOCKS                   = 0x8A2E,
	MAX_UNIFORM_BUFFER_BINDINGS                   = 0x8A2F,
	MAX_UNIFORM_BLOCK_SIZE                        = 0x8A30,
	MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS        = 0x8A31,
	MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS      = 0x8A32,
	MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS      = 0x8A33,
	UNIFORM_BUFFER_OFFSET_ALIGNMENT               = 0x8A34,
	ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH          = 0x8A35,
	ACTIVE_UNIFORM_BLOCKS                         = 0x8A36,
	UNIFORM_TYPE                                  = 0x8A37,
	UNIFORM_SIZE                                  = 0x8A38,
	UNIFORM_NAME_LENGTH                           = 0x8A39,
	UNIFORM_BLOCK_INDEX                           = 0x8A3A,
	UNIFORM_OFFSET                                = 0x8A3B,
	UNIFORM_ARRAY_STRIDE                          = 0x8A3C,
	UNIFORM_MATRIX_STRIDE                         = 0x8A3D,
	UNIFORM_IS_ROW_MAJOR                          = 0x8A3E,
	UNIFORM_BLOCK_BINDING                         = 0x8A3F,
	UNIFORM_BLOCK_DATA_SIZE                       = 0x8A40,
	UNIFORM_BLOCK_NAME_LENGTH                     = 0x8A41,
	UNIFORM_BLOCK_ACTIVE_UNIFORMS                 = 0x8A42,
	UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES          = 0x8A43,
	UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER     = 0x8A44,
	UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER   = 0x8A45,
	UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER   = 0x8A46,
	INVALID_INDEX                                 = 0xFFFFFFFF,

	// GL_VERSION_3_2

	CONTEXT_CORE_PROFILE_BIT                      = 0x00000001,
	CONTEXT_COMPATIBILITY_PROFILE_BIT             = 0x00000002,
	LINES_ADJACENCY                               = 0x000A,
	LINE_STRIP_ADJACENCY                          = 0x000B,
	TRIANGLES_ADJACENCY                           = 0x000C,
	TRIANGLE_STRIP_ADJACENCY                      = 0x000D,
	PROGRAM_POINT_SIZE                            = 0x8642,
	MAX_GEOMETRY_TEXTURE_IMAGE_UNITS              = 0x8C29,
	FRAMEBUFFER_ATTACHMENT_LAYERED                = 0x8DA7,
	FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS          = 0x8DA8,
	GEOMETRY_SHADER                               = 0x8DD9,
	GEOMETRY_VERTICES_OUT                         = 0x8916,
	GEOMETRY_INPUT_TYPE                           = 0x8917,
	GEOMETRY_OUTPUT_TYPE                          = 0x8918,
	MAX_GEOMETRY_UNIFORM_COMPONENTS               = 0x8DDF,
	MAX_GEOMETRY_OUTPUT_VERTICES                  = 0x8DE0,
	MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS          = 0x8DE1,
	MAX_VERTEX_OUTPUT_COMPONENTS                  = 0x9122,
	MAX_GEOMETRY_INPUT_COMPONENTS                 = 0x9123,
	MAX_GEOMETRY_OUTPUT_COMPONENTS                = 0x9124,
	MAX_FRAGMENT_INPUT_COMPONENTS                 = 0x9125,
	CONTEXT_PROFILE_MASK                          = 0x9126,
	DEPTH_CLAMP                                   = 0x864F,
	QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION      = 0x8E4C,
	FIRST_VERTEX_CONVENTION                       = 0x8E4D,
	LAST_VERTEX_CONVENTION                        = 0x8E4E,
	PROVOKING_VERTEX                              = 0x8E4F,
	TEXTURE_CUBE_MAP_SEAMLESS                     = 0x884F,
	MAX_SERVER_WAIT_TIMEOUT                       = 0x9111,
	OBJECT_TYPE                                   = 0x9112,
	SYNC_CONDITION                                = 0x9113,
	SYNC_STATUS                                   = 0x9114,
	SYNC_FLAGS                                    = 0x9115,
	SYNC_FENCE                                    = 0x9116,
	SYNC_GPU_COMMANDS_COMPLETE                    = 0x9117,
	UNSIGNALED                                    = 0x9118,
	SIGNALED                                      = 0x9119,
	ALREADY_SIGNALED                              = 0x911A,
	TIMEOUT_EXPIRED                               = 0x911B,
	CONDITION_SATISFIED                           = 0x911C,
	WAIT_FAILED                                   = 0x911D,
	TIMEOUT_IGNORED                               = 0xFFFFFFFFFFFFFFFF,
	SYNC_FLUSH_COMMANDS_BIT                       = 0x00000001,
	SAMPLE_POSITION                               = 0x8E50,
	SAMPLE_MASK                                   = 0x8E51,
	SAMPLE_MASK_VALUE                             = 0x8E52,
	MAX_SAMPLE_MASK_WORDS                         = 0x8E59,
	TEXTURE_2D_MULTISAMPLE                        = 0x9100,
	PROXY_TEXTURE_2D_MULTISAMPLE                  = 0x9101,
	TEXTURE_2D_MULTISAMPLE_ARRAY                  = 0x9102,
	PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY            = 0x9103,
	TEXTURE_BINDING_2D_MULTISAMPLE                = 0x9104,
	TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY          = 0x9105,
	TEXTURE_SAMPLES                               = 0x9106,
	TEXTURE_FIXED_SAMPLE_LOCATIONS                = 0x9107,
	SAMPLER_2D_MULTISAMPLE                        = 0x9108,
	INT_SAMPLER_2D_MULTISAMPLE                    = 0x9109,
	UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE           = 0x910A,
	SAMPLER_2D_MULTISAMPLE_ARRAY                  = 0x910B,
	INT_SAMPLER_2D_MULTISAMPLE_ARRAY              = 0x910C,
	UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY     = 0x910D,
	MAX_COLOR_TEXTURE_SAMPLES                     = 0x910E,
	MAX_DEPTH_TEXTURE_SAMPLES                     = 0x910F,
	MAX_INTEGER_SAMPLES                           = 0x9110,

	// GL_EXT_texture_filter_anisotropic

	TEXTURE_MAX_ANISOTROPY_EXT                    = 0x84FE,
	MAX_TEXTURE_MAX_ANISOTROPY_EXT                = 0x84FF,

	// GL_ARB_debug_output

	DEBUG_OUTPUT_SYNCHRONOUS_ARB                  = 0x8242,
	DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB          = 0x8243,
	DEBUG_CALLBACK_FUNCTION_ARB                   = 0x8244,
	DEBUG_CALLBACK_USER_PARAM_ARB                 = 0x8245,
	DEBUG_SOURCE_API_ARB                          = 0x8246,
	DEBUG_SOURCE_WINDOW_SYSTEM_ARB                = 0x8247,
	DEBUG_SOURCE_SHADER_COMPILER_ARB              = 0x8248,
	DEBUG_SOURCE_THIRD_PARTY_ARB                  = 0x8249,
	DEBUG_SOURCE_APPLICATION_ARB                  = 0x824A,
	DEBUG_SOURCE_OTHER_ARB                        = 0x824B,
	DEBUG_TYPE_ERROR_ARB                          = 0x824C,
	DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB            = 0x824D,
	DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB             = 0x824E,
	DEBUG_TYPE_PORTABILITY_ARB                    = 0x824F,
	DEBUG_TYPE_PERFORMANCE_ARB                    = 0x8250,
	DEBUG_TYPE_OTHER_ARB                          = 0x8251,
	MAX_DEBUG_MESSAGE_LENGTH_ARB                  = 0x9143,
	MAX_DEBUG_LOGGED_MESSAGES_ARB                 = 0x9144,
	DEBUG_LOGGED_MESSAGES_ARB                     = 0x9145,
	DEBUG_SEVERITY_HIGH_ARB                       = 0x9146,
	DEBUG_SEVERITY_MEDIUM_ARB                     = 0x9147,
	DEBUG_SEVERITY_LOW_ARB                        = 0x9148,
};
}


struct GLOutOfMemory : public std::runtime_error {
	GLOutOfMemory();
};


class Context {
public:
	typedef void* (*GlGetProcAddress)(const char*);

public:
	Context(GlGetProcAddress glGetProcAddress, Logger* logger);

	Context(const Context&) = delete;
	Context(Context&&)      = default;
	~Context() = default;

	Context& operator=(const Context&) = delete;
	Context& operator=(Context&&)      = default;

	bool initialize(bool debugGl = false);
	void shutdown();

	inline void setAbortOnError(bool enable) {
		_abortOnError = enable;
	}

	inline void setLogErrors(bool enable) {
		_logErrors = enable;
	}

	inline void setLogCalls(bool enable) {
		_logCalls = enable;
	}

	Logger& log();

	// Enum names

	const char* getEnumName(GLenum enum_) const;
	const char* getBlendEquationModeEXTName(GLenum enum_) const;
	const char* getBooleanName(GLenum enum_) const;
	const char* getBufferTargetARBName(GLenum enum_) const;
	const char* getBufferUsageARBName(GLenum enum_) const;
	const char* getBufferAccessARBName(GLenum enum_) const;
	const char* getBufferAccessMaskName(GLenum enum_) const;
	const char* getClearBufferMaskName(GLenum enum_) const;
	const char* getCullFaceModeName(GLenum enum_) const;
	const char* getDepthFunctionName(GLenum enum_) const;
	const char* getDrawBufferModeName(GLenum enum_) const;
	const char* getDrawElementsTypeName(GLenum enum_) const;
	const char* getEnableCapName(GLenum enum_) const;
	const char* getErrorCodeName(GLenum enum_) const;
	const char* getFrontFaceDirectionName(GLenum enum_) const;
	const char* getGetPNameName(GLenum enum_) const;
	const char* getGetTextureParameterName(GLenum enum_) const;
	const char* getHintModeName(GLenum enum_) const;
	const char* getHintTargetName(GLenum enum_) const;
	const char* getLogicOpName(GLenum enum_) const;
	const char* getMaterialFaceName(GLenum enum_) const;
	const char* getPixelFormatName(GLenum enum_) const;
	const char* getInternalFormatName(GLenum enum_) const;
	const char* getPixelStoreParameterName(GLenum enum_) const;
	const char* getPixelTypeName(GLenum enum_) const;
	const char* getPolygonModeName(GLenum enum_) const;
	const char* getPrimitiveTypeName(GLenum enum_) const;
	const char* getReadBufferModeName(GLenum enum_) const;
	const char* getStencilFaceDirectionName(GLenum enum_) const;
	const char* getStencilFunctionName(GLenum enum_) const;
	const char* getStencilOpName(GLenum enum_) const;
	const char* getStringNameName(GLenum enum_) const;
	const char* getSyncObjectMaskName(GLenum enum_) const;
	const char* getTextureParameterNameName(GLenum enum_) const;
	const char* getTextureTargetName(GLenum enum_) const;
	const char* getFramebufferAttachmentName(GLenum enum_) const;
	const char* getRenderbufferTargetName(GLenum enum_) const;
	const char* getFramebufferTargetName(GLenum enum_) const;
	const char* getTextureUnitName(GLenum enum_) const;
	const char* getTypeEnumName(GLenum enum_) const;
	const char* getQueryTargetName(GLenum enum_) const;
	const char* getRenderbufferParameterNameName(GLenum enum_) const;
	const char* getBlendingFactorName(GLenum enum_) const;
	const char* getBlitFramebufferFilterName(GLenum enum_) const;
	const char* getBufferName(GLenum enum_) const;
	const char* getCopyBufferSubDataTargetName(GLenum enum_) const;
	const char* getShaderTypeName(GLenum enum_) const;
	const char* getDebugSourceName(GLenum enum_) const;
	const char* getDebugTypeName(GLenum enum_) const;
	const char* getDebugSeverityName(GLenum enum_) const;
	const char* getSyncConditionName(GLenum enum_) const;
	const char* getUniformBlockPNameName(GLenum enum_) const;
	const char* getUniformPNameName(GLenum enum_) const;
	const char* getVertexProvokingModeName(GLenum enum_) const;
	const char* getSyncParameterNameName(GLenum enum_) const;
	const char* getShaderParameterNameName(GLenum enum_) const;
	const char* getQueryObjectParameterNameName(GLenum enum_) const;
	const char* getQueryParameterNameName(GLenum enum_) const;
	const char* getVertexAttribEnumName(GLenum enum_) const;
	const char* getAttributeTypeName(GLenum enum_) const;
	const char* getFramebufferAttachmentParameterNameName(GLenum enum_) const;
	const char* getVertexAttribPointerTypeName(GLenum enum_) const;
	const char* getProgramPropertyARBName(GLenum enum_) const;

	// Bitset to string

	String bitsetAsString(GLbitfield bitfield) const;
	String bitsetAttribMaskAsString(GLbitfield bitfield) const;
	String bitsetBufferAccessMaskAsString(GLbitfield bitfield) const;
	String bitsetClearBufferMaskAsString(GLbitfield bitfield) const;
	String bitsetContextFlagMaskAsString(GLbitfield bitfield) const;
	String bitsetContextProfileMaskAsString(GLbitfield bitfield) const;
	String bitsetMapBufferUsageMaskAsString(GLbitfield bitfield) const;
	String bitsetSyncObjectMaskAsString(GLbitfield bitfield) const;

	// Custom methods

	bool hasExtension(const char* ext) const;
	void checkGlErrors();
	void setEnabled(GLenum cap, bool enabled);

	// GL_VERSION_1_0

	void cullFace(GLenum mode);
	void frontFace(GLenum mode);
	void hint(GLenum target, GLenum mode);
	void lineWidth(GLfloat width);
	void pointSize(GLfloat size);
	void polygonMode(GLenum face, GLenum mode);
	void scissor(GLint x, GLint y, GLsizei width, GLsizei height);
	void texParameterf(GLenum target, GLenum pname, GLfloat param);
	void texParameterfv(GLenum target, GLenum pname, const GLfloat *params);
	void texParameteri(GLenum target, GLenum pname, GLint param);
	void texParameteriv(GLenum target, GLenum pname, const GLint *params);
	void texImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
	void texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
	void drawBuffer(GLenum buf);
	void clear(GLbitfield mask);
	void clearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void clearStencil(GLint s);
	void clearDepth(GLdouble depth);
	void stencilMask(GLuint mask);
	void colorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
	void depthMask(GLboolean flag);
	void disable(GLenum cap);
	void enable(GLenum cap);
	void finish();
	void flush();
	void blendFunc(GLenum sfactor, GLenum dfactor);
	void logicOp(GLenum opcode);
	void stencilFunc(GLenum func, GLint ref, GLuint mask);
	void stencilOp(GLenum fail, GLenum zfail, GLenum zpass);
	void depthFunc(GLenum func);
	void pixelStoref(GLenum pname, GLfloat param);
	void pixelStorei(GLenum pname, GLint param);
	void readBuffer(GLenum src);
	void readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
	void getBooleanv(GLenum pname, GLboolean *data);
	void getDoublev(GLenum pname, GLdouble *data);
	GLenum getError();
	void getFloatv(GLenum pname, GLfloat *data);
	void getIntegerv(GLenum pname, GLint *data);
	const GLubyte * getString(GLenum name);
	void getTexImage(GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
	void getTexParameterfv(GLenum target, GLenum pname, GLfloat *params);
	void getTexParameteriv(GLenum target, GLenum pname, GLint *params);
	void getTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params);
	void getTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params);
	GLboolean isEnabled(GLenum cap);
	void depthRange(GLdouble near, GLdouble far);
	void viewport(GLint x, GLint y, GLsizei width, GLsizei height);

	// GL_VERSION_1_1

	void drawArrays(GLenum mode, GLint first, GLsizei count);
	void drawElements(GLenum mode, GLsizei count, GLenum type, const void *indices);
	void polygonOffset(GLfloat factor, GLfloat units);
	void copyTexImage1D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
	void copyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
	void copyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
	void copyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	void texSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
	void texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
	void bindTexture(GLenum target, GLuint texture);
	void deleteTextures(GLsizei n, const GLuint *textures);
	void genTextures(GLsizei n, GLuint *textures);
	GLboolean isTexture(GLuint texture);

	// GL_VERSION_1_2

	void drawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
	void texImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
	void texSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
	void copyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);

	// GL_VERSION_1_3

	void activeTexture(GLenum texture);
	void sampleCoverage(GLfloat value, GLboolean invert);
	void compressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
	void compressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
	void compressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data);
	void compressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
	void compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
	void compressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
	void getCompressedTexImage(GLenum target, GLint level, void *img);

	// GL_VERSION_1_4

	void blendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
	void multiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
	void multiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount);
	void pointParameterf(GLenum pname, GLfloat param);
	void pointParameterfv(GLenum pname, const GLfloat *params);
	void pointParameteri(GLenum pname, GLint param);
	void pointParameteriv(GLenum pname, const GLint *params);
	void blendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void blendEquation(GLenum mode);

	// GL_VERSION_1_5

	void genQueries(GLsizei n, GLuint *ids);
	void deleteQueries(GLsizei n, const GLuint *ids);
	GLboolean isQuery(GLuint id);
	void beginQuery(GLenum target, GLuint id);
	void endQuery(GLenum target);
	void getQueryiv(GLenum target, GLenum pname, GLint *params);
	void getQueryObjectiv(GLuint id, GLenum pname, GLint *params);
	void getQueryObjectuiv(GLuint id, GLenum pname, GLuint *params);
	void bindBuffer(GLenum target, GLuint buffer);
	void deleteBuffers(GLsizei n, const GLuint *buffers);
	void genBuffers(GLsizei n, GLuint *buffers);
	GLboolean isBuffer(GLuint buffer);
	void bufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
	void bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
	void getBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, void *data);
	void * mapBuffer(GLenum target, GLenum access);
	GLboolean unmapBuffer(GLenum target);
	void getBufferParameteriv(GLenum target, GLenum pname, GLint *params);
	void getBufferPointerv(GLenum target, GLenum pname, void **params);

	// GL_VERSION_2_0

	void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
	void drawBuffers(GLsizei n, const GLenum *bufs);
	void stencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
	void stencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask);
	void stencilMaskSeparate(GLenum face, GLuint mask);
	void attachShader(GLuint program, GLuint shader);
	void bindAttribLocation(GLuint program, GLuint index, const GLchar *name);
	void compileShader(GLuint shader);
	GLuint createProgram();
	GLuint createShader(GLenum type);
	void deleteProgram(GLuint program);
	void deleteShader(GLuint shader);
	void detachShader(GLuint program, GLuint shader);
	void disableVertexAttribArray(GLuint index);
	void enableVertexAttribArray(GLuint index);
	void getActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
	void getActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
	void getAttachedShaders(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
	GLint getAttribLocation(GLuint program, const GLchar *name);
	void getProgramiv(GLuint program, GLenum pname, GLint *params);
	void getProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
	void getShaderiv(GLuint shader, GLenum pname, GLint *params);
	void getShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
	void getShaderSource(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
	GLint getUniformLocation(GLuint program, const GLchar *name);
	void getUniformfv(GLuint program, GLint location, GLfloat *params);
	void getUniformiv(GLuint program, GLint location, GLint *params);
	void getVertexAttribdv(GLuint index, GLenum pname, GLdouble *params);
	void getVertexAttribfv(GLuint index, GLenum pname, GLfloat *params);
	void getVertexAttribiv(GLuint index, GLenum pname, GLint *params);
	void getVertexAttribPointerv(GLuint index, GLenum pname, void **pointer);
	GLboolean isProgram(GLuint program);
	GLboolean isShader(GLuint shader);
	void linkProgram(GLuint program);
	void shaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
	void useProgram(GLuint program);
	void uniform1f(GLint location, GLfloat v0);
	void uniform2f(GLint location, GLfloat v0, GLfloat v1);
	void uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	void uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	void uniform1i(GLint location, GLint v0);
	void uniform2i(GLint location, GLint v0, GLint v1);
	void uniform3i(GLint location, GLint v0, GLint v1, GLint v2);
	void uniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
	void uniform1fv(GLint location, GLsizei count, const GLfloat *value);
	void uniform2fv(GLint location, GLsizei count, const GLfloat *value);
	void uniform3fv(GLint location, GLsizei count, const GLfloat *value);
	void uniform4fv(GLint location, GLsizei count, const GLfloat *value);
	void uniform1iv(GLint location, GLsizei count, const GLint *value);
	void uniform2iv(GLint location, GLsizei count, const GLint *value);
	void uniform3iv(GLint location, GLsizei count, const GLint *value);
	void uniform4iv(GLint location, GLsizei count, const GLint *value);
	void uniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	void uniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	void uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	void validateProgram(GLuint program);
	void vertexAttrib1d(GLuint index, GLdouble x);
	void vertexAttrib1dv(GLuint index, const GLdouble *v);
	void vertexAttrib1f(GLuint index, GLfloat x);
	void vertexAttrib1fv(GLuint index, const GLfloat *v);
	void vertexAttrib1s(GLuint index, GLshort x);
	void vertexAttrib1sv(GLuint index, const GLshort *v);
	void vertexAttrib2d(GLuint index, GLdouble x, GLdouble y);
	void vertexAttrib2dv(GLuint index, const GLdouble *v);
	void vertexAttrib2f(GLuint index, GLfloat x, GLfloat y);
	void vertexAttrib2fv(GLuint index, const GLfloat *v);
	void vertexAttrib2s(GLuint index, GLshort x, GLshort y);
	void vertexAttrib2sv(GLuint index, const GLshort *v);
	void vertexAttrib3d(GLuint index, GLdouble x, GLdouble y, GLdouble z);
	void vertexAttrib3dv(GLuint index, const GLdouble *v);
	void vertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z);
	void vertexAttrib3fv(GLuint index, const GLfloat *v);
	void vertexAttrib3s(GLuint index, GLshort x, GLshort y, GLshort z);
	void vertexAttrib3sv(GLuint index, const GLshort *v);
	void vertexAttrib4Nbv(GLuint index, const GLbyte *v);
	void vertexAttrib4Niv(GLuint index, const GLint *v);
	void vertexAttrib4Nsv(GLuint index, const GLshort *v);
	void vertexAttrib4Nub(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
	void vertexAttrib4Nubv(GLuint index, const GLubyte *v);
	void vertexAttrib4Nuiv(GLuint index, const GLuint *v);
	void vertexAttrib4Nusv(GLuint index, const GLushort *v);
	void vertexAttrib4bv(GLuint index, const GLbyte *v);
	void vertexAttrib4d(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	void vertexAttrib4dv(GLuint index, const GLdouble *v);
	void vertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void vertexAttrib4fv(GLuint index, const GLfloat *v);
	void vertexAttrib4iv(GLuint index, const GLint *v);
	void vertexAttrib4s(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
	void vertexAttrib4sv(GLuint index, const GLshort *v);
	void vertexAttrib4ubv(GLuint index, const GLubyte *v);
	void vertexAttrib4uiv(GLuint index, const GLuint *v);
	void vertexAttrib4usv(GLuint index, const GLushort *v);
	void vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

	// GL_VERSION_2_1

	void uniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	void uniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	void uniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	void uniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	void uniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	void uniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

	// GL_VERSION_3_0

	void colorMaski(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
	void getBooleani_v(GLenum target, GLuint index, GLboolean *data);
	void getIntegeri_v(GLenum target, GLuint index, GLint *data);
	void enablei(GLenum target, GLuint index);
	void disablei(GLenum target, GLuint index);
	GLboolean isEnabledi(GLenum target, GLuint index);
	void beginTransformFeedback(GLenum primitiveMode);
	void endTransformFeedback();
	void bindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
	void bindBufferBase(GLenum target, GLuint index, GLuint buffer);
	void transformFeedbackVaryings(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode);
	void getTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
	void clampColor(GLenum target, GLenum clamp);
	void beginConditionalRender(GLuint id, GLenum mode);
	void endConditionalRender();
	void vertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
	void getVertexAttribIiv(GLuint index, GLenum pname, GLint *params);
	void getVertexAttribIuiv(GLuint index, GLenum pname, GLuint *params);
	void vertexAttribI1i(GLuint index, GLint x);
	void vertexAttribI2i(GLuint index, GLint x, GLint y);
	void vertexAttribI3i(GLuint index, GLint x, GLint y, GLint z);
	void vertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w);
	void vertexAttribI1ui(GLuint index, GLuint x);
	void vertexAttribI2ui(GLuint index, GLuint x, GLuint y);
	void vertexAttribI3ui(GLuint index, GLuint x, GLuint y, GLuint z);
	void vertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
	void vertexAttribI1iv(GLuint index, const GLint *v);
	void vertexAttribI2iv(GLuint index, const GLint *v);
	void vertexAttribI3iv(GLuint index, const GLint *v);
	void vertexAttribI4iv(GLuint index, const GLint *v);
	void vertexAttribI1uiv(GLuint index, const GLuint *v);
	void vertexAttribI2uiv(GLuint index, const GLuint *v);
	void vertexAttribI3uiv(GLuint index, const GLuint *v);
	void vertexAttribI4uiv(GLuint index, const GLuint *v);
	void vertexAttribI4bv(GLuint index, const GLbyte *v);
	void vertexAttribI4sv(GLuint index, const GLshort *v);
	void vertexAttribI4ubv(GLuint index, const GLubyte *v);
	void vertexAttribI4usv(GLuint index, const GLushort *v);
	void getUniformuiv(GLuint program, GLint location, GLuint *params);
	void bindFragDataLocation(GLuint program, GLuint color, const GLchar *name);
	GLint getFragDataLocation(GLuint program, const GLchar *name);
	void uniform1ui(GLint location, GLuint v0);
	void uniform2ui(GLint location, GLuint v0, GLuint v1);
	void uniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2);
	void uniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
	void uniform1uiv(GLint location, GLsizei count, const GLuint *value);
	void uniform2uiv(GLint location, GLsizei count, const GLuint *value);
	void uniform3uiv(GLint location, GLsizei count, const GLuint *value);
	void uniform4uiv(GLint location, GLsizei count, const GLuint *value);
	void texParameterIiv(GLenum target, GLenum pname, const GLint *params);
	void texParameterIuiv(GLenum target, GLenum pname, const GLuint *params);
	void getTexParameterIiv(GLenum target, GLenum pname, GLint *params);
	void getTexParameterIuiv(GLenum target, GLenum pname, GLuint *params);
	void clearBufferiv(GLenum buffer, GLint drawbuffer, const GLint *value);
	void clearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint *value);
	void clearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value);
	void clearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
	const GLubyte * getStringi(GLenum name, GLuint index);
	GLboolean isRenderbuffer(GLuint renderbuffer);
	void bindRenderbuffer(GLenum target, GLuint renderbuffer);
	void deleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
	void genRenderbuffers(GLsizei n, GLuint *renderbuffers);
	void renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	void getRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params);
	GLboolean isFramebuffer(GLuint framebuffer);
	void bindFramebuffer(GLenum target, GLuint framebuffer);
	void deleteFramebuffers(GLsizei n, const GLuint *framebuffers);
	void genFramebuffers(GLsizei n, GLuint *framebuffers);
	GLenum checkFramebufferStatus(GLenum target);
	void framebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	void framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	void framebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
	void framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	void getFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params);
	void generateMipmap(GLenum target);
	void blitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
	void renderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	void framebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
	void * mapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
	void flushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length);
	void bindVertexArray(GLuint array);
	void deleteVertexArrays(GLsizei n, const GLuint *arrays);
	void genVertexArrays(GLsizei n, GLuint *arrays);
	GLboolean isVertexArray(GLuint array);

	// GL_VERSION_3_1

	void drawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
	void drawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
	void texBuffer(GLenum target, GLenum internalformat, GLuint buffer);
	void primitiveRestartIndex(GLuint index);
	void copyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
	void getUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices);
	void getActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
	void getActiveUniformName(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);
	GLuint getUniformBlockIndex(GLuint program, const GLchar *uniformBlockName);
	void getActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
	void getActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
	void uniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);

	// GL_VERSION_3_2

	void drawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
	void drawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
	void drawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);
	void multiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount, const GLint *basevertex);
	void provokingVertex(GLenum mode);
	GLsync fenceSync(GLenum condition, GLbitfield flags);
	GLboolean isSync(GLsync sync);
	void deleteSync(GLsync sync);
	GLenum clientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout);
	void waitSync(GLsync sync, GLbitfield flags, GLuint64 timeout);
	void getInteger64v(GLenum pname, GLint64 *data);
	void getSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);
	void getInteger64i_v(GLenum target, GLuint index, GLint64 *data);
	void getBufferParameteri64v(GLenum target, GLenum pname, GLint64 *params);
	void framebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level);
	void texImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
	void texImage3DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
	void getMultisamplefv(GLenum pname, GLuint index, GLfloat *val);
	void sampleMaski(GLuint maskNumber, GLbitfield mask);

	// GL_ARB_debug_output

	void debugMessageControlARB(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
	void debugMessageInsertARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
	void debugMessageCallbackARB(GLDEBUGPROCARB callback, const void *userParam);
	GLuint getDebugMessageLogARB(GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);

public:
	bool _gl_1_0;
	bool _gl_1_1;
	bool _gl_1_2;
	bool _gl_1_3;
	bool _gl_1_4;
	bool _gl_1_5;
	bool _gl_2_0;
	bool _gl_2_1;
	bool _gl_3_0;
	bool _gl_3_1;
	bool _gl_3_2;
	bool _gl_ext_texture_filter_anisotropic;
	bool _gl_arb_debug_output;

public:

	// GL_VERSION_1_0

	typedef void (APIENTRYP _PfnGlCullFace)(GLenum mode);
	_PfnGlCullFace _glCullFace;
	typedef void (APIENTRYP _PfnGlFrontFace)(GLenum mode);
	_PfnGlFrontFace _glFrontFace;
	typedef void (APIENTRYP _PfnGlHint)(GLenum target, GLenum mode);
	_PfnGlHint _glHint;
	typedef void (APIENTRYP _PfnGlLineWidth)(GLfloat width);
	_PfnGlLineWidth _glLineWidth;
	typedef void (APIENTRYP _PfnGlPointSize)(GLfloat size);
	_PfnGlPointSize _glPointSize;
	typedef void (APIENTRYP _PfnGlPolygonMode)(GLenum face, GLenum mode);
	_PfnGlPolygonMode _glPolygonMode;
	typedef void (APIENTRYP _PfnGlScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
	_PfnGlScissor _glScissor;
	typedef void (APIENTRYP _PfnGlTexParameterf)(GLenum target, GLenum pname, GLfloat param);
	_PfnGlTexParameterf _glTexParameterf;
	typedef void (APIENTRYP _PfnGlTexParameterfv)(GLenum target, GLenum pname, const GLfloat *params);
	_PfnGlTexParameterfv _glTexParameterfv;
	typedef void (APIENTRYP _PfnGlTexParameteri)(GLenum target, GLenum pname, GLint param);
	_PfnGlTexParameteri _glTexParameteri;
	typedef void (APIENTRYP _PfnGlTexParameteriv)(GLenum target, GLenum pname, const GLint *params);
	_PfnGlTexParameteriv _glTexParameteriv;
	typedef void (APIENTRYP _PfnGlTexImage1D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
	_PfnGlTexImage1D _glTexImage1D;
	typedef void (APIENTRYP _PfnGlTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
	_PfnGlTexImage2D _glTexImage2D;
	typedef void (APIENTRYP _PfnGlDrawBuffer)(GLenum buf);
	_PfnGlDrawBuffer _glDrawBuffer;
	typedef void (APIENTRYP _PfnGlClear)(GLbitfield mask);
	_PfnGlClear _glClear;
	typedef void (APIENTRYP _PfnGlClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	_PfnGlClearColor _glClearColor;
	typedef void (APIENTRYP _PfnGlClearStencil)(GLint s);
	_PfnGlClearStencil _glClearStencil;
	typedef void (APIENTRYP _PfnGlClearDepth)(GLdouble depth);
	_PfnGlClearDepth _glClearDepth;
	typedef void (APIENTRYP _PfnGlStencilMask)(GLuint mask);
	_PfnGlStencilMask _glStencilMask;
	typedef void (APIENTRYP _PfnGlColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
	_PfnGlColorMask _glColorMask;
	typedef void (APIENTRYP _PfnGlDepthMask)(GLboolean flag);
	_PfnGlDepthMask _glDepthMask;
	typedef void (APIENTRYP _PfnGlDisable)(GLenum cap);
	_PfnGlDisable _glDisable;
	typedef void (APIENTRYP _PfnGlEnable)(GLenum cap);
	_PfnGlEnable _glEnable;
	typedef void (APIENTRYP _PfnGlFinish)();
	_PfnGlFinish _glFinish;
	typedef void (APIENTRYP _PfnGlFlush)();
	_PfnGlFlush _glFlush;
	typedef void (APIENTRYP _PfnGlBlendFunc)(GLenum sfactor, GLenum dfactor);
	_PfnGlBlendFunc _glBlendFunc;
	typedef void (APIENTRYP _PfnGlLogicOp)(GLenum opcode);
	_PfnGlLogicOp _glLogicOp;
	typedef void (APIENTRYP _PfnGlStencilFunc)(GLenum func, GLint ref, GLuint mask);
	_PfnGlStencilFunc _glStencilFunc;
	typedef void (APIENTRYP _PfnGlStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
	_PfnGlStencilOp _glStencilOp;
	typedef void (APIENTRYP _PfnGlDepthFunc)(GLenum func);
	_PfnGlDepthFunc _glDepthFunc;
	typedef void (APIENTRYP _PfnGlPixelStoref)(GLenum pname, GLfloat param);
	_PfnGlPixelStoref _glPixelStoref;
	typedef void (APIENTRYP _PfnGlPixelStorei)(GLenum pname, GLint param);
	_PfnGlPixelStorei _glPixelStorei;
	typedef void (APIENTRYP _PfnGlReadBuffer)(GLenum src);
	_PfnGlReadBuffer _glReadBuffer;
	typedef void (APIENTRYP _PfnGlReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
	_PfnGlReadPixels _glReadPixels;
	typedef void (APIENTRYP _PfnGlGetBooleanv)(GLenum pname, GLboolean *data);
	_PfnGlGetBooleanv _glGetBooleanv;
	typedef void (APIENTRYP _PfnGlGetDoublev)(GLenum pname, GLdouble *data);
	_PfnGlGetDoublev _glGetDoublev;
	typedef GLenum (APIENTRYP _PfnGlGetError)();
	_PfnGlGetError _glGetError;
	typedef void (APIENTRYP _PfnGlGetFloatv)(GLenum pname, GLfloat *data);
	_PfnGlGetFloatv _glGetFloatv;
	typedef void (APIENTRYP _PfnGlGetIntegerv)(GLenum pname, GLint *data);
	_PfnGlGetIntegerv _glGetIntegerv;
	typedef const GLubyte * (APIENTRYP _PfnGlGetString)(GLenum name);
	_PfnGlGetString _glGetString;
	typedef void (APIENTRYP _PfnGlGetTexImage)(GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
	_PfnGlGetTexImage _glGetTexImage;
	typedef void (APIENTRYP _PfnGlGetTexParameterfv)(GLenum target, GLenum pname, GLfloat *params);
	_PfnGlGetTexParameterfv _glGetTexParameterfv;
	typedef void (APIENTRYP _PfnGlGetTexParameteriv)(GLenum target, GLenum pname, GLint *params);
	_PfnGlGetTexParameteriv _glGetTexParameteriv;
	typedef void (APIENTRYP _PfnGlGetTexLevelParameterfv)(GLenum target, GLint level, GLenum pname, GLfloat *params);
	_PfnGlGetTexLevelParameterfv _glGetTexLevelParameterfv;
	typedef void (APIENTRYP _PfnGlGetTexLevelParameteriv)(GLenum target, GLint level, GLenum pname, GLint *params);
	_PfnGlGetTexLevelParameteriv _glGetTexLevelParameteriv;
	typedef GLboolean (APIENTRYP _PfnGlIsEnabled)(GLenum cap);
	_PfnGlIsEnabled _glIsEnabled;
	typedef void (APIENTRYP _PfnGlDepthRange)(GLdouble near, GLdouble far);
	_PfnGlDepthRange _glDepthRange;
	typedef void (APIENTRYP _PfnGlViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
	_PfnGlViewport _glViewport;

	// GL_VERSION_1_1

	typedef void (APIENTRYP _PfnGlDrawArrays)(GLenum mode, GLint first, GLsizei count);
	_PfnGlDrawArrays _glDrawArrays;
	typedef void (APIENTRYP _PfnGlDrawElements)(GLenum mode, GLsizei count, GLenum type, const void *indices);
	_PfnGlDrawElements _glDrawElements;
	typedef void (APIENTRYP _PfnGlPolygonOffset)(GLfloat factor, GLfloat units);
	_PfnGlPolygonOffset _glPolygonOffset;
	typedef void (APIENTRYP _PfnGlCopyTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
	_PfnGlCopyTexImage1D _glCopyTexImage1D;
	typedef void (APIENTRYP _PfnGlCopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
	_PfnGlCopyTexImage2D _glCopyTexImage2D;
	typedef void (APIENTRYP _PfnGlCopyTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
	_PfnGlCopyTexSubImage1D _glCopyTexSubImage1D;
	typedef void (APIENTRYP _PfnGlCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	_PfnGlCopyTexSubImage2D _glCopyTexSubImage2D;
	typedef void (APIENTRYP _PfnGlTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
	_PfnGlTexSubImage1D _glTexSubImage1D;
	typedef void (APIENTRYP _PfnGlTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
	_PfnGlTexSubImage2D _glTexSubImage2D;
	typedef void (APIENTRYP _PfnGlBindTexture)(GLenum target, GLuint texture);
	_PfnGlBindTexture _glBindTexture;
	typedef void (APIENTRYP _PfnGlDeleteTextures)(GLsizei n, const GLuint *textures);
	_PfnGlDeleteTextures _glDeleteTextures;
	typedef void (APIENTRYP _PfnGlGenTextures)(GLsizei n, GLuint *textures);
	_PfnGlGenTextures _glGenTextures;
	typedef GLboolean (APIENTRYP _PfnGlIsTexture)(GLuint texture);
	_PfnGlIsTexture _glIsTexture;

	// GL_VERSION_1_2

	typedef void (APIENTRYP _PfnGlDrawRangeElements)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
	_PfnGlDrawRangeElements _glDrawRangeElements;
	typedef void (APIENTRYP _PfnGlTexImage3D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
	_PfnGlTexImage3D _glTexImage3D;
	typedef void (APIENTRYP _PfnGlTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
	_PfnGlTexSubImage3D _glTexSubImage3D;
	typedef void (APIENTRYP _PfnGlCopyTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	_PfnGlCopyTexSubImage3D _glCopyTexSubImage3D;

	// GL_VERSION_1_3

	typedef void (APIENTRYP _PfnGlActiveTexture)(GLenum texture);
	_PfnGlActiveTexture _glActiveTexture;
	typedef void (APIENTRYP _PfnGlSampleCoverage)(GLfloat value, GLboolean invert);
	_PfnGlSampleCoverage _glSampleCoverage;
	typedef void (APIENTRYP _PfnGlCompressedTexImage3D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
	_PfnGlCompressedTexImage3D _glCompressedTexImage3D;
	typedef void (APIENTRYP _PfnGlCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
	_PfnGlCompressedTexImage2D _glCompressedTexImage2D;
	typedef void (APIENTRYP _PfnGlCompressedTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data);
	_PfnGlCompressedTexImage1D _glCompressedTexImage1D;
	typedef void (APIENTRYP _PfnGlCompressedTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
	_PfnGlCompressedTexSubImage3D _glCompressedTexSubImage3D;
	typedef void (APIENTRYP _PfnGlCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
	_PfnGlCompressedTexSubImage2D _glCompressedTexSubImage2D;
	typedef void (APIENTRYP _PfnGlCompressedTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
	_PfnGlCompressedTexSubImage1D _glCompressedTexSubImage1D;
	typedef void (APIENTRYP _PfnGlGetCompressedTexImage)(GLenum target, GLint level, void *img);
	_PfnGlGetCompressedTexImage _glGetCompressedTexImage;

	// GL_VERSION_1_4

	typedef void (APIENTRYP _PfnGlBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
	_PfnGlBlendFuncSeparate _glBlendFuncSeparate;
	typedef void (APIENTRYP _PfnGlMultiDrawArrays)(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
	_PfnGlMultiDrawArrays _glMultiDrawArrays;
	typedef void (APIENTRYP _PfnGlMultiDrawElements)(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount);
	_PfnGlMultiDrawElements _glMultiDrawElements;
	typedef void (APIENTRYP _PfnGlPointParameterf)(GLenum pname, GLfloat param);
	_PfnGlPointParameterf _glPointParameterf;
	typedef void (APIENTRYP _PfnGlPointParameterfv)(GLenum pname, const GLfloat *params);
	_PfnGlPointParameterfv _glPointParameterfv;
	typedef void (APIENTRYP _PfnGlPointParameteri)(GLenum pname, GLint param);
	_PfnGlPointParameteri _glPointParameteri;
	typedef void (APIENTRYP _PfnGlPointParameteriv)(GLenum pname, const GLint *params);
	_PfnGlPointParameteriv _glPointParameteriv;
	typedef void (APIENTRYP _PfnGlBlendColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	_PfnGlBlendColor _glBlendColor;
	typedef void (APIENTRYP _PfnGlBlendEquation)(GLenum mode);
	_PfnGlBlendEquation _glBlendEquation;

	// GL_VERSION_1_5

	typedef void (APIENTRYP _PfnGlGenQueries)(GLsizei n, GLuint *ids);
	_PfnGlGenQueries _glGenQueries;
	typedef void (APIENTRYP _PfnGlDeleteQueries)(GLsizei n, const GLuint *ids);
	_PfnGlDeleteQueries _glDeleteQueries;
	typedef GLboolean (APIENTRYP _PfnGlIsQuery)(GLuint id);
	_PfnGlIsQuery _glIsQuery;
	typedef void (APIENTRYP _PfnGlBeginQuery)(GLenum target, GLuint id);
	_PfnGlBeginQuery _glBeginQuery;
	typedef void (APIENTRYP _PfnGlEndQuery)(GLenum target);
	_PfnGlEndQuery _glEndQuery;
	typedef void (APIENTRYP _PfnGlGetQueryiv)(GLenum target, GLenum pname, GLint *params);
	_PfnGlGetQueryiv _glGetQueryiv;
	typedef void (APIENTRYP _PfnGlGetQueryObjectiv)(GLuint id, GLenum pname, GLint *params);
	_PfnGlGetQueryObjectiv _glGetQueryObjectiv;
	typedef void (APIENTRYP _PfnGlGetQueryObjectuiv)(GLuint id, GLenum pname, GLuint *params);
	_PfnGlGetQueryObjectuiv _glGetQueryObjectuiv;
	typedef void (APIENTRYP _PfnGlBindBuffer)(GLenum target, GLuint buffer);
	_PfnGlBindBuffer _glBindBuffer;
	typedef void (APIENTRYP _PfnGlDeleteBuffers)(GLsizei n, const GLuint *buffers);
	_PfnGlDeleteBuffers _glDeleteBuffers;
	typedef void (APIENTRYP _PfnGlGenBuffers)(GLsizei n, GLuint *buffers);
	_PfnGlGenBuffers _glGenBuffers;
	typedef GLboolean (APIENTRYP _PfnGlIsBuffer)(GLuint buffer);
	_PfnGlIsBuffer _glIsBuffer;
	typedef void (APIENTRYP _PfnGlBufferData)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
	_PfnGlBufferData _glBufferData;
	typedef void (APIENTRYP _PfnGlBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
	_PfnGlBufferSubData _glBufferSubData;
	typedef void (APIENTRYP _PfnGlGetBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, void *data);
	_PfnGlGetBufferSubData _glGetBufferSubData;
	typedef void * (APIENTRYP _PfnGlMapBuffer)(GLenum target, GLenum access);
	_PfnGlMapBuffer _glMapBuffer;
	typedef GLboolean (APIENTRYP _PfnGlUnmapBuffer)(GLenum target);
	_PfnGlUnmapBuffer _glUnmapBuffer;
	typedef void (APIENTRYP _PfnGlGetBufferParameteriv)(GLenum target, GLenum pname, GLint *params);
	_PfnGlGetBufferParameteriv _glGetBufferParameteriv;
	typedef void (APIENTRYP _PfnGlGetBufferPointerv)(GLenum target, GLenum pname, void **params);
	_PfnGlGetBufferPointerv _glGetBufferPointerv;

	// GL_VERSION_2_0

	typedef void (APIENTRYP _PfnGlBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
	_PfnGlBlendEquationSeparate _glBlendEquationSeparate;
	typedef void (APIENTRYP _PfnGlDrawBuffers)(GLsizei n, const GLenum *bufs);
	_PfnGlDrawBuffers _glDrawBuffers;
	typedef void (APIENTRYP _PfnGlStencilOpSeparate)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
	_PfnGlStencilOpSeparate _glStencilOpSeparate;
	typedef void (APIENTRYP _PfnGlStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
	_PfnGlStencilFuncSeparate _glStencilFuncSeparate;
	typedef void (APIENTRYP _PfnGlStencilMaskSeparate)(GLenum face, GLuint mask);
	_PfnGlStencilMaskSeparate _glStencilMaskSeparate;
	typedef void (APIENTRYP _PfnGlAttachShader)(GLuint program, GLuint shader);
	_PfnGlAttachShader _glAttachShader;
	typedef void (APIENTRYP _PfnGlBindAttribLocation)(GLuint program, GLuint index, const GLchar *name);
	_PfnGlBindAttribLocation _glBindAttribLocation;
	typedef void (APIENTRYP _PfnGlCompileShader)(GLuint shader);
	_PfnGlCompileShader _glCompileShader;
	typedef GLuint (APIENTRYP _PfnGlCreateProgram)();
	_PfnGlCreateProgram _glCreateProgram;
	typedef GLuint (APIENTRYP _PfnGlCreateShader)(GLenum type);
	_PfnGlCreateShader _glCreateShader;
	typedef void (APIENTRYP _PfnGlDeleteProgram)(GLuint program);
	_PfnGlDeleteProgram _glDeleteProgram;
	typedef void (APIENTRYP _PfnGlDeleteShader)(GLuint shader);
	_PfnGlDeleteShader _glDeleteShader;
	typedef void (APIENTRYP _PfnGlDetachShader)(GLuint program, GLuint shader);
	_PfnGlDetachShader _glDetachShader;
	typedef void (APIENTRYP _PfnGlDisableVertexAttribArray)(GLuint index);
	_PfnGlDisableVertexAttribArray _glDisableVertexAttribArray;
	typedef void (APIENTRYP _PfnGlEnableVertexAttribArray)(GLuint index);
	_PfnGlEnableVertexAttribArray _glEnableVertexAttribArray;
	typedef void (APIENTRYP _PfnGlGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
	_PfnGlGetActiveAttrib _glGetActiveAttrib;
	typedef void (APIENTRYP _PfnGlGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
	_PfnGlGetActiveUniform _glGetActiveUniform;
	typedef void (APIENTRYP _PfnGlGetAttachedShaders)(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
	_PfnGlGetAttachedShaders _glGetAttachedShaders;
	typedef GLint (APIENTRYP _PfnGlGetAttribLocation)(GLuint program, const GLchar *name);
	_PfnGlGetAttribLocation _glGetAttribLocation;
	typedef void (APIENTRYP _PfnGlGetProgramiv)(GLuint program, GLenum pname, GLint *params);
	_PfnGlGetProgramiv _glGetProgramiv;
	typedef void (APIENTRYP _PfnGlGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
	_PfnGlGetProgramInfoLog _glGetProgramInfoLog;
	typedef void (APIENTRYP _PfnGlGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
	_PfnGlGetShaderiv _glGetShaderiv;
	typedef void (APIENTRYP _PfnGlGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
	_PfnGlGetShaderInfoLog _glGetShaderInfoLog;
	typedef void (APIENTRYP _PfnGlGetShaderSource)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
	_PfnGlGetShaderSource _glGetShaderSource;
	typedef GLint (APIENTRYP _PfnGlGetUniformLocation)(GLuint program, const GLchar *name);
	_PfnGlGetUniformLocation _glGetUniformLocation;
	typedef void (APIENTRYP _PfnGlGetUniformfv)(GLuint program, GLint location, GLfloat *params);
	_PfnGlGetUniformfv _glGetUniformfv;
	typedef void (APIENTRYP _PfnGlGetUniformiv)(GLuint program, GLint location, GLint *params);
	_PfnGlGetUniformiv _glGetUniformiv;
	typedef void (APIENTRYP _PfnGlGetVertexAttribdv)(GLuint index, GLenum pname, GLdouble *params);
	_PfnGlGetVertexAttribdv _glGetVertexAttribdv;
	typedef void (APIENTRYP _PfnGlGetVertexAttribfv)(GLuint index, GLenum pname, GLfloat *params);
	_PfnGlGetVertexAttribfv _glGetVertexAttribfv;
	typedef void (APIENTRYP _PfnGlGetVertexAttribiv)(GLuint index, GLenum pname, GLint *params);
	_PfnGlGetVertexAttribiv _glGetVertexAttribiv;
	typedef void (APIENTRYP _PfnGlGetVertexAttribPointerv)(GLuint index, GLenum pname, void **pointer);
	_PfnGlGetVertexAttribPointerv _glGetVertexAttribPointerv;
	typedef GLboolean (APIENTRYP _PfnGlIsProgram)(GLuint program);
	_PfnGlIsProgram _glIsProgram;
	typedef GLboolean (APIENTRYP _PfnGlIsShader)(GLuint shader);
	_PfnGlIsShader _glIsShader;
	typedef void (APIENTRYP _PfnGlLinkProgram)(GLuint program);
	_PfnGlLinkProgram _glLinkProgram;
	typedef void (APIENTRYP _PfnGlShaderSource)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
	_PfnGlShaderSource _glShaderSource;
	typedef void (APIENTRYP _PfnGlUseProgram)(GLuint program);
	_PfnGlUseProgram _glUseProgram;
	typedef void (APIENTRYP _PfnGlUniform1f)(GLint location, GLfloat v0);
	_PfnGlUniform1f _glUniform1f;
	typedef void (APIENTRYP _PfnGlUniform2f)(GLint location, GLfloat v0, GLfloat v1);
	_PfnGlUniform2f _glUniform2f;
	typedef void (APIENTRYP _PfnGlUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	_PfnGlUniform3f _glUniform3f;
	typedef void (APIENTRYP _PfnGlUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	_PfnGlUniform4f _glUniform4f;
	typedef void (APIENTRYP _PfnGlUniform1i)(GLint location, GLint v0);
	_PfnGlUniform1i _glUniform1i;
	typedef void (APIENTRYP _PfnGlUniform2i)(GLint location, GLint v0, GLint v1);
	_PfnGlUniform2i _glUniform2i;
	typedef void (APIENTRYP _PfnGlUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);
	_PfnGlUniform3i _glUniform3i;
	typedef void (APIENTRYP _PfnGlUniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
	_PfnGlUniform4i _glUniform4i;
	typedef void (APIENTRYP _PfnGlUniform1fv)(GLint location, GLsizei count, const GLfloat *value);
	_PfnGlUniform1fv _glUniform1fv;
	typedef void (APIENTRYP _PfnGlUniform2fv)(GLint location, GLsizei count, const GLfloat *value);
	_PfnGlUniform2fv _glUniform2fv;
	typedef void (APIENTRYP _PfnGlUniform3fv)(GLint location, GLsizei count, const GLfloat *value);
	_PfnGlUniform3fv _glUniform3fv;
	typedef void (APIENTRYP _PfnGlUniform4fv)(GLint location, GLsizei count, const GLfloat *value);
	_PfnGlUniform4fv _glUniform4fv;
	typedef void (APIENTRYP _PfnGlUniform1iv)(GLint location, GLsizei count, const GLint *value);
	_PfnGlUniform1iv _glUniform1iv;
	typedef void (APIENTRYP _PfnGlUniform2iv)(GLint location, GLsizei count, const GLint *value);
	_PfnGlUniform2iv _glUniform2iv;
	typedef void (APIENTRYP _PfnGlUniform3iv)(GLint location, GLsizei count, const GLint *value);
	_PfnGlUniform3iv _glUniform3iv;
	typedef void (APIENTRYP _PfnGlUniform4iv)(GLint location, GLsizei count, const GLint *value);
	_PfnGlUniform4iv _glUniform4iv;
	typedef void (APIENTRYP _PfnGlUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	_PfnGlUniformMatrix2fv _glUniformMatrix2fv;
	typedef void (APIENTRYP _PfnGlUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	_PfnGlUniformMatrix3fv _glUniformMatrix3fv;
	typedef void (APIENTRYP _PfnGlUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	_PfnGlUniformMatrix4fv _glUniformMatrix4fv;
	typedef void (APIENTRYP _PfnGlValidateProgram)(GLuint program);
	_PfnGlValidateProgram _glValidateProgram;
	typedef void (APIENTRYP _PfnGlVertexAttrib1d)(GLuint index, GLdouble x);
	_PfnGlVertexAttrib1d _glVertexAttrib1d;
	typedef void (APIENTRYP _PfnGlVertexAttrib1dv)(GLuint index, const GLdouble *v);
	_PfnGlVertexAttrib1dv _glVertexAttrib1dv;
	typedef void (APIENTRYP _PfnGlVertexAttrib1f)(GLuint index, GLfloat x);
	_PfnGlVertexAttrib1f _glVertexAttrib1f;
	typedef void (APIENTRYP _PfnGlVertexAttrib1fv)(GLuint index, const GLfloat *v);
	_PfnGlVertexAttrib1fv _glVertexAttrib1fv;
	typedef void (APIENTRYP _PfnGlVertexAttrib1s)(GLuint index, GLshort x);
	_PfnGlVertexAttrib1s _glVertexAttrib1s;
	typedef void (APIENTRYP _PfnGlVertexAttrib1sv)(GLuint index, const GLshort *v);
	_PfnGlVertexAttrib1sv _glVertexAttrib1sv;
	typedef void (APIENTRYP _PfnGlVertexAttrib2d)(GLuint index, GLdouble x, GLdouble y);
	_PfnGlVertexAttrib2d _glVertexAttrib2d;
	typedef void (APIENTRYP _PfnGlVertexAttrib2dv)(GLuint index, const GLdouble *v);
	_PfnGlVertexAttrib2dv _glVertexAttrib2dv;
	typedef void (APIENTRYP _PfnGlVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
	_PfnGlVertexAttrib2f _glVertexAttrib2f;
	typedef void (APIENTRYP _PfnGlVertexAttrib2fv)(GLuint index, const GLfloat *v);
	_PfnGlVertexAttrib2fv _glVertexAttrib2fv;
	typedef void (APIENTRYP _PfnGlVertexAttrib2s)(GLuint index, GLshort x, GLshort y);
	_PfnGlVertexAttrib2s _glVertexAttrib2s;
	typedef void (APIENTRYP _PfnGlVertexAttrib2sv)(GLuint index, const GLshort *v);
	_PfnGlVertexAttrib2sv _glVertexAttrib2sv;
	typedef void (APIENTRYP _PfnGlVertexAttrib3d)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
	_PfnGlVertexAttrib3d _glVertexAttrib3d;
	typedef void (APIENTRYP _PfnGlVertexAttrib3dv)(GLuint index, const GLdouble *v);
	_PfnGlVertexAttrib3dv _glVertexAttrib3dv;
	typedef void (APIENTRYP _PfnGlVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
	_PfnGlVertexAttrib3f _glVertexAttrib3f;
	typedef void (APIENTRYP _PfnGlVertexAttrib3fv)(GLuint index, const GLfloat *v);
	_PfnGlVertexAttrib3fv _glVertexAttrib3fv;
	typedef void (APIENTRYP _PfnGlVertexAttrib3s)(GLuint index, GLshort x, GLshort y, GLshort z);
	_PfnGlVertexAttrib3s _glVertexAttrib3s;
	typedef void (APIENTRYP _PfnGlVertexAttrib3sv)(GLuint index, const GLshort *v);
	_PfnGlVertexAttrib3sv _glVertexAttrib3sv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4Nbv)(GLuint index, const GLbyte *v);
	_PfnGlVertexAttrib4Nbv _glVertexAttrib4Nbv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4Niv)(GLuint index, const GLint *v);
	_PfnGlVertexAttrib4Niv _glVertexAttrib4Niv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4Nsv)(GLuint index, const GLshort *v);
	_PfnGlVertexAttrib4Nsv _glVertexAttrib4Nsv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4Nub)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
	_PfnGlVertexAttrib4Nub _glVertexAttrib4Nub;
	typedef void (APIENTRYP _PfnGlVertexAttrib4Nubv)(GLuint index, const GLubyte *v);
	_PfnGlVertexAttrib4Nubv _glVertexAttrib4Nubv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4Nuiv)(GLuint index, const GLuint *v);
	_PfnGlVertexAttrib4Nuiv _glVertexAttrib4Nuiv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4Nusv)(GLuint index, const GLushort *v);
	_PfnGlVertexAttrib4Nusv _glVertexAttrib4Nusv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4bv)(GLuint index, const GLbyte *v);
	_PfnGlVertexAttrib4bv _glVertexAttrib4bv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4d)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	_PfnGlVertexAttrib4d _glVertexAttrib4d;
	typedef void (APIENTRYP _PfnGlVertexAttrib4dv)(GLuint index, const GLdouble *v);
	_PfnGlVertexAttrib4dv _glVertexAttrib4dv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	_PfnGlVertexAttrib4f _glVertexAttrib4f;
	typedef void (APIENTRYP _PfnGlVertexAttrib4fv)(GLuint index, const GLfloat *v);
	_PfnGlVertexAttrib4fv _glVertexAttrib4fv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4iv)(GLuint index, const GLint *v);
	_PfnGlVertexAttrib4iv _glVertexAttrib4iv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4s)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
	_PfnGlVertexAttrib4s _glVertexAttrib4s;
	typedef void (APIENTRYP _PfnGlVertexAttrib4sv)(GLuint index, const GLshort *v);
	_PfnGlVertexAttrib4sv _glVertexAttrib4sv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4ubv)(GLuint index, const GLubyte *v);
	_PfnGlVertexAttrib4ubv _glVertexAttrib4ubv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4uiv)(GLuint index, const GLuint *v);
	_PfnGlVertexAttrib4uiv _glVertexAttrib4uiv;
	typedef void (APIENTRYP _PfnGlVertexAttrib4usv)(GLuint index, const GLushort *v);
	_PfnGlVertexAttrib4usv _glVertexAttrib4usv;
	typedef void (APIENTRYP _PfnGlVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
	_PfnGlVertexAttribPointer _glVertexAttribPointer;

	// GL_VERSION_2_1

	typedef void (APIENTRYP _PfnGlUniformMatrix2x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	_PfnGlUniformMatrix2x3fv _glUniformMatrix2x3fv;
	typedef void (APIENTRYP _PfnGlUniformMatrix3x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	_PfnGlUniformMatrix3x2fv _glUniformMatrix3x2fv;
	typedef void (APIENTRYP _PfnGlUniformMatrix2x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	_PfnGlUniformMatrix2x4fv _glUniformMatrix2x4fv;
	typedef void (APIENTRYP _PfnGlUniformMatrix4x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	_PfnGlUniformMatrix4x2fv _glUniformMatrix4x2fv;
	typedef void (APIENTRYP _PfnGlUniformMatrix3x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	_PfnGlUniformMatrix3x4fv _glUniformMatrix3x4fv;
	typedef void (APIENTRYP _PfnGlUniformMatrix4x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	_PfnGlUniformMatrix4x3fv _glUniformMatrix4x3fv;

	// GL_VERSION_3_0

	typedef void (APIENTRYP _PfnGlColorMaski)(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
	_PfnGlColorMaski _glColorMaski;
	typedef void (APIENTRYP _PfnGlGetBooleani_v)(GLenum target, GLuint index, GLboolean *data);
	_PfnGlGetBooleani_v _glGetBooleani_v;
	typedef void (APIENTRYP _PfnGlGetIntegeri_v)(GLenum target, GLuint index, GLint *data);
	_PfnGlGetIntegeri_v _glGetIntegeri_v;
	typedef void (APIENTRYP _PfnGlEnablei)(GLenum target, GLuint index);
	_PfnGlEnablei _glEnablei;
	typedef void (APIENTRYP _PfnGlDisablei)(GLenum target, GLuint index);
	_PfnGlDisablei _glDisablei;
	typedef GLboolean (APIENTRYP _PfnGlIsEnabledi)(GLenum target, GLuint index);
	_PfnGlIsEnabledi _glIsEnabledi;
	typedef void (APIENTRYP _PfnGlBeginTransformFeedback)(GLenum primitiveMode);
	_PfnGlBeginTransformFeedback _glBeginTransformFeedback;
	typedef void (APIENTRYP _PfnGlEndTransformFeedback)();
	_PfnGlEndTransformFeedback _glEndTransformFeedback;
	typedef void (APIENTRYP _PfnGlBindBufferRange)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
	_PfnGlBindBufferRange _glBindBufferRange;
	typedef void (APIENTRYP _PfnGlBindBufferBase)(GLenum target, GLuint index, GLuint buffer);
	_PfnGlBindBufferBase _glBindBufferBase;
	typedef void (APIENTRYP _PfnGlTransformFeedbackVaryings)(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode);
	_PfnGlTransformFeedbackVaryings _glTransformFeedbackVaryings;
	typedef void (APIENTRYP _PfnGlGetTransformFeedbackVarying)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
	_PfnGlGetTransformFeedbackVarying _glGetTransformFeedbackVarying;
	typedef void (APIENTRYP _PfnGlClampColor)(GLenum target, GLenum clamp);
	_PfnGlClampColor _glClampColor;
	typedef void (APIENTRYP _PfnGlBeginConditionalRender)(GLuint id, GLenum mode);
	_PfnGlBeginConditionalRender _glBeginConditionalRender;
	typedef void (APIENTRYP _PfnGlEndConditionalRender)();
	_PfnGlEndConditionalRender _glEndConditionalRender;
	typedef void (APIENTRYP _PfnGlVertexAttribIPointer)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
	_PfnGlVertexAttribIPointer _glVertexAttribIPointer;
	typedef void (APIENTRYP _PfnGlGetVertexAttribIiv)(GLuint index, GLenum pname, GLint *params);
	_PfnGlGetVertexAttribIiv _glGetVertexAttribIiv;
	typedef void (APIENTRYP _PfnGlGetVertexAttribIuiv)(GLuint index, GLenum pname, GLuint *params);
	_PfnGlGetVertexAttribIuiv _glGetVertexAttribIuiv;
	typedef void (APIENTRYP _PfnGlVertexAttribI1i)(GLuint index, GLint x);
	_PfnGlVertexAttribI1i _glVertexAttribI1i;
	typedef void (APIENTRYP _PfnGlVertexAttribI2i)(GLuint index, GLint x, GLint y);
	_PfnGlVertexAttribI2i _glVertexAttribI2i;
	typedef void (APIENTRYP _PfnGlVertexAttribI3i)(GLuint index, GLint x, GLint y, GLint z);
	_PfnGlVertexAttribI3i _glVertexAttribI3i;
	typedef void (APIENTRYP _PfnGlVertexAttribI4i)(GLuint index, GLint x, GLint y, GLint z, GLint w);
	_PfnGlVertexAttribI4i _glVertexAttribI4i;
	typedef void (APIENTRYP _PfnGlVertexAttribI1ui)(GLuint index, GLuint x);
	_PfnGlVertexAttribI1ui _glVertexAttribI1ui;
	typedef void (APIENTRYP _PfnGlVertexAttribI2ui)(GLuint index, GLuint x, GLuint y);
	_PfnGlVertexAttribI2ui _glVertexAttribI2ui;
	typedef void (APIENTRYP _PfnGlVertexAttribI3ui)(GLuint index, GLuint x, GLuint y, GLuint z);
	_PfnGlVertexAttribI3ui _glVertexAttribI3ui;
	typedef void (APIENTRYP _PfnGlVertexAttribI4ui)(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
	_PfnGlVertexAttribI4ui _glVertexAttribI4ui;
	typedef void (APIENTRYP _PfnGlVertexAttribI1iv)(GLuint index, const GLint *v);
	_PfnGlVertexAttribI1iv _glVertexAttribI1iv;
	typedef void (APIENTRYP _PfnGlVertexAttribI2iv)(GLuint index, const GLint *v);
	_PfnGlVertexAttribI2iv _glVertexAttribI2iv;
	typedef void (APIENTRYP _PfnGlVertexAttribI3iv)(GLuint index, const GLint *v);
	_PfnGlVertexAttribI3iv _glVertexAttribI3iv;
	typedef void (APIENTRYP _PfnGlVertexAttribI4iv)(GLuint index, const GLint *v);
	_PfnGlVertexAttribI4iv _glVertexAttribI4iv;
	typedef void (APIENTRYP _PfnGlVertexAttribI1uiv)(GLuint index, const GLuint *v);
	_PfnGlVertexAttribI1uiv _glVertexAttribI1uiv;
	typedef void (APIENTRYP _PfnGlVertexAttribI2uiv)(GLuint index, const GLuint *v);
	_PfnGlVertexAttribI2uiv _glVertexAttribI2uiv;
	typedef void (APIENTRYP _PfnGlVertexAttribI3uiv)(GLuint index, const GLuint *v);
	_PfnGlVertexAttribI3uiv _glVertexAttribI3uiv;
	typedef void (APIENTRYP _PfnGlVertexAttribI4uiv)(GLuint index, const GLuint *v);
	_PfnGlVertexAttribI4uiv _glVertexAttribI4uiv;
	typedef void (APIENTRYP _PfnGlVertexAttribI4bv)(GLuint index, const GLbyte *v);
	_PfnGlVertexAttribI4bv _glVertexAttribI4bv;
	typedef void (APIENTRYP _PfnGlVertexAttribI4sv)(GLuint index, const GLshort *v);
	_PfnGlVertexAttribI4sv _glVertexAttribI4sv;
	typedef void (APIENTRYP _PfnGlVertexAttribI4ubv)(GLuint index, const GLubyte *v);
	_PfnGlVertexAttribI4ubv _glVertexAttribI4ubv;
	typedef void (APIENTRYP _PfnGlVertexAttribI4usv)(GLuint index, const GLushort *v);
	_PfnGlVertexAttribI4usv _glVertexAttribI4usv;
	typedef void (APIENTRYP _PfnGlGetUniformuiv)(GLuint program, GLint location, GLuint *params);
	_PfnGlGetUniformuiv _glGetUniformuiv;
	typedef void (APIENTRYP _PfnGlBindFragDataLocation)(GLuint program, GLuint color, const GLchar *name);
	_PfnGlBindFragDataLocation _glBindFragDataLocation;
	typedef GLint (APIENTRYP _PfnGlGetFragDataLocation)(GLuint program, const GLchar *name);
	_PfnGlGetFragDataLocation _glGetFragDataLocation;
	typedef void (APIENTRYP _PfnGlUniform1ui)(GLint location, GLuint v0);
	_PfnGlUniform1ui _glUniform1ui;
	typedef void (APIENTRYP _PfnGlUniform2ui)(GLint location, GLuint v0, GLuint v1);
	_PfnGlUniform2ui _glUniform2ui;
	typedef void (APIENTRYP _PfnGlUniform3ui)(GLint location, GLuint v0, GLuint v1, GLuint v2);
	_PfnGlUniform3ui _glUniform3ui;
	typedef void (APIENTRYP _PfnGlUniform4ui)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
	_PfnGlUniform4ui _glUniform4ui;
	typedef void (APIENTRYP _PfnGlUniform1uiv)(GLint location, GLsizei count, const GLuint *value);
	_PfnGlUniform1uiv _glUniform1uiv;
	typedef void (APIENTRYP _PfnGlUniform2uiv)(GLint location, GLsizei count, const GLuint *value);
	_PfnGlUniform2uiv _glUniform2uiv;
	typedef void (APIENTRYP _PfnGlUniform3uiv)(GLint location, GLsizei count, const GLuint *value);
	_PfnGlUniform3uiv _glUniform3uiv;
	typedef void (APIENTRYP _PfnGlUniform4uiv)(GLint location, GLsizei count, const GLuint *value);
	_PfnGlUniform4uiv _glUniform4uiv;
	typedef void (APIENTRYP _PfnGlTexParameterIiv)(GLenum target, GLenum pname, const GLint *params);
	_PfnGlTexParameterIiv _glTexParameterIiv;
	typedef void (APIENTRYP _PfnGlTexParameterIuiv)(GLenum target, GLenum pname, const GLuint *params);
	_PfnGlTexParameterIuiv _glTexParameterIuiv;
	typedef void (APIENTRYP _PfnGlGetTexParameterIiv)(GLenum target, GLenum pname, GLint *params);
	_PfnGlGetTexParameterIiv _glGetTexParameterIiv;
	typedef void (APIENTRYP _PfnGlGetTexParameterIuiv)(GLenum target, GLenum pname, GLuint *params);
	_PfnGlGetTexParameterIuiv _glGetTexParameterIuiv;
	typedef void (APIENTRYP _PfnGlClearBufferiv)(GLenum buffer, GLint drawbuffer, const GLint *value);
	_PfnGlClearBufferiv _glClearBufferiv;
	typedef void (APIENTRYP _PfnGlClearBufferuiv)(GLenum buffer, GLint drawbuffer, const GLuint *value);
	_PfnGlClearBufferuiv _glClearBufferuiv;
	typedef void (APIENTRYP _PfnGlClearBufferfv)(GLenum buffer, GLint drawbuffer, const GLfloat *value);
	_PfnGlClearBufferfv _glClearBufferfv;
	typedef void (APIENTRYP _PfnGlClearBufferfi)(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
	_PfnGlClearBufferfi _glClearBufferfi;
	typedef const GLubyte * (APIENTRYP _PfnGlGetStringi)(GLenum name, GLuint index);
	_PfnGlGetStringi _glGetStringi;
	typedef GLboolean (APIENTRYP _PfnGlIsRenderbuffer)(GLuint renderbuffer);
	_PfnGlIsRenderbuffer _glIsRenderbuffer;
	typedef void (APIENTRYP _PfnGlBindRenderbuffer)(GLenum target, GLuint renderbuffer);
	_PfnGlBindRenderbuffer _glBindRenderbuffer;
	typedef void (APIENTRYP _PfnGlDeleteRenderbuffers)(GLsizei n, const GLuint *renderbuffers);
	_PfnGlDeleteRenderbuffers _glDeleteRenderbuffers;
	typedef void (APIENTRYP _PfnGlGenRenderbuffers)(GLsizei n, GLuint *renderbuffers);
	_PfnGlGenRenderbuffers _glGenRenderbuffers;
	typedef void (APIENTRYP _PfnGlRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	_PfnGlRenderbufferStorage _glRenderbufferStorage;
	typedef void (APIENTRYP _PfnGlGetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint *params);
	_PfnGlGetRenderbufferParameteriv _glGetRenderbufferParameteriv;
	typedef GLboolean (APIENTRYP _PfnGlIsFramebuffer)(GLuint framebuffer);
	_PfnGlIsFramebuffer _glIsFramebuffer;
	typedef void (APIENTRYP _PfnGlBindFramebuffer)(GLenum target, GLuint framebuffer);
	_PfnGlBindFramebuffer _glBindFramebuffer;
	typedef void (APIENTRYP _PfnGlDeleteFramebuffers)(GLsizei n, const GLuint *framebuffers);
	_PfnGlDeleteFramebuffers _glDeleteFramebuffers;
	typedef void (APIENTRYP _PfnGlGenFramebuffers)(GLsizei n, GLuint *framebuffers);
	_PfnGlGenFramebuffers _glGenFramebuffers;
	typedef GLenum (APIENTRYP _PfnGlCheckFramebufferStatus)(GLenum target);
	_PfnGlCheckFramebufferStatus _glCheckFramebufferStatus;
	typedef void (APIENTRYP _PfnGlFramebufferTexture1D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	_PfnGlFramebufferTexture1D _glFramebufferTexture1D;
	typedef void (APIENTRYP _PfnGlFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	_PfnGlFramebufferTexture2D _glFramebufferTexture2D;
	typedef void (APIENTRYP _PfnGlFramebufferTexture3D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
	_PfnGlFramebufferTexture3D _glFramebufferTexture3D;
	typedef void (APIENTRYP _PfnGlFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	_PfnGlFramebufferRenderbuffer _glFramebufferRenderbuffer;
	typedef void (APIENTRYP _PfnGlGetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint *params);
	_PfnGlGetFramebufferAttachmentParameteriv _glGetFramebufferAttachmentParameteriv;
	typedef void (APIENTRYP _PfnGlGenerateMipmap)(GLenum target);
	_PfnGlGenerateMipmap _glGenerateMipmap;
	typedef void (APIENTRYP _PfnGlBlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
	_PfnGlBlitFramebuffer _glBlitFramebuffer;
	typedef void (APIENTRYP _PfnGlRenderbufferStorageMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	_PfnGlRenderbufferStorageMultisample _glRenderbufferStorageMultisample;
	typedef void (APIENTRYP _PfnGlFramebufferTextureLayer)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
	_PfnGlFramebufferTextureLayer _glFramebufferTextureLayer;
	typedef void * (APIENTRYP _PfnGlMapBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
	_PfnGlMapBufferRange _glMapBufferRange;
	typedef void (APIENTRYP _PfnGlFlushMappedBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length);
	_PfnGlFlushMappedBufferRange _glFlushMappedBufferRange;
	typedef void (APIENTRYP _PfnGlBindVertexArray)(GLuint array);
	_PfnGlBindVertexArray _glBindVertexArray;
	typedef void (APIENTRYP _PfnGlDeleteVertexArrays)(GLsizei n, const GLuint *arrays);
	_PfnGlDeleteVertexArrays _glDeleteVertexArrays;
	typedef void (APIENTRYP _PfnGlGenVertexArrays)(GLsizei n, GLuint *arrays);
	_PfnGlGenVertexArrays _glGenVertexArrays;
	typedef GLboolean (APIENTRYP _PfnGlIsVertexArray)(GLuint array);
	_PfnGlIsVertexArray _glIsVertexArray;

	// GL_VERSION_3_1

	typedef void (APIENTRYP _PfnGlDrawArraysInstanced)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
	_PfnGlDrawArraysInstanced _glDrawArraysInstanced;
	typedef void (APIENTRYP _PfnGlDrawElementsInstanced)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
	_PfnGlDrawElementsInstanced _glDrawElementsInstanced;
	typedef void (APIENTRYP _PfnGlTexBuffer)(GLenum target, GLenum internalformat, GLuint buffer);
	_PfnGlTexBuffer _glTexBuffer;
	typedef void (APIENTRYP _PfnGlPrimitiveRestartIndex)(GLuint index);
	_PfnGlPrimitiveRestartIndex _glPrimitiveRestartIndex;
	typedef void (APIENTRYP _PfnGlCopyBufferSubData)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
	_PfnGlCopyBufferSubData _glCopyBufferSubData;
	typedef void (APIENTRYP _PfnGlGetUniformIndices)(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices);
	_PfnGlGetUniformIndices _glGetUniformIndices;
	typedef void (APIENTRYP _PfnGlGetActiveUniformsiv)(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
	_PfnGlGetActiveUniformsiv _glGetActiveUniformsiv;
	typedef void (APIENTRYP _PfnGlGetActiveUniformName)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);
	_PfnGlGetActiveUniformName _glGetActiveUniformName;
	typedef GLuint (APIENTRYP _PfnGlGetUniformBlockIndex)(GLuint program, const GLchar *uniformBlockName);
	_PfnGlGetUniformBlockIndex _glGetUniformBlockIndex;
	typedef void (APIENTRYP _PfnGlGetActiveUniformBlockiv)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
	_PfnGlGetActiveUniformBlockiv _glGetActiveUniformBlockiv;
	typedef void (APIENTRYP _PfnGlGetActiveUniformBlockName)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
	_PfnGlGetActiveUniformBlockName _glGetActiveUniformBlockName;
	typedef void (APIENTRYP _PfnGlUniformBlockBinding)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
	_PfnGlUniformBlockBinding _glUniformBlockBinding;

	// GL_VERSION_3_2

	typedef void (APIENTRYP _PfnGlDrawElementsBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
	_PfnGlDrawElementsBaseVertex _glDrawElementsBaseVertex;
	typedef void (APIENTRYP _PfnGlDrawRangeElementsBaseVertex)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
	_PfnGlDrawRangeElementsBaseVertex _glDrawRangeElementsBaseVertex;
	typedef void (APIENTRYP _PfnGlDrawElementsInstancedBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);
	_PfnGlDrawElementsInstancedBaseVertex _glDrawElementsInstancedBaseVertex;
	typedef void (APIENTRYP _PfnGlMultiDrawElementsBaseVertex)(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount, const GLint *basevertex);
	_PfnGlMultiDrawElementsBaseVertex _glMultiDrawElementsBaseVertex;
	typedef void (APIENTRYP _PfnGlProvokingVertex)(GLenum mode);
	_PfnGlProvokingVertex _glProvokingVertex;
	typedef GLsync (APIENTRYP _PfnGlFenceSync)(GLenum condition, GLbitfield flags);
	_PfnGlFenceSync _glFenceSync;
	typedef GLboolean (APIENTRYP _PfnGlIsSync)(GLsync sync);
	_PfnGlIsSync _glIsSync;
	typedef void (APIENTRYP _PfnGlDeleteSync)(GLsync sync);
	_PfnGlDeleteSync _glDeleteSync;
	typedef GLenum (APIENTRYP _PfnGlClientWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
	_PfnGlClientWaitSync _glClientWaitSync;
	typedef void (APIENTRYP _PfnGlWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
	_PfnGlWaitSync _glWaitSync;
	typedef void (APIENTRYP _PfnGlGetInteger64v)(GLenum pname, GLint64 *data);
	_PfnGlGetInteger64v _glGetInteger64v;
	typedef void (APIENTRYP _PfnGlGetSynciv)(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);
	_PfnGlGetSynciv _glGetSynciv;
	typedef void (APIENTRYP _PfnGlGetInteger64i_v)(GLenum target, GLuint index, GLint64 *data);
	_PfnGlGetInteger64i_v _glGetInteger64i_v;
	typedef void (APIENTRYP _PfnGlGetBufferParameteri64v)(GLenum target, GLenum pname, GLint64 *params);
	_PfnGlGetBufferParameteri64v _glGetBufferParameteri64v;
	typedef void (APIENTRYP _PfnGlFramebufferTexture)(GLenum target, GLenum attachment, GLuint texture, GLint level);
	_PfnGlFramebufferTexture _glFramebufferTexture;
	typedef void (APIENTRYP _PfnGlTexImage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
	_PfnGlTexImage2DMultisample _glTexImage2DMultisample;
	typedef void (APIENTRYP _PfnGlTexImage3DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
	_PfnGlTexImage3DMultisample _glTexImage3DMultisample;
	typedef void (APIENTRYP _PfnGlGetMultisamplefv)(GLenum pname, GLuint index, GLfloat *val);
	_PfnGlGetMultisamplefv _glGetMultisamplefv;
	typedef void (APIENTRYP _PfnGlSampleMaski)(GLuint maskNumber, GLbitfield mask);
	_PfnGlSampleMaski _glSampleMaski;

	// GL_ARB_debug_output

	typedef void (APIENTRYP _PfnGlDebugMessageControlARB)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
	_PfnGlDebugMessageControlARB _glDebugMessageControlARB;
	typedef void (APIENTRYP _PfnGlDebugMessageInsertARB)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
	_PfnGlDebugMessageInsertARB _glDebugMessageInsertARB;
	typedef void (APIENTRYP _PfnGlDebugMessageCallbackARB)(GLDEBUGPROCARB callback, const void *userParam);
	_PfnGlDebugMessageCallbackARB _glDebugMessageCallbackARB;
	typedef GLuint (APIENTRYP _PfnGlGetDebugMessageLogARB)(GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
	_PfnGlGetDebugMessageLogARB _glGetDebugMessageLogARB;

private:
	void* _getProcAddress(const char* proc);

private:
	Logger*  _log;

	bool     _abortOnError;
	bool     _logErrors;

	bool     _logCalls;
	LogLevel _logLevel;

	GlGetProcAddress _glGetProcAddress;
	unsigned _procCount;
};

}

#endif
