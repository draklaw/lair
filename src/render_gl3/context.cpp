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


#include <sstream>

#include <lair/render_gl3/context.h>


namespace lair {


struct BitMask {
	GLbitfield  mask;
	const char* label;
};

String bitsetAsString(const BitMask* masks, GLbitfield value) {
	std::ostringstream out;
	int count = 0;
	while(masks->label) {
		if((value & masks->mask) == masks->mask) {
			value &= ~masks->mask;
			if(count) out << " | ";
			out << masks->label;
			++count;
		}
		++masks;
	}
	if(value) {
		if(count) out << " | ";
		out << "0x" << std::hex << value;
	}
	return out.str();
}


GLOutOfMemory::GLOutOfMemory()
	: std::runtime_error("out of graphic memory") {
}


Context::Context(GlGetProcAddress glGetProcAddress, Logger* logger):
	_gl_1_0(false),
	_gl_1_1(false),
	_gl_1_2(false),
	_gl_1_3(false),
	_gl_1_4(false),
	_gl_1_5(false),
	_gl_2_0(false),
	_gl_2_1(false),
	_gl_3_0(false),
	_gl_3_1(false),
	_gl_3_2(false),
	_gl_3_3(false),
	_gl_ext_texture_filter_anisotropic(false),
	_gl_khr_debug(false),
	_gl_arb_conservative_depth(false),
	_gl_arb_texture_storage(false),

	// GL_VERSION_1_0

	_glCullFace(nullptr),
	_glFrontFace(nullptr),
	_glHint(nullptr),
	_glLineWidth(nullptr),
	_glPointSize(nullptr),
	_glPolygonMode(nullptr),
	_glScissor(nullptr),
	_glTexParameterf(nullptr),
	_glTexParameterfv(nullptr),
	_glTexParameteri(nullptr),
	_glTexParameteriv(nullptr),
	_glTexImage1D(nullptr),
	_glTexImage2D(nullptr),
	_glDrawBuffer(nullptr),
	_glClear(nullptr),
	_glClearColor(nullptr),
	_glClearStencil(nullptr),
	_glClearDepth(nullptr),
	_glStencilMask(nullptr),
	_glColorMask(nullptr),
	_glDepthMask(nullptr),
	_glDisable(nullptr),
	_glEnable(nullptr),
	_glFinish(nullptr),
	_glFlush(nullptr),
	_glBlendFunc(nullptr),
	_glLogicOp(nullptr),
	_glStencilFunc(nullptr),
	_glStencilOp(nullptr),
	_glDepthFunc(nullptr),
	_glPixelStoref(nullptr),
	_glPixelStorei(nullptr),
	_glReadBuffer(nullptr),
	_glReadPixels(nullptr),
	_glGetBooleanv(nullptr),
	_glGetDoublev(nullptr),
	_glGetError(nullptr),
	_glGetFloatv(nullptr),
	_glGetIntegerv(nullptr),
	_glGetString(nullptr),
	_glGetTexImage(nullptr),
	_glGetTexParameterfv(nullptr),
	_glGetTexParameteriv(nullptr),
	_glGetTexLevelParameterfv(nullptr),
	_glGetTexLevelParameteriv(nullptr),
	_glIsEnabled(nullptr),
	_glDepthRange(nullptr),
	_glViewport(nullptr),

	// GL_VERSION_1_1

	_glDrawArrays(nullptr),
	_glDrawElements(nullptr),
	_glPolygonOffset(nullptr),
	_glCopyTexImage1D(nullptr),
	_glCopyTexImage2D(nullptr),
	_glCopyTexSubImage1D(nullptr),
	_glCopyTexSubImage2D(nullptr),
	_glTexSubImage1D(nullptr),
	_glTexSubImage2D(nullptr),
	_glBindTexture(nullptr),
	_glDeleteTextures(nullptr),
	_glGenTextures(nullptr),
	_glIsTexture(nullptr),

	// GL_VERSION_1_2

	_glDrawRangeElements(nullptr),
	_glTexImage3D(nullptr),
	_glTexSubImage3D(nullptr),
	_glCopyTexSubImage3D(nullptr),

	// GL_VERSION_1_3

	_glActiveTexture(nullptr),
	_glSampleCoverage(nullptr),
	_glCompressedTexImage3D(nullptr),
	_glCompressedTexImage2D(nullptr),
	_glCompressedTexImage1D(nullptr),
	_glCompressedTexSubImage3D(nullptr),
	_glCompressedTexSubImage2D(nullptr),
	_glCompressedTexSubImage1D(nullptr),
	_glGetCompressedTexImage(nullptr),

	// GL_VERSION_1_4

	_glBlendFuncSeparate(nullptr),
	_glMultiDrawArrays(nullptr),
	_glMultiDrawElements(nullptr),
	_glPointParameterf(nullptr),
	_glPointParameterfv(nullptr),
	_glPointParameteri(nullptr),
	_glPointParameteriv(nullptr),
	_glBlendColor(nullptr),
	_glBlendEquation(nullptr),

	// GL_VERSION_1_5

	_glGenQueries(nullptr),
	_glDeleteQueries(nullptr),
	_glIsQuery(nullptr),
	_glBeginQuery(nullptr),
	_glEndQuery(nullptr),
	_glGetQueryiv(nullptr),
	_glGetQueryObjectiv(nullptr),
	_glGetQueryObjectuiv(nullptr),
	_glBindBuffer(nullptr),
	_glDeleteBuffers(nullptr),
	_glGenBuffers(nullptr),
	_glIsBuffer(nullptr),
	_glBufferData(nullptr),
	_glBufferSubData(nullptr),
	_glGetBufferSubData(nullptr),
	_glMapBuffer(nullptr),
	_glUnmapBuffer(nullptr),
	_glGetBufferParameteriv(nullptr),
	_glGetBufferPointerv(nullptr),

	// GL_VERSION_2_0

	_glBlendEquationSeparate(nullptr),
	_glDrawBuffers(nullptr),
	_glStencilOpSeparate(nullptr),
	_glStencilFuncSeparate(nullptr),
	_glStencilMaskSeparate(nullptr),
	_glAttachShader(nullptr),
	_glBindAttribLocation(nullptr),
	_glCompileShader(nullptr),
	_glCreateProgram(nullptr),
	_glCreateShader(nullptr),
	_glDeleteProgram(nullptr),
	_glDeleteShader(nullptr),
	_glDetachShader(nullptr),
	_glDisableVertexAttribArray(nullptr),
	_glEnableVertexAttribArray(nullptr),
	_glGetActiveAttrib(nullptr),
	_glGetActiveUniform(nullptr),
	_glGetAttachedShaders(nullptr),
	_glGetAttribLocation(nullptr),
	_glGetProgramiv(nullptr),
	_glGetProgramInfoLog(nullptr),
	_glGetShaderiv(nullptr),
	_glGetShaderInfoLog(nullptr),
	_glGetShaderSource(nullptr),
	_glGetUniformLocation(nullptr),
	_glGetUniformfv(nullptr),
	_glGetUniformiv(nullptr),
	_glGetVertexAttribdv(nullptr),
	_glGetVertexAttribfv(nullptr),
	_glGetVertexAttribiv(nullptr),
	_glGetVertexAttribPointerv(nullptr),
	_glIsProgram(nullptr),
	_glIsShader(nullptr),
	_glLinkProgram(nullptr),
	_glShaderSource(nullptr),
	_glUseProgram(nullptr),
	_glUniform1f(nullptr),
	_glUniform2f(nullptr),
	_glUniform3f(nullptr),
	_glUniform4f(nullptr),
	_glUniform1i(nullptr),
	_glUniform2i(nullptr),
	_glUniform3i(nullptr),
	_glUniform4i(nullptr),
	_glUniform1fv(nullptr),
	_glUniform2fv(nullptr),
	_glUniform3fv(nullptr),
	_glUniform4fv(nullptr),
	_glUniform1iv(nullptr),
	_glUniform2iv(nullptr),
	_glUniform3iv(nullptr),
	_glUniform4iv(nullptr),
	_glUniformMatrix2fv(nullptr),
	_glUniformMatrix3fv(nullptr),
	_glUniformMatrix4fv(nullptr),
	_glValidateProgram(nullptr),
	_glVertexAttrib1d(nullptr),
	_glVertexAttrib1dv(nullptr),
	_glVertexAttrib1f(nullptr),
	_glVertexAttrib1fv(nullptr),
	_glVertexAttrib1s(nullptr),
	_glVertexAttrib1sv(nullptr),
	_glVertexAttrib2d(nullptr),
	_glVertexAttrib2dv(nullptr),
	_glVertexAttrib2f(nullptr),
	_glVertexAttrib2fv(nullptr),
	_glVertexAttrib2s(nullptr),
	_glVertexAttrib2sv(nullptr),
	_glVertexAttrib3d(nullptr),
	_glVertexAttrib3dv(nullptr),
	_glVertexAttrib3f(nullptr),
	_glVertexAttrib3fv(nullptr),
	_glVertexAttrib3s(nullptr),
	_glVertexAttrib3sv(nullptr),
	_glVertexAttrib4Nbv(nullptr),
	_glVertexAttrib4Niv(nullptr),
	_glVertexAttrib4Nsv(nullptr),
	_glVertexAttrib4Nub(nullptr),
	_glVertexAttrib4Nubv(nullptr),
	_glVertexAttrib4Nuiv(nullptr),
	_glVertexAttrib4Nusv(nullptr),
	_glVertexAttrib4bv(nullptr),
	_glVertexAttrib4d(nullptr),
	_glVertexAttrib4dv(nullptr),
	_glVertexAttrib4f(nullptr),
	_glVertexAttrib4fv(nullptr),
	_glVertexAttrib4iv(nullptr),
	_glVertexAttrib4s(nullptr),
	_glVertexAttrib4sv(nullptr),
	_glVertexAttrib4ubv(nullptr),
	_glVertexAttrib4uiv(nullptr),
	_glVertexAttrib4usv(nullptr),
	_glVertexAttribPointer(nullptr),

	// GL_VERSION_2_1

	_glUniformMatrix2x3fv(nullptr),
	_glUniformMatrix3x2fv(nullptr),
	_glUniformMatrix2x4fv(nullptr),
	_glUniformMatrix4x2fv(nullptr),
	_glUniformMatrix3x4fv(nullptr),
	_glUniformMatrix4x3fv(nullptr),

	// GL_VERSION_3_0

	_glColorMaski(nullptr),
	_glGetBooleani_v(nullptr),
	_glGetIntegeri_v(nullptr),
	_glEnablei(nullptr),
	_glDisablei(nullptr),
	_glIsEnabledi(nullptr),
	_glBeginTransformFeedback(nullptr),
	_glEndTransformFeedback(nullptr),
	_glBindBufferRange(nullptr),
	_glBindBufferBase(nullptr),
	_glTransformFeedbackVaryings(nullptr),
	_glGetTransformFeedbackVarying(nullptr),
	_glClampColor(nullptr),
	_glBeginConditionalRender(nullptr),
	_glEndConditionalRender(nullptr),
	_glVertexAttribIPointer(nullptr),
	_glGetVertexAttribIiv(nullptr),
	_glGetVertexAttribIuiv(nullptr),
	_glVertexAttribI1i(nullptr),
	_glVertexAttribI2i(nullptr),
	_glVertexAttribI3i(nullptr),
	_glVertexAttribI4i(nullptr),
	_glVertexAttribI1ui(nullptr),
	_glVertexAttribI2ui(nullptr),
	_glVertexAttribI3ui(nullptr),
	_glVertexAttribI4ui(nullptr),
	_glVertexAttribI1iv(nullptr),
	_glVertexAttribI2iv(nullptr),
	_glVertexAttribI3iv(nullptr),
	_glVertexAttribI4iv(nullptr),
	_glVertexAttribI1uiv(nullptr),
	_glVertexAttribI2uiv(nullptr),
	_glVertexAttribI3uiv(nullptr),
	_glVertexAttribI4uiv(nullptr),
	_glVertexAttribI4bv(nullptr),
	_glVertexAttribI4sv(nullptr),
	_glVertexAttribI4ubv(nullptr),
	_glVertexAttribI4usv(nullptr),
	_glGetUniformuiv(nullptr),
	_glBindFragDataLocation(nullptr),
	_glGetFragDataLocation(nullptr),
	_glUniform1ui(nullptr),
	_glUniform2ui(nullptr),
	_glUniform3ui(nullptr),
	_glUniform4ui(nullptr),
	_glUniform1uiv(nullptr),
	_glUniform2uiv(nullptr),
	_glUniform3uiv(nullptr),
	_glUniform4uiv(nullptr),
	_glTexParameterIiv(nullptr),
	_glTexParameterIuiv(nullptr),
	_glGetTexParameterIiv(nullptr),
	_glGetTexParameterIuiv(nullptr),
	_glClearBufferiv(nullptr),
	_glClearBufferuiv(nullptr),
	_glClearBufferfv(nullptr),
	_glClearBufferfi(nullptr),
	_glGetStringi(nullptr),
	_glIsRenderbuffer(nullptr),
	_glBindRenderbuffer(nullptr),
	_glDeleteRenderbuffers(nullptr),
	_glGenRenderbuffers(nullptr),
	_glRenderbufferStorage(nullptr),
	_glGetRenderbufferParameteriv(nullptr),
	_glIsFramebuffer(nullptr),
	_glBindFramebuffer(nullptr),
	_glDeleteFramebuffers(nullptr),
	_glGenFramebuffers(nullptr),
	_glCheckFramebufferStatus(nullptr),
	_glFramebufferTexture1D(nullptr),
	_glFramebufferTexture2D(nullptr),
	_glFramebufferTexture3D(nullptr),
	_glFramebufferRenderbuffer(nullptr),
	_glGetFramebufferAttachmentParameteriv(nullptr),
	_glGenerateMipmap(nullptr),
	_glBlitFramebuffer(nullptr),
	_glRenderbufferStorageMultisample(nullptr),
	_glFramebufferTextureLayer(nullptr),
	_glMapBufferRange(nullptr),
	_glFlushMappedBufferRange(nullptr),
	_glBindVertexArray(nullptr),
	_glDeleteVertexArrays(nullptr),
	_glGenVertexArrays(nullptr),
	_glIsVertexArray(nullptr),

	// GL_VERSION_3_1

	_glDrawArraysInstanced(nullptr),
	_glDrawElementsInstanced(nullptr),
	_glTexBuffer(nullptr),
	_glPrimitiveRestartIndex(nullptr),
	_glCopyBufferSubData(nullptr),
	_glGetUniformIndices(nullptr),
	_glGetActiveUniformsiv(nullptr),
	_glGetActiveUniformName(nullptr),
	_glGetUniformBlockIndex(nullptr),
	_glGetActiveUniformBlockiv(nullptr),
	_glGetActiveUniformBlockName(nullptr),
	_glUniformBlockBinding(nullptr),

	// GL_VERSION_3_2

	_glDrawElementsBaseVertex(nullptr),
	_glDrawRangeElementsBaseVertex(nullptr),
	_glDrawElementsInstancedBaseVertex(nullptr),
	_glMultiDrawElementsBaseVertex(nullptr),
	_glProvokingVertex(nullptr),
	_glFenceSync(nullptr),
	_glIsSync(nullptr),
	_glDeleteSync(nullptr),
	_glClientWaitSync(nullptr),
	_glWaitSync(nullptr),
	_glGetInteger64v(nullptr),
	_glGetSynciv(nullptr),
	_glGetInteger64i_v(nullptr),
	_glGetBufferParameteri64v(nullptr),
	_glFramebufferTexture(nullptr),
	_glTexImage2DMultisample(nullptr),
	_glTexImage3DMultisample(nullptr),
	_glGetMultisamplefv(nullptr),
	_glSampleMaski(nullptr),

	// GL_VERSION_3_3

	_glBindFragDataLocationIndexed(nullptr),
	_glGetFragDataIndex(nullptr),
	_glGenSamplers(nullptr),
	_glDeleteSamplers(nullptr),
	_glIsSampler(nullptr),
	_glBindSampler(nullptr),
	_glSamplerParameteri(nullptr),
	_glSamplerParameteriv(nullptr),
	_glSamplerParameterf(nullptr),
	_glSamplerParameterfv(nullptr),
	_glSamplerParameterIiv(nullptr),
	_glSamplerParameterIuiv(nullptr),
	_glGetSamplerParameteriv(nullptr),
	_glGetSamplerParameterIiv(nullptr),
	_glGetSamplerParameterfv(nullptr),
	_glGetSamplerParameterIuiv(nullptr),
	_glQueryCounter(nullptr),
	_glGetQueryObjecti64v(nullptr),
	_glGetQueryObjectui64v(nullptr),
	_glVertexAttribDivisor(nullptr),
	_glVertexAttribP1ui(nullptr),
	_glVertexAttribP1uiv(nullptr),
	_glVertexAttribP2ui(nullptr),
	_glVertexAttribP2uiv(nullptr),
	_glVertexAttribP3ui(nullptr),
	_glVertexAttribP3uiv(nullptr),
	_glVertexAttribP4ui(nullptr),
	_glVertexAttribP4uiv(nullptr),

	// GL_KHR_debug

	_glDebugMessageControl(nullptr),
	_glDebugMessageInsert(nullptr),
	_glDebugMessageCallback(nullptr),
	_glGetDebugMessageLog(nullptr),
	_glPushDebugGroup(nullptr),
	_glPopDebugGroup(nullptr),
	_glObjectLabel(nullptr),
	_glGetObjectLabel(nullptr),
	_glObjectPtrLabel(nullptr),
	_glGetObjectPtrLabel(nullptr),
	_glGetPointerv(nullptr),

	// GL_ARB_texture_storage

	_glTexStorage1D(nullptr),
	_glTexStorage2D(nullptr),
	_glTexStorage3D(nullptr),

