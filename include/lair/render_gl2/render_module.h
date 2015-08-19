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


#ifndef _LAIR_RENDER_GL2_RENDER_MODULE_H
#define _LAIR_RENDER_GL2_RENDER_MODULE_H


#include <memory>
#include <vector>

#include <lair/core/lair.h>
#include <lair/core/log.h>


namespace lair
{


class SysModule;

class Renderer;


class RenderModule {
public:
	RenderModule(SysModule* sys, MasterLogger* logger = nullptr);
	RenderModule(const RenderModule&) = delete;
	RenderModule(RenderModule&&)      = delete;
	~RenderModule();

	RenderModule& operator=(const RenderModule&) = delete;
	RenderModule& operator=(RenderModule&&)      = delete;

	bool initialize();
	void shutdown();

	Renderer* createRenderer();

	SysModule* sys() {
		return _sys;
	}

	inline Logger& log() {
		return _log;
	}

protected:
	typedef std::unique_ptr<Renderer> RendererPtr;
	typedef std::vector<RendererPtr> RendererList;

protected:
	Logger       _log;
	bool         _initialized;
	SysModule*   _sys;

	RendererList _renderers;
};


}


#endif
