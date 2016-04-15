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


#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glext.h>
#else
#  define GLEW_NO_GLU
#  include <GL/glew.h>
#endif

#include "lair/render_gl2/gl.h"

#include "lair/render_gl2/context.h"


#ifdef NDEBUG
	#define LAIR_GL2_LOG_CALL(...)
	#define LAIR_GL2_CHECK_GL_CALL()
#else
	#define LAIR_GL2_LOG_CALL(...) do { if(_logCalls) { _log->write(_logLevel, __VA_ARGS__); } } while(false)
	#define LAIR_GL2_CHECK_GL_CALL() LAIR_LOG_OPENGL_ERRORS(this)
#endif

namespace lair
{


Context::Context(Logger* logger)
	: _log(logger),
	  _logCalls(false),
	  _logLevel(LogLevel::Info) {
}


bool Context::initialize() {
#ifndef __APPLE__
	GLenum err = glewInit();
	if(err != GLEW_OK) {
		log().error("GLEW initialization failed: ", glewGetErrorString(err), ".");
		return false;
	}

	if(!GL_VERSION_2_1) {
		log().error("OpenGL 2.1 is not supported.");
		return false;
	}
#endif

	log().info("OpenGL version: ",      getString(GL_VERSION));
	log().info("OpenGL GLSL version: ", getString(GL_SHADING_LANGUAGE_VERSION));
	log().info("OpenGL vendor: ",       getString(GL_VENDOR));
	log().info("OpenGL renderer: ",     getString(GL_RENDERER));

	return true;
}


void Context::shutdown() {
}


Logger& Context::log() {
	return *_log;
}


//void Context::setFromGl() {
//	int vec4i[4];
//	GLboolean vec4b[4];

//	// Implementation dependant values
//	int maxVertexAttrib;
//	int maxTextureUnits;
//	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttrib);
//	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

//	// Vertex array data
//	vertexAttribArrayEnabled      .resize(maxVertexAttrib);
//	vertexAttribArraySize         .resize(maxVertexAttrib);
//	vertexAttribArrayStride       .resize(maxVertexAttrib);
//	vertexAttribArrayType         .resize(maxVertexAttrib);
//	vertexAttribArrayNormalized   .resize(maxVertexAttrib);
//	vertexAttribArrayBufferBinding.resize(maxVertexAttrib);
//	for(int i = 0; i < maxVertexAttrib; ++i) {
//		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED,        &vertexAttribArrayEnabled[i]);
//		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE,           &vertexAttribArraySize[i]);
//		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE,         &vertexAttribArrayStride[i]);
//		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE,           &vertexAttribArrayType[i]);
//		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,     &vertexAttribArrayNormalized[i]);
//		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vertexAttribArrayBufferBinding[i]);
//	}
//	glGetIntegerv(GL_ARRAY_BUFFER_BINDING,         &arrayBufferBinding);
//	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding);

//	// Transformation
//	// TODO: check viewport initial value;
//	glGetIntegerv(GL_VIEWPORT, vec4i);
//	viewport = Box2i(Vector2i(vec4i[0],            vec4i[1]),
//					 Vector2i(vec4i[0] + vec4i[2], vec4i[1] + vec4i[3]));
//	glGetFloatv(GL_DEPTH_RANGE, depthRange.data());

//	// Rasterization
//	glGetFloatv(GL_LINE_WIDTH, &lineWidth);
//	cullFace = glIsEnabled(GL_CULL_FACE);
//	glGetIntegerv(GL_CULL_FACE_MODE, &frontFace);
//	glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &polygonOffsetFactor);
//	glGetFloatv(GL_POLYGON_OFFSET_UNITS, &polygonOffsetUnits);
//	polygonOffsetFill = glIsEnabled(GL_POLYGON_OFFSET_FILL);

//	// Multisampling
//	sampleAlphaToCoverage = glIsEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE);
//	sampleCoverage = glIsEnabled(GL_SAMPLE_COVERAGE);
//	glGetFloatv(GL_SAMPLE_COVERAGE_VALUE, &sampleCoverageValue);
//	glGetBooleanv(GL_SAMPLE_COVERAGE_INVERT, vec4b);
//	sampleCoverageInvert = vec4b[0];

//	// Texture environment and generation
//	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

//	// Textures
//	textureBinding2D     .resize(maxTextureUnits);
//	textureBindingCubeMap.resize(maxTextureUnits);
//	for(int i = 0; i < maxTextureUnits; ++i) {
//		glActiveTexture(GL_TEXTURE0 + i);
//		glGetIntegerv(GL_TEXTURE_BINDING_2D,       &textureBinding2D[i]);
//		glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &textureBindingCubeMap[i]);
//	}
//	glActiveTexture(activeTexture);

//	// Pixel operations
//	scissorTest = glIsEnabled(GL_SCISSOR_TEST);
//	glGetIntegerv(GL_SCISSOR_BOX, vec4i);
//	scissorBox = Box2i(Vector2i(vec4i[0],            vec4i[1]),
//					   Vector2i(vec4i[0] + vec4i[2], vec4i[1] + vec4i[3]));
//	stencilTest = glIsEnabled(GL_STENCIL_TEST);
//	glGetIntegerv(GL_STENCIL_FUNC,                 &stencilFunc);
//	glGetIntegerv(GL_STENCIL_VALUE_MASK,           &stencilValueMask);
//	glGetIntegerv(GL_STENCIL_REF,                  &stencilRef);
//	glGetIntegerv(GL_STENCIL_FAIL,                 &stencilFail);
//	glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL,      &stencilPassDepthFail);
//	glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS,      &stencilPassDepthPass);
//	glGetIntegerv(GL_STENCIL_BACK_FUNC,            &stencilBackFunc);
//	glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK,      &stencilBackValueMask);
//	glGetIntegerv(GL_STENCIL_BACK_REF,             &stencilBackRef);
//	glGetIntegerv(GL_STENCIL_BACK_FAIL,            &stencilFail);
//	glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &stencilBackPassDepthFail);
//	glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &stencilBackPassDepthPass);
//	depthTest   = glIsEnabled(GL_DEPTH_TEST);
//	glGetIntegerv(GL_DEPTH_FUNC,                   &depthFunc);
//	blend       = glIsEnabled(GL_BLEND);
//	glGetIntegerv(GL_BLEND_SRC_RGB,                &blendSrcRgb);
//	glGetIntegerv(GL_BLEND_SRC_ALPHA,              &blendSrcAlpha);
//	glGetIntegerv(GL_BLEND_DST_RGB,                &blendDstRgb);
//	glGetIntegerv(GL_BLEND_DST_ALPHA,              &blendDstAlpha);
//	glGetIntegerv(GL_BLEND_EQUATION_RGB,           &blendEquationRgb);
//	glGetIntegerv(GL_BLEND_EQUATION_ALPHA,         &blendEquationAlpha);
//	dither      = glIsEnabled(GL_DITHER);

//	// Framebuffer control
//	glGetBooleanv(GL_COLOR_WRITEMASK,        vec4b);
//	for(int i = 0; i < 4; ++i) colorWritemask[i] = vec4b[i];
//	glGetBooleanv(GL_DEPTH_WRITEMASK,        vec4b);
//	depthWritemask = vec4b[0];
//	glGetIntegerv(GL_STENCIL_WRITEMASK,      &stencilWritemask);
//	glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &stencilBackWritemask);
//	glGetFloatv(  GL_COLOR_CLEAR_VALUE,      colorClearValue.data());
//	glGetFloatv(  GL_DEPTH_CLEAR_VALUE,      &depthClearValue);
//	glGetIntegerv(GL_STENCIL_CLEAR_VALUE,    &stencilClearValue);

//	// Pixels
//	glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpackAlignment);
//	glGetIntegerv(GL_PACK_ALIGNMENT,   &packAlignment);

//	// Program object state
//	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

//	// Vertex shader state
//	for(int i = 0; i < maxVertexAttrib; ++i) {
//		glGetVertexAttribfv(i, GL_CURRENT_VERTEX_ATTRIB, currentVertexAttrib[i].data());
//	}

//	// Hints
//	glGetIntegerv(GL_GENERATE_MIPMAP_HINT, &generateMipmapHint);

//	// Renderbuffer state
//	glGetIntegerv(GL_RENDERBUFFER_BINDING, &renderbufferBinding);

//	// Framebuffer state
//	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebufferBinding);
//}


void Context::setEnabled(GLenum cap, bool enabled) {
	if(enabled) {
		enable(cap);
	} else {
		disable(cap);
	}
}


GLenum Context::getError() {
	LAIR_GL2_LOG_CALL("glGetError()");
	GLenum ret = ::glGetError();
	return ret;
}

void Context::genBuffers(GLsizei  n, GLuint* buffers) {
	LAIR_GL2_LOG_CALL("glGenBuffers(", n, ", ", buffers, ")");
	::glGenBuffers(n, buffers);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::deleteBuffers(GLsizei  n, GLuint* buffers) {
	LAIR_GL2_LOG_CALL("glDeleteBuffers(", n, ", ", buffers, ")");
	::glDeleteBuffers(n, buffers);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::bindBuffer(GLenum  target, GLuint  buffer) {
	LAIR_GL2_LOG_CALL("glBindBuffer(", target, ", ", buffer, ")");
	::glBindBuffer(target, buffer);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::bufferData(GLenum  target, GLsizeiptr  size, const void* data, GLenum  usage) {
	LAIR_GL2_LOG_CALL("glBufferData(", target, ", ", size, ", ", data, ", ", usage, ")");
	::glBufferData(target, size, data, usage);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::bufferSubData(GLenum  target, GLintptr  offset, GLsizeiptr  size, const void* data) {
	LAIR_GL2_LOG_CALL("glBufferSubData(", target, ", ", offset, ", ", size, ", ", data, ")");
	::glBufferSubData(target, offset, size, data);
	LAIR_GL2_CHECK_GL_CALL();
}
GLboolean Context::isBuffer(GLuint  buffer) {
	LAIR_GL2_LOG_CALL("glIsBuffer(", buffer, ")");
	GLboolean ret = ::glIsBuffer(buffer);
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}
void Context::getBufferParameteriv(GLenum  target, GLenum  pname, GLint* params) {
	LAIR_GL2_LOG_CALL("glGetBufferParameteriv(", target, ", ", pname, ", ", params, ")");
	::glGetBufferParameteriv(target, pname, params);
	LAIR_GL2_CHECK_GL_CALL();
}

//void Context::depthRangef(GLfloat  n, GLfloat  f) {
//	LAIR_GL2_LOG_CALL("glDepthRangef(", n, ", ", f, ")");
//	::glDepthRangef(n, f);
//	LAIR_GL2_CHECK_GL_CALL();
//}
void Context::viewport(GLint  x, GLint  y, GLsizei  width, GLsizei  height) {
	LAIR_GL2_LOG_CALL("glViewport(", x, ", ", y, ", ", width, ", ", height, ")");
	::glViewport(x, y, width, height);
	LAIR_GL2_CHECK_GL_CALL();
}

void Context::readPixels(GLint  x, GLint  y, GLsizei  width, GLsizei  height, GLenum  format, GLenum  type, void* pixels) {
	LAIR_GL2_LOG_CALL("glReadPixels(", x, ", ", y, ", ", width, ", ", height, ", ", format, ", ", type, ", ", pixels, ")");
	::glReadPixels(x, y, width, height, format, type, pixels);
	LAIR_GL2_CHECK_GL_CALL();
}

void Context::activeTexture(GLenum  texture) {
	LAIR_GL2_LOG_CALL("glActiveTexture(", texture, ")");
	::glActiveTexture(texture);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::texImage2D(GLenum  target, GLint  level, GLint  internalformat, GLsizei  width, GLsizei  height, GLint  border, GLenum  format, GLenum  type, const void* pixels) {
	LAIR_GL2_LOG_CALL("glTexImage2D(", target, ", ", level, ", ", internalformat, ", ", width, ", ", height, ", ", border, ", ", format, ", ", type, ", ", pixels, ")");
	::glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::copyTexImage2D(GLenum  target, GLint  level, GLenum  internalformat, GLint  x, GLint  y, GLsizei  width, GLsizei  height, GLint  border) {
	LAIR_GL2_LOG_CALL("glCopyTexImage2D(", target, ", ", level, ", ", internalformat, ", ", x, ", ", y, ", ", width, ", ", height, ", ", border, ")");
	::glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::texSubImage2D(GLenum  target, GLint  level, GLint  xoffset, GLint  yoffset, GLsizei  width, GLsizei  height, GLenum  format, GLenum  type, const void* pixels) {
	LAIR_GL2_LOG_CALL("glTexSubImage2D(", target, ", ", level, ", ", xoffset, ", ", yoffset, ", ", width, ", ", height, ", ", format, ", ", type, ", ", pixels, ")");
	::glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::copyTexSubImage2D(GLenum  target, GLint  level, GLint  xoffset, GLint  yoffset, GLint  x, GLint  y, GLsizei  width, GLsizei  height) {
	LAIR_GL2_LOG_CALL("glCopyTexSubImage2D(", target, ", ", level, ", ", xoffset, ", ", yoffset, ", ", x, ", ", y, ", ", width, ", ", height, ")");
	::glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::compressedTexImage2D(GLenum  target, GLint  level, GLenum  internalformat, GLsizei  width, GLsizei  height, GLint  border, GLsizei  imageSize, const void* data) {
	LAIR_GL2_LOG_CALL("glCompressedTexImage2D(", target, ", ", level, ", ", internalformat, ", ", width, ", ", height, ", ", border, ", ", imageSize, ", ", data, ")");
	::glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::compressedTexSubImage2D(GLenum  target, GLint  level, GLint  xoffset, GLint  yoffset, GLsizei  width, GLsizei  height, GLenum  format, GLsizei  imageSize, const void* data) {
	LAIR_GL2_LOG_CALL("glCompressedTexSubImage2D(", target, ", ", level, ", ", xoffset, ", ", yoffset, ", ", width, ", ", height, ", ", format, ", ", imageSize, ", ", data, ")");
	::glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::texParameterf(GLenum  target, GLenum  pname, GLfloat  param) {
	LAIR_GL2_LOG_CALL("glTexParameterf(", target, ", ", pname, ", ", param, ")");
	::glTexParameterf(target, pname, param);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::texParameterfv(GLenum  target, GLenum  pname, GLfloat* params) {
	LAIR_GL2_LOG_CALL("glTexParameterfv(", target, ", ", pname, ", ", params, ")");
	::glTexParameterfv(target, pname, params);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::texParameteri(GLenum  target, GLenum  pname, GLint  param) {
	LAIR_GL2_LOG_CALL("glTexParameteri(", target, ", ", pname, ", ", param, ")");
	::glTexParameteri(target, pname, param);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::texParameteriv(GLenum  target, GLenum  pname, GLint* params) {
	LAIR_GL2_LOG_CALL("glTexParameteriv(", target, ", ", pname, ", ", params, ")");
	::glTexParameteriv(target, pname, params);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::generateMipmap(GLenum  target) {
	LAIR_GL2_LOG_CALL("glGenerateMipmap(", target, ")");
	::glGenerateMipmap(target);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::bindTexture(GLenum  target, GLuint  texture) {
	LAIR_GL2_LOG_CALL("glBindTexture(", target, ", ", texture, ")");
	::glBindTexture(target, texture);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::deleteTextures(GLsizei  n, GLuint* textures) {
	LAIR_GL2_LOG_CALL("glDeleteTextures(", n, ", ", textures, ")");
	::glDeleteTextures(n, textures);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::genTextures(GLsizei  n, GLuint* textures) {
	LAIR_GL2_LOG_CALL("glGenTextures(", n, ", ", textures, ")");
	::glGenTextures(n, textures);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getTexParameterfv(GLenum  target, GLenum  pname, GLfloat* params) {
	LAIR_GL2_LOG_CALL("glGetTexParameterfv(", target, ", ", pname, ", ", params, ")");
	::glGetTexParameterfv(target, pname, params);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getTexParameteriv(GLenum  target, GLenum  pname, GLint* params) {
	LAIR_GL2_LOG_CALL("glGetTexParameteriv(", target, ", ", pname, ", ", params, ")");
	::glGetTexParameteriv(target, pname, params);
	LAIR_GL2_CHECK_GL_CALL();
}
GLboolean Context::isTexture(GLuint  texture) {
	LAIR_GL2_LOG_CALL("glIsTexture(", texture, ")");
	GLboolean ret = ::glIsTexture(texture);
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}

void Context::vertexAttrib1f(GLuint  index, GLfloat  x) {
	LAIR_GL2_LOG_CALL("glVertexAttrib1f(", index, ", ", x, ")");
	::glVertexAttrib1f(index, x);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::vertexAttrib1fv(GLuint  index, GLfloat* v) {
	LAIR_GL2_LOG_CALL("glVertexAttrib1fv(", index, ", ", v, ")");
	::glVertexAttrib1fv(index, v);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::vertexAttrib2f(GLuint  index, GLfloat  x, GLfloat  y) {
	LAIR_GL2_LOG_CALL("glVertexAttrib2f(", index, ", ", x, ", ", y, ")");
	::glVertexAttrib2f(index, x, y);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::vertexAttrib2fv(GLuint  index, GLfloat* v) {
	LAIR_GL2_LOG_CALL("glVertexAttrib2fv(", index, ", ", v, ")");
	::glVertexAttrib2fv(index, v);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::vertexAttrib3f(GLuint  index, GLfloat  x, GLfloat  y, GLfloat  z) {
	LAIR_GL2_LOG_CALL("glVertexAttrib3f(", index, ", ", x, ", ", y, ", ", z, ")");
	::glVertexAttrib3f(index, x, y, z);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::vertexAttrib3fv(GLuint  index, GLfloat* v) {
	LAIR_GL2_LOG_CALL("glVertexAttrib3fv(", index, ", ", v, ")");
	::glVertexAttrib3fv(index, v);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::vertexAttrib4f(GLuint  index, GLfloat  x, GLfloat  y, GLfloat  z, GLfloat  w) {
	LAIR_GL2_LOG_CALL("glVertexAttrib4f(", index, ", ", x, ", ", y, ", ", z, ", ", w, ")");
	::glVertexAttrib4f(index, x, y, z, w);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::vertexAttrib4fv(GLuint  index, GLfloat* v) {
	LAIR_GL2_LOG_CALL("glVertexAttrib4fv(", index, ", ", v, ")");
	::glVertexAttrib4fv(index, v);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::vertexAttribPointer(GLuint  index, GLint  size, GLenum  type, GLboolean  normalized, GLsizei  stride, const void* pointer) {
	LAIR_GL2_LOG_CALL("glVertexAttribPointer(", index, ", ", size, ", ", type, ", ", normalized, ", ", stride, ", ", pointer, ")");
	::glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::enableVertexAttribArray(GLuint  index) {
	LAIR_GL2_LOG_CALL("glEnableVertexAttribArray(", index, ")");
	::glEnableVertexAttribArray(index);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::disableVertexAttribArray(GLuint  index) {
	LAIR_GL2_LOG_CALL("glDisableVertexAttribArray(", index, ")");
	::glDisableVertexAttribArray(index);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::drawArrays(GLenum  mode, GLint  first, GLsizei  count) {
	LAIR_GL2_LOG_CALL("glDrawArrays(", mode, ", ", first, ", ", count, ")");
	::glDrawArrays(mode, first, count);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::drawElements(GLenum  mode, GLsizei  count, GLenum  type, const void* indices) {
	LAIR_GL2_LOG_CALL("glDrawElements(", mode, ", ", count, ", ", type, ", ", indices, ")");
	::glDrawElements(mode, count, type, indices);
	LAIR_GL2_CHECK_GL_CALL();
}

void Context::lineWidth(GLfloat  width) {
	LAIR_GL2_LOG_CALL("glLineWidth(", width, ")");
	::glLineWidth(width);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::frontFace(GLenum  mode) {
	LAIR_GL2_LOG_CALL("glFrontFace(", mode, ")");
	::glFrontFace(mode);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::cullFace(GLenum  mode) {
	LAIR_GL2_LOG_CALL("glCullFace(", mode, ")");
	::glCullFace(mode);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::polygonOffset(GLfloat  factor, GLfloat  units) {
	LAIR_GL2_LOG_CALL("glPolygonOffset(", factor, ", ", units, ")");
	::glPolygonOffset(factor, units);
	LAIR_GL2_CHECK_GL_CALL();
}

void Context::pixelStorei(GLenum  pname, GLint  param) {
	LAIR_GL2_LOG_CALL("glPixelStorei(", pname, ", ", param, ")");
	::glPixelStorei(pname, param);
	LAIR_GL2_CHECK_GL_CALL();
}

GLuint Context::createShader(GLenum  type) {
	LAIR_GL2_LOG_CALL("glCreateShader(", type, ")");
	GLuint ret = ::glCreateShader(type);
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}
void Context::shaderSource(GLuint  shader, GLsizei  count, GLchar*const* string, GLint* length) {
	LAIR_GL2_LOG_CALL("glShaderSource(", shader, ", ", count, ", ", string, ", ", length, ")");
	::glShaderSource(shader, count, const_cast<const char**>(string), length);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::compileShader(GLuint  shader) {
	LAIR_GL2_LOG_CALL("glCompileShader(", shader, ")");
	::glCompileShader(shader);
	LAIR_GL2_CHECK_GL_CALL();
}
//void Context::releaseShaderCompiler() {
//	LAIR_GL2_LOG_CALL("glReleaseShaderCompiler()");
//	::glReleaseShaderCompiler();
//	LAIR_GL2_CHECK_GL_CALL();
//}
void Context::deleteShader(GLuint  shader) {
	LAIR_GL2_LOG_CALL("glDeleteShader(", shader, ")");
	::glDeleteShader(shader);
	LAIR_GL2_CHECK_GL_CALL();
}
//void Context::shaderBinary(GLsizei  count, GLuint* shaders, GLenum  binaryformat, const void* binary, GLsizei  length) {
//	LAIR_GL2_LOG_CALL("glShaderBinary(", count, ", ", shaders, ", ", binaryformat, ", ", binary, ", ", length, ")");
//	::glShaderBinary(count, shaders, binaryformat, binary, length);
//	LAIR_GL2_CHECK_GL_CALL();
//}
GLuint Context::createProgram() {
	LAIR_GL2_LOG_CALL("glCreateProgram()");
	GLuint ret = ::glCreateProgram();
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}
void Context::attachShader(GLuint  program, GLuint  shader) {
	LAIR_GL2_LOG_CALL("glAttachShader(", program, ", ", shader, ")");
	::glAttachShader(program, shader);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::detachShader(GLuint  program, GLuint  shader) {
	LAIR_GL2_LOG_CALL("glDetachShader(", program, ", ", shader, ")");
	::glDetachShader(program, shader);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::linkProgram(GLuint  program) {
	LAIR_GL2_LOG_CALL("glLinkProgram(", program, ")");
	::glLinkProgram(program);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::useProgram(GLuint  program) {
	LAIR_GL2_LOG_CALL("glUseProgram(", program, ")");
	::glUseProgram(program);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::deleteProgram(GLuint  program) {
	LAIR_GL2_LOG_CALL("glDeleteProgram(", program, ")");
	::glDeleteProgram(program);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getActiveAttrib(GLuint  program, GLuint  index, GLsizei  bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) {
	LAIR_GL2_LOG_CALL("glGetActiveAttrib(", program, ", ", index, ", ", bufSize, ", ", length, ", ", size, ", ", type, ", ", name, ")");
	::glGetActiveAttrib(program, index, bufSize, length, size, type, name);
	LAIR_GL2_CHECK_GL_CALL();
}
GLint Context::getAttribLocation(GLuint  program, GLchar* name) {
	LAIR_GL2_LOG_CALL("glGetAttribLocation(", program, ", ", name, ")");
	GLint ret = ::glGetAttribLocation(program, name);
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}
void Context::bindAttribLocation(GLuint  program, GLuint  index, GLchar* name) {
	LAIR_GL2_LOG_CALL("glBindAttribLocation(", program, ", ", index, ", ", name, ")");
	::glBindAttribLocation(program, index, name);
	LAIR_GL2_CHECK_GL_CALL();
}
GLint Context::getUniformLocation(GLuint  program, GLchar* name) {
	LAIR_GL2_LOG_CALL("glGetUniformLocation(", program, ", ", name, ")");
	GLint ret = ::glGetUniformLocation(program, name);
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}
void Context::getActiveUniform(GLuint  program, GLuint  index, GLsizei  bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) {
	LAIR_GL2_LOG_CALL("glGetActiveUniform(", program, ", ", index, ", ", bufSize, ", ", length, ", ", size, ", ", type, ", ", name, ")");
	::glGetActiveUniform(program, index, bufSize, length, size, type, name);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform1f(GLint  location, GLfloat  v0) {
	LAIR_GL2_LOG_CALL("glUniform1f(", location, ", ", v0, ")");
	::glUniform1f(location, v0);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform1fv(GLint  location, GLsizei  count, GLfloat* value) {
	LAIR_GL2_LOG_CALL("glUniform1fv(", location, ", ", count, ", ", value, ")");
	::glUniform1fv(location, count, value);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform1i(GLint  location, GLint  v0) {
	LAIR_GL2_LOG_CALL("glUniform1i(", location, ", ", v0, ")");
	::glUniform1i(location, v0);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform1iv(GLint  location, GLsizei  count, GLint* value) {
	LAIR_GL2_LOG_CALL("glUniform1iv(", location, ", ", count, ", ", value, ")");
	::glUniform1iv(location, count, value);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform2f(GLint  location, GLfloat  v0, GLfloat  v1) {
	LAIR_GL2_LOG_CALL("glUniform2f(", location, ", ", v0, ", ", v1, ")");
	::glUniform2f(location, v0, v1);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform2fv(GLint  location, GLsizei  count, GLfloat* value) {
	LAIR_GL2_LOG_CALL("glUniform2fv(", location, ", ", count, ", ", value, ")");
	::glUniform2fv(location, count, value);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform2i(GLint  location, GLint  v0, GLint  v1) {
	LAIR_GL2_LOG_CALL("glUniform2i(", location, ", ", v0, ", ", v1, ")");
	::glUniform2i(location, v0, v1);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform2iv(GLint  location, GLsizei  count, GLint* value) {
	LAIR_GL2_LOG_CALL("glUniform2iv(", location, ", ", count, ", ", value, ")");
	::glUniform2iv(location, count, value);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform3f(GLint  location, GLfloat  v0, GLfloat  v1, GLfloat  v2) {
	LAIR_GL2_LOG_CALL("glUniform3f(", location, ", ", v0, ", ", v1, ", ", v2, ")");
	::glUniform3f(location, v0, v1, v2);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform3fv(GLint  location, GLsizei  count, GLfloat* value) {
	LAIR_GL2_LOG_CALL("glUniform3fv(", location, ", ", count, ", ", value, ")");
	::glUniform3fv(location, count, value);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform3i(GLint  location, GLint  v0, GLint  v1, GLint  v2) {
	LAIR_GL2_LOG_CALL("glUniform3i(", location, ", ", v0, ", ", v1, ", ", v2, ")");
	::glUniform3i(location, v0, v1, v2);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform3iv(GLint  location, GLsizei  count, GLint* value) {
	LAIR_GL2_LOG_CALL("glUniform3iv(", location, ", ", count, ", ", value, ")");
	::glUniform3iv(location, count, value);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform4f(GLint  location, GLfloat  v0, GLfloat  v1, GLfloat  v2, GLfloat  v3) {
	LAIR_GL2_LOG_CALL("glUniform4f(", location, ", ", v0, ", ", v1, ", ", v2, ", ", v3, ")");
	::glUniform4f(location, v0, v1, v2, v3);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform4fv(GLint  location, GLsizei  count, GLfloat* value) {
	LAIR_GL2_LOG_CALL("glUniform4fv(", location, ", ", count, ", ", value, ")");
	::glUniform4fv(location, count, value);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform4i(GLint  location, GLint  v0, GLint  v1, GLint  v2, GLint  v3) {
	LAIR_GL2_LOG_CALL("glUniform4i(", location, ", ", v0, ", ", v1, ", ", v2, ", ", v3, ")");
	::glUniform4i(location, v0, v1, v2, v3);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniform4iv(GLint  location, GLsizei  count, GLint* value) {
	LAIR_GL2_LOG_CALL("glUniform4iv(", location, ", ", count, ", ", value, ")");
	::glUniform4iv(location, count, value);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniformMatrix2fv(GLint  location, GLsizei  count, GLboolean  transpose, GLfloat* value) {
	LAIR_GL2_LOG_CALL("glUniformMatrix2fv(", location, ", ", count, ", ", transpose, ", ", value, ")");
	::glUniformMatrix2fv(location, count, transpose, value);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniformMatrix3fv(GLint  location, GLsizei  count, GLboolean  transpose, GLfloat* value) {
	LAIR_GL2_LOG_CALL("glUniformMatrix3fv(", location, ", ", count, ", ", transpose, ", ", value, ")");
	::glUniformMatrix3fv(location, count, transpose, value);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::uniformMatrix4fv(GLint  location, GLsizei  count, GLboolean  transpose, GLfloat* value) {
	LAIR_GL2_LOG_CALL("glUniformMatrix4fv(", location, ", ", count, ", ", transpose, ", ", value, ")");
	::glUniformMatrix4fv(location, count, transpose, value);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::validateProgram(GLuint  program) {
	LAIR_GL2_LOG_CALL("glValidateProgram(", program, ")");
	::glValidateProgram(program);
	LAIR_GL2_CHECK_GL_CALL();
}
GLboolean Context::isShader(GLuint  shader) {
	LAIR_GL2_LOG_CALL("glIsShader(", shader, ")");
	GLboolean ret = ::glIsShader(shader);
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}
void Context::getShaderiv(GLuint  shader, GLenum  pname, GLint* params) {
	LAIR_GL2_LOG_CALL("glGetShaderiv(", shader, ", ", pname, ", ", params, ")");
	::glGetShaderiv(shader, pname, params);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getAttachedShaders(GLuint  program, GLsizei  maxCount, GLsizei* count, GLuint* shaders) {
	LAIR_GL2_LOG_CALL("glGetAttachedShaders(", program, ", ", maxCount, ", ", count, ", ", shaders, ")");
	::glGetAttachedShaders(program, maxCount, count, shaders);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getShaderInfoLog(GLuint  shader, GLsizei  bufSize, GLsizei* length, GLchar* infoLog) {
	LAIR_GL2_LOG_CALL("glGetShaderInfoLog(", shader, ", ", bufSize, ", ", length, ", ", infoLog, ")");
	::glGetShaderInfoLog(shader, bufSize, length, infoLog);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getShaderSource(GLuint  shader, GLsizei  bufSize, GLsizei* length, GLchar* source) {
	LAIR_GL2_LOG_CALL("glGetShaderSource(", shader, ", ", bufSize, ", ", length, ", ", source, ")");
	::glGetShaderSource(shader, bufSize, length, source);
	LAIR_GL2_CHECK_GL_CALL();
}
//void Context::getShaderPrecisionFormat(GLenum  shadertype, GLenum  precisiontype, GLint* range, GLint* precision) {
//	LAIR_GL2_LOG_CALL("glGetShaderPrecisionFormat(", shadertype, ", ", precisiontype, ", ", range, ", ", precision, ")");
//	::glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
//	LAIR_GL2_CHECK_GL_CALL();
//}
void Context::getVertexAttribfv(GLuint  index, GLenum  pname, GLfloat* params) {
	LAIR_GL2_LOG_CALL("glGetVertexAttribfv(", index, ", ", pname, ", ", params, ")");
	::glGetVertexAttribfv(index, pname, params);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getVertexAttribiv(GLuint  index, GLenum  pname, GLint* params) {
	LAIR_GL2_LOG_CALL("glGetVertexAttribiv(", index, ", ", pname, ", ", params, ")");
	::glGetVertexAttribiv(index, pname, params);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getVertexAttribPointerv(GLuint  index, GLenum  pname, void** pointer) {
	LAIR_GL2_LOG_CALL("glGetVertexAttribPointerv(", index, ", ", pname, ", ", pointer, ")");
	::glGetVertexAttribPointerv(index, pname, pointer);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getUniformfv(GLuint  program, GLint  location, GLfloat* params) {
	LAIR_GL2_LOG_CALL("glGetUniformfv(", program, ", ", location, ", ", params, ")");
	::glGetUniformfv(program, location, params);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getUniformiv(GLuint  program, GLint  location, GLint* params) {
	LAIR_GL2_LOG_CALL("glGetUniformiv(", program, ", ", location, ", ", params, ")");
	::glGetUniformiv(program, location, params);
	LAIR_GL2_CHECK_GL_CALL();
}
GLboolean Context::isProgram(GLuint  program) {
	LAIR_GL2_LOG_CALL("glIsProgram(", program, ")");
	GLboolean ret = ::glIsProgram(program);
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}
void Context::getProgramiv(GLuint  program, GLenum  pname, GLint* params) {
	LAIR_GL2_LOG_CALL("glGetProgramiv(", program, ", ", pname, ", ", params, ")");
	::glGetProgramiv(program, pname, params);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getProgramInfoLog(GLuint  program, GLsizei  bufSize, GLsizei* length, GLchar* infoLog) {
	LAIR_GL2_LOG_CALL("glGetProgramInfoLog(", program, ", ", bufSize, ", ", length, ", ", infoLog, ")");
	::glGetProgramInfoLog(program, bufSize, length, infoLog);
	LAIR_GL2_CHECK_GL_CALL();
}

void Context::scissor(GLint  x, GLint  y, GLsizei  width, GLsizei  height) {
	LAIR_GL2_LOG_CALL("glScissor(", x, ", ", y, ", ", width, ", ", height, ")");
	::glScissor(x, y, width, height);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::sampleCoverage(GLfloat  value, GLboolean  invert) {
	LAIR_GL2_LOG_CALL("glSampleCoverage(", value, ", ", invert, ")");
	::glSampleCoverage(value, invert);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::stencilFunc(GLenum  func, GLint  ref, GLuint  mask) {
	LAIR_GL2_LOG_CALL("glStencilFunc(", func, ", ", ref, ", ", mask, ")");
	::glStencilFunc(func, ref, mask);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::stencilFuncSeparate(GLenum  face, GLenum  func, GLint  ref, GLuint  mask) {
	LAIR_GL2_LOG_CALL("glStencilFuncSeparate(", face, ", ", func, ", ", ref, ", ", mask, ")");
	::glStencilFuncSeparate(face, func, ref, mask);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::stencilOp(GLenum  fail, GLenum  zfail, GLenum  zpass) {
	LAIR_GL2_LOG_CALL("glStencilOp(", fail, ", ", zfail, ", ", zpass, ")");
	::glStencilOp(fail, zfail, zpass);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::stencilOpSeparate(GLenum  face, GLenum  sfail, GLenum  dpfail, GLenum  dppass) {
	LAIR_GL2_LOG_CALL("glStencilOpSeparate(", face, ", ", sfail, ", ", dpfail, ", ", dppass, ")");
	::glStencilOpSeparate(face, sfail, dpfail, dppass);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::depthFunc(GLenum  func) {
	LAIR_GL2_LOG_CALL("glDepthFunc(", func, ")");
	::glDepthFunc(func);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::blendEquation(GLenum  mode) {
	LAIR_GL2_LOG_CALL("glBlendEquation(", mode, ")");
	::glBlendEquation(mode);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::blendEquationSeparate(GLenum  modeRGB, GLenum  modeAlpha) {
	LAIR_GL2_LOG_CALL("glBlendEquationSeparate(", modeRGB, ", ", modeAlpha, ")");
	::glBlendEquationSeparate(modeRGB, modeAlpha);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::blendFunc(GLenum  sfactor, GLenum  dfactor) {
	LAIR_GL2_LOG_CALL("glBlendFunc(", sfactor, ", ", dfactor, ")");
	::glBlendFunc(sfactor, dfactor);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::blendFuncSeparate(GLenum  sfactorRGB, GLenum  dfactorRGB, GLenum  sfactorAlpha, GLenum  dfactorAlpha) {
	LAIR_GL2_LOG_CALL("glBlendFuncSeparate(", sfactorRGB, ", ", dfactorRGB, ", ", sfactorAlpha, ", ", dfactorAlpha, ")");
	::glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::blendColor(GLfloat  red, GLfloat  green, GLfloat  blue, GLfloat  alpha) {
	LAIR_GL2_LOG_CALL("glBlendColor(", red, ", ", green, ", ", blue, ", ", alpha, ")");
	::glBlendColor(red, green, blue, alpha);
	LAIR_GL2_CHECK_GL_CALL();
}

void Context::colorMask(GLboolean  red, GLboolean  green, GLboolean  blue, GLboolean  alpha) {
	LAIR_GL2_LOG_CALL("glColorMask(", red, ", ", green, ", ", blue, ", ", alpha, ")");
	::glColorMask(red, green, blue, alpha);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::depthMask(GLboolean  flag) {
	LAIR_GL2_LOG_CALL("glDepthMask(", flag, ")");
	::glDepthMask(flag);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::stencilMask(GLuint  mask) {
	LAIR_GL2_LOG_CALL("glStencilMask(", mask, ")");
	::glStencilMask(mask);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::stencilMaskSeparate(GLenum  face, GLuint  mask) {
	LAIR_GL2_LOG_CALL("glStencilMaskSeparate(", face, ", ", mask, ")");
	::glStencilMaskSeparate(face, mask);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::clear(GLbitfield  mask) {
	LAIR_GL2_LOG_CALL("glClear(", mask, ")");
	::glClear(mask);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::clearColor(GLfloat  red, GLfloat  green, GLfloat  blue, GLfloat  alpha) {
	LAIR_GL2_LOG_CALL("glClearColor(", red, ", ", green, ", ", blue, ", ", alpha, ")");
	::glClearColor(red, green, blue, alpha);
	LAIR_GL2_CHECK_GL_CALL();
}
//void Context::clearDepthf(GLfloat  d) {
//	LAIR_GL2_LOG_CALL("glClearDepthf(", d, ")");
//	::glClearDepthf(d);
//	LAIR_GL2_CHECK_GL_CALL();
//}
void Context::clearStencil(GLint  s) {
	LAIR_GL2_LOG_CALL("glClearStencil(", s, ")");
	::glClearStencil(s);
	LAIR_GL2_CHECK_GL_CALL();
}

void Context::flush() {
	LAIR_GL2_LOG_CALL("glFlush()");
	::glFlush();
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::finish() {
	LAIR_GL2_LOG_CALL("glFinish()");
	::glFinish();
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::hint(GLenum  target, GLenum  mode) {
	LAIR_GL2_LOG_CALL("glHint(", target, ", ", mode, ")");
	::glHint(target, mode);
	LAIR_GL2_CHECK_GL_CALL();
}

void Context::enable(GLenum  cap) {
	LAIR_GL2_LOG_CALL("glEnable(", cap, ")");
	::glEnable(cap);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::disable(GLenum  cap) {
	LAIR_GL2_LOG_CALL("glDisable(", cap, ")");
	::glDisable(cap);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getBooleanv(GLenum  pname, GLboolean* data) {
	LAIR_GL2_LOG_CALL("glGetBooleanv(", pname, ", ", data, ")");
	::glGetBooleanv(pname, data);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getIntegerv(GLenum  pname, GLint* data) {
	LAIR_GL2_LOG_CALL("glGetIntegerv(", pname, ", ", data, ")");
	::glGetIntegerv(pname, data);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::getFloatv(GLenum  pname, GLfloat* data) {
	LAIR_GL2_LOG_CALL("glGetFloatv(", pname, ", ", data, ")");
	::glGetFloatv(pname, data);
	LAIR_GL2_CHECK_GL_CALL();
}
GLboolean Context::isEnabled(GLenum  cap) {
	LAIR_GL2_LOG_CALL("glIsEnabled(", cap, ")");
	GLboolean ret = ::glIsEnabled(cap);
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}
const GLubyte* Context::getString(GLenum  name) {
	LAIR_GL2_LOG_CALL("glGetString(", name, ")");
	const GLubyte* ret = ::glGetString(name);
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}

void Context::bindFramebuffer(GLenum  target, GLuint  framebuffer) {
	LAIR_GL2_LOG_CALL("glBindFramebuffer(", target, ", ", framebuffer, ")");
	::glBindFramebuffer(target, framebuffer);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::deleteFramebuffers(GLsizei  n, GLuint* framebuffers) {
	LAIR_GL2_LOG_CALL("glDeleteFramebuffers(", n, ", ", framebuffers, ")");
	::glDeleteFramebuffers(n, framebuffers);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::genFramebuffers(GLsizei  n, GLuint* framebuffers) {
	LAIR_GL2_LOG_CALL("glGenFramebuffers(", n, ", ", framebuffers, ")");
	::glGenFramebuffers(n, framebuffers);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::bindRenderbuffer(GLenum  target, GLuint  renderbuffer) {
	LAIR_GL2_LOG_CALL("glBindRenderbuffer(", target, ", ", renderbuffer, ")");
	::glBindRenderbuffer(target, renderbuffer);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::deleteRenderbuffers(GLsizei  n, GLuint* renderbuffers) {
	LAIR_GL2_LOG_CALL("glDeleteRenderbuffers(", n, ", ", renderbuffers, ")");
	::glDeleteRenderbuffers(n, renderbuffers);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::genRenderbuffers(GLsizei  n, GLuint* renderbuffers) {
	LAIR_GL2_LOG_CALL("glGenRenderbuffers(", n, ", ", renderbuffers, ")");
	::glGenRenderbuffers(n, renderbuffers);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::renderbufferStorage(GLenum  target, GLenum  internalformat, GLsizei  width, GLsizei  height) {
	LAIR_GL2_LOG_CALL("glRenderbufferStorage(", target, ", ", internalformat, ", ", width, ", ", height, ")");
	::glRenderbufferStorage(target, internalformat, width, height);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::framebufferRenderbuffer(GLenum  target, GLenum  attachment, GLenum  renderbuffertarget, GLuint  renderbuffer) {
	LAIR_GL2_LOG_CALL("glFramebufferRenderbuffer(", target, ", ", attachment, ", ", renderbuffertarget, ", ", renderbuffer, ")");
	::glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
	LAIR_GL2_CHECK_GL_CALL();
}
void Context::framebufferTexture2D(GLenum  target, GLenum  attachment, GLenum  textarget, GLuint  texture, GLint  level) {
	LAIR_GL2_LOG_CALL("glFramebufferTexture2D(", target, ", ", attachment, ", ", textarget, ", ", texture, ", ", level, ")");
	::glFramebufferTexture2D(target, attachment, textarget, texture, level);
	LAIR_GL2_CHECK_GL_CALL();
}
GLenum Context::checkFramebufferStatus(GLenum  target) {
	LAIR_GL2_LOG_CALL("glCheckFramebufferStatus(", target, ")");
	GLenum ret = ::glCheckFramebufferStatus(target);
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}
GLboolean Context::isFramebuffer(GLuint  framebuffer) {
	LAIR_GL2_LOG_CALL("glIsFramebuffer(", framebuffer, ")");
	GLboolean ret = ::glIsFramebuffer(framebuffer);
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}
void Context::getFramebufferAttachmentParameteriv(GLenum  target, GLenum  attachment, GLenum  pname, GLint* params) {
	LAIR_GL2_LOG_CALL("glGetFramebufferAttachmentParameteriv(", target, ", ", attachment, ", ", pname, ", ", params, ")");
	::glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
	LAIR_GL2_CHECK_GL_CALL();
}
GLboolean Context::isRenderbuffer(GLuint  renderbuffer) {
	LAIR_GL2_LOG_CALL("glIsRenderbuffer(", renderbuffer, ")");
	GLboolean ret = ::glIsRenderbuffer(renderbuffer);
	LAIR_GL2_CHECK_GL_CALL();
	return ret;
}
void Context::getRenderbufferParameteriv(GLenum  target, GLenum  pname, GLint* params) {
	LAIR_GL2_LOG_CALL("glGetRenderbufferParameteriv(", target, ", ", pname, ", ", params, ")");
	::glGetRenderbufferParameteriv(target, pname, params);
	LAIR_GL2_CHECK_GL_CALL();
}


}