	_log(logger),
	_abortOnError(false),
	_logErrors(true),
	_logCalls(false),
	_logLevel(LogLevel::Info),
	_glGetProcAddress(glGetProcAddress),
	_procCount(0) {
}


bool Context::initialize(bool debugGl) {
	_abortOnError = debugGl;
	_logCalls = debugGl;


	// GL_VERSION_1_0

	_procCount = 0;
	_glCullFace = (_PfnGlCullFace)_getProcAddress("glCullFace");
	_glFrontFace = (_PfnGlFrontFace)_getProcAddress("glFrontFace");
	_glHint = (_PfnGlHint)_getProcAddress("glHint");
	_glLineWidth = (_PfnGlLineWidth)_getProcAddress("glLineWidth");
	_glPointSize = (_PfnGlPointSize)_getProcAddress("glPointSize");
	_glPolygonMode = (_PfnGlPolygonMode)_getProcAddress("glPolygonMode");
	_glScissor = (_PfnGlScissor)_getProcAddress("glScissor");
	_glTexParameterf = (_PfnGlTexParameterf)_getProcAddress("glTexParameterf");
	_glTexParameterfv = (_PfnGlTexParameterfv)_getProcAddress("glTexParameterfv");
	_glTexParameteri = (_PfnGlTexParameteri)_getProcAddress("glTexParameteri");
	_glTexParameteriv = (_PfnGlTexParameteriv)_getProcAddress("glTexParameteriv");
	_glTexImage1D = (_PfnGlTexImage1D)_getProcAddress("glTexImage1D");
	_glTexImage2D = (_PfnGlTexImage2D)_getProcAddress("glTexImage2D");
	_glDrawBuffer = (_PfnGlDrawBuffer)_getProcAddress("glDrawBuffer");
	_glClear = (_PfnGlClear)_getProcAddress("glClear");
	_glClearColor = (_PfnGlClearColor)_getProcAddress("glClearColor");
	_glClearStencil = (_PfnGlClearStencil)_getProcAddress("glClearStencil");
	_glClearDepth = (_PfnGlClearDepth)_getProcAddress("glClearDepth");
	_glStencilMask = (_PfnGlStencilMask)_getProcAddress("glStencilMask");
	_glColorMask = (_PfnGlColorMask)_getProcAddress("glColorMask");
	_glDepthMask = (_PfnGlDepthMask)_getProcAddress("glDepthMask");
	_glDisable = (_PfnGlDisable)_getProcAddress("glDisable");
	_glEnable = (_PfnGlEnable)_getProcAddress("glEnable");
	_glFinish = (_PfnGlFinish)_getProcAddress("glFinish");
	_glFlush = (_PfnGlFlush)_getProcAddress("glFlush");
	_glBlendFunc = (_PfnGlBlendFunc)_getProcAddress("glBlendFunc");
	_glLogicOp = (_PfnGlLogicOp)_getProcAddress("glLogicOp");
	_glStencilFunc = (_PfnGlStencilFunc)_getProcAddress("glStencilFunc");
	_glStencilOp = (_PfnGlStencilOp)_getProcAddress("glStencilOp");
	_glDepthFunc = (_PfnGlDepthFunc)_getProcAddress("glDepthFunc");
	_glPixelStoref = (_PfnGlPixelStoref)_getProcAddress("glPixelStoref");
	_glPixelStorei = (_PfnGlPixelStorei)_getProcAddress("glPixelStorei");
	_glReadBuffer = (_PfnGlReadBuffer)_getProcAddress("glReadBuffer");
	_glReadPixels = (_PfnGlReadPixels)_getProcAddress("glReadPixels");
	_glGetBooleanv = (_PfnGlGetBooleanv)_getProcAddress("glGetBooleanv");
	_glGetDoublev = (_PfnGlGetDoublev)_getProcAddress("glGetDoublev");
	_glGetError = (_PfnGlGetError)_getProcAddress("glGetError");
	_glGetFloatv = (_PfnGlGetFloatv)_getProcAddress("glGetFloatv");
	_glGetIntegerv = (_PfnGlGetIntegerv)_getProcAddress("glGetIntegerv");
	_glGetString = (_PfnGlGetString)_getProcAddress("glGetString");
	_glGetTexImage = (_PfnGlGetTexImage)_getProcAddress("glGetTexImage");
	_glGetTexParameterfv = (_PfnGlGetTexParameterfv)_getProcAddress("glGetTexParameterfv");
	_glGetTexParameteriv = (_PfnGlGetTexParameteriv)_getProcAddress("glGetTexParameteriv");
	_glGetTexLevelParameterfv = (_PfnGlGetTexLevelParameterfv)_getProcAddress("glGetTexLevelParameterfv");
	_glGetTexLevelParameteriv = (_PfnGlGetTexLevelParameteriv)_getProcAddress("glGetTexLevelParameteriv");
	_glIsEnabled = (_PfnGlIsEnabled)_getProcAddress("glIsEnabled");
	_glDepthRange = (_PfnGlDepthRange)_getProcAddress("glDepthRange");
	_glViewport = (_PfnGlViewport)_getProcAddress("glViewport");
	_gl_1_0 = (_procCount == 48);

	// GL_VERSION_1_1

	_procCount = 0;
	_glDrawArrays = (_PfnGlDrawArrays)_getProcAddress("glDrawArrays");
	_glDrawElements = (_PfnGlDrawElements)_getProcAddress("glDrawElements");
	_glPolygonOffset = (_PfnGlPolygonOffset)_getProcAddress("glPolygonOffset");
	_glCopyTexImage1D = (_PfnGlCopyTexImage1D)_getProcAddress("glCopyTexImage1D");
	_glCopyTexImage2D = (_PfnGlCopyTexImage2D)_getProcAddress("glCopyTexImage2D");
	_glCopyTexSubImage1D = (_PfnGlCopyTexSubImage1D)_getProcAddress("glCopyTexSubImage1D");
	_glCopyTexSubImage2D = (_PfnGlCopyTexSubImage2D)_getProcAddress("glCopyTexSubImage2D");
	_glTexSubImage1D = (_PfnGlTexSubImage1D)_getProcAddress("glTexSubImage1D");
	_glTexSubImage2D = (_PfnGlTexSubImage2D)_getProcAddress("glTexSubImage2D");
	_glBindTexture = (_PfnGlBindTexture)_getProcAddress("glBindTexture");
	_glDeleteTextures = (_PfnGlDeleteTextures)_getProcAddress("glDeleteTextures");
	_glGenTextures = (_PfnGlGenTextures)_getProcAddress("glGenTextures");
	_glIsTexture = (_PfnGlIsTexture)_getProcAddress("glIsTexture");
	_gl_1_1 = (_gl_1_0 && _procCount == 13);

	// GL_VERSION_1_2

	_procCount = 0;
	_glDrawRangeElements = (_PfnGlDrawRangeElements)_getProcAddress("glDrawRangeElements");
	_glTexImage3D = (_PfnGlTexImage3D)_getProcAddress("glTexImage3D");
	_glTexSubImage3D = (_PfnGlTexSubImage3D)_getProcAddress("glTexSubImage3D");
	_glCopyTexSubImage3D = (_PfnGlCopyTexSubImage3D)_getProcAddress("glCopyTexSubImage3D");
	_gl_1_2 = (_gl_1_1 && _procCount == 4);

	// GL_VERSION_1_3

	_procCount = 0;
	_glActiveTexture = (_PfnGlActiveTexture)_getProcAddress("glActiveTexture");
	_glSampleCoverage = (_PfnGlSampleCoverage)_getProcAddress("glSampleCoverage");
	_glCompressedTexImage3D = (_PfnGlCompressedTexImage3D)_getProcAddress("glCompressedTexImage3D");
	_glCompressedTexImage2D = (_PfnGlCompressedTexImage2D)_getProcAddress("glCompressedTexImage2D");
	_glCompressedTexImage1D = (_PfnGlCompressedTexImage1D)_getProcAddress("glCompressedTexImage1D");
	_glCompressedTexSubImage3D = (_PfnGlCompressedTexSubImage3D)_getProcAddress("glCompressedTexSubImage3D");
	_glCompressedTexSubImage2D = (_PfnGlCompressedTexSubImage2D)_getProcAddress("glCompressedTexSubImage2D");
	_glCompressedTexSubImage1D = (_PfnGlCompressedTexSubImage1D)_getProcAddress("glCompressedTexSubImage1D");
	_glGetCompressedTexImage = (_PfnGlGetCompressedTexImage)_getProcAddress("glGetCompressedTexImage");
	_gl_1_3 = (_gl_1_2 && _procCount == 9);

	// GL_VERSION_1_4

	_procCount = 0;
	_glBlendFuncSeparate = (_PfnGlBlendFuncSeparate)_getProcAddress("glBlendFuncSeparate");
	_glMultiDrawArrays = (_PfnGlMultiDrawArrays)_getProcAddress("glMultiDrawArrays");
	_glMultiDrawElements = (_PfnGlMultiDrawElements)_getProcAddress("glMultiDrawElements");
	_glPointParameterf = (_PfnGlPointParameterf)_getProcAddress("glPointParameterf");
	_glPointParameterfv = (_PfnGlPointParameterfv)_getProcAddress("glPointParameterfv");
	_glPointParameteri = (_PfnGlPointParameteri)_getProcAddress("glPointParameteri");
	_glPointParameteriv = (_PfnGlPointParameteriv)_getProcAddress("glPointParameteriv");
	_glBlendColor = (_PfnGlBlendColor)_getProcAddress("glBlendColor");
	_glBlendEquation = (_PfnGlBlendEquation)_getProcAddress("glBlendEquation");
	_gl_1_4 = (_gl_1_3 && _procCount == 9);

	// GL_VERSION_1_5

	_procCount = 0;
	_glGenQueries = (_PfnGlGenQueries)_getProcAddress("glGenQueries");
	_glDeleteQueries = (_PfnGlDeleteQueries)_getProcAddress("glDeleteQueries");
	_glIsQuery = (_PfnGlIsQuery)_getProcAddress("glIsQuery");
	_glBeginQuery = (_PfnGlBeginQuery)_getProcAddress("glBeginQuery");
	_glEndQuery = (_PfnGlEndQuery)_getProcAddress("glEndQuery");
	_glGetQueryiv = (_PfnGlGetQueryiv)_getProcAddress("glGetQueryiv");
	_glGetQueryObjectiv = (_PfnGlGetQueryObjectiv)_getProcAddress("glGetQueryObjectiv");
	_glGetQueryObjectuiv = (_PfnGlGetQueryObjectuiv)_getProcAddress("glGetQueryObjectuiv");
	_glBindBuffer = (_PfnGlBindBuffer)_getProcAddress("glBindBuffer");
	_glDeleteBuffers = (_PfnGlDeleteBuffers)_getProcAddress("glDeleteBuffers");
	_glGenBuffers = (_PfnGlGenBuffers)_getProcAddress("glGenBuffers");
	_glIsBuffer = (_PfnGlIsBuffer)_getProcAddress("glIsBuffer");
	_glBufferData = (_PfnGlBufferData)_getProcAddress("glBufferData");
	_glBufferSubData = (_PfnGlBufferSubData)_getProcAddress("glBufferSubData");
	_glGetBufferSubData = (_PfnGlGetBufferSubData)_getProcAddress("glGetBufferSubData");
	_glMapBuffer = (_PfnGlMapBuffer)_getProcAddress("glMapBuffer");
	_glUnmapBuffer = (_PfnGlUnmapBuffer)_getProcAddress("glUnmapBuffer");
	_glGetBufferParameteriv = (_PfnGlGetBufferParameteriv)_getProcAddress("glGetBufferParameteriv");
	_glGetBufferPointerv = (_PfnGlGetBufferPointerv)_getProcAddress("glGetBufferPointerv");
	_gl_1_5 = (_gl_1_4 && _procCount == 19);

	// GL_VERSION_2_0

	_procCount = 0;
	_glBlendEquationSeparate = (_PfnGlBlendEquationSeparate)_getProcAddress("glBlendEquationSeparate");
	_glDrawBuffers = (_PfnGlDrawBuffers)_getProcAddress("glDrawBuffers");
	_glStencilOpSeparate = (_PfnGlStencilOpSeparate)_getProcAddress("glStencilOpSeparate");
	_glStencilFuncSeparate = (_PfnGlStencilFuncSeparate)_getProcAddress("glStencilFuncSeparate");
	_glStencilMaskSeparate = (_PfnGlStencilMaskSeparate)_getProcAddress("glStencilMaskSeparate");
	_glAttachShader = (_PfnGlAttachShader)_getProcAddress("glAttachShader");
	_glBindAttribLocation = (_PfnGlBindAttribLocation)_getProcAddress("glBindAttribLocation");
	_glCompileShader = (_PfnGlCompileShader)_getProcAddress("glCompileShader");
	_glCreateProgram = (_PfnGlCreateProgram)_getProcAddress("glCreateProgram");
	_glCreateShader = (_PfnGlCreateShader)_getProcAddress("glCreateShader");
	_glDeleteProgram = (_PfnGlDeleteProgram)_getProcAddress("glDeleteProgram");
	_glDeleteShader = (_PfnGlDeleteShader)_getProcAddress("glDeleteShader");
	_glDetachShader = (_PfnGlDetachShader)_getProcAddress("glDetachShader");
	_glDisableVertexAttribArray = (_PfnGlDisableVertexAttribArray)_getProcAddress("glDisableVertexAttribArray");
	_glEnableVertexAttribArray = (_PfnGlEnableVertexAttribArray)_getProcAddress("glEnableVertexAttribArray");
	_glGetActiveAttrib = (_PfnGlGetActiveAttrib)_getProcAddress("glGetActiveAttrib");
	_glGetActiveUniform = (_PfnGlGetActiveUniform)_getProcAddress("glGetActiveUniform");
	_glGetAttachedShaders = (_PfnGlGetAttachedShaders)_getProcAddress("glGetAttachedShaders");
	_glGetAttribLocation = (_PfnGlGetAttribLocation)_getProcAddress("glGetAttribLocation");
	_glGetProgramiv = (_PfnGlGetProgramiv)_getProcAddress("glGetProgramiv");
	_glGetProgramInfoLog = (_PfnGlGetProgramInfoLog)_getProcAddress("glGetProgramInfoLog");
	_glGetShaderiv = (_PfnGlGetShaderiv)_getProcAddress("glGetShaderiv");
	_glGetShaderInfoLog = (_PfnGlGetShaderInfoLog)_getProcAddress("glGetShaderInfoLog");
	_glGetShaderSource = (_PfnGlGetShaderSource)_getProcAddress("glGetShaderSource");
	_glGetUniformLocation = (_PfnGlGetUniformLocation)_getProcAddress("glGetUniformLocation");
	_glGetUniformfv = (_PfnGlGetUniformfv)_getProcAddress("glGetUniformfv");
	_glGetUniformiv = (_PfnGlGetUniformiv)_getProcAddress("glGetUniformiv");
	_glGetVertexAttribdv = (_PfnGlGetVertexAttribdv)_getProcAddress("glGetVertexAttribdv");
	_glGetVertexAttribfv = (_PfnGlGetVertexAttribfv)_getProcAddress("glGetVertexAttribfv");
	_glGetVertexAttribiv = (_PfnGlGetVertexAttribiv)_getProcAddress("glGetVertexAttribiv");
	_glGetVertexAttribPointerv = (_PfnGlGetVertexAttribPointerv)_getProcAddress("glGetVertexAttribPointerv");
	_glIsProgram = (_PfnGlIsProgram)_getProcAddress("glIsProgram");
	_glIsShader = (_PfnGlIsShader)_getProcAddress("glIsShader");
	_glLinkProgram = (_PfnGlLinkProgram)_getProcAddress("glLinkProgram");
	_glShaderSource = (_PfnGlShaderSource)_getProcAddress("glShaderSource");
	_glUseProgram = (_PfnGlUseProgram)_getProcAddress("glUseProgram");
	_glUniform1f = (_PfnGlUniform1f)_getProcAddress("glUniform1f");
	_glUniform2f = (_PfnGlUniform2f)_getProcAddress("glUniform2f");
	_glUniform3f = (_PfnGlUniform3f)_getProcAddress("glUniform3f");
	_glUniform4f = (_PfnGlUniform4f)_getProcAddress("glUniform4f");
	_glUniform1i = (_PfnGlUniform1i)_getProcAddress("glUniform1i");
	_glUniform2i = (_PfnGlUniform2i)_getProcAddress("glUniform2i");
	_glUniform3i = (_PfnGlUniform3i)_getProcAddress("glUniform3i");
	_glUniform4i = (_PfnGlUniform4i)_getProcAddress("glUniform4i");
	_glUniform1fv = (_PfnGlUniform1fv)_getProcAddress("glUniform1fv");
	_glUniform2fv = (_PfnGlUniform2fv)_getProcAddress("glUniform2fv");
	_glUniform3fv = (_PfnGlUniform3fv)_getProcAddress("glUniform3fv");
	_glUniform4fv = (_PfnGlUniform4fv)_getProcAddress("glUniform4fv");
	_glUniform1iv = (_PfnGlUniform1iv)_getProcAddress("glUniform1iv");
	_glUniform2iv = (_PfnGlUniform2iv)_getProcAddress("glUniform2iv");
	_glUniform3iv = (_PfnGlUniform3iv)_getProcAddress("glUniform3iv");
	_glUniform4iv = (_PfnGlUniform4iv)_getProcAddress("glUniform4iv");
	_glUniformMatrix2fv = (_PfnGlUniformMatrix2fv)_getProcAddress("glUniformMatrix2fv");
	_glUniformMatrix3fv = (_PfnGlUniformMatrix3fv)_getProcAddress("glUniformMatrix3fv");
	_glUniformMatrix4fv = (_PfnGlUniformMatrix4fv)_getProcAddress("glUniformMatrix4fv");
	_glValidateProgram = (_PfnGlValidateProgram)_getProcAddress("glValidateProgram");
	_glVertexAttrib1d = (_PfnGlVertexAttrib1d)_getProcAddress("glVertexAttrib1d");
	_glVertexAttrib1dv = (_PfnGlVertexAttrib1dv)_getProcAddress("glVertexAttrib1dv");
	_glVertexAttrib1f = (_PfnGlVertexAttrib1f)_getProcAddress("glVertexAttrib1f");
	_glVertexAttrib1fv = (_PfnGlVertexAttrib1fv)_getProcAddress("glVertexAttrib1fv");
	_glVertexAttrib1s = (_PfnGlVertexAttrib1s)_getProcAddress("glVertexAttrib1s");
	_glVertexAttrib1sv = (_PfnGlVertexAttrib1sv)_getProcAddress("glVertexAttrib1sv");
	_glVertexAttrib2d = (_PfnGlVertexAttrib2d)_getProcAddress("glVertexAttrib2d");
	_glVertexAttrib2dv = (_PfnGlVertexAttrib2dv)_getProcAddress("glVertexAttrib2dv");
	_glVertexAttrib2f = (_PfnGlVertexAttrib2f)_getProcAddress("glVertexAttrib2f");
	_glVertexAttrib2fv = (_PfnGlVertexAttrib2fv)_getProcAddress("glVertexAttrib2fv");
	_glVertexAttrib2s = (_PfnGlVertexAttrib2s)_getProcAddress("glVertexAttrib2s");
	_glVertexAttrib2sv = (_PfnGlVertexAttrib2sv)_getProcAddress("glVertexAttrib2sv");
	_glVertexAttrib3d = (_PfnGlVertexAttrib3d)_getProcAddress("glVertexAttrib3d");
	_glVertexAttrib3dv = (_PfnGlVertexAttrib3dv)_getProcAddress("glVertexAttrib3dv");
	_glVertexAttrib3f = (_PfnGlVertexAttrib3f)_getProcAddress("glVertexAttrib3f");
	_glVertexAttrib3fv = (_PfnGlVertexAttrib3fv)_getProcAddress("glVertexAttrib3fv");
	_glVertexAttrib3s = (_PfnGlVertexAttrib3s)_getProcAddress("glVertexAttrib3s");
	_glVertexAttrib3sv = (_PfnGlVertexAttrib3sv)_getProcAddress("glVertexAttrib3sv");
	_glVertexAttrib4Nbv = (_PfnGlVertexAttrib4Nbv)_getProcAddress("glVertexAttrib4Nbv");
	_glVertexAttrib4Niv = (_PfnGlVertexAttrib4Niv)_getProcAddress("glVertexAttrib4Niv");
	_glVertexAttrib4Nsv = (_PfnGlVertexAttrib4Nsv)_getProcAddress("glVertexAttrib4Nsv");
	_glVertexAttrib4Nub = (_PfnGlVertexAttrib4Nub)_getProcAddress("glVertexAttrib4Nub");
	_glVertexAttrib4Nubv = (_PfnGlVertexAttrib4Nubv)_getProcAddress("glVertexAttrib4Nubv");
	_glVertexAttrib4Nuiv = (_PfnGlVertexAttrib4Nuiv)_getProcAddress("glVertexAttrib4Nuiv");
	_glVertexAttrib4Nusv = (_PfnGlVertexAttrib4Nusv)_getProcAddress("glVertexAttrib4Nusv");
	_glVertexAttrib4bv = (_PfnGlVertexAttrib4bv)_getProcAddress("glVertexAttrib4bv");
	_glVertexAttrib4d = (_PfnGlVertexAttrib4d)_getProcAddress("glVertexAttrib4d");
	_glVertexAttrib4dv = (_PfnGlVertexAttrib4dv)_getProcAddress("glVertexAttrib4dv");
	_glVertexAttrib4f = (_PfnGlVertexAttrib4f)_getProcAddress("glVertexAttrib4f");
	_glVertexAttrib4fv = (_PfnGlVertexAttrib4fv)_getProcAddress("glVertexAttrib4fv");
	_glVertexAttrib4iv = (_PfnGlVertexAttrib4iv)_getProcAddress("glVertexAttrib4iv");
	_glVertexAttrib4s = (_PfnGlVertexAttrib4s)_getProcAddress("glVertexAttrib4s");
	_glVertexAttrib4sv = (_PfnGlVertexAttrib4sv)_getProcAddress("glVertexAttrib4sv");
	_glVertexAttrib4ubv = (_PfnGlVertexAttrib4ubv)_getProcAddress("glVertexAttrib4ubv");
	_glVertexAttrib4uiv = (_PfnGlVertexAttrib4uiv)_getProcAddress("glVertexAttrib4uiv");
	_glVertexAttrib4usv = (_PfnGlVertexAttrib4usv)_getProcAddress("glVertexAttrib4usv");
	_glVertexAttribPointer = (_PfnGlVertexAttribPointer)_getProcAddress("glVertexAttribPointer");
	_gl_2_0 = (_gl_1_5 && _procCount == 93);

	// GL_VERSION_2_1

	_procCount = 0;
	_glUniformMatrix2x3fv = (_PfnGlUniformMatrix2x3fv)_getProcAddress("glUniformMatrix2x3fv");
	_glUniformMatrix3x2fv = (_PfnGlUniformMatrix3x2fv)_getProcAddress("glUniformMatrix3x2fv");
	_glUniformMatrix2x4fv = (_PfnGlUniformMatrix2x4fv)_getProcAddress("glUniformMatrix2x4fv");
	_glUniformMatrix4x2fv = (_PfnGlUniformMatrix4x2fv)_getProcAddress("glUniformMatrix4x2fv");
	_glUniformMatrix3x4fv = (_PfnGlUniformMatrix3x4fv)_getProcAddress("glUniformMatrix3x4fv");
	_glUniformMatrix4x3fv = (_PfnGlUniformMatrix4x3fv)_getProcAddress("glUniformMatrix4x3fv");
	_gl_2_1 = (_gl_2_0 && _procCount == 6);

	// GL_VERSION_3_0

	_procCount = 0;
	_glColorMaski = (_PfnGlColorMaski)_getProcAddress("glColorMaski");
	_glGetBooleani_v = (_PfnGlGetBooleani_v)_getProcAddress("glGetBooleani_v");
	_glGetIntegeri_v = (_PfnGlGetIntegeri_v)_getProcAddress("glGetIntegeri_v");
	_glEnablei = (_PfnGlEnablei)_getProcAddress("glEnablei");
	_glDisablei = (_PfnGlDisablei)_getProcAddress("glDisablei");
	_glIsEnabledi = (_PfnGlIsEnabledi)_getProcAddress("glIsEnabledi");
	_glBeginTransformFeedback = (_PfnGlBeginTransformFeedback)_getProcAddress("glBeginTransformFeedback");
	_glEndTransformFeedback = (_PfnGlEndTransformFeedback)_getProcAddress("glEndTransformFeedback");
	_glBindBufferRange = (_PfnGlBindBufferRange)_getProcAddress("glBindBufferRange");
	_glBindBufferBase = (_PfnGlBindBufferBase)_getProcAddress("glBindBufferBase");
	_glTransformFeedbackVaryings = (_PfnGlTransformFeedbackVaryings)_getProcAddress("glTransformFeedbackVaryings");
	_glGetTransformFeedbackVarying = (_PfnGlGetTransformFeedbackVarying)_getProcAddress("glGetTransformFeedbackVarying");
	_glClampColor = (_PfnGlClampColor)_getProcAddress("glClampColor");
	_glBeginConditionalRender = (_PfnGlBeginConditionalRender)_getProcAddress("glBeginConditionalRender");
	_glEndConditionalRender = (_PfnGlEndConditionalRender)_getProcAddress("glEndConditionalRender");
	_glVertexAttribIPointer = (_PfnGlVertexAttribIPointer)_getProcAddress("glVertexAttribIPointer");
	_glGetVertexAttribIiv = (_PfnGlGetVertexAttribIiv)_getProcAddress("glGetVertexAttribIiv");
	_glGetVertexAttribIuiv = (_PfnGlGetVertexAttribIuiv)_getProcAddress("glGetVertexAttribIuiv");
	_glVertexAttribI1i = (_PfnGlVertexAttribI1i)_getProcAddress("glVertexAttribI1i");
	_glVertexAttribI2i = (_PfnGlVertexAttribI2i)_getProcAddress("glVertexAttribI2i");
	_glVertexAttribI3i = (_PfnGlVertexAttribI3i)_getProcAddress("glVertexAttribI3i");
	_glVertexAttribI4i = (_PfnGlVertexAttribI4i)_getProcAddress("glVertexAttribI4i");
	_glVertexAttribI1ui = (_PfnGlVertexAttribI1ui)_getProcAddress("glVertexAttribI1ui");
	_glVertexAttribI2ui = (_PfnGlVertexAttribI2ui)_getProcAddress("glVertexAttribI2ui");
	_glVertexAttribI3ui = (_PfnGlVertexAttribI3ui)_getProcAddress("glVertexAttribI3ui");
	_glVertexAttribI4ui = (_PfnGlVertexAttribI4ui)_getProcAddress("glVertexAttribI4ui");
	_glVertexAttribI1iv = (_PfnGlVertexAttribI1iv)_getProcAddress("glVertexAttribI1iv");
	_glVertexAttribI2iv = (_PfnGlVertexAttribI2iv)_getProcAddress("glVertexAttribI2iv");
	_glVertexAttribI3iv = (_PfnGlVertexAttribI3iv)_getProcAddress("glVertexAttribI3iv");
	_glVertexAttribI4iv = (_PfnGlVertexAttribI4iv)_getProcAddress("glVertexAttribI4iv");
	_glVertexAttribI1uiv = (_PfnGlVertexAttribI1uiv)_getProcAddress("glVertexAttribI1uiv");
	_glVertexAttribI2uiv = (_PfnGlVertexAttribI2uiv)_getProcAddress("glVertexAttribI2uiv");
	_glVertexAttribI3uiv = (_PfnGlVertexAttribI3uiv)_getProcAddress("glVertexAttribI3uiv");
	_glVertexAttribI4uiv = (_PfnGlVertexAttribI4uiv)_getProcAddress("glVertexAttribI4uiv");
	_glVertexAttribI4bv = (_PfnGlVertexAttribI4bv)_getProcAddress("glVertexAttribI4bv");
	_glVertexAttribI4sv = (_PfnGlVertexAttribI4sv)_getProcAddress("glVertexAttribI4sv");
	_glVertexAttribI4ubv = (_PfnGlVertexAttribI4ubv)_getProcAddress("glVertexAttribI4ubv");
	_glVertexAttribI4usv = (_PfnGlVertexAttribI4usv)_getProcAddress("glVertexAttribI4usv");
	_glGetUniformuiv = (_PfnGlGetUniformuiv)_getProcAddress("glGetUniformuiv");
	_glBindFragDataLocation = (_PfnGlBindFragDataLocation)_getProcAddress("glBindFragDataLocation");
	_glGetFragDataLocation = (_PfnGlGetFragDataLocation)_getProcAddress("glGetFragDataLocation");
	_glUniform1ui = (_PfnGlUniform1ui)_getProcAddress("glUniform1ui");
	_glUniform2ui = (_PfnGlUniform2ui)_getProcAddress("glUniform2ui");
	_glUniform3ui = (_PfnGlUniform3ui)_getProcAddress("glUniform3ui");
	_glUniform4ui = (_PfnGlUniform4ui)_getProcAddress("glUniform4ui");
	_glUniform1uiv = (_PfnGlUniform1uiv)_getProcAddress("glUniform1uiv");
	_glUniform2uiv = (_PfnGlUniform2uiv)_getProcAddress("glUniform2uiv");
	_glUniform3uiv = (_PfnGlUniform3uiv)_getProcAddress("glUniform3uiv");
	_glUniform4uiv = (_PfnGlUniform4uiv)_getProcAddress("glUniform4uiv");
	_glTexParameterIiv = (_PfnGlTexParameterIiv)_getProcAddress("glTexParameterIiv");
	_glTexParameterIuiv = (_PfnGlTexParameterIuiv)_getProcAddress("glTexParameterIuiv");
	_glGetTexParameterIiv = (_PfnGlGetTexParameterIiv)_getProcAddress("glGetTexParameterIiv");
	_glGetTexParameterIuiv = (_PfnGlGetTexParameterIuiv)_getProcAddress("glGetTexParameterIuiv");
	_glClearBufferiv = (_PfnGlClearBufferiv)_getProcAddress("glClearBufferiv");
	_glClearBufferuiv = (_PfnGlClearBufferuiv)_getProcAddress("glClearBufferuiv");
	_glClearBufferfv = (_PfnGlClearBufferfv)_getProcAddress("glClearBufferfv");
	_glClearBufferfi = (_PfnGlClearBufferfi)_getProcAddress("glClearBufferfi");
	_glGetStringi = (_PfnGlGetStringi)_getProcAddress("glGetStringi");
	_glIsRenderbuffer = (_PfnGlIsRenderbuffer)_getProcAddress("glIsRenderbuffer");
	_glBindRenderbuffer = (_PfnGlBindRenderbuffer)_getProcAddress("glBindRenderbuffer");
	_glDeleteRenderbuffers = (_PfnGlDeleteRenderbuffers)_getProcAddress("glDeleteRenderbuffers");
	_glGenRenderbuffers = (_PfnGlGenRenderbuffers)_getProcAddress("glGenRenderbuffers");
	_glRenderbufferStorage = (_PfnGlRenderbufferStorage)_getProcAddress("glRenderbufferStorage");
	_glGetRenderbufferParameteriv = (_PfnGlGetRenderbufferParameteriv)_getProcAddress("glGetRenderbufferParameteriv");
	_glIsFramebuffer = (_PfnGlIsFramebuffer)_getProcAddress("glIsFramebuffer");
	_glBindFramebuffer = (_PfnGlBindFramebuffer)_getProcAddress("glBindFramebuffer");
	_glDeleteFramebuffers = (_PfnGlDeleteFramebuffers)_getProcAddress("glDeleteFramebuffers");
	_glGenFramebuffers = (_PfnGlGenFramebuffers)_getProcAddress("glGenFramebuffers");
	_glCheckFramebufferStatus = (_PfnGlCheckFramebufferStatus)_getProcAddress("glCheckFramebufferStatus");
	_glFramebufferTexture1D = (_PfnGlFramebufferTexture1D)_getProcAddress("glFramebufferTexture1D");
	_glFramebufferTexture2D = (_PfnGlFramebufferTexture2D)_getProcAddress("glFramebufferTexture2D");
	_glFramebufferTexture3D = (_PfnGlFramebufferTexture3D)_getProcAddress("glFramebufferTexture3D");
	_glFramebufferRenderbuffer = (_PfnGlFramebufferRenderbuffer)_getProcAddress("glFramebufferRenderbuffer");
	_glGetFramebufferAttachmentParameteriv = (_PfnGlGetFramebufferAttachmentParameteriv)_getProcAddress("glGetFramebufferAttachmentParameteriv");
	_glGenerateMipmap = (_PfnGlGenerateMipmap)_getProcAddress("glGenerateMipmap");
	_glBlitFramebuffer = (_PfnGlBlitFramebuffer)_getProcAddress("glBlitFramebuffer");
	_glRenderbufferStorageMultisample = (_PfnGlRenderbufferStorageMultisample)_getProcAddress("glRenderbufferStorageMultisample");
	_glFramebufferTextureLayer = (_PfnGlFramebufferTextureLayer)_getProcAddress("glFramebufferTextureLayer");
	_glMapBufferRange = (_PfnGlMapBufferRange)_getProcAddress("glMapBufferRange");
	_glFlushMappedBufferRange = (_PfnGlFlushMappedBufferRange)_getProcAddress("glFlushMappedBufferRange");
	_glBindVertexArray = (_PfnGlBindVertexArray)_getProcAddress("glBindVertexArray");
	_glDeleteVertexArrays = (_PfnGlDeleteVertexArrays)_getProcAddress("glDeleteVertexArrays");
	_glGenVertexArrays = (_PfnGlGenVertexArrays)_getProcAddress("glGenVertexArrays");
	_glIsVertexArray = (_PfnGlIsVertexArray)_getProcAddress("glIsVertexArray");
	_gl_3_0 = (_gl_2_1 && _procCount == 84);

	// GL_VERSION_3_1

	_procCount = 0;
	_glDrawArraysInstanced = (_PfnGlDrawArraysInstanced)_getProcAddress("glDrawArraysInstanced");
	_glDrawElementsInstanced = (_PfnGlDrawElementsInstanced)_getProcAddress("glDrawElementsInstanced");
	_glTexBuffer = (_PfnGlTexBuffer)_getProcAddress("glTexBuffer");
	_glPrimitiveRestartIndex = (_PfnGlPrimitiveRestartIndex)_getProcAddress("glPrimitiveRestartIndex");
	_glCopyBufferSubData = (_PfnGlCopyBufferSubData)_getProcAddress("glCopyBufferSubData");
	_glGetUniformIndices = (_PfnGlGetUniformIndices)_getProcAddress("glGetUniformIndices");
	_glGetActiveUniformsiv = (_PfnGlGetActiveUniformsiv)_getProcAddress("glGetActiveUniformsiv");
	_glGetActiveUniformName = (_PfnGlGetActiveUniformName)_getProcAddress("glGetActiveUniformName");
	_glGetUniformBlockIndex = (_PfnGlGetUniformBlockIndex)_getProcAddress("glGetUniformBlockIndex");
	_glGetActiveUniformBlockiv = (_PfnGlGetActiveUniformBlockiv)_getProcAddress("glGetActiveUniformBlockiv");
	_glGetActiveUniformBlockName = (_PfnGlGetActiveUniformBlockName)_getProcAddress("glGetActiveUniformBlockName");
	_glUniformBlockBinding = (_PfnGlUniformBlockBinding)_getProcAddress("glUniformBlockBinding");
	_gl_3_1 = (_gl_3_0 && _procCount == 12);

	// GL_VERSION_3_2

	_procCount = 0;
	_glDrawElementsBaseVertex = (_PfnGlDrawElementsBaseVertex)_getProcAddress("glDrawElementsBaseVertex");
	_glDrawRangeElementsBaseVertex = (_PfnGlDrawRangeElementsBaseVertex)_getProcAddress("glDrawRangeElementsBaseVertex");
	_glDrawElementsInstancedBaseVertex = (_PfnGlDrawElementsInstancedBaseVertex)_getProcAddress("glDrawElementsInstancedBaseVertex");
	_glMultiDrawElementsBaseVertex = (_PfnGlMultiDrawElementsBaseVertex)_getProcAddress("glMultiDrawElementsBaseVertex");
	_glProvokingVertex = (_PfnGlProvokingVertex)_getProcAddress("glProvokingVertex");
	_glFenceSync = (_PfnGlFenceSync)_getProcAddress("glFenceSync");
	_glIsSync = (_PfnGlIsSync)_getProcAddress("glIsSync");
	_glDeleteSync = (_PfnGlDeleteSync)_getProcAddress("glDeleteSync");
	_glClientWaitSync = (_PfnGlClientWaitSync)_getProcAddress("glClientWaitSync");
	_glWaitSync = (_PfnGlWaitSync)_getProcAddress("glWaitSync");
	_glGetInteger64v = (_PfnGlGetInteger64v)_getProcAddress("glGetInteger64v");
	_glGetSynciv = (_PfnGlGetSynciv)_getProcAddress("glGetSynciv");
	_glGetInteger64i_v = (_PfnGlGetInteger64i_v)_getProcAddress("glGetInteger64i_v");
	_glGetBufferParameteri64v = (_PfnGlGetBufferParameteri64v)_getProcAddress("glGetBufferParameteri64v");
	_glFramebufferTexture = (_PfnGlFramebufferTexture)_getProcAddress("glFramebufferTexture");
	_glTexImage2DMultisample = (_PfnGlTexImage2DMultisample)_getProcAddress("glTexImage2DMultisample");
	_glTexImage3DMultisample = (_PfnGlTexImage3DMultisample)_getProcAddress("glTexImage3DMultisample");
	_glGetMultisamplefv = (_PfnGlGetMultisamplefv)_getProcAddress("glGetMultisamplefv");
	_glSampleMaski = (_PfnGlSampleMaski)_getProcAddress("glSampleMaski");
	_gl_3_2 = (_gl_3_1 && _procCount == 19);

	// GL_VERSION_3_3

	_procCount = 0;
	_glBindFragDataLocationIndexed = (_PfnGlBindFragDataLocationIndexed)_getProcAddress("glBindFragDataLocationIndexed");
	_glGetFragDataIndex = (_PfnGlGetFragDataIndex)_getProcAddress("glGetFragDataIndex");
	_glGenSamplers = (_PfnGlGenSamplers)_getProcAddress("glGenSamplers");
	_glDeleteSamplers = (_PfnGlDeleteSamplers)_getProcAddress("glDeleteSamplers");
	_glIsSampler = (_PfnGlIsSampler)_getProcAddress("glIsSampler");
	_glBindSampler = (_PfnGlBindSampler)_getProcAddress("glBindSampler");
	_glSamplerParameteri = (_PfnGlSamplerParameteri)_getProcAddress("glSamplerParameteri");
	_glSamplerParameteriv = (_PfnGlSamplerParameteriv)_getProcAddress("glSamplerParameteriv");
	_glSamplerParameterf = (_PfnGlSamplerParameterf)_getProcAddress("glSamplerParameterf");
	_glSamplerParameterfv = (_PfnGlSamplerParameterfv)_getProcAddress("glSamplerParameterfv");
	_glSamplerParameterIiv = (_PfnGlSamplerParameterIiv)_getProcAddress("glSamplerParameterIiv");
	_glSamplerParameterIuiv = (_PfnGlSamplerParameterIuiv)_getProcAddress("glSamplerParameterIuiv");
	_glGetSamplerParameteriv = (_PfnGlGetSamplerParameteriv)_getProcAddress("glGetSamplerParameteriv");
	_glGetSamplerParameterIiv = (_PfnGlGetSamplerParameterIiv)_getProcAddress("glGetSamplerParameterIiv");
	_glGetSamplerParameterfv = (_PfnGlGetSamplerParameterfv)_getProcAddress("glGetSamplerParameterfv");
	_glGetSamplerParameterIuiv = (_PfnGlGetSamplerParameterIuiv)_getProcAddress("glGetSamplerParameterIuiv");
	_glQueryCounter = (_PfnGlQueryCounter)_getProcAddress("glQueryCounter");
	_glGetQueryObjecti64v = (_PfnGlGetQueryObjecti64v)_getProcAddress("glGetQueryObjecti64v");
	_glGetQueryObjectui64v = (_PfnGlGetQueryObjectui64v)_getProcAddress("glGetQueryObjectui64v");
	_glVertexAttribDivisor = (_PfnGlVertexAttribDivisor)_getProcAddress("glVertexAttribDivisor");
	_glVertexAttribP1ui = (_PfnGlVertexAttribP1ui)_getProcAddress("glVertexAttribP1ui");
	_glVertexAttribP1uiv = (_PfnGlVertexAttribP1uiv)_getProcAddress("glVertexAttribP1uiv");
	_glVertexAttribP2ui = (_PfnGlVertexAttribP2ui)_getProcAddress("glVertexAttribP2ui");
	_glVertexAttribP2uiv = (_PfnGlVertexAttribP2uiv)_getProcAddress("glVertexAttribP2uiv");
	_glVertexAttribP3ui = (_PfnGlVertexAttribP3ui)_getProcAddress("glVertexAttribP3ui");
	_glVertexAttribP3uiv = (_PfnGlVertexAttribP3uiv)_getProcAddress("glVertexAttribP3uiv");
	_glVertexAttribP4ui = (_PfnGlVertexAttribP4ui)_getProcAddress("glVertexAttribP4ui");
	_glVertexAttribP4uiv = (_PfnGlVertexAttribP4uiv)_getProcAddress("glVertexAttribP4uiv");
	_gl_3_3 = (_gl_3_2 && _procCount == 28);

	// GL_KHR_debug

	_procCount = 0;
	_glDebugMessageControl = (_PfnGlDebugMessageControl)_getProcAddress("glDebugMessageControl");
	_glDebugMessageInsert = (_PfnGlDebugMessageInsert)_getProcAddress("glDebugMessageInsert");
	_glDebugMessageCallback = (_PfnGlDebugMessageCallback)_getProcAddress("glDebugMessageCallback");
	_glGetDebugMessageLog = (_PfnGlGetDebugMessageLog)_getProcAddress("glGetDebugMessageLog");
	_glPushDebugGroup = (_PfnGlPushDebugGroup)_getProcAddress("glPushDebugGroup");
	_glPopDebugGroup = (_PfnGlPopDebugGroup)_getProcAddress("glPopDebugGroup");
	_glObjectLabel = (_PfnGlObjectLabel)_getProcAddress("glObjectLabel");
	_glGetObjectLabel = (_PfnGlGetObjectLabel)_getProcAddress("glGetObjectLabel");
	_glObjectPtrLabel = (_PfnGlObjectPtrLabel)_getProcAddress("glObjectPtrLabel");
	_glGetObjectPtrLabel = (_PfnGlGetObjectPtrLabel)_getProcAddress("glGetObjectPtrLabel");
	_glGetPointerv = (_PfnGlGetPointerv)_getProcAddress("glGetPointerv");
	_gl_khr_debug = (_procCount == 11);

	// GL_ARB_texture_storage

	_procCount = 0;
	_glTexStorage1D = (_PfnGlTexStorage1D)_getProcAddress("glTexStorage1D");
	_glTexStorage2D = (_PfnGlTexStorage2D)_getProcAddress("glTexStorage2D");
	_glTexStorage3D = (_PfnGlTexStorage3D)_getProcAddress("glTexStorage3D");
	_gl_arb_texture_storage = (_procCount == 3);

	_gl_ext_texture_filter_anisotropic = hasExtension("GL_EXT_texture_filter_anisotropic");
	_gl_khr_debug = _gl_khr_debug && hasExtension("GL_KHR_debug");
	_gl_arb_conservative_depth = hasExtension("GL_ARB_conservative_depth");
	_gl_arb_texture_storage = _gl_arb_texture_storage && hasExtension("GL_ARB_texture_storage");

	if(!_gl_3_3) {
		log().error("Failed to load gl 3.3.");
		return false;
	}

	log().info("OpenGL version: ",      getString(gl::VERSION));
	log().info("OpenGL GLSL version: ", getString(gl::SHADING_LANGUAGE_VERSION));
	log().info("OpenGL vendor: ",       getString(gl::VENDOR));
	log().info("OpenGL renderer: ",     getString(gl::RENDERER));
	if(_gl_ext_texture_filter_anisotropic) log().info("OpenGL enable extension: GL_EXT_texture_filter_anisotropic");
	if(_gl_khr_debug) log().info("OpenGL enable extension: GL_KHR_debug");
	if(_gl_arb_conservative_depth) log().info("OpenGL enable extension: GL_ARB_conservative_depth");
	if(_gl_arb_texture_storage) log().info("OpenGL enable extension: GL_ARB_texture_storage");

	return _gl_3_3;
}


void Context::shutdown() {
}


Logger& Context::log() {
	return *_log;
}


const char* Context::getEnumName(GLenum enum_) const {
	switch(enum_) {
		case 0x00000000: return "[GL_FALSE|GL_POINTS|GL_ZERO|GL_NONE|GL_NO_ERROR]";
		case 0x00000001: return "[GL_TRUE|GL_LINES|GL_ONE|GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT|GL_MAP_READ_BIT|GL_CONTEXT_CORE_PROFILE_BIT|GL_SYNC_FLUSH_COMMANDS_BIT]";
		case 0x00000002: return "[GL_LINE_LOOP|GL_MAP_WRITE_BIT|GL_CONTEXT_COMPATIBILITY_PROFILE_BIT|GL_CONTEXT_FLAG_DEBUG_BIT]";
		case 0x00000003: return "GL_LINE_STRIP";
		case 0x00000004: return "[GL_TRIANGLES|GL_MAP_INVALIDATE_RANGE_BIT]";
		case 0x00000005: return "GL_TRIANGLE_STRIP";
		case 0x00000006: return "GL_TRIANGLE_FAN";
		case 0x00000008: return "GL_MAP_INVALIDATE_BUFFER_BIT";
		case 0x0000000a: return "GL_LINES_ADJACENCY";
		case 0x0000000b: return "GL_LINE_STRIP_ADJACENCY";
		case 0x0000000c: return "GL_TRIANGLES_ADJACENCY";
		case 0x0000000d: return "GL_TRIANGLE_STRIP_ADJACENCY";
		case 0x00000010: return "GL_MAP_FLUSH_EXPLICIT_BIT";
		case 0x00000020: return "GL_MAP_UNSYNCHRONIZED_BIT";
		case 0x00000100: return "GL_DEPTH_BUFFER_BIT";
		case 0x00000200: return "GL_NEVER";
		case 0x00000201: return "GL_LESS";
		case 0x00000202: return "GL_EQUAL";
		case 0x00000203: return "GL_LEQUAL";
		case 0x00000204: return "GL_GREATER";
		case 0x00000205: return "GL_NOTEQUAL";
		case 0x00000206: return "GL_GEQUAL";
		case 0x00000207: return "GL_ALWAYS";
		case 0x00000300: return "GL_SRC_COLOR";
		case 0x00000301: return "GL_ONE_MINUS_SRC_COLOR";
		case 0x00000302: return "GL_SRC_ALPHA";
		case 0x00000303: return "GL_ONE_MINUS_SRC_ALPHA";
		case 0x00000304: return "GL_DST_ALPHA";
		case 0x00000305: return "GL_ONE_MINUS_DST_ALPHA";
		case 0x00000306: return "GL_DST_COLOR";
		case 0x00000307: return "GL_ONE_MINUS_DST_COLOR";
		case 0x00000308: return "GL_SRC_ALPHA_SATURATE";
		case 0x00000400: return "[GL_STENCIL_BUFFER_BIT|GL_FRONT_LEFT]";
		case 0x00000401: return "GL_FRONT_RIGHT";
		case 0x00000402: return "GL_BACK_LEFT";
		case 0x00000403: return "GL_BACK_RIGHT";
		case 0x00000404: return "GL_FRONT";
		case 0x00000405: return "GL_BACK";
		case 0x00000406: return "GL_LEFT";
		case 0x00000407: return "GL_RIGHT";
		case 0x00000408: return "GL_FRONT_AND_BACK";
		case 0x00000500: return "GL_INVALID_ENUM";
		case 0x00000501: return "GL_INVALID_VALUE";
		case 0x00000502: return "GL_INVALID_OPERATION";
		case 0x00000503: return "GL_STACK_OVERFLOW";
		case 0x00000504: return "GL_STACK_UNDERFLOW";
		case 0x00000505: return "GL_OUT_OF_MEMORY";
		case 0x00000506: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case 0x00000900: return "GL_CW";
		case 0x00000901: return "GL_CCW";
		case 0x00000b11: return "GL_POINT_SIZE";
		case 0x00000b12: return "[GL_POINT_SIZE_RANGE|GL_SMOOTH_POINT_SIZE_RANGE]";
		case 0x00000b13: return "[GL_POINT_SIZE_GRANULARITY|GL_SMOOTH_POINT_SIZE_GRANULARITY]";
		case 0x00000b20: return "GL_LINE_SMOOTH";
		case 0x00000b21: return "GL_LINE_WIDTH";
		case 0x00000b22: return "[GL_LINE_WIDTH_RANGE|GL_SMOOTH_LINE_WIDTH_RANGE]";
		case 0x00000b23: return "[GL_LINE_WIDTH_GRANULARITY|GL_SMOOTH_LINE_WIDTH_GRANULARITY]";
		case 0x00000b40: return "GL_POLYGON_MODE";
		case 0x00000b41: return "GL_POLYGON_SMOOTH";
		case 0x00000b44: return "GL_CULL_FACE";
		case 0x00000b45: return "GL_CULL_FACE_MODE";
		case 0x00000b46: return "GL_FRONT_FACE";
		case 0x00000b70: return "GL_DEPTH_RANGE";
		case 0x00000b71: return "GL_DEPTH_TEST";
		case 0x00000b72: return "GL_DEPTH_WRITEMASK";
		case 0x00000b73: return "GL_DEPTH_CLEAR_VALUE";
		case 0x00000b74: return "GL_DEPTH_FUNC";
		case 0x00000b90: return "GL_STENCIL_TEST";
		case 0x00000b91: return "GL_STENCIL_CLEAR_VALUE";
		case 0x00000b92: return "GL_STENCIL_FUNC";
		case 0x00000b93: return "GL_STENCIL_VALUE_MASK";
		case 0x00000b94: return "GL_STENCIL_FAIL";
		case 0x00000b95: return "GL_STENCIL_PASS_DEPTH_FAIL";
		case 0x00000b96: return "GL_STENCIL_PASS_DEPTH_PASS";
		case 0x00000b97: return "GL_STENCIL_REF";
		case 0x00000b98: return "GL_STENCIL_WRITEMASK";
		case 0x00000ba2: return "GL_VIEWPORT";
		case 0x00000bd0: return "GL_DITHER";
		case 0x00000be0: return "GL_BLEND_DST";
		case 0x00000be1: return "GL_BLEND_SRC";
		case 0x00000be2: return "GL_BLEND";
		case 0x00000bf0: return "GL_LOGIC_OP_MODE";
		case 0x00000bf2: return "GL_COLOR_LOGIC_OP";
		case 0x00000c01: return "GL_DRAW_BUFFER";
		case 0x00000c02: return "GL_READ_BUFFER";
		case 0x00000c10: return "GL_SCISSOR_BOX";
		case 0x00000c11: return "GL_SCISSOR_TEST";
		case 0x00000c22: return "GL_COLOR_CLEAR_VALUE";
		case 0x00000c23: return "GL_COLOR_WRITEMASK";
		case 0x00000c32: return "GL_DOUBLEBUFFER";
		case 0x00000c33: return "GL_STEREO";
		case 0x00000c52: return "GL_LINE_SMOOTH_HINT";
		case 0x00000c53: return "GL_POLYGON_SMOOTH_HINT";
		case 0x00000cf0: return "GL_UNPACK_SWAP_BYTES";
		case 0x00000cf1: return "GL_UNPACK_LSB_FIRST";
		case 0x00000cf2: return "GL_UNPACK_ROW_LENGTH";
		case 0x00000cf3: return "GL_UNPACK_SKIP_ROWS";
		case 0x00000cf4: return "GL_UNPACK_SKIP_PIXELS";
		case 0x00000cf5: return "GL_UNPACK_ALIGNMENT";
		case 0x00000d00: return "GL_PACK_SWAP_BYTES";
		case 0x00000d01: return "GL_PACK_LSB_FIRST";
		case 0x00000d02: return "GL_PACK_ROW_LENGTH";
		case 0x00000d03: return "GL_PACK_SKIP_ROWS";
		case 0x00000d04: return "GL_PACK_SKIP_PIXELS";
		case 0x00000d05: return "GL_PACK_ALIGNMENT";
		case 0x00000d32: return "GL_MAX_CLIP_DISTANCES";
		case 0x00000d33: return "GL_MAX_TEXTURE_SIZE";
		case 0x00000d3a: return "GL_MAX_VIEWPORT_DIMS";
		case 0x00000d50: return "GL_SUBPIXEL_BITS";
		case 0x00000de0: return "GL_TEXTURE_1D";
		case 0x00000de1: return "GL_TEXTURE_2D";
		case 0x00001000: return "GL_TEXTURE_WIDTH";
		case 0x00001001: return "GL_TEXTURE_HEIGHT";
		case 0x00001003: return "GL_TEXTURE_INTERNAL_FORMAT";
		case 0x00001004: return "GL_TEXTURE_BORDER_COLOR";
		case 0x00001100: return "GL_DONT_CARE";
		case 0x00001101: return "GL_FASTEST";
		case 0x00001102: return "GL_NICEST";
		case 0x00001400: return "GL_BYTE";
		case 0x00001401: return "GL_UNSIGNED_BYTE";
		case 0x00001402: return "GL_SHORT";
		case 0x00001403: return "GL_UNSIGNED_SHORT";
		case 0x00001404: return "GL_INT";
		case 0x00001405: return "GL_UNSIGNED_INT";
		case 0x00001406: return "GL_FLOAT";
		case 0x0000140a: return "GL_DOUBLE";
		case 0x0000140b: return "GL_HALF_FLOAT";
		case 0x00001500: return "GL_CLEAR";
		case 0x00001501: return "GL_AND";
		case 0x00001502: return "GL_AND_REVERSE";
		case 0x00001503: return "GL_COPY";
		case 0x00001504: return "GL_AND_INVERTED";
		case 0x00001505: return "GL_NOOP";
		case 0x00001506: return "GL_XOR";
		case 0x00001507: return "GL_OR";
		case 0x00001508: return "GL_NOR";
		case 0x00001509: return "GL_EQUIV";
		case 0x0000150a: return "GL_INVERT";
		case 0x0000150b: return "GL_OR_REVERSE";
		case 0x0000150c: return "GL_COPY_INVERTED";
		case 0x0000150d: return "GL_OR_INVERTED";
		case 0x0000150e: return "GL_NAND";
		case 0x0000150f: return "GL_SET";
		case 0x00001702: return "GL_TEXTURE";
		case 0x00001800: return "GL_COLOR";
		case 0x00001801: return "GL_DEPTH";
		case 0x00001802: return "GL_STENCIL";
		case 0x00001901: return "GL_STENCIL_INDEX";
		case 0x00001902: return "GL_DEPTH_COMPONENT";
		case 0x00001903: return "GL_RED";
		case 0x00001904: return "GL_GREEN";
		case 0x00001905: return "GL_BLUE";
		case 0x00001906: return "GL_ALPHA";
		case 0x00001907: return "GL_RGB";
		case 0x00001908: return "GL_RGBA";
		case 0x00001b00: return "GL_POINT";
		case 0x00001b01: return "GL_LINE";
		case 0x00001b02: return "GL_FILL";
		case 0x00001e00: return "GL_KEEP";
		case 0x00001e01: return "GL_REPLACE";
		case 0x00001e02: return "GL_INCR";
		case 0x00001e03: return "GL_DECR";
		case 0x00001f00: return "GL_VENDOR";
		case 0x00001f01: return "GL_RENDERER";
		case 0x00001f02: return "GL_VERSION";
		case 0x00001f03: return "GL_EXTENSIONS";
		case 0x00002600: return "GL_NEAREST";
		case 0x00002601: return "GL_LINEAR";
		case 0x00002700: return "GL_NEAREST_MIPMAP_NEAREST";
		case 0x00002701: return "GL_LINEAR_MIPMAP_NEAREST";
		case 0x00002702: return "GL_NEAREST_MIPMAP_LINEAR";
		case 0x00002703: return "GL_LINEAR_MIPMAP_LINEAR";
		case 0x00002800: return "GL_TEXTURE_MAG_FILTER";
		case 0x00002801: return "GL_TEXTURE_MIN_FILTER";
		case 0x00002802: return "GL_TEXTURE_WRAP_S";
		case 0x00002803: return "GL_TEXTURE_WRAP_T";
		case 0x00002901: return "GL_REPEAT";
		case 0x00002a00: return "GL_POLYGON_OFFSET_UNITS";
		case 0x00002a01: return "GL_POLYGON_OFFSET_POINT";
		case 0x00002a02: return "GL_POLYGON_OFFSET_LINE";
		case 0x00002a10: return "GL_R3_G3_B2";
		case 0x00003000: return "GL_CLIP_DISTANCE0";
		case 0x00003001: return "GL_CLIP_DISTANCE1";
		case 0x00003002: return "GL_CLIP_DISTANCE2";
		case 0x00003003: return "GL_CLIP_DISTANCE3";
		case 0x00003004: return "GL_CLIP_DISTANCE4";
		case 0x00003005: return "GL_CLIP_DISTANCE5";
		case 0x00003006: return "GL_CLIP_DISTANCE6";
		case 0x00003007: return "GL_CLIP_DISTANCE7";
		case 0x00004000: return "GL_COLOR_BUFFER_BIT";
		case 0x00008001: return "GL_CONSTANT_COLOR";
		case 0x00008002: return "GL_ONE_MINUS_CONSTANT_COLOR";
		case 0x00008003: return "GL_CONSTANT_ALPHA";
		case 0x00008004: return "GL_ONE_MINUS_CONSTANT_ALPHA";
		case 0x00008005: return "GL_BLEND_COLOR";
		case 0x00008006: return "GL_FUNC_ADD";
		case 0x00008007: return "GL_MIN";
		case 0x00008008: return "GL_MAX";
		case 0x00008009: return "[GL_BLEND_EQUATION|GL_BLEND_EQUATION_RGB]";
		case 0x0000800a: return "GL_FUNC_SUBTRACT";
		case 0x0000800b: return "GL_FUNC_REVERSE_SUBTRACT";
		case 0x00008032: return "GL_UNSIGNED_BYTE_3_3_2";
		case 0x00008033: return "GL_UNSIGNED_SHORT_4_4_4_4";
		case 0x00008034: return "GL_UNSIGNED_SHORT_5_5_5_1";
		case 0x00008035: return "GL_UNSIGNED_INT_8_8_8_8";
		case 0x00008036: return "GL_UNSIGNED_INT_10_10_10_2";
		case 0x00008037: return "GL_POLYGON_OFFSET_FILL";
		case 0x00008038: return "GL_POLYGON_OFFSET_FACTOR";
		case 0x0000804f: return "GL_RGB4";
		case 0x00008050: return "GL_RGB5";
		case 0x00008051: return "GL_RGB8";
		case 0x00008052: return "GL_RGB10";
		case 0x00008053: return "GL_RGB12";
		case 0x00008054: return "GL_RGB16";
		case 0x00008055: return "GL_RGBA2";
		case 0x00008056: return "GL_RGBA4";
		case 0x00008057: return "GL_RGB5_A1";
		case 0x00008058: return "GL_RGBA8";
		case 0x00008059: return "GL_RGB10_A2";
		case 0x0000805a: return "GL_RGBA12";
		case 0x0000805b: return "GL_RGBA16";
		case 0x0000805c: return "GL_TEXTURE_RED_SIZE";
		case 0x0000805d: return "GL_TEXTURE_GREEN_SIZE";
		case 0x0000805e: return "GL_TEXTURE_BLUE_SIZE";
		case 0x0000805f: return "GL_TEXTURE_ALPHA_SIZE";
		case 0x00008063: return "GL_PROXY_TEXTURE_1D";
		case 0x00008064: return "GL_PROXY_TEXTURE_2D";
		case 0x00008068: return "GL_TEXTURE_BINDING_1D";
		case 0x00008069: return "GL_TEXTURE_BINDING_2D";
		case 0x0000806a: return "GL_TEXTURE_BINDING_3D";
		case 0x0000806b: return "GL_PACK_SKIP_IMAGES";
		case 0x0000806c: return "GL_PACK_IMAGE_HEIGHT";
		case 0x0000806d: return "GL_UNPACK_SKIP_IMAGES";
		case 0x0000806e: return "GL_UNPACK_IMAGE_HEIGHT";
		case 0x0000806f: return "GL_TEXTURE_3D";
		case 0x00008070: return "GL_PROXY_TEXTURE_3D";
		case 0x00008071: return "GL_TEXTURE_DEPTH";
		case 0x00008072: return "GL_TEXTURE_WRAP_R";
		case 0x00008073: return "GL_MAX_3D_TEXTURE_SIZE";
		case 0x00008074: return "GL_VERTEX_ARRAY";
		case 0x0000809d: return "GL_MULTISAMPLE";
		case 0x0000809e: return "GL_SAMPLE_ALPHA_TO_COVERAGE";
		case 0x0000809f: return "GL_SAMPLE_ALPHA_TO_ONE";
		case 0x000080a0: return "GL_SAMPLE_COVERAGE";
		case 0x000080a8: return "GL_SAMPLE_BUFFERS";
		case 0x000080a9: return "GL_SAMPLES";
		case 0x000080aa: return "GL_SAMPLE_COVERAGE_VALUE";
		case 0x000080ab: return "GL_SAMPLE_COVERAGE_INVERT";
		case 0x000080c8: return "GL_BLEND_DST_RGB";
		case 0x000080c9: return "GL_BLEND_SRC_RGB";
		case 0x000080ca: return "GL_BLEND_DST_ALPHA";
		case 0x000080cb: return "GL_BLEND_SRC_ALPHA";
		case 0x000080e0: return "GL_BGR";
		case 0x000080e1: return "GL_BGRA";
		case 0x000080e8: return "GL_MAX_ELEMENTS_VERTICES";
		case 0x000080e9: return "GL_MAX_ELEMENTS_INDICES";
		case 0x00008128: return "GL_POINT_FADE_THRESHOLD_SIZE";
		case 0x0000812d: return "GL_CLAMP_TO_BORDER";
		case 0x0000812f: return "GL_CLAMP_TO_EDGE";
		case 0x0000813a: return "GL_TEXTURE_MIN_LOD";
		case 0x0000813b: return "GL_TEXTURE_MAX_LOD";
		case 0x0000813c: return "GL_TEXTURE_BASE_LEVEL";
		case 0x0000813d: return "GL_TEXTURE_MAX_LEVEL";
		case 0x000081a5: return "GL_DEPTH_COMPONENT16";
		case 0x000081a6: return "GL_DEPTH_COMPONENT24";
		case 0x000081a7: return "GL_DEPTH_COMPONENT32";
		case 0x00008210: return "GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING";
		case 0x00008211: return "GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE";
		case 0x00008212: return "GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE";
		case 0x00008213: return "GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE";
		case 0x00008214: return "GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE";
		case 0x00008215: return "GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE";
		case 0x00008216: return "GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE";
		case 0x00008217: return "GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE";
		case 0x00008218: return "GL_FRAMEBUFFER_DEFAULT";
		case 0x00008219: return "GL_FRAMEBUFFER_UNDEFINED";
		case 0x0000821a: return "GL_DEPTH_STENCIL_ATTACHMENT";
		case 0x0000821b: return "GL_MAJOR_VERSION";
		case 0x0000821c: return "GL_MINOR_VERSION";
		case 0x0000821d: return "GL_NUM_EXTENSIONS";
		case 0x0000821e: return "GL_CONTEXT_FLAGS";
		case 0x00008225: return "GL_COMPRESSED_RED";
		case 0x00008226: return "GL_COMPRESSED_RG";
		case 0x00008227: return "GL_RG";
		case 0x00008228: return "GL_RG_INTEGER";
		case 0x00008229: return "GL_R8";
		case 0x0000822a: return "GL_R16";
		case 0x0000822b: return "GL_RG8";
		case 0x0000822c: return "GL_RG16";
		case 0x0000822d: return "GL_R16F";
		case 0x0000822e: return "GL_R32F";
		case 0x0000822f: return "GL_RG16F";
		case 0x00008230: return "GL_RG32F";
		case 0x00008231: return "GL_R8I";
		case 0x00008232: return "GL_R8UI";
		case 0x00008233: return "GL_R16I";
		case 0x00008234: return "GL_R16UI";
		case 0x00008235: return "GL_R32I";
		case 0x00008236: return "GL_R32UI";
		case 0x00008237: return "GL_RG8I";
		case 0x00008238: return "GL_RG8UI";
		case 0x00008239: return "GL_RG16I";
		case 0x0000823a: return "GL_RG16UI";
		case 0x0000823b: return "GL_RG32I";
		case 0x0000823c: return "GL_RG32UI";
		case 0x00008242: return "GL_DEBUG_OUTPUT_SYNCHRONOUS";
		case 0x00008243: return "GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH";
		case 0x00008244: return "GL_DEBUG_CALLBACK_FUNCTION";
		case 0x00008245: return "GL_DEBUG_CALLBACK_USER_PARAM";
		case 0x00008246: return "GL_DEBUG_SOURCE_API";
		case 0x00008247: return "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
		case 0x00008248: return "GL_DEBUG_SOURCE_SHADER_COMPILER";
		case 0x00008249: return "GL_DEBUG_SOURCE_THIRD_PARTY";
		case 0x0000824a: return "GL_DEBUG_SOURCE_APPLICATION";
		case 0x0000824b: return "GL_DEBUG_SOURCE_OTHER";
		case 0x0000824c: return "GL_DEBUG_TYPE_ERROR";
		case 0x0000824d: return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
		case 0x0000824e: return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
		case 0x0000824f: return "GL_DEBUG_TYPE_PORTABILITY";
		case 0x00008250: return "GL_DEBUG_TYPE_PERFORMANCE";
		case 0x00008251: return "GL_DEBUG_TYPE_OTHER";
		case 0x00008268: return "GL_DEBUG_TYPE_MARKER";
		case 0x00008269: return "GL_DEBUG_TYPE_PUSH_GROUP";
		case 0x0000826a: return "GL_DEBUG_TYPE_POP_GROUP";
		case 0x0000826b: return "GL_DEBUG_SEVERITY_NOTIFICATION";
		case 0x0000826c: return "GL_MAX_DEBUG_GROUP_STACK_DEPTH";
		case 0x0000826d: return "GL_DEBUG_GROUP_STACK_DEPTH";
		case 0x000082e0: return "GL_BUFFER";
		case 0x000082e1: return "GL_SHADER";
		case 0x000082e2: return "GL_PROGRAM";
		case 0x000082e3: return "GL_QUERY";
		case 0x000082e4: return "GL_PROGRAM_PIPELINE";
		case 0x000082e6: return "GL_SAMPLER";
		case 0x000082e8: return "GL_MAX_LABEL_LENGTH";
		case 0x00008362: return "GL_UNSIGNED_BYTE_2_3_3_REV";
		case 0x00008363: return "GL_UNSIGNED_SHORT_5_6_5";
		case 0x00008364: return "GL_UNSIGNED_SHORT_5_6_5_REV";
		case 0x00008365: return "GL_UNSIGNED_SHORT_4_4_4_4_REV";
		case 0x00008366: return "GL_UNSIGNED_SHORT_1_5_5_5_REV";
		case 0x00008367: return "GL_UNSIGNED_INT_8_8_8_8_REV";
		case 0x00008368: return "GL_UNSIGNED_INT_2_10_10_10_REV";
		case 0x00008370: return "GL_MIRRORED_REPEAT";
		case 0x0000846e: return "GL_ALIASED_LINE_WIDTH_RANGE";
		case 0x000084c0: return "GL_TEXTURE0";
		case 0x000084c1: return "GL_TEXTURE1";
		case 0x000084c2: return "GL_TEXTURE2";
		case 0x000084c3: return "GL_TEXTURE3";
		case 0x000084c4: return "GL_TEXTURE4";
		case 0x000084c5: return "GL_TEXTURE5";
		case 0x000084c6: return "GL_TEXTURE6";
		case 0x000084c7: return "GL_TEXTURE7";
		case 0x000084c8: return "GL_TEXTURE8";
		case 0x000084c9: return "GL_TEXTURE9";
		case 0x000084ca: return "GL_TEXTURE10";
		case 0x000084cb: return "GL_TEXTURE11";
		case 0x000084cc: return "GL_TEXTURE12";
		case 0x000084cd: return "GL_TEXTURE13";
		case 0x000084ce: return "GL_TEXTURE14";
		case 0x000084cf: return "GL_TEXTURE15";
		case 0x000084d0: return "GL_TEXTURE16";
		case 0x000084d1: return "GL_TEXTURE17";
		case 0x000084d2: return "GL_TEXTURE18";
		case 0x000084d3: return "GL_TEXTURE19";
		case 0x000084d4: return "GL_TEXTURE20";
		case 0x000084d5: return "GL_TEXTURE21";
		case 0x000084d6: return "GL_TEXTURE22";
		case 0x000084d7: return "GL_TEXTURE23";
		case 0x000084d8: return "GL_TEXTURE24";
		case 0x000084d9: return "GL_TEXTURE25";
		case 0x000084da: return "GL_TEXTURE26";
		case 0x000084db: return "GL_TEXTURE27";
		case 0x000084dc: return "GL_TEXTURE28";
		case 0x000084dd: return "GL_TEXTURE29";
		case 0x000084de: return "GL_TEXTURE30";
		case 0x000084df: return "GL_TEXTURE31";
		case 0x000084e0: return "GL_ACTIVE_TEXTURE";
		case 0x000084e8: return "GL_MAX_RENDERBUFFER_SIZE";
		case 0x000084ed: return "GL_COMPRESSED_RGB";
		case 0x000084ee: return "GL_COMPRESSED_RGBA";
		case 0x000084ef: return "GL_TEXTURE_COMPRESSION_HINT";
		case 0x000084f5: return "GL_TEXTURE_RECTANGLE";
		case 0x000084f6: return "GL_TEXTURE_BINDING_RECTANGLE";
		case 0x000084f7: return "GL_PROXY_TEXTURE_RECTANGLE";
		case 0x000084f8: return "GL_MAX_RECTANGLE_TEXTURE_SIZE";
		case 0x000084f9: return "GL_DEPTH_STENCIL";
		case 0x000084fa: return "GL_UNSIGNED_INT_24_8";
		case 0x000084fd: return "GL_MAX_TEXTURE_LOD_BIAS";
		case 0x000084fe: return "GL_TEXTURE_MAX_ANISOTROPY_EXT";
		case 0x000084ff: return "GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT";
		case 0x00008501: return "GL_TEXTURE_LOD_BIAS";
		case 0x00008507: return "GL_INCR_WRAP";
		case 0x00008508: return "GL_DECR_WRAP";
		case 0x00008513: return "GL_TEXTURE_CUBE_MAP";
		case 0x00008514: return "GL_TEXTURE_BINDING_CUBE_MAP";
		case 0x00008515: return "GL_TEXTURE_CUBE_MAP_POSITIVE_X";
		case 0x00008516: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_X";
		case 0x00008517: return "GL_TEXTURE_CUBE_MAP_POSITIVE_Y";
		case 0x00008518: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";
		case 0x00008519: return "GL_TEXTURE_CUBE_MAP_POSITIVE_Z";
		case 0x0000851a: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";
		case 0x0000851b: return "GL_PROXY_TEXTURE_CUBE_MAP";
		case 0x0000851c: return "GL_MAX_CUBE_MAP_TEXTURE_SIZE";
		case 0x00008589: return "GL_SRC1_ALPHA";
		case 0x000085b5: return "GL_VERTEX_ARRAY_BINDING";
		case 0x00008622: return "GL_VERTEX_ATTRIB_ARRAY_ENABLED";
		case 0x00008623: return "GL_VERTEX_ATTRIB_ARRAY_SIZE";
		case 0x00008624: return "GL_VERTEX_ATTRIB_ARRAY_STRIDE";
		case 0x00008625: return "GL_VERTEX_ATTRIB_ARRAY_TYPE";
		case 0x00008626: return "GL_CURRENT_VERTEX_ATTRIB";
		case 0x00008642: return "[GL_VERTEX_PROGRAM_POINT_SIZE|GL_PROGRAM_POINT_SIZE]";
		case 0x00008645: return "GL_VERTEX_ATTRIB_ARRAY_POINTER";
		case 0x0000864f: return "GL_DEPTH_CLAMP";
		case 0x000086a0: return "GL_TEXTURE_COMPRESSED_IMAGE_SIZE";
		case 0x000086a1: return "GL_TEXTURE_COMPRESSED";
		case 0x000086a2: return "GL_NUM_COMPRESSED_TEXTURE_FORMATS";
		case 0x000086a3: return "GL_COMPRESSED_TEXTURE_FORMATS";
		case 0x00008764: return "GL_BUFFER_SIZE";
		case 0x00008765: return "GL_BUFFER_USAGE";
		case 0x00008800: return "GL_STENCIL_BACK_FUNC";
		case 0x00008801: return "GL_STENCIL_BACK_FAIL";
		case 0x00008802: return "GL_STENCIL_BACK_PASS_DEPTH_FAIL";
		case 0x00008803: return "GL_STENCIL_BACK_PASS_DEPTH_PASS";
		case 0x00008814: return "GL_RGBA32F";
		case 0x00008815: return "GL_RGB32F";
		case 0x0000881a: return "GL_RGBA16F";
		case 0x0000881b: return "GL_RGB16F";
		case 0x00008824: return "GL_MAX_DRAW_BUFFERS";
		case 0x00008825: return "GL_DRAW_BUFFER0";
		case 0x00008826: return "GL_DRAW_BUFFER1";
		case 0x00008827: return "GL_DRAW_BUFFER2";
		case 0x00008828: return "GL_DRAW_BUFFER3";
		case 0x00008829: return "GL_DRAW_BUFFER4";
		case 0x0000882a: return "GL_DRAW_BUFFER5";
		case 0x0000882b: return "GL_DRAW_BUFFER6";
		case 0x0000882c: return "GL_DRAW_BUFFER7";
		case 0x0000882d: return "GL_DRAW_BUFFER8";
		case 0x0000882e: return "GL_DRAW_BUFFER9";
		case 0x0000882f: return "GL_DRAW_BUFFER10";
		case 0x00008830: return "GL_DRAW_BUFFER11";
		case 0x00008831: return "GL_DRAW_BUFFER12";
		case 0x00008832: return "GL_DRAW_BUFFER13";
		case 0x00008833: return "GL_DRAW_BUFFER14";
		case 0x00008834: return "GL_DRAW_BUFFER15";
		case 0x0000883d: return "GL_BLEND_EQUATION_ALPHA";
		case 0x0000884a: return "GL_TEXTURE_DEPTH_SIZE";
		case 0x0000884c: return "GL_TEXTURE_COMPARE_MODE";
		case 0x0000884d: return "GL_TEXTURE_COMPARE_FUNC";
		case 0x0000884e: return "GL_COMPARE_REF_TO_TEXTURE";
		case 0x0000884f: return "GL_TEXTURE_CUBE_MAP_SEAMLESS";
		case 0x00008864: return "GL_QUERY_COUNTER_BITS";
		case 0x00008865: return "GL_CURRENT_QUERY";
		case 0x00008866: return "GL_QUERY_RESULT";
		case 0x00008867: return "GL_QUERY_RESULT_AVAILABLE";
		case 0x00008869: return "GL_MAX_VERTEX_ATTRIBS";
		case 0x0000886a: return "GL_VERTEX_ATTRIB_ARRAY_NORMALIZED";
		case 0x00008872: return "GL_MAX_TEXTURE_IMAGE_UNITS";
		case 0x00008892: return "GL_ARRAY_BUFFER";
		case 0x00008893: return "GL_ELEMENT_ARRAY_BUFFER";
		case 0x00008894: return "GL_ARRAY_BUFFER_BINDING";
		case 0x00008895: return "GL_ELEMENT_ARRAY_BUFFER_BINDING";
		case 0x0000889f: return "GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING";
		case 0x000088b8: return "GL_READ_ONLY";
		case 0x000088b9: return "GL_WRITE_ONLY";
		case 0x000088ba: return "GL_READ_WRITE";
		case 0x000088bb: return "GL_BUFFER_ACCESS";
		case 0x000088bc: return "GL_BUFFER_MAPPED";
		case 0x000088bd: return "GL_BUFFER_MAP_POINTER";
		case 0x000088bf: return "GL_TIME_ELAPSED";
		case 0x000088e0: return "GL_STREAM_DRAW";
		case 0x000088e1: return "GL_STREAM_READ";
		case 0x000088e2: return "GL_STREAM_COPY";
		case 0x000088e4: return "GL_STATIC_DRAW";
		case 0x000088e5: return "GL_STATIC_READ";
		case 0x000088e6: return "GL_STATIC_COPY";
		case 0x000088e8: return "GL_DYNAMIC_DRAW";
		case 0x000088e9: return "GL_DYNAMIC_READ";
		case 0x000088ea: return "GL_DYNAMIC_COPY";
		case 0x000088eb: return "GL_PIXEL_PACK_BUFFER";
		case 0x000088ec: return "GL_PIXEL_UNPACK_BUFFER";
		case 0x000088ed: return "GL_PIXEL_PACK_BUFFER_BINDING";
		case 0x000088ef: return "GL_PIXEL_UNPACK_BUFFER_BINDING";
		case 0x000088f0: return "GL_DEPTH24_STENCIL8";
		case 0x000088f1: return "GL_TEXTURE_STENCIL_SIZE";
		case 0x000088f9: return "GL_SRC1_COLOR";
		case 0x000088fa: return "GL_ONE_MINUS_SRC1_COLOR";
		case 0x000088fb: return "GL_ONE_MINUS_SRC1_ALPHA";
		case 0x000088fc: return "GL_MAX_DUAL_SOURCE_DRAW_BUFFERS";
		case 0x000088fd: return "GL_VERTEX_ATTRIB_ARRAY_INTEGER";
		case 0x000088fe: return "GL_VERTEX_ATTRIB_ARRAY_DIVISOR";
		case 0x000088ff: return "GL_MAX_ARRAY_TEXTURE_LAYERS";
		case 0x00008904: return "GL_MIN_PROGRAM_TEXEL_OFFSET";
		case 0x00008905: return "GL_MAX_PROGRAM_TEXEL_OFFSET";
		case 0x00008914: return "GL_SAMPLES_PASSED";
		case 0x00008916: return "GL_GEOMETRY_VERTICES_OUT";
		case 0x00008917: return "GL_GEOMETRY_INPUT_TYPE";
		case 0x00008918: return "GL_GEOMETRY_OUTPUT_TYPE";
		case 0x00008919: return "GL_SAMPLER_BINDING";
		case 0x0000891c: return "GL_CLAMP_READ_COLOR";
		case 0x0000891d: return "GL_FIXED_ONLY";
		case 0x00008a11: return "GL_UNIFORM_BUFFER";
		case 0x00008a28: return "GL_UNIFORM_BUFFER_BINDING";
		case 0x00008a29: return "GL_UNIFORM_BUFFER_START";
		case 0x00008a2a: return "GL_UNIFORM_BUFFER_SIZE";
		case 0x00008a2b: return "GL_MAX_VERTEX_UNIFORM_BLOCKS";
		case 0x00008a2c: return "GL_MAX_GEOMETRY_UNIFORM_BLOCKS";
		case 0x00008a2d: return "GL_MAX_FRAGMENT_UNIFORM_BLOCKS";
		case 0x00008a2e: return "GL_MAX_COMBINED_UNIFORM_BLOCKS";
		case 0x00008a2f: return "GL_MAX_UNIFORM_BUFFER_BINDINGS";
		case 0x00008a30: return "GL_MAX_UNIFORM_BLOCK_SIZE";
		case 0x00008a31: return "GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS";
		case 0x00008a32: return "GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS";
		case 0x00008a33: return "GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS";
		case 0x00008a34: return "GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT";
		case 0x00008a35: return "GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH";
		case 0x00008a36: return "GL_ACTIVE_UNIFORM_BLOCKS";
		case 0x00008a37: return "GL_UNIFORM_TYPE";
		case 0x00008a38: return "GL_UNIFORM_SIZE";
		case 0x00008a39: return "GL_UNIFORM_NAME_LENGTH";
		case 0x00008a3a: return "GL_UNIFORM_BLOCK_INDEX";
		case 0x00008a3b: return "GL_UNIFORM_OFFSET";
		case 0x00008a3c: return "GL_UNIFORM_ARRAY_STRIDE";
		case 0x00008a3d: return "GL_UNIFORM_MATRIX_STRIDE";
		case 0x00008a3e: return "GL_UNIFORM_IS_ROW_MAJOR";
		case 0x00008a3f: return "GL_UNIFORM_BLOCK_BINDING";
		case 0x00008a40: return "GL_UNIFORM_BLOCK_DATA_SIZE";
		case 0x00008a41: return "GL_UNIFORM_BLOCK_NAME_LENGTH";
		case 0x00008a42: return "GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS";
		case 0x00008a43: return "GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES";
		case 0x00008a44: return "GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER";
		case 0x00008a45: return "GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER";
		case 0x00008a46: return "GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER";
		case 0x00008b30: return "GL_FRAGMENT_SHADER";
		case 0x00008b31: return "GL_VERTEX_SHADER";
		case 0x00008b49: return "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS";
		case 0x00008b4a: return "GL_MAX_VERTEX_UNIFORM_COMPONENTS";
		case 0x00008b4b: return "[GL_MAX_VARYING_FLOATS|GL_MAX_VARYING_COMPONENTS]";
		case 0x00008b4c: return "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS";
		case 0x00008b4d: return "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS";
		case 0x00008b4f: return "GL_SHADER_TYPE";
		case 0x00008b50: return "GL_FLOAT_VEC2";
		case 0x00008b51: return "GL_FLOAT_VEC3";
		case 0x00008b52: return "GL_FLOAT_VEC4";
		case 0x00008b53: return "GL_INT_VEC2";
		case 0x00008b54: return "GL_INT_VEC3";
		case 0x00008b55: return "GL_INT_VEC4";
		case 0x00008b56: return "GL_BOOL";
		case 0x00008b57: return "GL_BOOL_VEC2";
		case 0x00008b58: return "GL_BOOL_VEC3";
		case 0x00008b59: return "GL_BOOL_VEC4";
		case 0x00008b5a: return "GL_FLOAT_MAT2";
		case 0x00008b5b: return "GL_FLOAT_MAT3";
		case 0x00008b5c: return "GL_FLOAT_MAT4";
		case 0x00008b5d: return "GL_SAMPLER_1D";
		case 0x00008b5e: return "GL_SAMPLER_2D";
		case 0x00008b5f: return "GL_SAMPLER_3D";
		case 0x00008b60: return "GL_SAMPLER_CUBE";
		case 0x00008b61: return "GL_SAMPLER_1D_SHADOW";
		case 0x00008b62: return "GL_SAMPLER_2D_SHADOW";
		case 0x00008b63: return "GL_SAMPLER_2D_RECT";
		case 0x00008b64: return "GL_SAMPLER_2D_RECT_SHADOW";
		case 0x00008b65: return "GL_FLOAT_MAT2x3";
		case 0x00008b66: return "GL_FLOAT_MAT2x4";
		case 0x00008b67: return "GL_FLOAT_MAT3x2";
		case 0x00008b68: return "GL_FLOAT_MAT3x4";
		case 0x00008b69: return "GL_FLOAT_MAT4x2";
		case 0x00008b6a: return "GL_FLOAT_MAT4x3";
		case 0x00008b80: return "GL_DELETE_STATUS";
		case 0x00008b81: return "GL_COMPILE_STATUS";
		case 0x00008b82: return "GL_LINK_STATUS";
		case 0x00008b83: return "GL_VALIDATE_STATUS";
		case 0x00008b84: return "GL_INFO_LOG_LENGTH";
		case 0x00008b85: return "GL_ATTACHED_SHADERS";
		case 0x00008b86: return "GL_ACTIVE_UNIFORMS";
		case 0x00008b87: return "GL_ACTIVE_UNIFORM_MAX_LENGTH";
		case 0x00008b88: return "GL_SHADER_SOURCE_LENGTH";
		case 0x00008b89: return "GL_ACTIVE_ATTRIBUTES";
		case 0x00008b8a: return "GL_ACTIVE_ATTRIBUTE_MAX_LENGTH";
		case 0x00008b8b: return "GL_FRAGMENT_SHADER_DERIVATIVE_HINT";
		case 0x00008b8c: return "GL_SHADING_LANGUAGE_VERSION";
		case 0x00008b8d: return "GL_CURRENT_PROGRAM";
		case 0x00008c10: return "GL_TEXTURE_RED_TYPE";
		case 0x00008c11: return "GL_TEXTURE_GREEN_TYPE";
		case 0x00008c12: return "GL_TEXTURE_BLUE_TYPE";
		case 0x00008c13: return "GL_TEXTURE_ALPHA_TYPE";
		case 0x00008c16: return "GL_TEXTURE_DEPTH_TYPE";
		case 0x00008c17: return "GL_UNSIGNED_NORMALIZED";
		case 0x00008c18: return "GL_TEXTURE_1D_ARRAY";
		case 0x00008c19: return "GL_PROXY_TEXTURE_1D_ARRAY";
		case 0x00008c1a: return "GL_TEXTURE_2D_ARRAY";
		case 0x00008c1b: return "GL_PROXY_TEXTURE_2D_ARRAY";
		case 0x00008c1c: return "GL_TEXTURE_BINDING_1D_ARRAY";
		case 0x00008c1d: return "GL_TEXTURE_BINDING_2D_ARRAY";
		case 0x00008c29: return "GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS";
		case 0x00008c2a: return "GL_TEXTURE_BUFFER";
		case 0x00008c2b: return "GL_MAX_TEXTURE_BUFFER_SIZE";
		case 0x00008c2c: return "GL_TEXTURE_BINDING_BUFFER";
		case 0x00008c2d: return "GL_TEXTURE_BUFFER_DATA_STORE_BINDING";
		case 0x00008c2f: return "GL_ANY_SAMPLES_PASSED";
		case 0x00008c3a: return "GL_R11F_G11F_B10F";
		case 0x00008c3b: return "GL_UNSIGNED_INT_10F_11F_11F_REV";
		case 0x00008c3d: return "GL_RGB9_E5";
		case 0x00008c3e: return "GL_UNSIGNED_INT_5_9_9_9_REV";
		case 0x00008c3f: return "GL_TEXTURE_SHARED_SIZE";
		case 0x00008c40: return "GL_SRGB";
		case 0x00008c41: return "GL_SRGB8";
		case 0x00008c42: return "GL_SRGB_ALPHA";
		case 0x00008c43: return "GL_SRGB8_ALPHA8";
		case 0x00008c48: return "GL_COMPRESSED_SRGB";
		case 0x00008c49: return "GL_COMPRESSED_SRGB_ALPHA";
		case 0x00008c76: return "GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH";
		case 0x00008c7f: return "GL_TRANSFORM_FEEDBACK_BUFFER_MODE";
		case 0x00008c80: return "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS";
		case 0x00008c83: return "GL_TRANSFORM_FEEDBACK_VARYINGS";
		case 0x00008c84: return "GL_TRANSFORM_FEEDBACK_BUFFER_START";
		case 0x00008c85: return "GL_TRANSFORM_FEEDBACK_BUFFER_SIZE";
		case 0x00008c87: return "GL_PRIMITIVES_GENERATED";
		case 0x00008c88: return "GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN";
		case 0x00008c89: return "GL_RASTERIZER_DISCARD";
		case 0x00008c8a: return "GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS";
		case 0x00008c8b: return "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS";
		case 0x00008c8c: return "GL_INTERLEAVED_ATTRIBS";
		case 0x00008c8d: return "GL_SEPARATE_ATTRIBS";
		case 0x00008c8e: return "GL_TRANSFORM_FEEDBACK_BUFFER";
		case 0x00008c8f: return "GL_TRANSFORM_FEEDBACK_BUFFER_BINDING";
		case 0x00008ca0: return "GL_POINT_SPRITE_COORD_ORIGIN";
		case 0x00008ca1: return "GL_LOWER_LEFT";
		case 0x00008ca2: return "GL_UPPER_LEFT";
		case 0x00008ca3: return "GL_STENCIL_BACK_REF";
		case 0x00008ca4: return "GL_STENCIL_BACK_VALUE_MASK";
		case 0x00008ca5: return "GL_STENCIL_BACK_WRITEMASK";
		case 0x00008ca6: return "[GL_FRAMEBUFFER_BINDING|GL_DRAW_FRAMEBUFFER_BINDING]";
		case 0x00008ca7: return "GL_RENDERBUFFER_BINDING";
		case 0x00008ca8: return "GL_READ_FRAMEBUFFER";
		case 0x00008ca9: return "GL_DRAW_FRAMEBUFFER";
		case 0x00008caa: return "GL_READ_FRAMEBUFFER_BINDING";
		case 0x00008cab: return "GL_RENDERBUFFER_SAMPLES";
		case 0x00008cac: return "GL_DEPTH_COMPONENT32F";
		case 0x00008cad: return "GL_DEPTH32F_STENCIL8";
		case 0x00008cd0: return "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE";
		case 0x00008cd1: return "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME";
		case 0x00008cd2: return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL";
		case 0x00008cd3: return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE";
		case 0x00008cd4: return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER";
		case 0x00008cd5: return "GL_FRAMEBUFFER_COMPLETE";
		case 0x00008cd6: return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		case 0x00008cd7: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
		case 0x00008cdb: return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
		case 0x00008cdc: return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
		case 0x00008cdd: return "GL_FRAMEBUFFER_UNSUPPORTED";
		case 0x00008cdf: return "GL_MAX_COLOR_ATTACHMENTS";
		case 0x00008ce0: return "GL_COLOR_ATTACHMENT0";
		case 0x00008ce1: return "GL_COLOR_ATTACHMENT1";
		case 0x00008ce2: return "GL_COLOR_ATTACHMENT2";
		case 0x00008ce3: return "GL_COLOR_ATTACHMENT3";
		case 0x00008ce4: return "GL_COLOR_ATTACHMENT4";
		case 0x00008ce5: return "GL_COLOR_ATTACHMENT5";
		case 0x00008ce6: return "GL_COLOR_ATTACHMENT6";
		case 0x00008ce7: return "GL_COLOR_ATTACHMENT7";
		case 0x00008ce8: return "GL_COLOR_ATTACHMENT8";
		case 0x00008ce9: return "GL_COLOR_ATTACHMENT9";
		case 0x00008cea: return "GL_COLOR_ATTACHMENT10";
		case 0x00008ceb: return "GL_COLOR_ATTACHMENT11";
		case 0x00008cec: return "GL_COLOR_ATTACHMENT12";
		case 0x00008ced: return "GL_COLOR_ATTACHMENT13";
		case 0x00008cee: return "GL_COLOR_ATTACHMENT14";
		case 0x00008cef: return "GL_COLOR_ATTACHMENT15";
		case 0x00008cf0: return "GL_COLOR_ATTACHMENT16";
		case 0x00008cf1: return "GL_COLOR_ATTACHMENT17";
		case 0x00008cf2: return "GL_COLOR_ATTACHMENT18";
		case 0x00008cf3: return "GL_COLOR_ATTACHMENT19";
		case 0x00008cf4: return "GL_COLOR_ATTACHMENT20";
		case 0x00008cf5: return "GL_COLOR_ATTACHMENT21";
		case 0x00008cf6: return "GL_COLOR_ATTACHMENT22";
		case 0x00008cf7: return "GL_COLOR_ATTACHMENT23";
		case 0x00008cf8: return "GL_COLOR_ATTACHMENT24";
		case 0x00008cf9: return "GL_COLOR_ATTACHMENT25";
		case 0x00008cfa: return "GL_COLOR_ATTACHMENT26";
		case 0x00008cfb: return "GL_COLOR_ATTACHMENT27";
		case 0x00008cfc: return "GL_COLOR_ATTACHMENT28";
		case 0x00008cfd: return "GL_COLOR_ATTACHMENT29";
		case 0x00008cfe: return "GL_COLOR_ATTACHMENT30";
		case 0x00008cff: return "GL_COLOR_ATTACHMENT31";
		case 0x00008d00: return "GL_DEPTH_ATTACHMENT";
		case 0x00008d20: return "GL_STENCIL_ATTACHMENT";
		case 0x00008d40: return "GL_FRAMEBUFFER";
		case 0x00008d41: return "GL_RENDERBUFFER";
		case 0x00008d42: return "GL_RENDERBUFFER_WIDTH";
		case 0x00008d43: return "GL_RENDERBUFFER_HEIGHT";
		case 0x00008d44: return "GL_RENDERBUFFER_INTERNAL_FORMAT";
		case 0x00008d46: return "GL_STENCIL_INDEX1";
		case 0x00008d47: return "GL_STENCIL_INDEX4";
		case 0x00008d48: return "GL_STENCIL_INDEX8";
		case 0x00008d49: return "GL_STENCIL_INDEX16";
		case 0x00008d50: return "GL_RENDERBUFFER_RED_SIZE";
		case 0x00008d51: return "GL_RENDERBUFFER_GREEN_SIZE";
		case 0x00008d52: return "GL_RENDERBUFFER_BLUE_SIZE";
		case 0x00008d53: return "GL_RENDERBUFFER_ALPHA_SIZE";
		case 0x00008d54: return "GL_RENDERBUFFER_DEPTH_SIZE";
		case 0x00008d55: return "GL_RENDERBUFFER_STENCIL_SIZE";
		case 0x00008d56: return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
		case 0x00008d57: return "GL_MAX_SAMPLES";
		case 0x00008d70: return "GL_RGBA32UI";
		case 0x00008d71: return "GL_RGB32UI";
		case 0x00008d76: return "GL_RGBA16UI";
		case 0x00008d77: return "GL_RGB16UI";
		case 0x00008d7c: return "GL_RGBA8UI";
		case 0x00008d7d: return "GL_RGB8UI";
		case 0x00008d82: return "GL_RGBA32I";
		case 0x00008d83: return "GL_RGB32I";
		case 0x00008d88: return "GL_RGBA16I";
		case 0x00008d89: return "GL_RGB16I";
		case 0x00008d8e: return "GL_RGBA8I";
		case 0x00008d8f: return "GL_RGB8I";
		case 0x00008d94: return "GL_RED_INTEGER";
		case 0x00008d95: return "GL_GREEN_INTEGER";
		case 0x00008d96: return "GL_BLUE_INTEGER";
		case 0x00008d98: return "GL_RGB_INTEGER";
		case 0x00008d99: return "GL_RGBA_INTEGER";
		case 0x00008d9a: return "GL_BGR_INTEGER";
		case 0x00008d9b: return "GL_BGRA_INTEGER";
		case 0x00008d9f: return "GL_INT_2_10_10_10_REV";
		case 0x00008da7: return "GL_FRAMEBUFFER_ATTACHMENT_LAYERED";
		case 0x00008da8: return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
		case 0x00008dad: return "GL_FLOAT_32_UNSIGNED_INT_24_8_REV";
		case 0x00008db9: return "GL_FRAMEBUFFER_SRGB";
		case 0x00008dbb: return "GL_COMPRESSED_RED_RGTC1";
		case 0x00008dbc: return "GL_COMPRESSED_SIGNED_RED_RGTC1";
		case 0x00008dbd: return "GL_COMPRESSED_RG_RGTC2";
		case 0x00008dbe: return "GL_COMPRESSED_SIGNED_RG_RGTC2";
		case 0x00008dc0: return "GL_SAMPLER_1D_ARRAY";
		case 0x00008dc1: return "GL_SAMPLER_2D_ARRAY";
		case 0x00008dc2: return "GL_SAMPLER_BUFFER";
		case 0x00008dc3: return "GL_SAMPLER_1D_ARRAY_SHADOW";
		case 0x00008dc4: return "GL_SAMPLER_2D_ARRAY_SHADOW";
		case 0x00008dc5: return "GL_SAMPLER_CUBE_SHADOW";
		case 0x00008dc6: return "GL_UNSIGNED_INT_VEC2";
		case 0x00008dc7: return "GL_UNSIGNED_INT_VEC3";
		case 0x00008dc8: return "GL_UNSIGNED_INT_VEC4";
		case 0x00008dc9: return "GL_INT_SAMPLER_1D";
		case 0x00008dca: return "GL_INT_SAMPLER_2D";
		case 0x00008dcb: return "GL_INT_SAMPLER_3D";
		case 0x00008dcc: return "GL_INT_SAMPLER_CUBE";
		case 0x00008dcd: return "GL_INT_SAMPLER_2D_RECT";
		case 0x00008dce: return "GL_INT_SAMPLER_1D_ARRAY";
		case 0x00008dcf: return "GL_INT_SAMPLER_2D_ARRAY";
		case 0x00008dd0: return "GL_INT_SAMPLER_BUFFER";
		case 0x00008dd1: return "GL_UNSIGNED_INT_SAMPLER_1D";
		case 0x00008dd2: return "GL_UNSIGNED_INT_SAMPLER_2D";
		case 0x00008dd3: return "GL_UNSIGNED_INT_SAMPLER_3D";
		case 0x00008dd4: return "GL_UNSIGNED_INT_SAMPLER_CUBE";
		case 0x00008dd5: return "GL_UNSIGNED_INT_SAMPLER_2D_RECT";
		case 0x00008dd6: return "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY";
		case 0x00008dd7: return "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY";
		case 0x00008dd8: return "GL_UNSIGNED_INT_SAMPLER_BUFFER";
		case 0x00008dd9: return "GL_GEOMETRY_SHADER";
		case 0x00008ddf: return "GL_MAX_GEOMETRY_UNIFORM_COMPONENTS";
		case 0x00008de0: return "GL_MAX_GEOMETRY_OUTPUT_VERTICES";
		case 0x00008de1: return "GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS";
		case 0x00008e13: return "GL_QUERY_WAIT";
		case 0x00008e14: return "GL_QUERY_NO_WAIT";
		case 0x00008e15: return "GL_QUERY_BY_REGION_WAIT";
		case 0x00008e16: return "GL_QUERY_BY_REGION_NO_WAIT";
		case 0x00008e28: return "GL_TIMESTAMP";
		case 0x00008e42: return "GL_TEXTURE_SWIZZLE_R";
		case 0x00008e43: return "GL_TEXTURE_SWIZZLE_G";
		case 0x00008e44: return "GL_TEXTURE_SWIZZLE_B";
		case 0x00008e45: return "GL_TEXTURE_SWIZZLE_A";
		case 0x00008e46: return "GL_TEXTURE_SWIZZLE_RGBA";
		case 0x00008e4c: return "GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION";
		case 0x00008e4d: return "GL_FIRST_VERTEX_CONVENTION";
		case 0x00008e4e: return "GL_LAST_VERTEX_CONVENTION";
		case 0x00008e4f: return "GL_PROVOKING_VERTEX";
		case 0x00008e50: return "GL_SAMPLE_POSITION";
		case 0x00008e51: return "GL_SAMPLE_MASK";
		case 0x00008e52: return "GL_SAMPLE_MASK_VALUE";
		case 0x00008e59: return "GL_MAX_SAMPLE_MASK_WORDS";
		case 0x00008f36: return "GL_COPY_READ_BUFFER";
		case 0x00008f37: return "GL_COPY_WRITE_BUFFER";
		case 0x00008f94: return "GL_R8_SNORM";
		case 0x00008f95: return "GL_RG8_SNORM";
		case 0x00008f96: return "GL_RGB8_SNORM";
		case 0x00008f97: return "GL_RGBA8_SNORM";
		case 0x00008f98: return "GL_R16_SNORM";
		case 0x00008f99: return "GL_RG16_SNORM";
		case 0x00008f9a: return "GL_RGB16_SNORM";
		case 0x00008f9b: return "GL_RGBA16_SNORM";
		case 0x00008f9c: return "GL_SIGNED_NORMALIZED";
		case 0x00008f9d: return "GL_PRIMITIVE_RESTART";
		case 0x00008f9e: return "GL_PRIMITIVE_RESTART_INDEX";
		case 0x0000906f: return "GL_RGB10_A2UI";
		case 0x00009100: return "GL_TEXTURE_2D_MULTISAMPLE";
		case 0x00009101: return "GL_PROXY_TEXTURE_2D_MULTISAMPLE";
		case 0x00009102: return "GL_TEXTURE_2D_MULTISAMPLE_ARRAY";
		case 0x00009103: return "GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY";
		case 0x00009104: return "GL_TEXTURE_BINDING_2D_MULTISAMPLE";
		case 0x00009105: return "GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY";
		case 0x00009106: return "GL_TEXTURE_SAMPLES";
		case 0x00009107: return "GL_TEXTURE_FIXED_SAMPLE_LOCATIONS";
		case 0x00009108: return "GL_SAMPLER_2D_MULTISAMPLE";
		case 0x00009109: return "GL_INT_SAMPLER_2D_MULTISAMPLE";
		case 0x0000910a: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE";
		case 0x0000910b: return "GL_SAMPLER_2D_MULTISAMPLE_ARRAY";
		case 0x0000910c: return "GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
		case 0x0000910d: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
		case 0x0000910e: return "GL_MAX_COLOR_TEXTURE_SAMPLES";
		case 0x0000910f: return "GL_MAX_DEPTH_TEXTURE_SAMPLES";
		case 0x00009110: return "GL_MAX_INTEGER_SAMPLES";
		case 0x00009111: return "GL_MAX_SERVER_WAIT_TIMEOUT";
		case 0x00009112: return "GL_OBJECT_TYPE";
		case 0x00009113: return "GL_SYNC_CONDITION";
		case 0x00009114: return "GL_SYNC_STATUS";
		case 0x00009115: return "GL_SYNC_FLAGS";
		case 0x00009116: return "GL_SYNC_FENCE";
		case 0x00009117: return "GL_SYNC_GPU_COMMANDS_COMPLETE";
		case 0x00009118: return "GL_UNSIGNALED";
		case 0x00009119: return "GL_SIGNALED";
		case 0x0000911a: return "GL_ALREADY_SIGNALED";
		case 0x0000911b: return "GL_TIMEOUT_EXPIRED";
		case 0x0000911c: return "GL_CONDITION_SATISFIED";
		case 0x0000911d: return "GL_WAIT_FAILED";
		case 0x0000911f: return "GL_BUFFER_ACCESS_FLAGS";
		case 0x00009120: return "GL_BUFFER_MAP_LENGTH";
		case 0x00009121: return "GL_BUFFER_MAP_OFFSET";
		case 0x00009122: return "GL_MAX_VERTEX_OUTPUT_COMPONENTS";
		case 0x00009123: return "GL_MAX_GEOMETRY_INPUT_COMPONENTS";
		case 0x00009124: return "GL_MAX_GEOMETRY_OUTPUT_COMPONENTS";
		case 0x00009125: return "GL_MAX_FRAGMENT_INPUT_COMPONENTS";
		case 0x00009126: return "GL_CONTEXT_PROFILE_MASK";
		case 0x0000912f: return "GL_TEXTURE_IMMUTABLE_FORMAT";
		case 0x00009143: return "GL_MAX_DEBUG_MESSAGE_LENGTH";
		case 0x00009144: return "GL_MAX_DEBUG_LOGGED_MESSAGES";
		case 0x00009145: return "GL_DEBUG_LOGGED_MESSAGES";
		case 0x00009146: return "GL_DEBUG_SEVERITY_HIGH";
		case 0x00009147: return "GL_DEBUG_SEVERITY_MEDIUM";
		case 0x00009148: return "GL_DEBUG_SEVERITY_LOW";
		case 0x000092e0: return "GL_DEBUG_OUTPUT";
		case 0xffffffff: return "GL_INVALID_INDEX";
	}
	return "<Unknown GLenum>";
}

const char* Context::getBlendEquationModeEXTName(GLenum enum_) const {
	switch(enum_) {
		case 0x8006: return "GL_FUNC_ADD";
		case 0x800B: return "GL_FUNC_REVERSE_SUBTRACT";
		case 0x800A: return "GL_FUNC_SUBTRACT";
	}
	return "<Unknown BlendEquationModeEXT>";
}

const char* Context::getBooleanName(GLenum enum_) const {
	switch(enum_) {
		case 0: return "GL_FALSE";
		case 1: return "GL_TRUE";
	}
	return "<Unknown Boolean>";
}

const char* Context::getBufferTargetARBName(GLenum enum_) const {
	switch(enum_) {
		case 0x8892: return "GL_ARRAY_BUFFER";
		case 0x8F36: return "GL_COPY_READ_BUFFER";
		case 0x8F37: return "GL_COPY_WRITE_BUFFER";
		case 0x8893: return "GL_ELEMENT_ARRAY_BUFFER";
		case 0x88EB: return "GL_PIXEL_PACK_BUFFER";
		case 0x88EC: return "GL_PIXEL_UNPACK_BUFFER";
		case 0x8C2A: return "GL_TEXTURE_BUFFER";
		case 0x8C8E: return "GL_TRANSFORM_FEEDBACK_BUFFER";
		case 0x8A11: return "GL_UNIFORM_BUFFER";
	}
	return "<Unknown BufferTargetARB>";
}

const char* Context::getBufferUsageARBName(GLenum enum_) const {
	switch(enum_) {
		case 0x88E0: return "GL_STREAM_DRAW";
		case 0x88E1: return "GL_STREAM_READ";
		case 0x88E2: return "GL_STREAM_COPY";
		case 0x88E4: return "GL_STATIC_DRAW";
		case 0x88E5: return "GL_STATIC_READ";
		case 0x88E6: return "GL_STATIC_COPY";
		case 0x88E8: return "GL_DYNAMIC_DRAW";
		case 0x88E9: return "GL_DYNAMIC_READ";
		case 0x88EA: return "GL_DYNAMIC_COPY";
	}
	return "<Unknown BufferUsageARB>";
}

const char* Context::getBufferAccessARBName(GLenum enum_) const {
	switch(enum_) {
		case 0x88B8: return "GL_READ_ONLY";
		case 0x88B9: return "GL_WRITE_ONLY";
		case 0x88BA: return "GL_READ_WRITE";
	}
	return "<Unknown BufferAccessARB>";
}

const char* Context::getBufferAccessMaskName(GLenum enum_) const {
	switch(enum_) {
		case 0x0010: return "GL_MAP_FLUSH_EXPLICIT_BIT";
		case 0x0008: return "GL_MAP_INVALIDATE_BUFFER_BIT";
		case 0x0004: return "GL_MAP_INVALIDATE_RANGE_BIT";
		case 0x0001: return "GL_MAP_READ_BIT";
		case 0x0020: return "GL_MAP_UNSYNCHRONIZED_BIT";
		case 0x0002: return "GL_MAP_WRITE_BIT";
	}
	return "<Unknown BufferAccessMask>";
}

const char* Context::getClearBufferMaskName(GLenum enum_) const {
	switch(enum_) {
		case 0x00004000: return "GL_COLOR_BUFFER_BIT";
		case 0x00000100: return "GL_DEPTH_BUFFER_BIT";
		case 0x00000400: return "GL_STENCIL_BUFFER_BIT";
	}
	return "<Unknown ClearBufferMask>";
}

const char* Context::getCullFaceModeName(GLenum enum_) const {
	switch(enum_) {
		case 0x0405: return "GL_BACK";
		case 0x0404: return "GL_FRONT";
		case 0x0408: return "GL_FRONT_AND_BACK";
	}
	return "<Unknown CullFaceMode>";
}

const char* Context::getDepthFunctionName(GLenum enum_) const {
	switch(enum_) {
		case 0x0207: return "GL_ALWAYS";
		case 0x0202: return "GL_EQUAL";
		case 0x0206: return "GL_GEQUAL";
		case 0x0204: return "GL_GREATER";
		case 0x0203: return "GL_LEQUAL";
		case 0x0201: return "GL_LESS";
		case 0x0200: return "GL_NEVER";
		case 0x0205: return "GL_NOTEQUAL";
	}
	return "<Unknown DepthFunction>";
}

const char* Context::getDrawBufferModeName(GLenum enum_) const {
	switch(enum_) {
		case 0x0405: return "GL_BACK";
		case 0x0402: return "GL_BACK_LEFT";
		case 0x0403: return "GL_BACK_RIGHT";
		case 0x0404: return "GL_FRONT";
		case 0x0408: return "GL_FRONT_AND_BACK";
		case 0x0400: return "GL_FRONT_LEFT";
		case 0x0401: return "GL_FRONT_RIGHT";
		case 0x0406: return "GL_LEFT";
		case 0: return "GL_NONE";
		case 0x0407: return "GL_RIGHT";
	}
	return "<Unknown DrawBufferMode>";
}

const char* Context::getDrawElementsTypeName(GLenum enum_) const {
	switch(enum_) {
		case 0x1401: return "GL_UNSIGNED_BYTE";
		case 0x1403: return "GL_UNSIGNED_SHORT";
		case 0x1405: return "GL_UNSIGNED_INT";
	}
	return "<Unknown DrawElementsType>";
}

const char* Context::getEnableCapName(GLenum enum_) const {
	switch(enum_) {
		case 0x0BE2: return "GL_BLEND";
		case 0x0BF2: return "GL_COLOR_LOGIC_OP";
		case 0x0B44: return "GL_CULL_FACE";
		case 0x0B71: return "GL_DEPTH_TEST";
		case 0x0BD0: return "GL_DITHER";
		case 0x0B20: return "GL_LINE_SMOOTH";
		case 0x8037: return "GL_POLYGON_OFFSET_FILL";
		case 0x2A02: return "GL_POLYGON_OFFSET_LINE";
		case 0x2A01: return "GL_POLYGON_OFFSET_POINT";
		case 0x0B41: return "GL_POLYGON_SMOOTH";
		case 0x0C11: return "GL_SCISSOR_TEST";
		case 0x0B90: return "GL_STENCIL_TEST";
		case 0x0DE0: return "GL_TEXTURE_1D";
		case 0x0DE1: return "GL_TEXTURE_2D";
		case 0x8074: return "GL_VERTEX_ARRAY";
	}
	return "<Unknown EnableCap>";
}

const char* Context::getErrorCodeName(GLenum enum_) const {
	switch(enum_) {
		case 0x0500: return "GL_INVALID_ENUM";
		case 0x0506: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case 0x0502: return "GL_INVALID_OPERATION";
		case 0x0501: return "GL_INVALID_VALUE";
		case 0: return "GL_NO_ERROR";
		case 0x0505: return "GL_OUT_OF_MEMORY";
		case 0x0503: return "GL_STACK_OVERFLOW";
		case 0x0504: return "GL_STACK_UNDERFLOW";
	}
	return "<Unknown ErrorCode>";
}

const char* Context::getFrontFaceDirectionName(GLenum enum_) const {
	switch(enum_) {
		case 0x0901: return "GL_CCW";
		case 0x0900: return "GL_CW";
	}
	return "<Unknown FrontFaceDirection>";
}

const char* Context::getGetPNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x846E: return "GL_ALIASED_LINE_WIDTH_RANGE";
		case 0x0BE2: return "GL_BLEND";
		case 0x0BE0: return "GL_BLEND_DST";
		case 0x0BE1: return "GL_BLEND_SRC";
		case 0x0C22: return "GL_COLOR_CLEAR_VALUE";
		case 0x0BF2: return "GL_COLOR_LOGIC_OP";
		case 0x0C23: return "GL_COLOR_WRITEMASK";
		case 0x0B44: return "GL_CULL_FACE";
		case 0x0B45: return "GL_CULL_FACE_MODE";
		case 0x0B73: return "GL_DEPTH_CLEAR_VALUE";
		case 0x0B74: return "GL_DEPTH_FUNC";
		case 0x0B70: return "GL_DEPTH_RANGE";
		case 0x0B71: return "GL_DEPTH_TEST";
		case 0x0B72: return "GL_DEPTH_WRITEMASK";
		case 0x0BD0: return "GL_DITHER";
		case 0x0C32: return "GL_DOUBLEBUFFER";
		case 0x0C01: return "GL_DRAW_BUFFER";
		case 0x0B46: return "GL_FRONT_FACE";
		case 0x0B20: return "GL_LINE_SMOOTH";
		case 0x0C52: return "GL_LINE_SMOOTH_HINT";
		case 0x0B21: return "GL_LINE_WIDTH";
		case 0x0B23: return "GL_LINE_WIDTH_GRANULARITY";
		case 0x0B22: return "GL_LINE_WIDTH_RANGE";
		case 0x0BF0: return "GL_LOGIC_OP_MODE";
		case 0x0D32: return "GL_MAX_CLIP_DISTANCES";
		case 0x0D33: return "GL_MAX_TEXTURE_SIZE";
		case 0x0D3A: return "GL_MAX_VIEWPORT_DIMS";
		case 0x0D05: return "GL_PACK_ALIGNMENT";
		case 0x0D01: return "GL_PACK_LSB_FIRST";
		case 0x0D02: return "GL_PACK_ROW_LENGTH";
		case 0x0D04: return "GL_PACK_SKIP_PIXELS";
		case 0x0D03: return "GL_PACK_SKIP_ROWS";
		case 0x0D00: return "GL_PACK_SWAP_BYTES";
		case 0x0B11: return "GL_POINT_SIZE";
		case 0x0B13: return "GL_POINT_SIZE_GRANULARITY";
		case 0x0B12: return "GL_POINT_SIZE_RANGE";
		case 0x0B40: return "GL_POLYGON_MODE";
		case 0x8038: return "GL_POLYGON_OFFSET_FACTOR";
		case 0x8037: return "GL_POLYGON_OFFSET_FILL";
		case 0x2A02: return "GL_POLYGON_OFFSET_LINE";
		case 0x2A01: return "GL_POLYGON_OFFSET_POINT";
		case 0x2A00: return "GL_POLYGON_OFFSET_UNITS";
		case 0x0B41: return "GL_POLYGON_SMOOTH";
		case 0x0C53: return "GL_POLYGON_SMOOTH_HINT";
		case 0x0C02: return "GL_READ_BUFFER";
		case 0x0C10: return "GL_SCISSOR_BOX";
		case 0x0C11: return "GL_SCISSOR_TEST";
		case 0x0B91: return "GL_STENCIL_CLEAR_VALUE";
		case 0x0B94: return "GL_STENCIL_FAIL";
		case 0x0B92: return "GL_STENCIL_FUNC";
		case 0x0B95: return "GL_STENCIL_PASS_DEPTH_FAIL";
		case 0x0B96: return "GL_STENCIL_PASS_DEPTH_PASS";
		case 0x0B97: return "GL_STENCIL_REF";
		case 0x0B90: return "GL_STENCIL_TEST";
		case 0x0B93: return "GL_STENCIL_VALUE_MASK";
		case 0x0B98: return "GL_STENCIL_WRITEMASK";
		case 0x0C33: return "GL_STEREO";
		case 0x0D50: return "GL_SUBPIXEL_BITS";
		case 0x0DE0: return "GL_TEXTURE_1D";
		case 0x0DE1: return "GL_TEXTURE_2D";
		case 0x8068: return "GL_TEXTURE_BINDING_1D";
		case 0x8069: return "GL_TEXTURE_BINDING_2D";
		case 0x806A: return "GL_TEXTURE_BINDING_3D";
		case 0x0CF5: return "GL_UNPACK_ALIGNMENT";
		case 0x0CF1: return "GL_UNPACK_LSB_FIRST";
		case 0x0CF2: return "GL_UNPACK_ROW_LENGTH";
		case 0x0CF4: return "GL_UNPACK_SKIP_PIXELS";
		case 0x0CF3: return "GL_UNPACK_SKIP_ROWS";
		case 0x0CF0: return "GL_UNPACK_SWAP_BYTES";
		case 0x8074: return "GL_VERTEX_ARRAY";
		case 0x0BA2: return "GL_VIEWPORT";
	}
	return "<Unknown GetPName>";
}

