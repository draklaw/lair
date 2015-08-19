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


#include <boost/functional/hash.hpp>

#include <lair/core/lair.h>
#include <lair/core/log.h>

#include <lair/sys_sdl2/sys_module.h>
#include <lair/sys_sdl2/sys_loader.h>

#include "lair/render_gl2/render_module.h"

#include "lair/render_gl2/renderer.h"


namespace lair
{


Renderer::Renderer(RenderModule* module)
    : _module(module) {
	lairAssert(_module);
}


Renderer::~Renderer() {
}


Texture* Renderer::loadTexture(const std::string& file, uint32 flags) {
	TexId id(file, flags);
	auto texIt = _textures.find(id);
	if(texIt != _textures.end()) {
		return &texIt->second;
	}

	texIt = _textures.emplace_hint(texIt, id, Texture());
	texIt->second._load(_module->sys()->loader().loadImage(file));
	texIt->second._setFlags(flags);

	return &texIt->second;
}


Logger& Renderer::log() { return _module->log(); }


size_t Renderer::HashTexId::operator()(const TexId& texId) const {
	using namespace boost;
	size_t h = 0;
	hash_combine(h, texId.file);
	hash_combine(h, texId.flags);
	return h;
}


}
