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


#include <vector>

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
    : _module(module),
      _defaultTexture(),
      _textures(),
      _defaultShader() {
	lairAssert(_module);

	_createDefaultTexture();
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


void Renderer::_createDefaultTexture() {
	unsigned size = 32;
	uint32 fillColor = 0xffffffff;
	uint32 bordColor = 0xffff00ff;
	uint32 lineColor = 0xff0000ff;

	std::vector<Byte> buffer(size * size * 4);
	uint32* pixels = reinterpret_cast<uint32*>(buffer.data());
	for(unsigned y = 0; y < size; ++y) {
		for(unsigned x = 0; x < size; ++x) {
			pixels[y * size + x] = fillColor;
		}
	}
	for(unsigned x = 0; x < size; ++x) {
		if(x > 0) {
			pixels[x * size + x - 1]    = lineColor;
			pixels[x * size + size - x] = lineColor;
		}
		pixels[x * size + x]            = lineColor;
		pixels[x * size + size - x - 1] = lineColor;
		if(x < size - 1) {
			pixels[x * size + x + 1]        = lineColor;
			pixels[x * size + size - x - 2] = lineColor;
		}
	}

	for(unsigned x = 0; x < size; ++x) {
		pixels[x * size + 0] = bordColor;
		pixels[x * size + 1] = bordColor;
		pixels[x * size + size - 1] = bordColor;
		pixels[x * size + size - 2] = bordColor;
		pixels[(0) * size + x] = bordColor;
		pixels[(1) * size + x] = bordColor;
		pixels[(size - 1) * size + x] = bordColor;
		pixels[(size - 2) * size + x] = bordColor;
	}

	Image img(size, size, Image::Format::FormatRGBA8, buffer.data());
	_defaultTexture._upload(img);
	_defaultTexture._setFlags(Texture::BILINEAR | Texture::REPEAT);
}

}