const char* Context::getGetPointervPNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x8244: return "GL_DEBUG_CALLBACK_FUNCTION";
		case 0x8245: return "GL_DEBUG_CALLBACK_USER_PARAM";
	}
	return "<Unknown GetPointervPName>";
}

const char* Context::getGetTextureParameterName(GLenum enum_) const {
	switch(enum_) {
		case 0x805F: return "GL_TEXTURE_ALPHA_SIZE";
		case 0x805E: return "GL_TEXTURE_BLUE_SIZE";
		case 0x1004: return "GL_TEXTURE_BORDER_COLOR";
		case 0x805D: return "GL_TEXTURE_GREEN_SIZE";
		case 0x1001: return "GL_TEXTURE_HEIGHT";
		case 0x1003: return "GL_TEXTURE_INTERNAL_FORMAT";
		case 0x2800: return "GL_TEXTURE_MAG_FILTER";
		case 0x2801: return "GL_TEXTURE_MIN_FILTER";
		case 0x805C: return "GL_TEXTURE_RED_SIZE";
		case 0x1000: return "GL_TEXTURE_WIDTH";
		case 0x2802: return "GL_TEXTURE_WRAP_S";
		case 0x2803: return "GL_TEXTURE_WRAP_T";
	}
	return "<Unknown GetTextureParameter>";
}

