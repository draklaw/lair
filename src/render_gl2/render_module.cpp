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

#include <lair/render_gl2/renderer.h>

#include "lair/render_gl2/render_module.h"


namespace lair
{


RenderModule::RenderModule(SysModule* sys, AssetManager* assetManager,
                           MasterLogger* logger, LogLevel level)
    : _log("render_gl2", logger, level),
      _initialized(false),
      _sys(sys),
      _assetManager(assetManager),
      _context(&_log),
      _renderers() {
	lairAssert(sys);
	lairAssert(_assetManager);
}


RenderModule::~RenderModule() {
}


bool RenderModule::initialize() {
	lairAssert(!_initialized);
	log().log("Render module initialization...");

	if(!_context.initialize()) {
		return false;
	}

	_initialized = true;

	return true;
}


void RenderModule::shutdown() {
	lairAssert(_initialized);

	log().log("Render module shutdown...");

	_initialized = false;
}


Renderer* RenderModule::createRenderer() {
	_renderers.emplace_back(new Renderer(this, _assetManager));
	return _renderers.back().get();
}

}
