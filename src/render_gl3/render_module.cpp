/*
 *  Copyright (C) 2015 Simon Boyé
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


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include <lair/sys_sdl2/sys_module.h>

#include <lair/render_gl3/renderer.h>

#include "lair/render_gl3/render_module.h"


namespace lair
{


void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
                              GLenum severity, GLsizei /*length*/,
                              const char* message, const void* userParam) {
	RenderModule* rm = (RenderModule*)userParam;

	LogLevel level = LogLevel::Debug;
	switch(severity) {
	case gl::DEBUG_SEVERITY_HIGH:         level = LogLevel::Error; break;
	case gl::DEBUG_SEVERITY_MEDIUM:       level = LogLevel::Warning; break;
	case gl::DEBUG_SEVERITY_LOW:          level = LogLevel::Log; break;
	case gl::DEBUG_SEVERITY_NOTIFICATION: level = LogLevel::Info; break;
	}

	rm->log().write(level, "OpenGL: ", message,
	                " (", rm->context()->getEnumName(source),
	                ", ", rm->context()->getEnumName(type),
	                ", ", id,
	                ", ", rm->context()->getEnumName(severity), ")");
}


RenderModule::RenderModule(SysModule* sys, AssetManager* assetManager,
                           MasterLogger* logger, LogLevel level)
    : _log("render_gl3", logger, level),
      _initialized(false),
      _sys(sys),
      _assetManager(assetManager),
      _context(sys->glGetProcAddressFunction(), &_log),
      _renderers() {
	lairAssert(sys);
	lairAssert(_assetManager);
}


RenderModule::~RenderModule() {
}


bool RenderModule::initialize(bool debugGl) {
	lairAssert(!_initialized);
	log().log("Render module initialization...");

	if(!_context.initialize(debugGl)) {
		log().error("Failed to initialize OpenGL context. Aborting.");
		return false;
	}

	if(_context._gl_khr_debug) {
		_context.debugMessageCallback(debugCallback, (const void*)this);

		_context.debugMessageControl(
		            gl::DONT_CARE, gl::DONT_CARE, gl::DEBUG_SEVERITY_LOW,
		            0, nullptr, debugGl);
		_context.debugMessageControl(
		            gl::DONT_CARE, gl::DONT_CARE, gl::DEBUG_SEVERITY_NOTIFICATION,
		            0, nullptr, debugGl);

		_context.enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);

		const char* msg = "GL_ARB_debug_output enabled and working";
		_context.debugMessageInsert(gl::DEBUG_SOURCE_APPLICATION,
		                            gl::DEBUG_TYPE_OTHER, 0,
		                            gl::DEBUG_SEVERITY_MEDIUM,
		                            strlen(msg), msg);

#ifndef NDEBUG
		// Disable error reporting using glGetError as it is redundant and less
		// detailed.
		_context.setLogErrors(false);
#endif
	}

	_context.enable(gl::DEPTH_TEST);
	_context.depthFunc(gl::LEQUAL);

	_context.enable(gl::FRAMEBUFFER_SRGB);

	_initialized = true;

	return true;
}


void RenderModule::shutdown() {
	lairAssert(_initialized);

	log().log("Render module shutdown...");

	if(_context._gl_khr_debug) {
		_context.debugMessageCallback(nullptr, nullptr);
	}

	_context.shutdown();

	_initialized = false;
}


Renderer* RenderModule::createRenderer() {
	_renderers.emplace_back(new Renderer(this, _assetManager));
	return _renderers.back().get();
}

}