const char* Context::getHintModeName(GLenum enum_) const {
	switch(enum_) {
		case 0x1100: return "GL_DONT_CARE";
		case 0x1101: return "GL_FASTEST";
		case 0x1102: return "GL_NICEST";
	}
	return "<Unknown HintMode>";
}

const char* Context::getHintTargetName(GLenum enum_) const {
	switch(enum_) {
		case 0x8B8B: return "GL_FRAGMENT_SHADER_DERIVATIVE_HINT";
		case 0x0C52: return "GL_LINE_SMOOTH_HINT";
		case 0x0C53: return "GL_POLYGON_SMOOTH_HINT";
		case 0x84EF: return "GL_TEXTURE_COMPRESSION_HINT";
	}
	return "<Unknown HintTarget>";
}

const char* Context::getLogicOpName(GLenum enum_) const {
	switch(enum_) {
		case 0x1501: return "GL_AND";
		case 0x1504: return "GL_AND_INVERTED";
		case 0x1502: return "GL_AND_REVERSE";
		case 0x1500: return "GL_CLEAR";
		case 0x1503: return "GL_COPY";
		case 0x150C: return "GL_COPY_INVERTED";
		case 0x1509: return "GL_EQUIV";
		case 0x150A: return "GL_INVERT";
		case 0x150E: return "GL_NAND";
		case 0x1505: return "GL_NOOP";
		case 0x1508: return "GL_NOR";
		case 0x1507: return "GL_OR";
		case 0x150D: return "GL_OR_INVERTED";
		case 0x150B: return "GL_OR_REVERSE";
		case 0x150F: return "GL_SET";
		case 0x1506: return "GL_XOR";
	}
	return "<Unknown LogicOp>";
}

