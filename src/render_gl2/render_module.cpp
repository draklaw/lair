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


#include <GL/glew.h>

#include <lair/core/lair.h>
#include <lair/core/log.h>

#include <lair/render_gl2/renderer.h>

#include "lair/render_gl2/render_module.h"


namespace lair
{


RenderModule::RenderModule(SysModule* sys, MasterLogger* logger, LogLevel level)
    : _log("render_gl2", logger, level),
      _initialized(false),
      _sys(sys),
      _renderers() {
	lairAssert(sys);
}


RenderModule::~RenderModule() {
}


bool RenderModule::initialize() {
	lairAssert(!_initialized);
	log().log("Render module initialization...");

	GLenum err = glewInit();
	if(err != GLEW_OK) {
		log().error("GLEW initialization failed: ", glewGetErrorString(err), ".");
		return false;
	}

	if(!GL_VERSION_2_1) {
		log().error("OpenGL 2.1 is not supported.");
		return false;
	}

	log().info("OpenGL version: ",      glGetString(GL_VERSION));
	log().info("OpenGL GLSL version: ", glGetString(GL_SHADING_LANGUAGE_VERSION));
	log().info("OpenGL vendor: ",       glGetString(GL_VENDOR));
	log().info("OpenGL renderer: ",     glGetString(GL_RENDERER));

	_initialized = true;

	return true;
}


void RenderModule::shutdown() {
	lairAssert(_initialized);

	log().log("Render module shutdown...");

	_initialized = false;
}


Renderer* RenderModule::createRenderer() {
	_renderers.emplace_back(new Renderer(this));
	return _renderers.back().get();
}

}