const char* Context::getMaterialFaceName(GLenum enum_) const {
	switch(enum_) {
		case 0x0405: return "GL_BACK";
		case 0x0404: return "GL_FRONT";
		case 0x0408: return "GL_FRONT_AND_BACK";
	}
	return "<Unknown MaterialFace>";
}

const char* Context::getPixelFormatName(GLenum enum_) const {
	switch(enum_) {
		case 0x1906: return "GL_ALPHA";
		case 0x1905: return "GL_BLUE";
		case 0x1902: return "GL_DEPTH_COMPONENT";
		case 0x1904: return "GL_GREEN";
		case 0x1903: return "GL_RED";
		case 0x1907: return "GL_RGB";
		case 0x1908: return "GL_RGBA";
		case 0x1901: return "GL_STENCIL_INDEX";
		case 0x1405: return "GL_UNSIGNED_INT";
		case 0x1403: return "GL_UNSIGNED_SHORT";
	}
	return "<Unknown PixelFormat>";
}

const char* Context::getInternalFormatName(GLenum enum_) const {
	switch(enum_) {
		case 0x1903: return "GL_RED";
		case 0x8229: return "GL_R8";
		case 0x8F94: return "GL_R8_SNORM";
		case 0x822A: return "GL_R16";
		case 0x8F98: return "GL_R16_SNORM";
		case 0x822D: return "GL_R16F";
		case 0x822E: return "GL_R32F";
		case 0x8231: return "GL_R8I";
		case 0x8233: return "GL_R16I";
		case 0x8235: return "GL_R32I";
		case 0x8232: return "GL_R8UI";
		case 0x8234: return "GL_R16UI";
		case 0x8236: return "GL_R32UI";
		case 0x8227: return "GL_RG";
		case 0x822B: return "GL_RG8";
		case 0x8F95: return "GL_RG8_SNORM";
		case 0x822C: return "GL_RG16";
		case 0x8F99: return "GL_RG16_SNORM";
		case 0x822F: return "GL_RG16F";
		case 0x8230: return "GL_RG32F";
		case 0x8237: return "GL_RG8I";
		case 0x8239: return "GL_RG16I";
		case 0x823B: return "GL_RG32I";
		case 0x8238: return "GL_RG8UI";
		case 0x823A: return "GL_RG16UI";
		case 0x823C: return "GL_RG32UI";
		case 0x1907: return "GL_RGB";
		case 0x804F: return "GL_RGB4";
		case 0x8050: return "GL_RGB5";
		case 0x8051: return "GL_RGB8";
		case 0x8F96: return "GL_RGB8_SNORM";
		case 0x8052: return "GL_RGB10";
		case 0x8053: return "GL_RGB12";
		case 0x8054: return "GL_RGB16";
		case 0x881B: return "GL_RGB16F";
		case 0x8F9A: return "GL_RGB16_SNORM";
		case 0x8D8F: return "GL_RGB8I";
		case 0x8D89: return "GL_RGB16I";
		case 0x8D83: return "GL_RGB32I";
		case 0x8D7D: return "GL_RGB8UI";
		case 0x8D77: return "GL_RGB16UI";
		case 0x8D71: return "GL_RGB32UI";
		case 0x8C40: return "GL_SRGB";
		case 0x8C42: return "GL_SRGB_ALPHA";
		case 0x8C41: return "GL_SRGB8";
		case 0x8C43: return "GL_SRGB8_ALPHA8";
		case 0x2A10: return "GL_R3_G3_B2";
		case 0x8C3A: return "GL_R11F_G11F_B10F";
		case 0x8C3D: return "GL_RGB9_E5";
		case 0x1908: return "GL_RGBA";
		case 0x8056: return "GL_RGBA4";
		case 0x8057: return "GL_RGB5_A1";
		case 0x8058: return "GL_RGBA8";
		case 0x8F97: return "GL_RGBA8_SNORM";
		case 0x8059: return "GL_RGB10_A2";
		case 0x805A: return "GL_RGBA12";
		case 0x805B: return "GL_RGBA16";
		case 0x881A: return "GL_RGBA16F";
		case 0x8814: return "GL_RGBA32F";
		case 0x8D8E: return "GL_RGBA8I";
		case 0x8D88: return "GL_RGBA16I";
		case 0x8D82: return "GL_RGBA32I";
		case 0x8D7C: return "GL_RGBA8UI";
		case 0x8D76: return "GL_RGBA16UI";
		case 0x8D70: return "GL_RGBA32UI";
		case 0x906F: return "GL_RGB10_A2UI";
		case 0x1902: return "GL_DEPTH_COMPONENT";
		case 0x81A5: return "GL_DEPTH_COMPONENT16";
		case 0x8CAC: return "GL_DEPTH_COMPONENT32F";
		case 0x84F9: return "GL_DEPTH_STENCIL";
		case 0x88F0: return "GL_DEPTH24_STENCIL8";
		case 0x8CAD: return "GL_DEPTH32F_STENCIL8";
		case 0x8225: return "GL_COMPRESSED_RED";
		case 0x8226: return "GL_COMPRESSED_RG";
		case 0x84ED: return "GL_COMPRESSED_RGB";
		case 0x84EE: return "GL_COMPRESSED_RGBA";
		case 0x8C48: return "GL_COMPRESSED_SRGB";
		case 0x8C49: return "GL_COMPRESSED_SRGB_ALPHA";
		case 0x8DBB: return "GL_COMPRESSED_RED_RGTC1";
		case 0x8DBC: return "GL_COMPRESSED_SIGNED_RED_RGTC1";
		case 0x8DBD: return "GL_COMPRESSED_RG_RGTC2";
		case 0x8DBE: return "GL_COMPRESSED_SIGNED_RG_RGTC2";
	}
	return "<Unknown InternalFormat>";
}

const char* Context::getPixelStoreParameterName(GLenum enum_) const {
	switch(enum_) {
		case 0x0D05: return "GL_PACK_ALIGNMENT";
		case 0x806C: return "GL_PACK_IMAGE_HEIGHT";
		case 0x0D01: return "GL_PACK_LSB_FIRST";
		case 0x0D02: return "GL_PACK_ROW_LENGTH";
		case 0x806B: return "GL_PACK_SKIP_IMAGES";
		case 0x0D04: return "GL_PACK_SKIP_PIXELS";
		case 0x0D03: return "GL_PACK_SKIP_ROWS";
		case 0x0D00: return "GL_PACK_SWAP_BYTES";
		case 0x0CF5: return "GL_UNPACK_ALIGNMENT";
		case 0x806E: return "GL_UNPACK_IMAGE_HEIGHT";
		case 0x0CF1: return "GL_UNPACK_LSB_FIRST";
		case 0x0CF2: return "GL_UNPACK_ROW_LENGTH";
		case 0x806D: return "GL_UNPACK_SKIP_IMAGES";
		case 0x0CF4: return "GL_UNPACK_SKIP_PIXELS";
		case 0x0CF3: return "GL_UNPACK_SKIP_ROWS";
		case 0x0CF0: return "GL_UNPACK_SWAP_BYTES";
	}
	return "<Unknown PixelStoreParameter>";
}

const char* Context::getPixelTypeName(GLenum enum_) const {
	switch(enum_) {
		case 0x1400: return "GL_BYTE";
		case 0x1406: return "GL_FLOAT";
		case 0x1404: return "GL_INT";
		case 0x1402: return "GL_SHORT";
		case 0x1401: return "GL_UNSIGNED_BYTE";
		case 0x8032: return "GL_UNSIGNED_BYTE_3_3_2";
		case 0x1405: return "GL_UNSIGNED_INT";
		case 0x8036: return "GL_UNSIGNED_INT_10_10_10_2";
		case 0x8035: return "GL_UNSIGNED_INT_8_8_8_8";
		case 0x1403: return "GL_UNSIGNED_SHORT";
		case 0x8033: return "GL_UNSIGNED_SHORT_4_4_4_4";
		case 0x8034: return "GL_UNSIGNED_SHORT_5_5_5_1";
	}
	return "<Unknown PixelType>";
}

const char* Context::getPolygonModeName(GLenum enum_) const {
	switch(enum_) {
		case 0x1B02: return "GL_FILL";
		case 0x1B01: return "GL_LINE";
		case 0x1B00: return "GL_POINT";
	}
	return "<Unknown PolygonMode>";
}

const char* Context::getPrimitiveTypeName(GLenum enum_) const {
	switch(enum_) {
		case 0x0001: return "GL_LINES";
		case 0x000A: return "GL_LINES_ADJACENCY";
		case 0x0002: return "GL_LINE_LOOP";
		case 0x0003: return "GL_LINE_STRIP";
		case 0x000B: return "GL_LINE_STRIP_ADJACENCY";
		case 0x0000: return "GL_POINTS";
		case 0x0004: return "GL_TRIANGLES";
		case 0x000C: return "GL_TRIANGLES_ADJACENCY";
		case 0x0006: return "GL_TRIANGLE_FAN";
		case 0x0005: return "GL_TRIANGLE_STRIP";
		case 0x000D: return "GL_TRIANGLE_STRIP_ADJACENCY";
	}
	return "<Unknown PrimitiveType>";
}

const char* Context::getReadBufferModeName(GLenum enum_) const {
	switch(enum_) {
		case 0x0405: return "GL_BACK";
		case 0x0402: return "GL_BACK_LEFT";
		case 0x0403: return "GL_BACK_RIGHT";
		case 0x0404: return "GL_FRONT";
		case 0x0400: return "GL_FRONT_LEFT";
		case 0x0401: return "GL_FRONT_RIGHT";
		case 0x0406: return "GL_LEFT";
		case 0x0407: return "GL_RIGHT";
	}
	return "<Unknown ReadBufferMode>";
}

const char* Context::getStencilFaceDirectionName(GLenum enum_) const {
	switch(enum_) {
		case 0x0404: return "GL_FRONT";
		case 0x0405: return "GL_BACK";
		case 0x0408: return "GL_FRONT_AND_BACK";
	}
	return "<Unknown StencilFaceDirection>";
}

const char* Context::getStencilFunctionName(GLenum enum_) const {
	switch(enum_) {
		case 0x0207: return "GL_ALWAYS";
		case 0x0202: return "GL_EQUAL";
		case 0x0206: return "GL_GEQUAL";
		case 0x0204: return "GL_GREATER";
		case 0x0203: return "GL_LEQUAL";
		case 0x0201: return "GL_LESS";
		case 0x0200: return "GL_NEVER";
		case 0x0205: return "GL_NOTEQUAL";
	}
	return "<Unknown StencilFunction>";
}

const char* Context::getStencilOpName(GLenum enum_) const {
	switch(enum_) {
		case 0x1E03: return "GL_DECR";
		case 0x1E02: return "GL_INCR";
		case 0x150A: return "GL_INVERT";
		case 0x1E00: return "GL_KEEP";
		case 0x1E01: return "GL_REPLACE";
		case 0: return "GL_ZERO";
	}
	return "<Unknown StencilOp>";
}

const char* Context::getStringNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x1F03: return "GL_EXTENSIONS";
		case 0x1F01: return "GL_RENDERER";
		case 0x1F00: return "GL_VENDOR";
		case 0x1F02: return "GL_VERSION";
		case 0x8B8C: return "GL_SHADING_LANGUAGE_VERSION";
	}
	return "<Unknown StringName>";
}

const char* Context::getSyncObjectMaskName(GLenum enum_) const {
	switch(enum_) {
		case 0x00000001: return "GL_SYNC_FLUSH_COMMANDS_BIT";
	}
	return "<Unknown SyncObjectMask>";
}

const char* Context::getTextureParameterNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x1004: return "GL_TEXTURE_BORDER_COLOR";
		case 0x2800: return "GL_TEXTURE_MAG_FILTER";
		case 0x2801: return "GL_TEXTURE_MIN_FILTER";
		case 0x8072: return "GL_TEXTURE_WRAP_R";
		case 0x2802: return "GL_TEXTURE_WRAP_S";
		case 0x2803: return "GL_TEXTURE_WRAP_T";
		case 0x813C: return "GL_TEXTURE_BASE_LEVEL";
		case 0x884C: return "GL_TEXTURE_COMPARE_MODE";
		case 0x884D: return "GL_TEXTURE_COMPARE_FUNC";
		case 0x8501: return "GL_TEXTURE_LOD_BIAS";
		case 0x813A: return "GL_TEXTURE_MIN_LOD";
		case 0x813B: return "GL_TEXTURE_MAX_LOD";
		case 0x813D: return "GL_TEXTURE_MAX_LEVEL";
		case 0x8E42: return "GL_TEXTURE_SWIZZLE_R";
		case 0x8E43: return "GL_TEXTURE_SWIZZLE_G";
		case 0x8E44: return "GL_TEXTURE_SWIZZLE_B";
		case 0x8E45: return "GL_TEXTURE_SWIZZLE_A";
		case 0x8E46: return "GL_TEXTURE_SWIZZLE_RGBA";
		case 0x805F: return "GL_TEXTURE_ALPHA_SIZE";
		case 0x805E: return "GL_TEXTURE_BLUE_SIZE";
		case 0x805D: return "GL_TEXTURE_GREEN_SIZE";
		case 0x1001: return "GL_TEXTURE_HEIGHT";
		case 0x1003: return "GL_TEXTURE_INTERNAL_FORMAT";
		case 0x805C: return "GL_TEXTURE_RED_SIZE";
		case 0x1000: return "GL_TEXTURE_WIDTH";
	}
	return "<Unknown TextureParameterName>";
}

const char* Context::getTextureTargetName(GLenum enum_) const {
	switch(enum_) {
		case 0x8063: return "GL_PROXY_TEXTURE_1D";
		case 0x8C19: return "GL_PROXY_TEXTURE_1D_ARRAY";
		case 0x8064: return "GL_PROXY_TEXTURE_2D";
		case 0x8C1B: return "GL_PROXY_TEXTURE_2D_ARRAY";
		case 0x9101: return "GL_PROXY_TEXTURE_2D_MULTISAMPLE";
		case 0x9103: return "GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY";
		case 0x8070: return "GL_PROXY_TEXTURE_3D";
		case 0x851B: return "GL_PROXY_TEXTURE_CUBE_MAP";
		case 0x84F7: return "GL_PROXY_TEXTURE_RECTANGLE";
		case 0x0DE0: return "GL_TEXTURE_1D";
		case 0x0DE1: return "GL_TEXTURE_2D";
		case 0x806F: return "GL_TEXTURE_3D";
		case 0x84F5: return "GL_TEXTURE_RECTANGLE";
		case 0x8513: return "GL_TEXTURE_CUBE_MAP";
		case 0x8515: return "GL_TEXTURE_CUBE_MAP_POSITIVE_X";
		case 0x8516: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_X";
		case 0x8517: return "GL_TEXTURE_CUBE_MAP_POSITIVE_Y";
		case 0x8518: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";
		case 0x8519: return "GL_TEXTURE_CUBE_MAP_POSITIVE_Z";
		case 0x851A: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";
		case 0x8C18: return "GL_TEXTURE_1D_ARRAY";
		case 0x8C1A: return "GL_TEXTURE_2D_ARRAY";
		case 0x9100: return "GL_TEXTURE_2D_MULTISAMPLE";
		case 0x9102: return "GL_TEXTURE_2D_MULTISAMPLE_ARRAY";
	}
	return "<Unknown TextureTarget>";
}

const char* Context::getFramebufferAttachmentName(GLenum enum_) const {
	switch(enum_) {
		case 0x8CDF: return "GL_MAX_COLOR_ATTACHMENTS";
		case 0x8CE0: return "GL_COLOR_ATTACHMENT0";
		case 0x8CE1: return "GL_COLOR_ATTACHMENT1";
		case 0x8CE2: return "GL_COLOR_ATTACHMENT2";
		case 0x8CE3: return "GL_COLOR_ATTACHMENT3";
		case 0x8CE4: return "GL_COLOR_ATTACHMENT4";
		case 0x8CE5: return "GL_COLOR_ATTACHMENT5";
		case 0x8CE6: return "GL_COLOR_ATTACHMENT6";
		case 0x8CE7: return "GL_COLOR_ATTACHMENT7";
		case 0x8CE8: return "GL_COLOR_ATTACHMENT8";
		case 0x8CE9: return "GL_COLOR_ATTACHMENT9";
		case 0x8CEA: return "GL_COLOR_ATTACHMENT10";
		case 0x8CEB: return "GL_COLOR_ATTACHMENT11";
		case 0x8CEC: return "GL_COLOR_ATTACHMENT12";
		case 0x8CED: return "GL_COLOR_ATTACHMENT13";
		case 0x8CEE: return "GL_COLOR_ATTACHMENT14";
		case 0x8CEF: return "GL_COLOR_ATTACHMENT15";
		case 0x8CF0: return "GL_COLOR_ATTACHMENT16";
		case 0x8CF1: return "GL_COLOR_ATTACHMENT17";
		case 0x8CF2: return "GL_COLOR_ATTACHMENT18";
		case 0x8CF3: return "GL_COLOR_ATTACHMENT19";
		case 0x8CF4: return "GL_COLOR_ATTACHMENT20";
		case 0x8CF5: return "GL_COLOR_ATTACHMENT21";
		case 0x8CF6: return "GL_COLOR_ATTACHMENT22";
		case 0x8CF7: return "GL_COLOR_ATTACHMENT23";
		case 0x8CF8: return "GL_COLOR_ATTACHMENT24";
		case 0x8CF9: return "GL_COLOR_ATTACHMENT25";
		case 0x8CFA: return "GL_COLOR_ATTACHMENT26";
		case 0x8CFB: return "GL_COLOR_ATTACHMENT27";
		case 0x8CFC: return "GL_COLOR_ATTACHMENT28";
		case 0x8CFD: return "GL_COLOR_ATTACHMENT29";
		case 0x8CFE: return "GL_COLOR_ATTACHMENT30";
		case 0x8CFF: return "GL_COLOR_ATTACHMENT31";
		case 0x8D00: return "GL_DEPTH_ATTACHMENT";
	}
	return "<Unknown FramebufferAttachment>";
}

const char* Context::getRenderbufferTargetName(GLenum enum_) const {
	switch(enum_) {
		case 0x8D41: return "GL_RENDERBUFFER";
	}
	return "<Unknown RenderbufferTarget>";
}

const char* Context::getFramebufferTargetName(GLenum enum_) const {
	switch(enum_) {
		case 0x8D40: return "GL_FRAMEBUFFER";
		case 0x8CA9: return "GL_DRAW_FRAMEBUFFER";
		case 0x8CA8: return "GL_READ_FRAMEBUFFER";
	}
	return "<Unknown FramebufferTarget>";
}

const char* Context::getTextureUnitName(GLenum enum_) const {
	switch(enum_) {
		case 0x84C0: return "GL_TEXTURE0";
		case 0x84C1: return "GL_TEXTURE1";
		case 0x84C2: return "GL_TEXTURE2";
		case 0x84C3: return "GL_TEXTURE3";
		case 0x84C4: return "GL_TEXTURE4";
		case 0x84C5: return "GL_TEXTURE5";
		case 0x84C6: return "GL_TEXTURE6";
		case 0x84C7: return "GL_TEXTURE7";
		case 0x84C8: return "GL_TEXTURE8";
		case 0x84C9: return "GL_TEXTURE9";
		case 0x84CA: return "GL_TEXTURE10";
		case 0x84CB: return "GL_TEXTURE11";
		case 0x84CC: return "GL_TEXTURE12";
		case 0x84CD: return "GL_TEXTURE13";
		case 0x84CE: return "GL_TEXTURE14";
		case 0x84CF: return "GL_TEXTURE15";
		case 0x84D0: return "GL_TEXTURE16";
		case 0x84D1: return "GL_TEXTURE17";
		case 0x84D2: return "GL_TEXTURE18";
		case 0x84D3: return "GL_TEXTURE19";
		case 0x84D4: return "GL_TEXTURE20";
		case 0x84D5: return "GL_TEXTURE21";
		case 0x84D6: return "GL_TEXTURE22";
		case 0x84D7: return "GL_TEXTURE23";
		case 0x84D8: return "GL_TEXTURE24";
		case 0x84D9: return "GL_TEXTURE25";
		case 0x84DA: return "GL_TEXTURE26";
		case 0x84DB: return "GL_TEXTURE27";
		case 0x84DC: return "GL_TEXTURE28";
		case 0x84DD: return "GL_TEXTURE29";
		case 0x84DE: return "GL_TEXTURE30";
		case 0x84DF: return "GL_TEXTURE31";
	}
	return "<Unknown TextureUnit>";
}

const char* Context::getTypeEnumName(GLenum enum_) const {
	switch(enum_) {
		case 0x8E13: return "GL_QUERY_WAIT";
		case 0x8E14: return "GL_QUERY_NO_WAIT";
		case 0x8E15: return "GL_QUERY_BY_REGION_WAIT";
		case 0x8E16: return "GL_QUERY_BY_REGION_NO_WAIT";
	}
	return "<Unknown TypeEnum>";
}

const char* Context::getQueryTargetName(GLenum enum_) const {
	switch(enum_) {
		case 0x8914: return "GL_SAMPLES_PASSED";
		case 0x8C2F: return "GL_ANY_SAMPLES_PASSED";
		case 0x8C87: return "GL_PRIMITIVES_GENERATED";
		case 0x8C88: return "GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN";
		case 0x88BF: return "GL_TIME_ELAPSED";
	}
	return "<Unknown QueryTarget>";
}

const char* Context::getRenderbufferParameterNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x8D42: return "GL_RENDERBUFFER_WIDTH";
		case 0x8D43: return "GL_RENDERBUFFER_HEIGHT";
		case 0x8D44: return "GL_RENDERBUFFER_INTERNAL_FORMAT";
		case 0x8CAB: return "GL_RENDERBUFFER_SAMPLES";
		case 0x8D50: return "GL_RENDERBUFFER_RED_SIZE";
		case 0x8D51: return "GL_RENDERBUFFER_GREEN_SIZE";
		case 0x8D52: return "GL_RENDERBUFFER_BLUE_SIZE";
		case 0x8D53: return "GL_RENDERBUFFER_ALPHA_SIZE";
		case 0x8D54: return "GL_RENDERBUFFER_DEPTH_SIZE";
		case 0x8D55: return "GL_RENDERBUFFER_STENCIL_SIZE";
	}
	return "<Unknown RenderbufferParameterName>";
}

const char* Context::getBlendingFactorName(GLenum enum_) const {
	switch(enum_) {
		case 0: return "GL_ZERO";
		case 1: return "GL_ONE";
		case 0x0300: return "GL_SRC_COLOR";
		case 0x0301: return "GL_ONE_MINUS_SRC_COLOR";
		case 0x0306: return "GL_DST_COLOR";
		case 0x0307: return "GL_ONE_MINUS_DST_COLOR";
		case 0x0302: return "GL_SRC_ALPHA";
		case 0x0303: return "GL_ONE_MINUS_SRC_ALPHA";
		case 0x0304: return "GL_DST_ALPHA";
		case 0x0305: return "GL_ONE_MINUS_DST_ALPHA";
		case 0x8001: return "GL_CONSTANT_COLOR";
		case 0x8002: return "GL_ONE_MINUS_CONSTANT_COLOR";
		case 0x8003: return "GL_CONSTANT_ALPHA";
		case 0x8004: return "GL_ONE_MINUS_CONSTANT_ALPHA";
		case 0x0308: return "GL_SRC_ALPHA_SATURATE";
		case 0x88F9: return "GL_SRC1_COLOR";
		case 0x88FA: return "GL_ONE_MINUS_SRC1_COLOR";
		case 0x8589: return "GL_SRC1_ALPHA";
		case 0x88FB: return "GL_ONE_MINUS_SRC1_ALPHA";
	}
	return "<Unknown BlendingFactor>";
}

const char* Context::getBlitFramebufferFilterName(GLenum enum_) const {
	switch(enum_) {
		case 0x2600: return "GL_NEAREST";
		case 0x2601: return "GL_LINEAR";
	}
	return "<Unknown BlitFramebufferFilter>";
}

const char* Context::getBufferName(GLenum enum_) const {
	switch(enum_) {
		case 0x1800: return "GL_COLOR";
		case 0x1801: return "GL_DEPTH";
		case 0x1802: return "GL_STENCIL";
	}
	return "<Unknown Buffer>";
}

const char* Context::getCopyBufferSubDataTargetName(GLenum enum_) const {
	switch(enum_) {
		case 0x8892: return "GL_ARRAY_BUFFER";
		case 0x8F36: return "GL_COPY_READ_BUFFER";
		case 0x8F37: return "GL_COPY_WRITE_BUFFER";
		case 0x8893: return "GL_ELEMENT_ARRAY_BUFFER";
		case 0x88EB: return "GL_PIXEL_PACK_BUFFER";
		case 0x88EC: return "GL_PIXEL_UNPACK_BUFFER";
		case 0x8C2A: return "GL_TEXTURE_BUFFER";
		case 0x8C8E: return "GL_TRANSFORM_FEEDBACK_BUFFER";
		case 0x8A11: return "GL_UNIFORM_BUFFER";
	}
	return "<Unknown CopyBufferSubDataTarget>";
}

const char* Context::getShaderTypeName(GLenum enum_) const {
	switch(enum_) {
		case 0x8B31: return "GL_VERTEX_SHADER";
		case 0x8DD9: return "GL_GEOMETRY_SHADER";
		case 0x8B30: return "GL_FRAGMENT_SHADER";
	}
	return "<Unknown ShaderType>";
}

const char* Context::getDebugSourceName(GLenum enum_) const {
	switch(enum_) {
		case 0x8246: return "GL_DEBUG_SOURCE_API";
		case 0x8247: return "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
		case 0x8248: return "GL_DEBUG_SOURCE_SHADER_COMPILER";
		case 0x8249: return "GL_DEBUG_SOURCE_THIRD_PARTY";
		case 0x824A: return "GL_DEBUG_SOURCE_APPLICATION";
		case 0x824B: return "GL_DEBUG_SOURCE_OTHER";
		case 0x1100: return "GL_DONT_CARE";
	}
	return "<Unknown DebugSource>";
}

const char* Context::getDebugTypeName(GLenum enum_) const {
	switch(enum_) {
		case 0x824C: return "GL_DEBUG_TYPE_ERROR";
		case 0x824D: return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
		case 0x824E: return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
		case 0x824F: return "GL_DEBUG_TYPE_PORTABILITY";
		case 0x8250: return "GL_DEBUG_TYPE_PERFORMANCE";
		case 0x8268: return "GL_DEBUG_TYPE_MARKER";
		case 0x8269: return "GL_DEBUG_TYPE_PUSH_GROUP";
		case 0x826A: return "GL_DEBUG_TYPE_POP_GROUP";
		case 0x8251: return "GL_DEBUG_TYPE_OTHER";
		case 0x1100: return "GL_DONT_CARE";
	}
	return "<Unknown DebugType>";
}

const char* Context::getDebugSeverityName(GLenum enum_) const {
	switch(enum_) {
		case 0x9148: return "GL_DEBUG_SEVERITY_LOW";
		case 0x9147: return "GL_DEBUG_SEVERITY_MEDIUM";
		case 0x9146: return "GL_DEBUG_SEVERITY_HIGH";
		case 0x1100: return "GL_DONT_CARE";
	}
	return "<Unknown DebugSeverity>";
}

const char* Context::getSyncConditionName(GLenum enum_) const {
	switch(enum_) {
		case 0x9117: return "GL_SYNC_GPU_COMMANDS_COMPLETE";
	}
	return "<Unknown SyncCondition>";
}

const char* Context::getUniformBlockPNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x8A3F: return "GL_UNIFORM_BLOCK_BINDING";
		case 0x8A40: return "GL_UNIFORM_BLOCK_DATA_SIZE";
		case 0x8A41: return "GL_UNIFORM_BLOCK_NAME_LENGTH";
		case 0x8A42: return "GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS";
		case 0x8A43: return "GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES";
		case 0x8A44: return "GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER";
		case 0x8A45: return "GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER";
		case 0x8A46: return "GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER";
	}
	return "<Unknown UniformBlockPName>";
}

const char* Context::getUniformPNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x8A37: return "GL_UNIFORM_TYPE";
		case 0x8A38: return "GL_UNIFORM_SIZE";
		case 0x8A39: return "GL_UNIFORM_NAME_LENGTH";
		case 0x8A3A: return "GL_UNIFORM_BLOCK_INDEX";
		case 0x8A3B: return "GL_UNIFORM_OFFSET";
		case 0x8A3C: return "GL_UNIFORM_ARRAY_STRIDE";
		case 0x8A3D: return "GL_UNIFORM_MATRIX_STRIDE";
		case 0x8A3E: return "GL_UNIFORM_IS_ROW_MAJOR";
	}
	return "<Unknown UniformPName>";
}

const char* Context::getSamplerParameterNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x2802: return "GL_TEXTURE_WRAP_S";
		case 0x2803: return "GL_TEXTURE_WRAP_T";
		case 0x8072: return "GL_TEXTURE_WRAP_R";
		case 0x2801: return "GL_TEXTURE_MIN_FILTER";
		case 0x2800: return "GL_TEXTURE_MAG_FILTER";
		case 0x1004: return "GL_TEXTURE_BORDER_COLOR";
		case 0x813A: return "GL_TEXTURE_MIN_LOD";
		case 0x813B: return "GL_TEXTURE_MAX_LOD";
		case 0x884C: return "GL_TEXTURE_COMPARE_MODE";
		case 0x884D: return "GL_TEXTURE_COMPARE_FUNC";
	}
	return "<Unknown SamplerParameterName>";
}

const char* Context::getVertexProvokingModeName(GLenum enum_) const {
	switch(enum_) {
		case 0x8E4D: return "GL_FIRST_VERTEX_CONVENTION";
		case 0x8E4E: return "GL_LAST_VERTEX_CONVENTION";
	}
	return "<Unknown VertexProvokingMode>";
}

const char* Context::getObjectIdentifierName(GLenum enum_) const {
	switch(enum_) {
		case 0x82E0: return "GL_BUFFER";
		case 0x82E1: return "GL_SHADER";
		case 0x82E2: return "GL_PROGRAM";
		case 0x8074: return "GL_VERTEX_ARRAY";
		case 0x82E3: return "GL_QUERY";
		case 0x82E4: return "GL_PROGRAM_PIPELINE";
		case 0x82E6: return "GL_SAMPLER";
		case 0x1702: return "GL_TEXTURE";
		case 0x8D41: return "GL_RENDERBUFFER";
		case 0x8D40: return "GL_FRAMEBUFFER";
	}
	return "<Unknown ObjectIdentifier>";
}

const char* Context::getSyncParameterNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x9112: return "GL_OBJECT_TYPE";
		case 0x9114: return "GL_SYNC_STATUS";
		case 0x9113: return "GL_SYNC_CONDITION";
		case 0x9115: return "GL_SYNC_FLAGS";
	}
	return "<Unknown SyncParameterName>";
}

const char* Context::getShaderParameterNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x8B4F: return "GL_SHADER_TYPE";
		case 0x8B80: return "GL_DELETE_STATUS";
		case 0x8B81: return "GL_COMPILE_STATUS";
		case 0x8B84: return "GL_INFO_LOG_LENGTH";
		case 0x8B88: return "GL_SHADER_SOURCE_LENGTH";
	}
	return "<Unknown ShaderParameterName>";
}

const char* Context::getQueryObjectParameterNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x8867: return "GL_QUERY_RESULT_AVAILABLE";
		case 0x8866: return "GL_QUERY_RESULT";
	}
	return "<Unknown QueryObjectParameterName>";
}

const char* Context::getQueryParameterNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x8865: return "GL_CURRENT_QUERY";
		case 0x8864: return "GL_QUERY_COUNTER_BITS";
	}
	return "<Unknown QueryParameterName>";
}

const char* Context::getVertexAttribEnumName(GLenum enum_) const {
	switch(enum_) {
		case 0x889F: return "GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING";
		case 0x8622: return "GL_VERTEX_ATTRIB_ARRAY_ENABLED";
		case 0x8623: return "GL_VERTEX_ATTRIB_ARRAY_SIZE";
		case 0x8624: return "GL_VERTEX_ATTRIB_ARRAY_STRIDE";
		case 0x8625: return "GL_VERTEX_ATTRIB_ARRAY_TYPE";
		case 0x886A: return "GL_VERTEX_ATTRIB_ARRAY_NORMALIZED";
		case 0x88FD: return "GL_VERTEX_ATTRIB_ARRAY_INTEGER";
		case 0x88FE: return "GL_VERTEX_ATTRIB_ARRAY_DIVISOR";
		case 0x8626: return "GL_CURRENT_VERTEX_ATTRIB";
	}
	return "<Unknown VertexAttribEnum>";
}

const char* Context::getAttributeTypeName(GLenum enum_) const {
	switch(enum_) {
		case 0x8B50: return "GL_FLOAT_VEC2";
		case 0x8B51: return "GL_FLOAT_VEC3";
		case 0x8B52: return "GL_FLOAT_VEC4";
		case 0x8B53: return "GL_INT_VEC2";
		case 0x8B54: return "GL_INT_VEC3";
		case 0x8B55: return "GL_INT_VEC4";
		case 0x8B56: return "GL_BOOL";
		case 0x8B57: return "GL_BOOL_VEC2";
		case 0x8B58: return "GL_BOOL_VEC3";
		case 0x8B59: return "GL_BOOL_VEC4";
		case 0x8B5A: return "GL_FLOAT_MAT2";
		case 0x8B5B: return "GL_FLOAT_MAT3";
		case 0x8B5C: return "GL_FLOAT_MAT4";
		case 0x8B5D: return "GL_SAMPLER_1D";
		case 0x8B5E: return "GL_SAMPLER_2D";
		case 0x8B5F: return "GL_SAMPLER_3D";
		case 0x8B60: return "GL_SAMPLER_CUBE";
		case 0x8B61: return "GL_SAMPLER_1D_SHADOW";
		case 0x8B62: return "GL_SAMPLER_2D_SHADOW";
		case 0x8B63: return "GL_SAMPLER_2D_RECT";
		case 0x8B64: return "GL_SAMPLER_2D_RECT_SHADOW";
		case 0x8B65: return "GL_FLOAT_MAT2x3";
		case 0x8B66: return "GL_FLOAT_MAT2x4";
		case 0x8B67: return "GL_FLOAT_MAT3x2";
		case 0x8B68: return "GL_FLOAT_MAT3x4";
		case 0x8B69: return "GL_FLOAT_MAT4x2";
		case 0x8B6A: return "GL_FLOAT_MAT4x3";
	}
	return "<Unknown AttributeType>";
}

const char* Context::getFramebufferAttachmentParameterNameName(GLenum enum_) const {
	switch(enum_) {
		case 0x8212: return "GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE";
		case 0x8213: return "GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE";
		case 0x8214: return "GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE";
		case 0x8215: return "GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE";
		case 0x8216: return "GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE";
		case 0x8217: return "GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE";
		case 0x8211: return "GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE";
		case 0x8210: return "GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING";
		case 0x8CD1: return "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME";
		case 0x8CD2: return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL";
		case 0x8CD3: return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE";
		case 0x8DA7: return "GL_FRAMEBUFFER_ATTACHMENT_LAYERED";
		case 0x8CD4: return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER";
	}
	return "<Unknown FramebufferAttachmentParameterName>";
}

const char* Context::getVertexAttribPointerTypeName(GLenum enum_) const {
	switch(enum_) {
		case 0x1400: return "GL_BYTE";
		case 0x1401: return "GL_UNSIGNED_BYTE";
		case 0x1402: return "GL_SHORT";
		case 0x1403: return "GL_UNSIGNED_SHORT";
		case 0x1404: return "GL_INT";
		case 0x1405: return "GL_UNSIGNED_INT";
		case 0x1406: return "GL_FLOAT";
		case 0x140A: return "GL_DOUBLE";
		case 0x140B: return "GL_HALF_FLOAT";
		case 0x8D9F: return "GL_INT_2_10_10_10_REV";
		case 0x8368: return "GL_UNSIGNED_INT_2_10_10_10_REV";
		case 0x8C3B: return "GL_UNSIGNED_INT_10F_11F_11F_REV";
	}
	return "<Unknown VertexAttribPointerType>";
}

const char* Context::getProgramPropertyARBName(GLenum enum_) const {
	switch(enum_) {
		case 0x8B80: return "GL_DELETE_STATUS";
		case 0x8B82: return "GL_LINK_STATUS";
		case 0x8B83: return "GL_VALIDATE_STATUS";
		case 0x8B84: return "GL_INFO_LOG_LENGTH";
		case 0x8B85: return "GL_ATTACHED_SHADERS";
		case 0x8B89: return "GL_ACTIVE_ATTRIBUTES";
		case 0x8B8A: return "GL_ACTIVE_ATTRIBUTE_MAX_LENGTH";
		case 0x8B86: return "GL_ACTIVE_UNIFORMS";
		case 0x8A36: return "GL_ACTIVE_UNIFORM_BLOCKS";
		case 0x8A35: return "GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH";
		case 0x8B87: return "GL_ACTIVE_UNIFORM_MAX_LENGTH";
		case 0x8C7F: return "GL_TRANSFORM_FEEDBACK_BUFFER_MODE";
		case 0x8C83: return "GL_TRANSFORM_FEEDBACK_VARYINGS";
		case 0x8C76: return "GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH";
		case 0x8916: return "GL_GEOMETRY_VERTICES_OUT";
		case 0x8917: return "GL_GEOMETRY_INPUT_TYPE";
		case 0x8918: return "GL_GEOMETRY_OUTPUT_TYPE";
	}
	return "<Unknown ProgramPropertyARB>";
}



String Context::bitsetAsString(GLbitfield bitfield) const {
	static const BitMask masks[] = { { 0, nullptr } };
	return lair::bitsetAsString(masks, bitfield);
}

String Context::bitsetAttribMaskAsString(GLbitfield bitfield) const {
	static const BitMask masks[] = {
		{ 0x00000100, "GL_DEPTH_BUFFER_BIT" },
		{ 0x00000400, "GL_STENCIL_BUFFER_BIT" },
		{ 0x00004000, "GL_COLOR_BUFFER_BIT" },
		{ 0, nullptr }
	};
	return lair::bitsetAsString(masks, bitfield);
}

String Context::bitsetBufferAccessMaskAsString(GLbitfield bitfield) const {
	static const BitMask masks[] = {
		{ 0x0010, "GL_MAP_FLUSH_EXPLICIT_BIT" },
		{ 0x0008, "GL_MAP_INVALIDATE_BUFFER_BIT" },
		{ 0x0004, "GL_MAP_INVALIDATE_RANGE_BIT" },
		{ 0x0001, "GL_MAP_READ_BIT" },
		{ 0x0020, "GL_MAP_UNSYNCHRONIZED_BIT" },
		{ 0x0002, "GL_MAP_WRITE_BIT" },
		{ 0, nullptr }
	};
	return lair::bitsetAsString(masks, bitfield);
}

String Context::bitsetClearBufferMaskAsString(GLbitfield bitfield) const {
	static const BitMask masks[] = {
		{ 0x00004000, "GL_COLOR_BUFFER_BIT" },
		{ 0x00000100, "GL_DEPTH_BUFFER_BIT" },
		{ 0x00000400, "GL_STENCIL_BUFFER_BIT" },
		{ 0, nullptr }
	};
	return lair::bitsetAsString(masks, bitfield);
}

String Context::bitsetContextFlagMaskAsString(GLbitfield bitfield) const {
	static const BitMask masks[] = {
		{ 0x00000001, "GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT" },
		{ 0x00000002, "GL_CONTEXT_FLAG_DEBUG_BIT" },
		{ 0, nullptr }
	};
	return lair::bitsetAsString(masks, bitfield);
}

String Context::bitsetContextProfileMaskAsString(GLbitfield bitfield) const {
	static const BitMask masks[] = {
		{ 0x00000001, "GL_CONTEXT_CORE_PROFILE_BIT" },
		{ 0x00000002, "GL_CONTEXT_COMPATIBILITY_PROFILE_BIT" },
		{ 0, nullptr }
	};
	return lair::bitsetAsString(masks, bitfield);
}

String Context::bitsetMapBufferUsageMaskAsString(GLbitfield bitfield) const {
	static const BitMask masks[] = {
		{ 0x0001, "GL_MAP_READ_BIT" },
		{ 0x0002, "GL_MAP_WRITE_BIT" },
		{ 0x0004, "GL_MAP_INVALIDATE_RANGE_BIT" },
		{ 0x0008, "GL_MAP_INVALIDATE_BUFFER_BIT" },
		{ 0x0010, "GL_MAP_FLUSH_EXPLICIT_BIT" },
		{ 0x0020, "GL_MAP_UNSYNCHRONIZED_BIT" },
		{ 0, nullptr }
	};
	return lair::bitsetAsString(masks, bitfield);
}

String Context::bitsetSyncObjectMaskAsString(GLbitfield bitfield) const {
	static const BitMask masks[] = {
		{ 0x00000001, "GL_SYNC_FLUSH_COMMANDS_BIT" },
		{ 0, nullptr }
	};
	return lair::bitsetAsString(masks, bitfield);
}



bool Context::hasExtension(const char* ext) const {
	GLint count = 0;
	_glGetIntegerv(gl::NUM_EXTENSIONS, &count);
	for(GLint i = 0; i < count; ++i) {
		const char* e = (const char*)_glGetStringi(gl::EXTENSIONS, i);
		if(strcmp(e, ext) == 0)
			return true;
	}
	return false;
}

void Context::checkGlErrors() {
	GLenum error;
	while((error = _glGetError()) != gl::NO_ERROR) {
		if(error == gl::OUT_OF_MEMORY)
			throw GLOutOfMemory();

		if(_logErrors) {
			_log->error("OpenGL error: ", getErrorCodeName(error));
		}

		if(_abortOnError) {
			abort();
		}
	}
}


void Context::setEnabled(GLenum cap, bool enabled) {
	if(enabled) {
		enable(cap);
	} else {
		disable(cap);
	}
}


// GL_VERSION_1_0

void Context::cullFace(GLenum mode) {
	if(_logCalls) {
		_log->write(_logLevel, "glCullFace(", getCullFaceModeName(mode), ")");
	}
	_glCullFace(mode);
	checkGlErrors();
}

void Context::frontFace(GLenum mode) {
	if(_logCalls) {
		_log->write(_logLevel, "glFrontFace(", getFrontFaceDirectionName(mode), ")");
	}
	_glFrontFace(mode);
	checkGlErrors();
}

void Context::hint(GLenum target, GLenum mode) {
	if(_logCalls) {
		_log->write(_logLevel, "glHint(", getHintTargetName(target), ", ", getHintModeName(mode), ")");
	}
	_glHint(target, mode);
	checkGlErrors();
}

void Context::lineWidth(GLfloat width) {
	if(_logCalls) {
		_log->write(_logLevel, "glLineWidth(", width, ")");
	}
	_glLineWidth(width);
	checkGlErrors();
}

void Context::pointSize(GLfloat size) {
	if(_logCalls) {
		_log->write(_logLevel, "glPointSize(", size, ")");
	}
	_glPointSize(size);
	checkGlErrors();
}

void Context::polygonMode(GLenum face, GLenum mode) {
	if(_logCalls) {
		_log->write(_logLevel, "glPolygonMode(", getMaterialFaceName(face), ", ", getPolygonModeName(mode), ")");
	}
	_glPolygonMode(face, mode);
	checkGlErrors();
}

void Context::scissor(GLint x, GLint y, GLsizei width, GLsizei height) {
	if(_logCalls) {
		_log->write(_logLevel, "glScissor(", x, ", ", y, ", ", width, ", ", height, ")");
	}
	_glScissor(x, y, width, height);
	checkGlErrors();
}

void Context::texParameterf(GLenum target, GLenum pname, GLfloat param) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexParameterf(", getTextureTargetName(target), ", ", getTextureParameterNameName(pname), ", ", param, ")");
	}
	_glTexParameterf(target, pname, param);
	checkGlErrors();
}

void Context::texParameterfv(GLenum target, GLenum pname, const GLfloat *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexParameterfv(", getTextureTargetName(target), ", ", getTextureParameterNameName(pname), ", ", params, ")");
	}
	_glTexParameterfv(target, pname, params);
	checkGlErrors();
}

void Context::texParameteri(GLenum target, GLenum pname, GLint param) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexParameteri(", getTextureTargetName(target), ", ", getTextureParameterNameName(pname), ", ", param, ")");
	}
	_glTexParameteri(target, pname, param);
	checkGlErrors();
}

void Context::texParameteriv(GLenum target, GLenum pname, const GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexParameteriv(", getTextureTargetName(target), ", ", getTextureParameterNameName(pname), ", ", params, ")");
	}
	_glTexParameteriv(target, pname, params);
	checkGlErrors();
}

void Context::texImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexImage1D(", getTextureTargetName(target), ", ", level, ", ", getInternalFormatName(internalformat), ", ", width, ", ", border, ", ", getPixelFormatName(format), ", ", getPixelTypeName(type), ", ", pixels, ")");
	}
	_glTexImage1D(target, level, internalformat, width, border, format, type, pixels);
	checkGlErrors();
}

void Context::texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexImage2D(", getTextureTargetName(target), ", ", level, ", ", getInternalFormatName(internalformat), ", ", width, ", ", height, ", ", border, ", ", getPixelFormatName(format), ", ", getPixelTypeName(type), ", ", pixels, ")");
	}
	_glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	checkGlErrors();
}

void Context::drawBuffer(GLenum buf) {
	if(_logCalls) {
		_log->write(_logLevel, "glDrawBuffer(", getDrawBufferModeName(buf), ")");
	}
	_glDrawBuffer(buf);
	checkGlErrors();
}

void Context::clear(GLbitfield mask) {
	if(_logCalls) {
		_log->write(_logLevel, "glClear(", bitsetClearBufferMaskAsString(mask), ")");
	}
	_glClear(mask);
	checkGlErrors();
}

void Context::clearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
	if(_logCalls) {
		_log->write(_logLevel, "glClearColor(", red, ", ", green, ", ", blue, ", ", alpha, ")");
	}
	_glClearColor(red, green, blue, alpha);
	checkGlErrors();
}

void Context::clearStencil(GLint s) {
	if(_logCalls) {
		_log->write(_logLevel, "glClearStencil(", s, ")");
	}
	_glClearStencil(s);
	checkGlErrors();
}

void Context::clearDepth(GLdouble depth) {
	if(_logCalls) {
		_log->write(_logLevel, "glClearDepth(", depth, ")");
	}
	_glClearDepth(depth);
	checkGlErrors();
}

void Context::stencilMask(GLuint mask) {
	if(_logCalls) {
		_log->write(_logLevel, "glStencilMask(", mask, ")");
	}
	_glStencilMask(mask);
	checkGlErrors();
}

void Context::colorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) {
	if(_logCalls) {
		_log->write(_logLevel, "glColorMask(", getBooleanName(red), ", ", getBooleanName(green), ", ", getBooleanName(blue), ", ", getBooleanName(alpha), ")");
	}
	_glColorMask(red, green, blue, alpha);
	checkGlErrors();
}

void Context::depthMask(GLboolean flag) {
	if(_logCalls) {
		_log->write(_logLevel, "glDepthMask(", getBooleanName(flag), ")");
	}
	_glDepthMask(flag);
	checkGlErrors();
}

void Context::disable(GLenum cap) {
	if(_logCalls) {
		_log->write(_logLevel, "glDisable(", getEnableCapName(cap), ")");
	}
	_glDisable(cap);
	checkGlErrors();
}

void Context::enable(GLenum cap) {
	if(_logCalls) {
		_log->write(_logLevel, "glEnable(", getEnableCapName(cap), ")");
	}
	_glEnable(cap);
	checkGlErrors();
}

void Context::finish() {
	if(_logCalls) {
		_log->write(_logLevel, "glFinish()");
	}
	_glFinish();
	checkGlErrors();
}

void Context::flush() {
	if(_logCalls) {
		_log->write(_logLevel, "glFlush()");
	}
	_glFlush();
	checkGlErrors();
}

void Context::blendFunc(GLenum sfactor, GLenum dfactor) {
	if(_logCalls) {
		_log->write(_logLevel, "glBlendFunc(", getBlendingFactorName(sfactor), ", ", getBlendingFactorName(dfactor), ")");
	}
	_glBlendFunc(sfactor, dfactor);
	checkGlErrors();
}

void Context::logicOp(GLenum opcode) {
	if(_logCalls) {
		_log->write(_logLevel, "glLogicOp(", getLogicOpName(opcode), ")");
	}
	_glLogicOp(opcode);
	checkGlErrors();
}

void Context::stencilFunc(GLenum func, GLint ref, GLuint mask) {
	if(_logCalls) {
		_log->write(_logLevel, "glStencilFunc(", getStencilFunctionName(func), ", ", ref, ", ", mask, ")");
	}
	_glStencilFunc(func, ref, mask);
	checkGlErrors();
}

void Context::stencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
	if(_logCalls) {
		_log->write(_logLevel, "glStencilOp(", getStencilOpName(fail), ", ", getStencilOpName(zfail), ", ", getStencilOpName(zpass), ")");
	}
	_glStencilOp(fail, zfail, zpass);
	checkGlErrors();
}

void Context::depthFunc(GLenum func) {
	if(_logCalls) {
		_log->write(_logLevel, "glDepthFunc(", getDepthFunctionName(func), ")");
	}
	_glDepthFunc(func);
	checkGlErrors();
}

void Context::pixelStoref(GLenum pname, GLfloat param) {
	if(_logCalls) {
		_log->write(_logLevel, "glPixelStoref(", getPixelStoreParameterName(pname), ", ", param, ")");
	}
	_glPixelStoref(pname, param);
	checkGlErrors();
}

void Context::pixelStorei(GLenum pname, GLint param) {
	if(_logCalls) {
		_log->write(_logLevel, "glPixelStorei(", getPixelStoreParameterName(pname), ", ", param, ")");
	}
	_glPixelStorei(pname, param);
	checkGlErrors();
}

void Context::readBuffer(GLenum src) {
	if(_logCalls) {
		_log->write(_logLevel, "glReadBuffer(", getReadBufferModeName(src), ")");
	}
	_glReadBuffer(src);
	checkGlErrors();
}

void Context::readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels) {
	if(_logCalls) {
		_log->write(_logLevel, "glReadPixels(", x, ", ", y, ", ", width, ", ", height, ", ", getPixelFormatName(format), ", ", getPixelTypeName(type), ", ", pixels, ")");
	}
	_glReadPixels(x, y, width, height, format, type, pixels);
	checkGlErrors();
}

void Context::getBooleanv(GLenum pname, GLboolean *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetBooleanv(", getGetPNameName(pname), ", ", data, ")");
	}
	_glGetBooleanv(pname, data);
	checkGlErrors();
}

void Context::getDoublev(GLenum pname, GLdouble *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetDoublev(", getGetPNameName(pname), ", ", data, ")");
	}
	_glGetDoublev(pname, data);
	checkGlErrors();
}

GLenum Context::getError() {
	if(_logCalls) {
		_log->write(_logLevel, "glGetError()");
	}
	GLenum result = _glGetError();
	checkGlErrors();
	return result;
}

void Context::getFloatv(GLenum pname, GLfloat *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetFloatv(", getGetPNameName(pname), ", ", data, ")");
	}
	_glGetFloatv(pname, data);
	checkGlErrors();
}

void Context::getIntegerv(GLenum pname, GLint *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetIntegerv(", getGetPNameName(pname), ", ", data, ")");
	}
	_glGetIntegerv(pname, data);
	checkGlErrors();
}

const GLubyte * Context::getString(GLenum name) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetString(", getStringNameName(name), ")");
	}
	const GLubyte * result = _glGetString(name);
	checkGlErrors();
	return result;
}

void Context::getTexImage(GLenum target, GLint level, GLenum format, GLenum type, void *pixels) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetTexImage(", getTextureTargetName(target), ", ", level, ", ", getPixelFormatName(format), ", ", getPixelTypeName(type), ", ", pixels, ")");
	}
	_glGetTexImage(target, level, format, type, pixels);
	checkGlErrors();
}

void Context::getTexParameterfv(GLenum target, GLenum pname, GLfloat *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetTexParameterfv(", getTextureTargetName(target), ", ", getGetTextureParameterName(pname), ", ", params, ")");
	}
	_glGetTexParameterfv(target, pname, params);
	checkGlErrors();
}

void Context::getTexParameteriv(GLenum target, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetTexParameteriv(", getTextureTargetName(target), ", ", getGetTextureParameterName(pname), ", ", params, ")");
	}
	_glGetTexParameteriv(target, pname, params);
	checkGlErrors();
}

void Context::getTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetTexLevelParameterfv(", getTextureTargetName(target), ", ", level, ", ", getGetTextureParameterName(pname), ", ", params, ")");
	}
	_glGetTexLevelParameterfv(target, level, pname, params);
	checkGlErrors();
}

void Context::getTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetTexLevelParameteriv(", getTextureTargetName(target), ", ", level, ", ", getGetTextureParameterName(pname), ", ", params, ")");
	}
	_glGetTexLevelParameteriv(target, level, pname, params);
	checkGlErrors();
}

GLboolean Context::isEnabled(GLenum cap) {
	if(_logCalls) {
		_log->write(_logLevel, "glIsEnabled(", getEnableCapName(cap), ")");
	}
	GLboolean result = _glIsEnabled(cap);
	checkGlErrors();
	return result;
}

void Context::depthRange(GLdouble near, GLdouble far) {
	if(_logCalls) {
		_log->write(_logLevel, "glDepthRange(", near, ", ", far, ")");
	}
	_glDepthRange(near, far);
	checkGlErrors();
}

void Context::viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
	if(_logCalls) {
		_log->write(_logLevel, "glViewport(", x, ", ", y, ", ", width, ", ", height, ")");
	}
	_glViewport(x, y, width, height);
	checkGlErrors();
}


// GL_VERSION_1_1

void Context::drawArrays(GLenum mode, GLint first, GLsizei count) {
	if(_logCalls) {
		_log->write(_logLevel, "glDrawArrays(", getPrimitiveTypeName(mode), ", ", first, ", ", count, ")");
	}
	_glDrawArrays(mode, first, count);
	checkGlErrors();
}

void Context::drawElements(GLenum mode, GLsizei count, GLenum type, const void *indices) {
	if(_logCalls) {
		_log->write(_logLevel, "glDrawElements(", getPrimitiveTypeName(mode), ", ", count, ", ", getDrawElementsTypeName(type), ", ", indices, ")");
	}
	_glDrawElements(mode, count, type, indices);
	checkGlErrors();
}

void Context::polygonOffset(GLfloat factor, GLfloat units) {
	if(_logCalls) {
		_log->write(_logLevel, "glPolygonOffset(", factor, ", ", units, ")");
	}
	_glPolygonOffset(factor, units);
	checkGlErrors();
}

void Context::copyTexImage1D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border) {
	if(_logCalls) {
		_log->write(_logLevel, "glCopyTexImage1D(", getTextureTargetName(target), ", ", level, ", ", getInternalFormatName(internalformat), ", ", x, ", ", y, ", ", width, ", ", border, ")");
	}
	_glCopyTexImage1D(target, level, internalformat, x, y, width, border);
	checkGlErrors();
}

void Context::copyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) {
	if(_logCalls) {
		_log->write(_logLevel, "glCopyTexImage2D(", getTextureTargetName(target), ", ", level, ", ", getInternalFormatName(internalformat), ", ", x, ", ", y, ", ", width, ", ", height, ", ", border, ")");
	}
	_glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
	checkGlErrors();
}

void Context::copyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) {
	if(_logCalls) {
		_log->write(_logLevel, "glCopyTexSubImage1D(", getTextureTargetName(target), ", ", level, ", ", xoffset, ", ", x, ", ", y, ", ", width, ")");
	}
	_glCopyTexSubImage1D(target, level, xoffset, x, y, width);
	checkGlErrors();
}

void Context::copyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) {
	if(_logCalls) {
		_log->write(_logLevel, "glCopyTexSubImage2D(", getTextureTargetName(target), ", ", level, ", ", xoffset, ", ", yoffset, ", ", x, ", ", y, ", ", width, ", ", height, ")");
	}
	_glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
	checkGlErrors();
}

void Context::texSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexSubImage1D(", getTextureTargetName(target), ", ", level, ", ", xoffset, ", ", width, ", ", getPixelFormatName(format), ", ", getPixelTypeName(type), ", ", pixels, ")");
	}
	_glTexSubImage1D(target, level, xoffset, width, format, type, pixels);
	checkGlErrors();
}

void Context::texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexSubImage2D(", getTextureTargetName(target), ", ", level, ", ", xoffset, ", ", yoffset, ", ", width, ", ", height, ", ", getPixelFormatName(format), ", ", getPixelTypeName(type), ", ", pixels, ")");
	}
	_glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
	checkGlErrors();
}

void Context::bindTexture(GLenum target, GLuint texture) {
	if(_logCalls) {
		_log->write(_logLevel, "glBindTexture(", getTextureTargetName(target), ", ", texture, ")");
	}
	_glBindTexture(target, texture);
	checkGlErrors();
}

void Context::deleteTextures(GLsizei n, const GLuint *textures) {
	if(_logCalls) {
		_log->write(_logLevel, "glDeleteTextures(", n, ", ", textures, ")");
	}
	_glDeleteTextures(n, textures);
	checkGlErrors();
}

void Context::genTextures(GLsizei n, GLuint *textures) {
	if(_logCalls) {
		_log->write(_logLevel, "glGenTextures(", n, ", ", textures, ")");
	}
	_glGenTextures(n, textures);
	checkGlErrors();
}

GLboolean Context::isTexture(GLuint texture) {
	if(_logCalls) {
		_log->write(_logLevel, "glIsTexture(", texture, ")");
	}
	GLboolean result = _glIsTexture(texture);
	checkGlErrors();
	return result;
}


// GL_VERSION_1_2

void Context::drawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices) {
	if(_logCalls) {
		_log->write(_logLevel, "glDrawRangeElements(", getPrimitiveTypeName(mode), ", ", start, ", ", end, ", ", count, ", ", getDrawElementsTypeName(type), ", ", indices, ")");
	}
	_glDrawRangeElements(mode, start, end, count, type, indices);
	checkGlErrors();
}

void Context::texImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexImage3D(", getTextureTargetName(target), ", ", level, ", ", getInternalFormatName(internalformat), ", ", width, ", ", height, ", ", depth, ", ", border, ", ", getPixelFormatName(format), ", ", getPixelTypeName(type), ", ", pixels, ")");
	}
	_glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
	checkGlErrors();
}

void Context::texSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexSubImage3D(", getTextureTargetName(target), ", ", level, ", ", xoffset, ", ", yoffset, ", ", zoffset, ", ", width, ", ", height, ", ", depth, ", ", getPixelFormatName(format), ", ", getPixelTypeName(type), ", ", pixels, ")");
	}
	_glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
	checkGlErrors();
}

void Context::copyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) {
	if(_logCalls) {
		_log->write(_logLevel, "glCopyTexSubImage3D(", getTextureTargetName(target), ", ", level, ", ", xoffset, ", ", yoffset, ", ", zoffset, ", ", x, ", ", y, ", ", width, ", ", height, ")");
	}
	_glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);
	checkGlErrors();
}


// GL_VERSION_1_3

void Context::activeTexture(GLenum texture) {
	if(_logCalls) {
		_log->write(_logLevel, "glActiveTexture(", getTextureUnitName(texture), ")");
	}
	_glActiveTexture(texture);
	checkGlErrors();
}

void Context::sampleCoverage(GLfloat value, GLboolean invert) {
	if(_logCalls) {
		_log->write(_logLevel, "glSampleCoverage(", value, ", ", getBooleanName(invert), ")");
	}
	_glSampleCoverage(value, invert);
	checkGlErrors();
}

void Context::compressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glCompressedTexImage3D(", getTextureTargetName(target), ", ", level, ", ", getInternalFormatName(internalformat), ", ", width, ", ", height, ", ", depth, ", ", border, ", ", imageSize, ", ", data, ")");
	}
	_glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data);
	checkGlErrors();
}

void Context::compressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glCompressedTexImage2D(", getTextureTargetName(target), ", ", level, ", ", getInternalFormatName(internalformat), ", ", width, ", ", height, ", ", border, ", ", imageSize, ", ", data, ")");
	}
	_glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
	checkGlErrors();
}

void Context::compressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glCompressedTexImage1D(", getTextureTargetName(target), ", ", level, ", ", getInternalFormatName(internalformat), ", ", width, ", ", border, ", ", imageSize, ", ", data, ")");
	}
	_glCompressedTexImage1D(target, level, internalformat, width, border, imageSize, data);
	checkGlErrors();
}

void Context::compressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glCompressedTexSubImage3D(", getTextureTargetName(target), ", ", level, ", ", xoffset, ", ", yoffset, ", ", zoffset, ", ", width, ", ", height, ", ", depth, ", ", getPixelFormatName(format), ", ", imageSize, ", ", data, ")");
	}
	_glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
	checkGlErrors();
}

void Context::compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glCompressedTexSubImage2D(", getTextureTargetName(target), ", ", level, ", ", xoffset, ", ", yoffset, ", ", width, ", ", height, ", ", getPixelFormatName(format), ", ", imageSize, ", ", data, ")");
	}
	_glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
	checkGlErrors();
}

void Context::compressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glCompressedTexSubImage1D(", getTextureTargetName(target), ", ", level, ", ", xoffset, ", ", width, ", ", getPixelFormatName(format), ", ", imageSize, ", ", data, ")");
	}
	_glCompressedTexSubImage1D(target, level, xoffset, width, format, imageSize, data);
	checkGlErrors();
}

void Context::getCompressedTexImage(GLenum target, GLint level, void *img) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetCompressedTexImage(", getTextureTargetName(target), ", ", level, ", ", img, ")");
	}
	_glGetCompressedTexImage(target, level, img);
	checkGlErrors();
}


// GL_VERSION_1_4

void Context::blendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) {
	if(_logCalls) {
		_log->write(_logLevel, "glBlendFuncSeparate(", getBlendingFactorName(sfactorRGB), ", ", getBlendingFactorName(dfactorRGB), ", ", getBlendingFactorName(sfactorAlpha), ", ", getBlendingFactorName(dfactorAlpha), ")");
	}
	_glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
	checkGlErrors();
}

void Context::multiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount) {
	if(_logCalls) {
		_log->write(_logLevel, "glMultiDrawArrays(", getPrimitiveTypeName(mode), ", ", first, ", ", count, ", ", drawcount, ")");
	}
	_glMultiDrawArrays(mode, first, count, drawcount);
	checkGlErrors();
}

void Context::multiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount) {
	if(_logCalls) {
		_log->write(_logLevel, "glMultiDrawElements(", getPrimitiveTypeName(mode), ", ", count, ", ", getDrawElementsTypeName(type), ", ", indices, ", ", drawcount, ")");
	}
	_glMultiDrawElements(mode, count, type, indices, drawcount);
	checkGlErrors();
}

void Context::pointParameterf(GLenum pname, GLfloat param) {
	if(_logCalls) {
		_log->write(_logLevel, "glPointParameterf(", pname, ", ", param, ")");
	}
	_glPointParameterf(pname, param);
	checkGlErrors();
}

void Context::pointParameterfv(GLenum pname, const GLfloat *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glPointParameterfv(", pname, ", ", params, ")");
	}
	_glPointParameterfv(pname, params);
	checkGlErrors();
}

void Context::pointParameteri(GLenum pname, GLint param) {
	if(_logCalls) {
		_log->write(_logLevel, "glPointParameteri(", pname, ", ", param, ")");
	}
	_glPointParameteri(pname, param);
	checkGlErrors();
}

void Context::pointParameteriv(GLenum pname, const GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glPointParameteriv(", pname, ", ", params, ")");
	}
	_glPointParameteriv(pname, params);
	checkGlErrors();
}

void Context::blendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
	if(_logCalls) {
		_log->write(_logLevel, "glBlendColor(", red, ", ", green, ", ", blue, ", ", alpha, ")");
	}
	_glBlendColor(red, green, blue, alpha);
	checkGlErrors();
}

void Context::blendEquation(GLenum mode) {
	if(_logCalls) {
		_log->write(_logLevel, "glBlendEquation(", getBlendEquationModeEXTName(mode), ")");
	}
	_glBlendEquation(mode);
	checkGlErrors();
}


// GL_VERSION_1_5

void Context::genQueries(GLsizei n, GLuint *ids) {
	if(_logCalls) {
		_log->write(_logLevel, "glGenQueries(", n, ", ", ids, ")");
	}
	_glGenQueries(n, ids);
	checkGlErrors();
}

void Context::deleteQueries(GLsizei n, const GLuint *ids) {
	if(_logCalls) {
		_log->write(_logLevel, "glDeleteQueries(", n, ", ", ids, ")");
	}
	_glDeleteQueries(n, ids);
	checkGlErrors();
}

GLboolean Context::isQuery(GLuint id) {
	if(_logCalls) {
		_log->write(_logLevel, "glIsQuery(", id, ")");
	}
	GLboolean result = _glIsQuery(id);
	checkGlErrors();
	return result;
}

void Context::beginQuery(GLenum target, GLuint id) {
	if(_logCalls) {
		_log->write(_logLevel, "glBeginQuery(", getQueryTargetName(target), ", ", id, ")");
	}
	_glBeginQuery(target, id);
	checkGlErrors();
}

void Context::endQuery(GLenum target) {
	if(_logCalls) {
		_log->write(_logLevel, "glEndQuery(", getQueryTargetName(target), ")");
	}
	_glEndQuery(target);
	checkGlErrors();
}

void Context::getQueryiv(GLenum target, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetQueryiv(", getQueryTargetName(target), ", ", getQueryParameterNameName(pname), ", ", params, ")");
	}
	_glGetQueryiv(target, pname, params);
	checkGlErrors();
}

void Context::getQueryObjectiv(GLuint id, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetQueryObjectiv(", id, ", ", getQueryObjectParameterNameName(pname), ", ", params, ")");
	}
	_glGetQueryObjectiv(id, pname, params);
	checkGlErrors();
}

void Context::getQueryObjectuiv(GLuint id, GLenum pname, GLuint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetQueryObjectuiv(", id, ", ", getQueryObjectParameterNameName(pname), ", ", params, ")");
	}
	_glGetQueryObjectuiv(id, pname, params);
	checkGlErrors();
}

void Context::bindBuffer(GLenum target, GLuint buffer) {
	if(_logCalls) {
		_log->write(_logLevel, "glBindBuffer(", getBufferTargetARBName(target), ", ", buffer, ")");
	}
	_glBindBuffer(target, buffer);
	checkGlErrors();
}

void Context::deleteBuffers(GLsizei n, const GLuint *buffers) {
	if(_logCalls) {
		_log->write(_logLevel, "glDeleteBuffers(", n, ", ", buffers, ")");
	}
	_glDeleteBuffers(n, buffers);
	checkGlErrors();
}

void Context::genBuffers(GLsizei n, GLuint *buffers) {
	if(_logCalls) {
		_log->write(_logLevel, "glGenBuffers(", n, ", ", buffers, ")");
	}
	_glGenBuffers(n, buffers);
	checkGlErrors();
}

GLboolean Context::isBuffer(GLuint buffer) {
	if(_logCalls) {
		_log->write(_logLevel, "glIsBuffer(", buffer, ")");
	}
	GLboolean result = _glIsBuffer(buffer);
	checkGlErrors();
	return result;
}

void Context::bufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {
	if(_logCalls) {
		_log->write(_logLevel, "glBufferData(", getBufferTargetARBName(target), ", ", size, ", ", data, ", ", getBufferUsageARBName(usage), ")");
	}
	_glBufferData(target, size, data, usage);
	checkGlErrors();
}

void Context::bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glBufferSubData(", getBufferTargetARBName(target), ", ", offset, ", ", size, ", ", data, ")");
	}
	_glBufferSubData(target, offset, size, data);
	checkGlErrors();
}

void Context::getBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, void *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetBufferSubData(", getBufferTargetARBName(target), ", ", offset, ", ", size, ", ", data, ")");
	}
	_glGetBufferSubData(target, offset, size, data);
	checkGlErrors();
}

void * Context::mapBuffer(GLenum target, GLenum access) {
	if(_logCalls) {
		_log->write(_logLevel, "glMapBuffer(", getBufferTargetARBName(target), ", ", getBufferAccessARBName(access), ")");
	}
	void * result = _glMapBuffer(target, access);
	checkGlErrors();
	return result;
}

GLboolean Context::unmapBuffer(GLenum target) {
	if(_logCalls) {
		_log->write(_logLevel, "glUnmapBuffer(", getBufferTargetARBName(target), ")");
	}
	GLboolean result = _glUnmapBuffer(target);
	checkGlErrors();
	return result;
}

void Context::getBufferParameteriv(GLenum target, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetBufferParameteriv(", getBufferTargetARBName(target), ", ", pname, ", ", params, ")");
	}
	_glGetBufferParameteriv(target, pname, params);
	checkGlErrors();
}

void Context::getBufferPointerv(GLenum target, GLenum pname, void **params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetBufferPointerv(", getBufferTargetARBName(target), ", ", pname, ", ", params, ")");
	}
	_glGetBufferPointerv(target, pname, params);
	checkGlErrors();
}


// GL_VERSION_2_0

void Context::blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) {
	if(_logCalls) {
		_log->write(_logLevel, "glBlendEquationSeparate(", getBlendEquationModeEXTName(modeRGB), ", ", getBlendEquationModeEXTName(modeAlpha), ")");
	}
	_glBlendEquationSeparate(modeRGB, modeAlpha);
	checkGlErrors();
}

void Context::drawBuffers(GLsizei n, const GLenum *bufs) {
	if(_logCalls) {
		_log->write(_logLevel, "glDrawBuffers(", n, ", ", bufs, ")");
	}
	_glDrawBuffers(n, bufs);
	checkGlErrors();
}

void Context::stencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass) {
	if(_logCalls) {
		_log->write(_logLevel, "glStencilOpSeparate(", getStencilFaceDirectionName(face), ", ", getStencilOpName(sfail), ", ", getStencilOpName(dpfail), ", ", getStencilOpName(dppass), ")");
	}
	_glStencilOpSeparate(face, sfail, dpfail, dppass);
	checkGlErrors();
}

void Context::stencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask) {
	if(_logCalls) {
		_log->write(_logLevel, "glStencilFuncSeparate(", getStencilFaceDirectionName(face), ", ", getStencilFunctionName(func), ", ", ref, ", ", mask, ")");
	}
	_glStencilFuncSeparate(face, func, ref, mask);
	checkGlErrors();
}

void Context::stencilMaskSeparate(GLenum face, GLuint mask) {
	if(_logCalls) {
		_log->write(_logLevel, "glStencilMaskSeparate(", getStencilFaceDirectionName(face), ", ", mask, ")");
	}
	_glStencilMaskSeparate(face, mask);
	checkGlErrors();
}

void Context::attachShader(GLuint program, GLuint shader) {
	if(_logCalls) {
		_log->write(_logLevel, "glAttachShader(", program, ", ", shader, ")");
	}
	_glAttachShader(program, shader);
	checkGlErrors();
}

void Context::bindAttribLocation(GLuint program, GLuint index, const GLchar *name) {
	if(_logCalls) {
		_log->write(_logLevel, "glBindAttribLocation(", program, ", ", index, ", ", name, ")");
	}
	_glBindAttribLocation(program, index, name);
	checkGlErrors();
}

void Context::compileShader(GLuint shader) {
	if(_logCalls) {
		_log->write(_logLevel, "glCompileShader(", shader, ")");
	}
	_glCompileShader(shader);
	checkGlErrors();
}

GLuint Context::createProgram() {
	if(_logCalls) {
		_log->write(_logLevel, "glCreateProgram()");
	}
	GLuint result = _glCreateProgram();
	checkGlErrors();
	return result;
}

GLuint Context::createShader(GLenum type) {
	if(_logCalls) {
		_log->write(_logLevel, "glCreateShader(", getShaderTypeName(type), ")");
	}
	GLuint result = _glCreateShader(type);
	checkGlErrors();
	return result;
}

void Context::deleteProgram(GLuint program) {
	if(_logCalls) {
		_log->write(_logLevel, "glDeleteProgram(", program, ")");
	}
	_glDeleteProgram(program);
	checkGlErrors();
}

void Context::deleteShader(GLuint shader) {
	if(_logCalls) {
		_log->write(_logLevel, "glDeleteShader(", shader, ")");
	}
	_glDeleteShader(shader);
	checkGlErrors();
}

void Context::detachShader(GLuint program, GLuint shader) {
	if(_logCalls) {
		_log->write(_logLevel, "glDetachShader(", program, ", ", shader, ")");
	}
	_glDetachShader(program, shader);
	checkGlErrors();
}

void Context::disableVertexAttribArray(GLuint index) {
	if(_logCalls) {
		_log->write(_logLevel, "glDisableVertexAttribArray(", index, ")");
	}
	_glDisableVertexAttribArray(index);
	checkGlErrors();
}

void Context::enableVertexAttribArray(GLuint index) {
	if(_logCalls) {
		_log->write(_logLevel, "glEnableVertexAttribArray(", index, ")");
	}
	_glEnableVertexAttribArray(index);
	checkGlErrors();
}

void Context::getActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetActiveAttrib(", program, ", ", index, ", ", bufSize, ", ", length, ", ", size, ", ", type, ", ", name, ")");
	}
	_glGetActiveAttrib(program, index, bufSize, length, size, type, name);
	checkGlErrors();
}

void Context::getActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetActiveUniform(", program, ", ", index, ", ", bufSize, ", ", length, ", ", size, ", ", type, ", ", name, ")");
	}
	_glGetActiveUniform(program, index, bufSize, length, size, type, name);
	checkGlErrors();
}

void Context::getAttachedShaders(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetAttachedShaders(", program, ", ", maxCount, ", ", count, ", ", shaders, ")");
	}
	_glGetAttachedShaders(program, maxCount, count, shaders);
	checkGlErrors();
}

GLint Context::getAttribLocation(GLuint program, const GLchar *name) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetAttribLocation(", program, ", ", name, ")");
	}
	GLint result = _glGetAttribLocation(program, name);
	checkGlErrors();
	return result;
}

void Context::getProgramiv(GLuint program, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetProgramiv(", program, ", ", getProgramPropertyARBName(pname), ", ", params, ")");
	}
	_glGetProgramiv(program, pname, params);
	checkGlErrors();
}

void Context::getProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetProgramInfoLog(", program, ", ", bufSize, ", ", length, ", ", infoLog, ")");
	}
	_glGetProgramInfoLog(program, bufSize, length, infoLog);
	checkGlErrors();
}

void Context::getShaderiv(GLuint shader, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetShaderiv(", shader, ", ", getShaderParameterNameName(pname), ", ", params, ")");
	}
	_glGetShaderiv(shader, pname, params);
	checkGlErrors();
}

void Context::getShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetShaderInfoLog(", shader, ", ", bufSize, ", ", length, ", ", infoLog, ")");
	}
	_glGetShaderInfoLog(shader, bufSize, length, infoLog);
	checkGlErrors();
}

void Context::getShaderSource(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetShaderSource(", shader, ", ", bufSize, ", ", length, ", ", source, ")");
	}
	_glGetShaderSource(shader, bufSize, length, source);
	checkGlErrors();
}

GLint Context::getUniformLocation(GLuint program, const GLchar *name) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetUniformLocation(", program, ", ", name, ")");
	}
	GLint result = _glGetUniformLocation(program, name);
	checkGlErrors();
	return result;
}

void Context::getUniformfv(GLuint program, GLint location, GLfloat *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetUniformfv(", program, ", ", location, ", ", params, ")");
	}
	_glGetUniformfv(program, location, params);
	checkGlErrors();
}

void Context::getUniformiv(GLuint program, GLint location, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetUniformiv(", program, ", ", location, ", ", params, ")");
	}
	_glGetUniformiv(program, location, params);
	checkGlErrors();
}

void Context::getVertexAttribdv(GLuint index, GLenum pname, GLdouble *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetVertexAttribdv(", index, ", ", pname, ", ", params, ")");
	}
	_glGetVertexAttribdv(index, pname, params);
	checkGlErrors();
}

void Context::getVertexAttribfv(GLuint index, GLenum pname, GLfloat *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetVertexAttribfv(", index, ", ", pname, ", ", params, ")");
	}
	_glGetVertexAttribfv(index, pname, params);
	checkGlErrors();
}

void Context::getVertexAttribiv(GLuint index, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetVertexAttribiv(", index, ", ", pname, ", ", params, ")");
	}
	_glGetVertexAttribiv(index, pname, params);
	checkGlErrors();
}

void Context::getVertexAttribPointerv(GLuint index, GLenum pname, void **pointer) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetVertexAttribPointerv(", index, ", ", pname, ", ", pointer, ")");
	}
	_glGetVertexAttribPointerv(index, pname, pointer);
	checkGlErrors();
}

GLboolean Context::isProgram(GLuint program) {
	if(_logCalls) {
		_log->write(_logLevel, "glIsProgram(", program, ")");
	}
	GLboolean result = _glIsProgram(program);
	checkGlErrors();
	return result;
}

GLboolean Context::isShader(GLuint shader) {
	if(_logCalls) {
		_log->write(_logLevel, "glIsShader(", shader, ")");
	}
	GLboolean result = _glIsShader(shader);
	checkGlErrors();
	return result;
}

void Context::linkProgram(GLuint program) {
	if(_logCalls) {
		_log->write(_logLevel, "glLinkProgram(", program, ")");
	}
	_glLinkProgram(program);
	checkGlErrors();
}

void Context::shaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length) {
	if(_logCalls) {
		_log->write(_logLevel, "glShaderSource(", shader, ", ", count, ", ", string, ", ", length, ")");
	}
	_glShaderSource(shader, count, string, length);
	checkGlErrors();
}

void Context::useProgram(GLuint program) {
	if(_logCalls) {
		_log->write(_logLevel, "glUseProgram(", program, ")");
	}
	_glUseProgram(program);
	checkGlErrors();
}

void Context::uniform1f(GLint location, GLfloat v0) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform1f(", location, ", ", v0, ")");
	}
	_glUniform1f(location, v0);
	checkGlErrors();
}

void Context::uniform2f(GLint location, GLfloat v0, GLfloat v1) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform2f(", location, ", ", v0, ", ", v1, ")");
	}
	_glUniform2f(location, v0, v1);
	checkGlErrors();
}

void Context::uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform3f(", location, ", ", v0, ", ", v1, ", ", v2, ")");
	}
	_glUniform3f(location, v0, v1, v2);
	checkGlErrors();
}

void Context::uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform4f(", location, ", ", v0, ", ", v1, ", ", v2, ", ", v3, ")");
	}
	_glUniform4f(location, v0, v1, v2, v3);
	checkGlErrors();
}

void Context::uniform1i(GLint location, GLint v0) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform1i(", location, ", ", v0, ")");
	}
	_glUniform1i(location, v0);
	checkGlErrors();
}

void Context::uniform2i(GLint location, GLint v0, GLint v1) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform2i(", location, ", ", v0, ", ", v1, ")");
	}
	_glUniform2i(location, v0, v1);
	checkGlErrors();
}

void Context::uniform3i(GLint location, GLint v0, GLint v1, GLint v2) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform3i(", location, ", ", v0, ", ", v1, ", ", v2, ")");
	}
	_glUniform3i(location, v0, v1, v2);
	checkGlErrors();
}

void Context::uniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform4i(", location, ", ", v0, ", ", v1, ", ", v2, ", ", v3, ")");
	}
	_glUniform4i(location, v0, v1, v2, v3);
	checkGlErrors();
}

void Context::uniform1fv(GLint location, GLsizei count, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform1fv(", location, ", ", count, ", ", value, ")");
	}
	_glUniform1fv(location, count, value);
	checkGlErrors();
}

void Context::uniform2fv(GLint location, GLsizei count, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform2fv(", location, ", ", count, ", ", value, ")");
	}
	_glUniform2fv(location, count, value);
	checkGlErrors();
}

void Context::uniform3fv(GLint location, GLsizei count, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform3fv(", location, ", ", count, ", ", value, ")");
	}
	_glUniform3fv(location, count, value);
	checkGlErrors();
}

void Context::uniform4fv(GLint location, GLsizei count, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform4fv(", location, ", ", count, ", ", value, ")");
	}
	_glUniform4fv(location, count, value);
	checkGlErrors();
}

void Context::uniform1iv(GLint location, GLsizei count, const GLint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform1iv(", location, ", ", count, ", ", value, ")");
	}
	_glUniform1iv(location, count, value);
	checkGlErrors();
}

void Context::uniform2iv(GLint location, GLsizei count, const GLint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform2iv(", location, ", ", count, ", ", value, ")");
	}
	_glUniform2iv(location, count, value);
	checkGlErrors();
}

void Context::uniform3iv(GLint location, GLsizei count, const GLint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform3iv(", location, ", ", count, ", ", value, ")");
	}
	_glUniform3iv(location, count, value);
	checkGlErrors();
}

void Context::uniform4iv(GLint location, GLsizei count, const GLint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform4iv(", location, ", ", count, ", ", value, ")");
	}
	_glUniform4iv(location, count, value);
	checkGlErrors();
}

void Context::uniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniformMatrix2fv(", location, ", ", count, ", ", getBooleanName(transpose), ", ", value, ")");
	}
	_glUniformMatrix2fv(location, count, transpose, value);
	checkGlErrors();
}

void Context::uniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniformMatrix3fv(", location, ", ", count, ", ", getBooleanName(transpose), ", ", value, ")");
	}
	_glUniformMatrix3fv(location, count, transpose, value);
	checkGlErrors();
}

void Context::uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniformMatrix4fv(", location, ", ", count, ", ", getBooleanName(transpose), ", ", value, ")");
	}
	_glUniformMatrix4fv(location, count, transpose, value);
	checkGlErrors();
}

void Context::validateProgram(GLuint program) {
	if(_logCalls) {
		_log->write(_logLevel, "glValidateProgram(", program, ")");
	}
	_glValidateProgram(program);
	checkGlErrors();
}

void Context::vertexAttrib1d(GLuint index, GLdouble x) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib1d(", index, ", ", x, ")");
	}
	_glVertexAttrib1d(index, x);
	checkGlErrors();
}

void Context::vertexAttrib1dv(GLuint index, const GLdouble *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib1dv(", index, ", ", v, ")");
	}
	_glVertexAttrib1dv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib1f(GLuint index, GLfloat x) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib1f(", index, ", ", x, ")");
	}
	_glVertexAttrib1f(index, x);
	checkGlErrors();
}

void Context::vertexAttrib1fv(GLuint index, const GLfloat *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib1fv(", index, ", ", v, ")");
	}
	_glVertexAttrib1fv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib1s(GLuint index, GLshort x) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib1s(", index, ", ", x, ")");
	}
	_glVertexAttrib1s(index, x);
	checkGlErrors();
}

void Context::vertexAttrib1sv(GLuint index, const GLshort *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib1sv(", index, ", ", v, ")");
	}
	_glVertexAttrib1sv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib2d(GLuint index, GLdouble x, GLdouble y) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib2d(", index, ", ", x, ", ", y, ")");
	}
	_glVertexAttrib2d(index, x, y);
	checkGlErrors();
}

void Context::vertexAttrib2dv(GLuint index, const GLdouble *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib2dv(", index, ", ", v, ")");
	}
	_glVertexAttrib2dv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib2f(GLuint index, GLfloat x, GLfloat y) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib2f(", index, ", ", x, ", ", y, ")");
	}
	_glVertexAttrib2f(index, x, y);
	checkGlErrors();
}

void Context::vertexAttrib2fv(GLuint index, const GLfloat *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib2fv(", index, ", ", v, ")");
	}
	_glVertexAttrib2fv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib2s(GLuint index, GLshort x, GLshort y) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib2s(", index, ", ", x, ", ", y, ")");
	}
	_glVertexAttrib2s(index, x, y);
	checkGlErrors();
}

void Context::vertexAttrib2sv(GLuint index, const GLshort *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib2sv(", index, ", ", v, ")");
	}
	_glVertexAttrib2sv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib3d(GLuint index, GLdouble x, GLdouble y, GLdouble z) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib3d(", index, ", ", x, ", ", y, ", ", z, ")");
	}
	_glVertexAttrib3d(index, x, y, z);
	checkGlErrors();
}

void Context::vertexAttrib3dv(GLuint index, const GLdouble *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib3dv(", index, ", ", v, ")");
	}
	_glVertexAttrib3dv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib3f(", index, ", ", x, ", ", y, ", ", z, ")");
	}
	_glVertexAttrib3f(index, x, y, z);
	checkGlErrors();
}

void Context::vertexAttrib3fv(GLuint index, const GLfloat *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib3fv(", index, ", ", v, ")");
	}
	_glVertexAttrib3fv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib3s(GLuint index, GLshort x, GLshort y, GLshort z) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib3s(", index, ", ", x, ", ", y, ", ", z, ")");
	}
	_glVertexAttrib3s(index, x, y, z);
	checkGlErrors();
}

void Context::vertexAttrib3sv(GLuint index, const GLshort *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib3sv(", index, ", ", v, ")");
	}
	_glVertexAttrib3sv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4Nbv(GLuint index, const GLbyte *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4Nbv(", index, ", ", v, ")");
	}
	_glVertexAttrib4Nbv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4Niv(GLuint index, const GLint *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4Niv(", index, ", ", v, ")");
	}
	_glVertexAttrib4Niv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4Nsv(GLuint index, const GLshort *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4Nsv(", index, ", ", v, ")");
	}
	_glVertexAttrib4Nsv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4Nub(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4Nub(", index, ", ", x, ", ", y, ", ", z, ", ", w, ")");
	}
	_glVertexAttrib4Nub(index, x, y, z, w);
	checkGlErrors();
}

void Context::vertexAttrib4Nubv(GLuint index, const GLubyte *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4Nubv(", index, ", ", v, ")");
	}
	_glVertexAttrib4Nubv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4Nuiv(GLuint index, const GLuint *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4Nuiv(", index, ", ", v, ")");
	}
	_glVertexAttrib4Nuiv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4Nusv(GLuint index, const GLushort *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4Nusv(", index, ", ", v, ")");
	}
	_glVertexAttrib4Nusv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4bv(GLuint index, const GLbyte *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4bv(", index, ", ", v, ")");
	}
	_glVertexAttrib4bv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4d(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4d(", index, ", ", x, ", ", y, ", ", z, ", ", w, ")");
	}
	_glVertexAttrib4d(index, x, y, z, w);
	checkGlErrors();
}

void Context::vertexAttrib4dv(GLuint index, const GLdouble *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4dv(", index, ", ", v, ")");
	}
	_glVertexAttrib4dv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4f(", index, ", ", x, ", ", y, ", ", z, ", ", w, ")");
	}
	_glVertexAttrib4f(index, x, y, z, w);
	checkGlErrors();
}

void Context::vertexAttrib4fv(GLuint index, const GLfloat *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4fv(", index, ", ", v, ")");
	}
	_glVertexAttrib4fv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4iv(GLuint index, const GLint *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4iv(", index, ", ", v, ")");
	}
	_glVertexAttrib4iv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4s(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4s(", index, ", ", x, ", ", y, ", ", z, ", ", w, ")");
	}
	_glVertexAttrib4s(index, x, y, z, w);
	checkGlErrors();
}

void Context::vertexAttrib4sv(GLuint index, const GLshort *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4sv(", index, ", ", v, ")");
	}
	_glVertexAttrib4sv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4ubv(GLuint index, const GLubyte *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4ubv(", index, ", ", v, ")");
	}
	_glVertexAttrib4ubv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4uiv(GLuint index, const GLuint *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4uiv(", index, ", ", v, ")");
	}
	_glVertexAttrib4uiv(index, v);
	checkGlErrors();
}

void Context::vertexAttrib4usv(GLuint index, const GLushort *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttrib4usv(", index, ", ", v, ")");
	}
	_glVertexAttrib4usv(index, v);
	checkGlErrors();
}

void Context::vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribPointer(", index, ", ", size, ", ", getVertexAttribPointerTypeName(type), ", ", getBooleanName(normalized), ", ", stride, ", ", pointer, ")");
	}
	_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	checkGlErrors();
}


// GL_VERSION_2_1

void Context::uniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniformMatrix2x3fv(", location, ", ", count, ", ", getBooleanName(transpose), ", ", value, ")");
	}
	_glUniformMatrix2x3fv(location, count, transpose, value);
	checkGlErrors();
}

void Context::uniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniformMatrix3x2fv(", location, ", ", count, ", ", getBooleanName(transpose), ", ", value, ")");
	}
	_glUniformMatrix3x2fv(location, count, transpose, value);
	checkGlErrors();
}

void Context::uniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniformMatrix2x4fv(", location, ", ", count, ", ", getBooleanName(transpose), ", ", value, ")");
	}
	_glUniformMatrix2x4fv(location, count, transpose, value);
	checkGlErrors();
}

void Context::uniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniformMatrix4x2fv(", location, ", ", count, ", ", getBooleanName(transpose), ", ", value, ")");
	}
	_glUniformMatrix4x2fv(location, count, transpose, value);
	checkGlErrors();
}

void Context::uniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniformMatrix3x4fv(", location, ", ", count, ", ", getBooleanName(transpose), ", ", value, ")");
	}
	_glUniformMatrix3x4fv(location, count, transpose, value);
	checkGlErrors();
}

void Context::uniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniformMatrix4x3fv(", location, ", ", count, ", ", getBooleanName(transpose), ", ", value, ")");
	}
	_glUniformMatrix4x3fv(location, count, transpose, value);
	checkGlErrors();
}


// GL_VERSION_3_0

void Context::colorMaski(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a) {
	if(_logCalls) {
		_log->write(_logLevel, "glColorMaski(", index, ", ", getBooleanName(r), ", ", getBooleanName(g), ", ", getBooleanName(b), ", ", getBooleanName(a), ")");
	}
	_glColorMaski(index, r, g, b, a);
	checkGlErrors();
}

void Context::getBooleani_v(GLenum target, GLuint index, GLboolean *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetBooleani_v(", getBufferTargetARBName(target), ", ", index, ", ", data, ")");
	}
	_glGetBooleani_v(target, index, data);
	checkGlErrors();
}

void Context::getIntegeri_v(GLenum target, GLuint index, GLint *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetIntegeri_v(", getTypeEnumName(target), ", ", index, ", ", data, ")");
	}
	_glGetIntegeri_v(target, index, data);
	checkGlErrors();
}

void Context::enablei(GLenum target, GLuint index) {
	if(_logCalls) {
		_log->write(_logLevel, "glEnablei(", getEnableCapName(target), ", ", index, ")");
	}
	_glEnablei(target, index);
	checkGlErrors();
}

void Context::disablei(GLenum target, GLuint index) {
	if(_logCalls) {
		_log->write(_logLevel, "glDisablei(", getEnableCapName(target), ", ", index, ")");
	}
	_glDisablei(target, index);
	checkGlErrors();
}

GLboolean Context::isEnabledi(GLenum target, GLuint index) {
	if(_logCalls) {
		_log->write(_logLevel, "glIsEnabledi(", getEnableCapName(target), ", ", index, ")");
	}
	GLboolean result = _glIsEnabledi(target, index);
	checkGlErrors();
	return result;
}

void Context::beginTransformFeedback(GLenum primitiveMode) {
	if(_logCalls) {
		_log->write(_logLevel, "glBeginTransformFeedback(", getPrimitiveTypeName(primitiveMode), ")");
	}
	_glBeginTransformFeedback(primitiveMode);
	checkGlErrors();
}

void Context::endTransformFeedback() {
	if(_logCalls) {
		_log->write(_logLevel, "glEndTransformFeedback()");
	}
	_glEndTransformFeedback();
	checkGlErrors();
}

void Context::bindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) {
	if(_logCalls) {
		_log->write(_logLevel, "glBindBufferRange(", getBufferTargetARBName(target), ", ", index, ", ", buffer, ", ", offset, ", ", size, ")");
	}
	_glBindBufferRange(target, index, buffer, offset, size);
	checkGlErrors();
}

void Context::bindBufferBase(GLenum target, GLuint index, GLuint buffer) {
	if(_logCalls) {
		_log->write(_logLevel, "glBindBufferBase(", getBufferTargetARBName(target), ", ", index, ", ", buffer, ")");
	}
	_glBindBufferBase(target, index, buffer);
	checkGlErrors();
}

void Context::transformFeedbackVaryings(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode) {
	if(_logCalls) {
		_log->write(_logLevel, "glTransformFeedbackVaryings(", program, ", ", count, ", ", varyings, ", ", bufferMode, ")");
	}
	_glTransformFeedbackVaryings(program, count, varyings, bufferMode);
	checkGlErrors();
}

void Context::getTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetTransformFeedbackVarying(", program, ", ", index, ", ", bufSize, ", ", length, ", ", size, ", ", type, ", ", name, ")");
	}
	_glGetTransformFeedbackVarying(program, index, bufSize, length, size, type, name);
	checkGlErrors();
}

void Context::clampColor(GLenum target, GLenum clamp) {
	if(_logCalls) {
		_log->write(_logLevel, "glClampColor(", target, ", ", clamp, ")");
	}
	_glClampColor(target, clamp);
	checkGlErrors();
}

void Context::beginConditionalRender(GLuint id, GLenum mode) {
	if(_logCalls) {
		_log->write(_logLevel, "glBeginConditionalRender(", id, ", ", getTypeEnumName(mode), ")");
	}
	_glBeginConditionalRender(id, mode);
	checkGlErrors();
}

void Context::endConditionalRender() {
	if(_logCalls) {
		_log->write(_logLevel, "glEndConditionalRender()");
	}
	_glEndConditionalRender();
	checkGlErrors();
}

void Context::vertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribIPointer(", index, ", ", size, ", ", getVertexAttribPointerTypeName(type), ", ", stride, ", ", pointer, ")");
	}
	_glVertexAttribIPointer(index, size, type, stride, pointer);
	checkGlErrors();
}

void Context::getVertexAttribIiv(GLuint index, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetVertexAttribIiv(", index, ", ", getVertexAttribEnumName(pname), ", ", params, ")");
	}
	_glGetVertexAttribIiv(index, pname, params);
	checkGlErrors();
}

void Context::getVertexAttribIuiv(GLuint index, GLenum pname, GLuint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetVertexAttribIuiv(", index, ", ", getVertexAttribEnumName(pname), ", ", params, ")");
	}
	_glGetVertexAttribIuiv(index, pname, params);
	checkGlErrors();
}

void Context::vertexAttribI1i(GLuint index, GLint x) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI1i(", index, ", ", x, ")");
	}
	_glVertexAttribI1i(index, x);
	checkGlErrors();
}

void Context::vertexAttribI2i(GLuint index, GLint x, GLint y) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI2i(", index, ", ", x, ", ", y, ")");
	}
	_glVertexAttribI2i(index, x, y);
	checkGlErrors();
}

void Context::vertexAttribI3i(GLuint index, GLint x, GLint y, GLint z) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI3i(", index, ", ", x, ", ", y, ", ", z, ")");
	}
	_glVertexAttribI3i(index, x, y, z);
	checkGlErrors();
}

void Context::vertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI4i(", index, ", ", x, ", ", y, ", ", z, ", ", w, ")");
	}
	_glVertexAttribI4i(index, x, y, z, w);
	checkGlErrors();
}

void Context::vertexAttribI1ui(GLuint index, GLuint x) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI1ui(", index, ", ", x, ")");
	}
	_glVertexAttribI1ui(index, x);
	checkGlErrors();
}

void Context::vertexAttribI2ui(GLuint index, GLuint x, GLuint y) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI2ui(", index, ", ", x, ", ", y, ")");
	}
	_glVertexAttribI2ui(index, x, y);
	checkGlErrors();
}

void Context::vertexAttribI3ui(GLuint index, GLuint x, GLuint y, GLuint z) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI3ui(", index, ", ", x, ", ", y, ", ", z, ")");
	}
	_glVertexAttribI3ui(index, x, y, z);
	checkGlErrors();
}

void Context::vertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI4ui(", index, ", ", x, ", ", y, ", ", z, ", ", w, ")");
	}
	_glVertexAttribI4ui(index, x, y, z, w);
	checkGlErrors();
}

void Context::vertexAttribI1iv(GLuint index, const GLint *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI1iv(", index, ", ", v, ")");
	}
	_glVertexAttribI1iv(index, v);
	checkGlErrors();
}

void Context::vertexAttribI2iv(GLuint index, const GLint *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI2iv(", index, ", ", v, ")");
	}
	_glVertexAttribI2iv(index, v);
	checkGlErrors();
}

void Context::vertexAttribI3iv(GLuint index, const GLint *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI3iv(", index, ", ", v, ")");
	}
	_glVertexAttribI3iv(index, v);
	checkGlErrors();
}

void Context::vertexAttribI4iv(GLuint index, const GLint *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI4iv(", index, ", ", v, ")");
	}
	_glVertexAttribI4iv(index, v);
	checkGlErrors();
}

void Context::vertexAttribI1uiv(GLuint index, const GLuint *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI1uiv(", index, ", ", v, ")");
	}
	_glVertexAttribI1uiv(index, v);
	checkGlErrors();
}

void Context::vertexAttribI2uiv(GLuint index, const GLuint *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI2uiv(", index, ", ", v, ")");
	}
	_glVertexAttribI2uiv(index, v);
	checkGlErrors();
}

void Context::vertexAttribI3uiv(GLuint index, const GLuint *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI3uiv(", index, ", ", v, ")");
	}
	_glVertexAttribI3uiv(index, v);
	checkGlErrors();
}

void Context::vertexAttribI4uiv(GLuint index, const GLuint *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI4uiv(", index, ", ", v, ")");
	}
	_glVertexAttribI4uiv(index, v);
	checkGlErrors();
}

void Context::vertexAttribI4bv(GLuint index, const GLbyte *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI4bv(", index, ", ", v, ")");
	}
	_glVertexAttribI4bv(index, v);
	checkGlErrors();
}

void Context::vertexAttribI4sv(GLuint index, const GLshort *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI4sv(", index, ", ", v, ")");
	}
	_glVertexAttribI4sv(index, v);
	checkGlErrors();
}

void Context::vertexAttribI4ubv(GLuint index, const GLubyte *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI4ubv(", index, ", ", v, ")");
	}
	_glVertexAttribI4ubv(index, v);
	checkGlErrors();
}

void Context::vertexAttribI4usv(GLuint index, const GLushort *v) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribI4usv(", index, ", ", v, ")");
	}
	_glVertexAttribI4usv(index, v);
	checkGlErrors();
}

void Context::getUniformuiv(GLuint program, GLint location, GLuint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetUniformuiv(", program, ", ", location, ", ", params, ")");
	}
	_glGetUniformuiv(program, location, params);
	checkGlErrors();
}

void Context::bindFragDataLocation(GLuint program, GLuint color, const GLchar *name) {
	if(_logCalls) {
		_log->write(_logLevel, "glBindFragDataLocation(", program, ", ", color, ", ", name, ")");
	}
	_glBindFragDataLocation(program, color, name);
	checkGlErrors();
}

GLint Context::getFragDataLocation(GLuint program, const GLchar *name) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetFragDataLocation(", program, ", ", name, ")");
	}
	GLint result = _glGetFragDataLocation(program, name);
	checkGlErrors();
	return result;
}

void Context::uniform1ui(GLint location, GLuint v0) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform1ui(", location, ", ", v0, ")");
	}
	_glUniform1ui(location, v0);
	checkGlErrors();
}

void Context::uniform2ui(GLint location, GLuint v0, GLuint v1) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform2ui(", location, ", ", v0, ", ", v1, ")");
	}
	_glUniform2ui(location, v0, v1);
	checkGlErrors();
}

void Context::uniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform3ui(", location, ", ", v0, ", ", v1, ", ", v2, ")");
	}
	_glUniform3ui(location, v0, v1, v2);
	checkGlErrors();
}

void Context::uniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform4ui(", location, ", ", v0, ", ", v1, ", ", v2, ", ", v3, ")");
	}
	_glUniform4ui(location, v0, v1, v2, v3);
	checkGlErrors();
}

void Context::uniform1uiv(GLint location, GLsizei count, const GLuint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform1uiv(", location, ", ", count, ", ", value, ")");
	}
	_glUniform1uiv(location, count, value);
	checkGlErrors();
}

void Context::uniform2uiv(GLint location, GLsizei count, const GLuint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform2uiv(", location, ", ", count, ", ", value, ")");
	}
	_glUniform2uiv(location, count, value);
	checkGlErrors();
}

void Context::uniform3uiv(GLint location, GLsizei count, const GLuint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform3uiv(", location, ", ", count, ", ", value, ")");
	}
	_glUniform3uiv(location, count, value);
	checkGlErrors();
}

void Context::uniform4uiv(GLint location, GLsizei count, const GLuint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniform4uiv(", location, ", ", count, ", ", value, ")");
	}
	_glUniform4uiv(location, count, value);
	checkGlErrors();
}

void Context::texParameterIiv(GLenum target, GLenum pname, const GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexParameterIiv(", getTextureTargetName(target), ", ", getTextureParameterNameName(pname), ", ", params, ")");
	}
	_glTexParameterIiv(target, pname, params);
	checkGlErrors();
}

void Context::texParameterIuiv(GLenum target, GLenum pname, const GLuint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexParameterIuiv(", getTextureTargetName(target), ", ", getTextureParameterNameName(pname), ", ", params, ")");
	}
	_glTexParameterIuiv(target, pname, params);
	checkGlErrors();
}

void Context::getTexParameterIiv(GLenum target, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetTexParameterIiv(", getTextureTargetName(target), ", ", getGetTextureParameterName(pname), ", ", params, ")");
	}
	_glGetTexParameterIiv(target, pname, params);
	checkGlErrors();
}

void Context::getTexParameterIuiv(GLenum target, GLenum pname, GLuint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetTexParameterIuiv(", getTextureTargetName(target), ", ", getGetTextureParameterName(pname), ", ", params, ")");
	}
	_glGetTexParameterIuiv(target, pname, params);
	checkGlErrors();
}

void Context::clearBufferiv(GLenum buffer, GLint drawbuffer, const GLint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glClearBufferiv(", getBufferName(buffer), ", ", drawbuffer, ", ", value, ")");
	}
	_glClearBufferiv(buffer, drawbuffer, value);
	checkGlErrors();
}

void Context::clearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glClearBufferuiv(", getBufferName(buffer), ", ", drawbuffer, ", ", value, ")");
	}
	_glClearBufferuiv(buffer, drawbuffer, value);
	checkGlErrors();
}

void Context::clearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glClearBufferfv(", getBufferName(buffer), ", ", drawbuffer, ", ", value, ")");
	}
	_glClearBufferfv(buffer, drawbuffer, value);
	checkGlErrors();
}

void Context::clearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) {
	if(_logCalls) {
		_log->write(_logLevel, "glClearBufferfi(", getBufferName(buffer), ", ", drawbuffer, ", ", depth, ", ", stencil, ")");
	}
	_glClearBufferfi(buffer, drawbuffer, depth, stencil);
	checkGlErrors();
}

const GLubyte * Context::getStringi(GLenum name, GLuint index) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetStringi(", getStringNameName(name), ", ", index, ")");
	}
	const GLubyte * result = _glGetStringi(name, index);
	checkGlErrors();
	return result;
}

GLboolean Context::isRenderbuffer(GLuint renderbuffer) {
	if(_logCalls) {
		_log->write(_logLevel, "glIsRenderbuffer(", renderbuffer, ")");
	}
	GLboolean result = _glIsRenderbuffer(renderbuffer);
	checkGlErrors();
	return result;
}

void Context::bindRenderbuffer(GLenum target, GLuint renderbuffer) {
	if(_logCalls) {
		_log->write(_logLevel, "glBindRenderbuffer(", getRenderbufferTargetName(target), ", ", renderbuffer, ")");
	}
	_glBindRenderbuffer(target, renderbuffer);
	checkGlErrors();
}

void Context::deleteRenderbuffers(GLsizei n, const GLuint *renderbuffers) {
	if(_logCalls) {
		_log->write(_logLevel, "glDeleteRenderbuffers(", n, ", ", renderbuffers, ")");
	}
	_glDeleteRenderbuffers(n, renderbuffers);
	checkGlErrors();
}

void Context::genRenderbuffers(GLsizei n, GLuint *renderbuffers) {
	if(_logCalls) {
		_log->write(_logLevel, "glGenRenderbuffers(", n, ", ", renderbuffers, ")");
	}
	_glGenRenderbuffers(n, renderbuffers);
	checkGlErrors();
}

void Context::renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
	if(_logCalls) {
		_log->write(_logLevel, "glRenderbufferStorage(", getRenderbufferTargetName(target), ", ", getInternalFormatName(internalformat), ", ", width, ", ", height, ")");
	}
	_glRenderbufferStorage(target, internalformat, width, height);
	checkGlErrors();
}

void Context::getRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetRenderbufferParameteriv(", getRenderbufferTargetName(target), ", ", getRenderbufferParameterNameName(pname), ", ", params, ")");
	}
	_glGetRenderbufferParameteriv(target, pname, params);
	checkGlErrors();
}

GLboolean Context::isFramebuffer(GLuint framebuffer) {
	if(_logCalls) {
		_log->write(_logLevel, "glIsFramebuffer(", framebuffer, ")");
	}
	GLboolean result = _glIsFramebuffer(framebuffer);
	checkGlErrors();
	return result;
}

void Context::bindFramebuffer(GLenum target, GLuint framebuffer) {
	if(_logCalls) {
		_log->write(_logLevel, "glBindFramebuffer(", getFramebufferTargetName(target), ", ", framebuffer, ")");
	}
	_glBindFramebuffer(target, framebuffer);
	checkGlErrors();
}

void Context::deleteFramebuffers(GLsizei n, const GLuint *framebuffers) {
	if(_logCalls) {
		_log->write(_logLevel, "glDeleteFramebuffers(", n, ", ", framebuffers, ")");
	}
	_glDeleteFramebuffers(n, framebuffers);
	checkGlErrors();
}

void Context::genFramebuffers(GLsizei n, GLuint *framebuffers) {
	if(_logCalls) {
		_log->write(_logLevel, "glGenFramebuffers(", n, ", ", framebuffers, ")");
	}
	_glGenFramebuffers(n, framebuffers);
	checkGlErrors();
}

GLenum Context::checkFramebufferStatus(GLenum target) {
	if(_logCalls) {
		_log->write(_logLevel, "glCheckFramebufferStatus(", getFramebufferTargetName(target), ")");
	}
	GLenum result = _glCheckFramebufferStatus(target);
	checkGlErrors();
	return result;
}

void Context::framebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
	if(_logCalls) {
		_log->write(_logLevel, "glFramebufferTexture1D(", getFramebufferTargetName(target), ", ", getFramebufferAttachmentName(attachment), ", ", getTextureTargetName(textarget), ", ", texture, ", ", level, ")");
	}
	_glFramebufferTexture1D(target, attachment, textarget, texture, level);
	checkGlErrors();
}

void Context::framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
	if(_logCalls) {
		_log->write(_logLevel, "glFramebufferTexture2D(", getFramebufferTargetName(target), ", ", getFramebufferAttachmentName(attachment), ", ", getTextureTargetName(textarget), ", ", texture, ", ", level, ")");
	}
	_glFramebufferTexture2D(target, attachment, textarget, texture, level);
	checkGlErrors();
}

void Context::framebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset) {
	if(_logCalls) {
		_log->write(_logLevel, "glFramebufferTexture3D(", getFramebufferTargetName(target), ", ", getFramebufferAttachmentName(attachment), ", ", getTextureTargetName(textarget), ", ", texture, ", ", level, ", ", zoffset, ")");
	}
	_glFramebufferTexture3D(target, attachment, textarget, texture, level, zoffset);
	checkGlErrors();
}

void Context::framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
	if(_logCalls) {
		_log->write(_logLevel, "glFramebufferRenderbuffer(", getFramebufferTargetName(target), ", ", getFramebufferAttachmentName(attachment), ", ", getRenderbufferTargetName(renderbuffertarget), ", ", renderbuffer, ")");
	}
	_glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
	checkGlErrors();
}

void Context::getFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetFramebufferAttachmentParameteriv(", getFramebufferTargetName(target), ", ", getFramebufferAttachmentName(attachment), ", ", getFramebufferAttachmentParameterNameName(pname), ", ", params, ")");
	}
	_glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
	checkGlErrors();
}

void Context::generateMipmap(GLenum target) {
	if(_logCalls) {
		_log->write(_logLevel, "glGenerateMipmap(", getTextureTargetName(target), ")");
	}
	_glGenerateMipmap(target);
	checkGlErrors();
}

void Context::blitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) {
	if(_logCalls) {
		_log->write(_logLevel, "glBlitFramebuffer(", srcX0, ", ", srcY0, ", ", srcX1, ", ", srcY1, ", ", dstX0, ", ", dstY0, ", ", dstX1, ", ", dstY1, ", ", bitsetClearBufferMaskAsString(mask), ", ", getBlitFramebufferFilterName(filter), ")");
	}
	_glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
	checkGlErrors();
}

void Context::renderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) {
	if(_logCalls) {
		_log->write(_logLevel, "glRenderbufferStorageMultisample(", getRenderbufferTargetName(target), ", ", samples, ", ", getInternalFormatName(internalformat), ", ", width, ", ", height, ")");
	}
	_glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
	checkGlErrors();
}

void Context::framebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer) {
	if(_logCalls) {
		_log->write(_logLevel, "glFramebufferTextureLayer(", getFramebufferTargetName(target), ", ", getFramebufferAttachmentName(attachment), ", ", texture, ", ", level, ", ", layer, ")");
	}
	_glFramebufferTextureLayer(target, attachment, texture, level, layer);
	checkGlErrors();
}

void * Context::mapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) {
	if(_logCalls) {
		_log->write(_logLevel, "glMapBufferRange(", getBufferTargetARBName(target), ", ", offset, ", ", length, ", ", bitsetBufferAccessMaskAsString(access), ")");
	}
	void * result = _glMapBufferRange(target, offset, length, access);
	checkGlErrors();
	return result;
}

void Context::flushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length) {
	if(_logCalls) {
		_log->write(_logLevel, "glFlushMappedBufferRange(", getBufferTargetARBName(target), ", ", offset, ", ", length, ")");
	}
	_glFlushMappedBufferRange(target, offset, length);
	checkGlErrors();
}

void Context::bindVertexArray(GLuint array) {
	if(_logCalls) {
		_log->write(_logLevel, "glBindVertexArray(", array, ")");
	}
	_glBindVertexArray(array);
	checkGlErrors();
}

void Context::deleteVertexArrays(GLsizei n, const GLuint *arrays) {
	if(_logCalls) {
		_log->write(_logLevel, "glDeleteVertexArrays(", n, ", ", arrays, ")");
	}
	_glDeleteVertexArrays(n, arrays);
	checkGlErrors();
}

void Context::genVertexArrays(GLsizei n, GLuint *arrays) {
	if(_logCalls) {
		_log->write(_logLevel, "glGenVertexArrays(", n, ", ", arrays, ")");
	}
	_glGenVertexArrays(n, arrays);
	checkGlErrors();
}

GLboolean Context::isVertexArray(GLuint array) {
	if(_logCalls) {
		_log->write(_logLevel, "glIsVertexArray(", array, ")");
	}
	GLboolean result = _glIsVertexArray(array);
	checkGlErrors();
	return result;
}


// GL_VERSION_3_1

void Context::drawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount) {
	if(_logCalls) {
		_log->write(_logLevel, "glDrawArraysInstanced(", getPrimitiveTypeName(mode), ", ", first, ", ", count, ", ", instancecount, ")");
	}
	_glDrawArraysInstanced(mode, first, count, instancecount);
	checkGlErrors();
}

void Context::drawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount) {
	if(_logCalls) {
		_log->write(_logLevel, "glDrawElementsInstanced(", getPrimitiveTypeName(mode), ", ", count, ", ", getDrawElementsTypeName(type), ", ", indices, ", ", instancecount, ")");
	}
	_glDrawElementsInstanced(mode, count, type, indices, instancecount);
	checkGlErrors();
}

void Context::texBuffer(GLenum target, GLenum internalformat, GLuint buffer) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexBuffer(", getTextureTargetName(target), ", ", getInternalFormatName(internalformat), ", ", buffer, ")");
	}
	_glTexBuffer(target, internalformat, buffer);
	checkGlErrors();
}

void Context::primitiveRestartIndex(GLuint index) {
	if(_logCalls) {
		_log->write(_logLevel, "glPrimitiveRestartIndex(", index, ")");
	}
	_glPrimitiveRestartIndex(index);
	checkGlErrors();
}

void Context::copyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
	if(_logCalls) {
		_log->write(_logLevel, "glCopyBufferSubData(", getCopyBufferSubDataTargetName(readTarget), ", ", getCopyBufferSubDataTargetName(writeTarget), ", ", readOffset, ", ", writeOffset, ", ", size, ")");
	}
	_glCopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size);
	checkGlErrors();
}

void Context::getUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetUniformIndices(", program, ", ", uniformCount, ", ", uniformNames, ", ", uniformIndices, ")");
	}
	_glGetUniformIndices(program, uniformCount, uniformNames, uniformIndices);
	checkGlErrors();
}

void Context::getActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetActiveUniformsiv(", program, ", ", uniformCount, ", ", uniformIndices, ", ", getUniformPNameName(pname), ", ", params, ")");
	}
	_glGetActiveUniformsiv(program, uniformCount, uniformIndices, pname, params);
	checkGlErrors();
}

void Context::getActiveUniformName(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetActiveUniformName(", program, ", ", uniformIndex, ", ", bufSize, ", ", length, ", ", uniformName, ")");
	}
	_glGetActiveUniformName(program, uniformIndex, bufSize, length, uniformName);
	checkGlErrors();
}

GLuint Context::getUniformBlockIndex(GLuint program, const GLchar *uniformBlockName) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetUniformBlockIndex(", program, ", ", uniformBlockName, ")");
	}
	GLuint result = _glGetUniformBlockIndex(program, uniformBlockName);
	checkGlErrors();
	return result;
}

void Context::getActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetActiveUniformBlockiv(", program, ", ", uniformBlockIndex, ", ", getUniformBlockPNameName(pname), ", ", params, ")");
	}
	_glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params);
	checkGlErrors();
}

void Context::getActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetActiveUniformBlockName(", program, ", ", uniformBlockIndex, ", ", bufSize, ", ", length, ", ", uniformBlockName, ")");
	}
	_glGetActiveUniformBlockName(program, uniformBlockIndex, bufSize, length, uniformBlockName);
	checkGlErrors();
}

void Context::uniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding) {
	if(_logCalls) {
		_log->write(_logLevel, "glUniformBlockBinding(", program, ", ", uniformBlockIndex, ", ", uniformBlockBinding, ")");
	}
	_glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
	checkGlErrors();
}


// GL_VERSION_3_2

void Context::drawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex) {
	if(_logCalls) {
		_log->write(_logLevel, "glDrawElementsBaseVertex(", getPrimitiveTypeName(mode), ", ", count, ", ", getDrawElementsTypeName(type), ", ", indices, ", ", basevertex, ")");
	}
	_glDrawElementsBaseVertex(mode, count, type, indices, basevertex);
	checkGlErrors();
}

void Context::drawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex) {
	if(_logCalls) {
		_log->write(_logLevel, "glDrawRangeElementsBaseVertex(", getPrimitiveTypeName(mode), ", ", start, ", ", end, ", ", count, ", ", getDrawElementsTypeName(type), ", ", indices, ", ", basevertex, ")");
	}
	_glDrawRangeElementsBaseVertex(mode, start, end, count, type, indices, basevertex);
	checkGlErrors();
}

void Context::drawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex) {
	if(_logCalls) {
		_log->write(_logLevel, "glDrawElementsInstancedBaseVertex(", getPrimitiveTypeName(mode), ", ", count, ", ", getDrawElementsTypeName(type), ", ", indices, ", ", instancecount, ", ", basevertex, ")");
	}
	_glDrawElementsInstancedBaseVertex(mode, count, type, indices, instancecount, basevertex);
	checkGlErrors();
}

void Context::multiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount, const GLint *basevertex) {
	if(_logCalls) {
		_log->write(_logLevel, "glMultiDrawElementsBaseVertex(", getPrimitiveTypeName(mode), ", ", count, ", ", getDrawElementsTypeName(type), ", ", indices, ", ", drawcount, ", ", basevertex, ")");
	}
	_glMultiDrawElementsBaseVertex(mode, count, type, indices, drawcount, basevertex);
	checkGlErrors();
}

void Context::provokingVertex(GLenum mode) {
	if(_logCalls) {
		_log->write(_logLevel, "glProvokingVertex(", getVertexProvokingModeName(mode), ")");
	}
	_glProvokingVertex(mode);
	checkGlErrors();
}

GLsync Context::fenceSync(GLenum condition, GLbitfield flags) {
	if(_logCalls) {
		_log->write(_logLevel, "glFenceSync(", getSyncConditionName(condition), ", ", bitsetAsString(flags), ")");
	}
	GLsync result = _glFenceSync(condition, flags);
	checkGlErrors();
	return result;
}

GLboolean Context::isSync(GLsync sync) {
	if(_logCalls) {
		_log->write(_logLevel, "glIsSync(", sync, ")");
	}
	GLboolean result = _glIsSync(sync);
	checkGlErrors();
	return result;
}

void Context::deleteSync(GLsync sync) {
	if(_logCalls) {
		_log->write(_logLevel, "glDeleteSync(", sync, ")");
	}
	_glDeleteSync(sync);
	checkGlErrors();
}

GLenum Context::clientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout) {
	if(_logCalls) {
		_log->write(_logLevel, "glClientWaitSync(", sync, ", ", bitsetSyncObjectMaskAsString(flags), ", ", timeout, ")");
	}
	GLenum result = _glClientWaitSync(sync, flags, timeout);
	checkGlErrors();
	return result;
}

void Context::waitSync(GLsync sync, GLbitfield flags, GLuint64 timeout) {
	if(_logCalls) {
		_log->write(_logLevel, "glWaitSync(", sync, ", ", bitsetAsString(flags), ", ", timeout, ")");
	}
	_glWaitSync(sync, flags, timeout);
	checkGlErrors();
}

void Context::getInteger64v(GLenum pname, GLint64 *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetInteger64v(", getGetPNameName(pname), ", ", data, ")");
	}
	_glGetInteger64v(pname, data);
	checkGlErrors();
}

void Context::getSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetSynciv(", sync, ", ", getSyncParameterNameName(pname), ", ", bufSize, ", ", length, ", ", values, ")");
	}
	_glGetSynciv(sync, pname, bufSize, length, values);
	checkGlErrors();
}

void Context::getInteger64i_v(GLenum target, GLuint index, GLint64 *data) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetInteger64i_v(", getTypeEnumName(target), ", ", index, ", ", data, ")");
	}
	_glGetInteger64i_v(target, index, data);
	checkGlErrors();
}

void Context::getBufferParameteri64v(GLenum target, GLenum pname, GLint64 *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetBufferParameteri64v(", getBufferTargetARBName(target), ", ", pname, ", ", params, ")");
	}
	_glGetBufferParameteri64v(target, pname, params);
	checkGlErrors();
}

void Context::framebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level) {
	if(_logCalls) {
		_log->write(_logLevel, "glFramebufferTexture(", getFramebufferTargetName(target), ", ", getFramebufferAttachmentName(attachment), ", ", texture, ", ", level, ")");
	}
	_glFramebufferTexture(target, attachment, texture, level);
	checkGlErrors();
}

void Context::texImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexImage2DMultisample(", getTextureTargetName(target), ", ", samples, ", ", getInternalFormatName(internalformat), ", ", width, ", ", height, ", ", getBooleanName(fixedsamplelocations), ")");
	}
	_glTexImage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations);
	checkGlErrors();
}

void Context::texImage3DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexImage3DMultisample(", getTextureTargetName(target), ", ", samples, ", ", getInternalFormatName(internalformat), ", ", width, ", ", height, ", ", depth, ", ", getBooleanName(fixedsamplelocations), ")");
	}
	_glTexImage3DMultisample(target, samples, internalformat, width, height, depth, fixedsamplelocations);
	checkGlErrors();
}

void Context::getMultisamplefv(GLenum pname, GLuint index, GLfloat *val) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetMultisamplefv(", pname, ", ", index, ", ", val, ")");
	}
	_glGetMultisamplefv(pname, index, val);
	checkGlErrors();
}

void Context::sampleMaski(GLuint maskNumber, GLbitfield mask) {
	if(_logCalls) {
		_log->write(_logLevel, "glSampleMaski(", maskNumber, ", ", bitsetAsString(mask), ")");
	}
	_glSampleMaski(maskNumber, mask);
	checkGlErrors();
}


// GL_VERSION_3_3

void Context::bindFragDataLocationIndexed(GLuint program, GLuint colorNumber, GLuint index, const GLchar *name) {
	if(_logCalls) {
		_log->write(_logLevel, "glBindFragDataLocationIndexed(", program, ", ", colorNumber, ", ", index, ", ", name, ")");
	}
	_glBindFragDataLocationIndexed(program, colorNumber, index, name);
	checkGlErrors();
}

GLint Context::getFragDataIndex(GLuint program, const GLchar *name) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetFragDataIndex(", program, ", ", name, ")");
	}
	GLint result = _glGetFragDataIndex(program, name);
	checkGlErrors();
	return result;
}

void Context::genSamplers(GLsizei count, GLuint *samplers) {
	if(_logCalls) {
		_log->write(_logLevel, "glGenSamplers(", count, ", ", samplers, ")");
	}
	_glGenSamplers(count, samplers);
	checkGlErrors();
}

void Context::deleteSamplers(GLsizei count, const GLuint *samplers) {
	if(_logCalls) {
		_log->write(_logLevel, "glDeleteSamplers(", count, ", ", samplers, ")");
	}
	_glDeleteSamplers(count, samplers);
	checkGlErrors();
}

GLboolean Context::isSampler(GLuint sampler) {
	if(_logCalls) {
		_log->write(_logLevel, "glIsSampler(", sampler, ")");
	}
	GLboolean result = _glIsSampler(sampler);
	checkGlErrors();
	return result;
}

void Context::bindSampler(GLuint unit, GLuint sampler) {
	if(_logCalls) {
		_log->write(_logLevel, "glBindSampler(", unit, ", ", sampler, ")");
	}
	_glBindSampler(unit, sampler);
	checkGlErrors();
}

void Context::samplerParameteri(GLuint sampler, GLenum pname, GLint param) {
	if(_logCalls) {
		_log->write(_logLevel, "glSamplerParameteri(", sampler, ", ", getSamplerParameterNameName(pname), ", ", param, ")");
	}
	_glSamplerParameteri(sampler, pname, param);
	checkGlErrors();
}

void Context::samplerParameteriv(GLuint sampler, GLenum pname, const GLint *param) {
	if(_logCalls) {
		_log->write(_logLevel, "glSamplerParameteriv(", sampler, ", ", getSamplerParameterNameName(pname), ", ", param, ")");
	}
	_glSamplerParameteriv(sampler, pname, param);
	checkGlErrors();
}

void Context::samplerParameterf(GLuint sampler, GLenum pname, GLfloat param) {
	if(_logCalls) {
		_log->write(_logLevel, "glSamplerParameterf(", sampler, ", ", getSamplerParameterNameName(pname), ", ", param, ")");
	}
	_glSamplerParameterf(sampler, pname, param);
	checkGlErrors();
}

void Context::samplerParameterfv(GLuint sampler, GLenum pname, const GLfloat *param) {
	if(_logCalls) {
		_log->write(_logLevel, "glSamplerParameterfv(", sampler, ", ", getSamplerParameterNameName(pname), ", ", param, ")");
	}
	_glSamplerParameterfv(sampler, pname, param);
	checkGlErrors();
}

void Context::samplerParameterIiv(GLuint sampler, GLenum pname, const GLint *param) {
	if(_logCalls) {
		_log->write(_logLevel, "glSamplerParameterIiv(", sampler, ", ", getSamplerParameterNameName(pname), ", ", param, ")");
	}
	_glSamplerParameterIiv(sampler, pname, param);
	checkGlErrors();
}

void Context::samplerParameterIuiv(GLuint sampler, GLenum pname, const GLuint *param) {
	if(_logCalls) {
		_log->write(_logLevel, "glSamplerParameterIuiv(", sampler, ", ", getSamplerParameterNameName(pname), ", ", param, ")");
	}
	_glSamplerParameterIuiv(sampler, pname, param);
	checkGlErrors();
}

void Context::getSamplerParameteriv(GLuint sampler, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetSamplerParameteriv(", sampler, ", ", getSamplerParameterNameName(pname), ", ", params, ")");
	}
	_glGetSamplerParameteriv(sampler, pname, params);
	checkGlErrors();
}

void Context::getSamplerParameterIiv(GLuint sampler, GLenum pname, GLint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetSamplerParameterIiv(", sampler, ", ", getSamplerParameterNameName(pname), ", ", params, ")");
	}
	_glGetSamplerParameterIiv(sampler, pname, params);
	checkGlErrors();
}

void Context::getSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetSamplerParameterfv(", sampler, ", ", getSamplerParameterNameName(pname), ", ", params, ")");
	}
	_glGetSamplerParameterfv(sampler, pname, params);
	checkGlErrors();
}

void Context::getSamplerParameterIuiv(GLuint sampler, GLenum pname, GLuint *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetSamplerParameterIuiv(", sampler, ", ", getSamplerParameterNameName(pname), ", ", params, ")");
	}
	_glGetSamplerParameterIuiv(sampler, pname, params);
	checkGlErrors();
}

void Context::queryCounter(GLuint id, GLenum target) {
	if(_logCalls) {
		_log->write(_logLevel, "glQueryCounter(", id, ", ", getQueryTargetName(target), ")");
	}
	_glQueryCounter(id, target);
	checkGlErrors();
}

void Context::getQueryObjecti64v(GLuint id, GLenum pname, GLint64 *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetQueryObjecti64v(", id, ", ", getQueryObjectParameterNameName(pname), ", ", params, ")");
	}
	_glGetQueryObjecti64v(id, pname, params);
	checkGlErrors();
}

void Context::getQueryObjectui64v(GLuint id, GLenum pname, GLuint64 *params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetQueryObjectui64v(", id, ", ", getQueryObjectParameterNameName(pname), ", ", params, ")");
	}
	_glGetQueryObjectui64v(id, pname, params);
	checkGlErrors();
}

void Context::vertexAttribDivisor(GLuint index, GLuint divisor) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribDivisor(", index, ", ", divisor, ")");
	}
	_glVertexAttribDivisor(index, divisor);
	checkGlErrors();
}

void Context::vertexAttribP1ui(GLuint index, GLenum type, GLboolean normalized, GLuint value) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribP1ui(", index, ", ", getVertexAttribPointerTypeName(type), ", ", getBooleanName(normalized), ", ", value, ")");
	}
	_glVertexAttribP1ui(index, type, normalized, value);
	checkGlErrors();
}

void Context::vertexAttribP1uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribP1uiv(", index, ", ", getVertexAttribPointerTypeName(type), ", ", getBooleanName(normalized), ", ", value, ")");
	}
	_glVertexAttribP1uiv(index, type, normalized, value);
	checkGlErrors();
}

void Context::vertexAttribP2ui(GLuint index, GLenum type, GLboolean normalized, GLuint value) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribP2ui(", index, ", ", getVertexAttribPointerTypeName(type), ", ", getBooleanName(normalized), ", ", value, ")");
	}
	_glVertexAttribP2ui(index, type, normalized, value);
	checkGlErrors();
}

void Context::vertexAttribP2uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribP2uiv(", index, ", ", getVertexAttribPointerTypeName(type), ", ", getBooleanName(normalized), ", ", value, ")");
	}
	_glVertexAttribP2uiv(index, type, normalized, value);
	checkGlErrors();
}

void Context::vertexAttribP3ui(GLuint index, GLenum type, GLboolean normalized, GLuint value) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribP3ui(", index, ", ", getVertexAttribPointerTypeName(type), ", ", getBooleanName(normalized), ", ", value, ")");
	}
	_glVertexAttribP3ui(index, type, normalized, value);
	checkGlErrors();
}

void Context::vertexAttribP3uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribP3uiv(", index, ", ", getVertexAttribPointerTypeName(type), ", ", getBooleanName(normalized), ", ", value, ")");
	}
	_glVertexAttribP3uiv(index, type, normalized, value);
	checkGlErrors();
}

void Context::vertexAttribP4ui(GLuint index, GLenum type, GLboolean normalized, GLuint value) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribP4ui(", index, ", ", getVertexAttribPointerTypeName(type), ", ", getBooleanName(normalized), ", ", value, ")");
	}
	_glVertexAttribP4ui(index, type, normalized, value);
	checkGlErrors();
}

void Context::vertexAttribP4uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint *value) {
	if(_logCalls) {
		_log->write(_logLevel, "glVertexAttribP4uiv(", index, ", ", getVertexAttribPointerTypeName(type), ", ", getBooleanName(normalized), ", ", value, ")");
	}
	_glVertexAttribP4uiv(index, type, normalized, value);
	checkGlErrors();
}


// GL_KHR_debug

void Context::debugMessageControl(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled) {
	if(_logCalls) {
		_log->write(_logLevel, "glDebugMessageControl(", getDebugSourceName(source), ", ", getDebugTypeName(type), ", ", getDebugSeverityName(severity), ", ", count, ", ", ids, ", ", getBooleanName(enabled), ")");
	}
	_glDebugMessageControl(source, type, severity, count, ids, enabled);
	checkGlErrors();
}

void Context::debugMessageInsert(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf) {
	if(_logCalls) {
		_log->write(_logLevel, "glDebugMessageInsert(", getDebugSourceName(source), ", ", getDebugTypeName(type), ", ", id, ", ", getDebugSeverityName(severity), ", ", length, ", ", buf, ")");
	}
	_glDebugMessageInsert(source, type, id, severity, length, buf);
	checkGlErrors();
}

void Context::debugMessageCallback(GLDEBUGPROC callback, const void *userParam) {
	if(_logCalls) {
		_log->write(_logLevel, "glDebugMessageCallback(", callback, ", ", userParam, ")");
	}
	_glDebugMessageCallback(callback, userParam);
	checkGlErrors();
}

GLuint Context::getDebugMessageLog(GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetDebugMessageLog(", count, ", ", bufSize, ", ", sources, ", ", types, ", ", ids, ", ", severities, ", ", lengths, ", ", messageLog, ")");
	}
	GLuint result = _glGetDebugMessageLog(count, bufSize, sources, types, ids, severities, lengths, messageLog);
	checkGlErrors();
	return result;
}

void Context::pushDebugGroup(GLenum source, GLuint id, GLsizei length, const GLchar *message) {
	if(_logCalls) {
		_log->write(_logLevel, "glPushDebugGroup(", getDebugSourceName(source), ", ", id, ", ", length, ", ", message, ")");
	}
	_glPushDebugGroup(source, id, length, message);
	checkGlErrors();
}

void Context::popDebugGroup() {
	if(_logCalls) {
		_log->write(_logLevel, "glPopDebugGroup()");
	}
	_glPopDebugGroup();
	checkGlErrors();
}

void Context::objectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar *label) {
	if(_logCalls) {
		_log->write(_logLevel, "glObjectLabel(", getObjectIdentifierName(identifier), ", ", name, ", ", length, ", ", label, ")");
	}
	_glObjectLabel(identifier, name, length, label);
	checkGlErrors();
}

void Context::getObjectLabel(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetObjectLabel(", identifier, ", ", name, ", ", bufSize, ", ", length, ", ", label, ")");
	}
	_glGetObjectLabel(identifier, name, bufSize, length, label);
	checkGlErrors();
}

void Context::objectPtrLabel(const void *ptr, GLsizei length, const GLchar *label) {
	if(_logCalls) {
		_log->write(_logLevel, "glObjectPtrLabel(", ptr, ", ", length, ", ", label, ")");
	}
	_glObjectPtrLabel(ptr, length, label);
	checkGlErrors();
}

void Context::getObjectPtrLabel(const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetObjectPtrLabel(", ptr, ", ", bufSize, ", ", length, ", ", label, ")");
	}
	_glGetObjectPtrLabel(ptr, bufSize, length, label);
	checkGlErrors();
}

void Context::getPointerv(GLenum pname, void **params) {
	if(_logCalls) {
		_log->write(_logLevel, "glGetPointerv(", getGetPointervPNameName(pname), ", ", params, ")");
	}
	_glGetPointerv(pname, params);
	checkGlErrors();
}


// GL_ARB_texture_storage

void Context::texStorage1D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexStorage1D(", getTextureTargetName(target), ", ", levels, ", ", getInternalFormatName(internalformat), ", ", width, ")");
	}
	_glTexStorage1D(target, levels, internalformat, width);
	checkGlErrors();
}

void Context::texStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexStorage2D(", getTextureTargetName(target), ", ", levels, ", ", getInternalFormatName(internalformat), ", ", width, ", ", height, ")");
	}
	_glTexStorage2D(target, levels, internalformat, width, height);
	checkGlErrors();
}

void Context::texStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) {
	if(_logCalls) {
		_log->write(_logLevel, "glTexStorage3D(", getTextureTargetName(target), ", ", levels, ", ", getInternalFormatName(internalformat), ", ", width, ", ", height, ", ", depth, ")");
	}
	_glTexStorage3D(target, levels, internalformat, width, height, depth);
	checkGlErrors();
}


void* Context::_getProcAddress(const char* proc) {
	void* ptr = _glGetProcAddress(proc);
	if(ptr) ++_procCount;
	return ptr;
}

}
