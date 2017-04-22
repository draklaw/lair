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
#include <algorithm>

#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/image.h>

#include <lair/sys_sdl2/image_loader.h>

#include "lair/render_gl2/render_module.h"

#include "lair/render_gl2/renderer.h"


namespace lair
{


Renderer::Renderer(RenderModule* module, AssetManager* assetManager)
    : _module(module),
      _assetManager(assetManager),
      _context(module? module->context(): nullptr),
      _defaultTexture() {
	lairAssert(_module);
	lairAssert(_assetManager);

	_createDefaultTexture();
}


Renderer::~Renderer() {
}


Context* Renderer::context() {
	return _context;
}


ShaderObject Renderer::compileShader(const char* name, GLenum type,
                                     const GlslSource& source) {
	ShaderObject shader(this, type);
	shader.generateObject();
	if(!shader.compile(source)) {
		std::string sLog;
		shader.getLog(sLog);
		log().error("Failed to compile ",
		            (type == gl::VERTEX_SHADER)? "vertex": "fragment",
		            " shader object \"", name, "\":\n", sLog);
	}
	return shader;
}


ProgramObject Renderer::compileProgram(const char* name,
                                       const VertexFormat* format,
                                       const ShaderObject* vert,
                                       const ShaderObject* frag) {
	ProgramObject prog(this);
	prog.generateObject();
	prog.attachShader(*vert);
	prog.attachShader(*frag);

	for(const VertexAttrib& attrib: *format) {
		prog.bindAttributeLocation(attrib.name, attrib.index);
		++format;
	}

	if(!prog.link()) {
		std::string sLog;
		prog.getLog(sLog);
		log().error("Failed to link shader \"", name, "\":\n", sLog);
	}
	prog.detachAllShaders();
	return prog;
}


TextureAspectSP Renderer::createTexture(AssetSP asset) {
	lairAssert(bool(asset));
	TextureAspectSP aspect = asset->aspect<TextureAspect>();
	if(!aspect) {
		aspect = asset->createAspect<TextureAspect>();
		enqueueToUpload(aspect);
	}
	return aspect;
}


void Renderer::enqueueToUpload(TextureAspectSP texture) {
	_pendingTextures.push_back(texture);
}


void Renderer::uploadPendingTextures() {
	for(TextureAspectSP texture: _pendingTextures) {
		ImageAspectSP image = texture->asset()->aspect<ImageAspect>();
		if(image && image->isValid() && !texture->isValid()) {
			log().info("Upload texture \"", texture->asset()->logicPath(), "\"...");

			Texture& tex = texture->_get();
			tex = std::move(Texture(this));
			tex._upload(image->get());
		}
	}

	auto end = std::remove_if(_pendingTextures.begin(), _pendingTextures.end(),
	                          [](TextureAspectSP tex) { return bool(tex->isValid()); });
	_pendingTextures.erase(end, _pendingTextures.end());
}


Logger& Renderer::log() {
	return _module->log();
}


void Renderer::_createDefaultTexture() {
	AssetSP texAsset = _assetManager->getOrCreateAsset("/__builtin__/placeholder_texture");

	ImageAspectSP imgAspect = texAsset->getOrCreateAspect<ImageAspect>();
	if(imgAspect && !imgAspect->isValid()) {
		unsigned width      = 32;
		unsigned height     = width;
		unsigned stripWidth = 2;
		std::vector<Byte> img(width * height * 3);
		Byte colors[] = { 255,   0, 255,
		                  0, 255,   0 };
		for(unsigned y = 0; y < height; ++y) {
			for(unsigned x = 0; x < width; ++x) {
				Byte* pixel = img.data() + (y * width + x) * 3;
				Byte* color = colors + (((x + y) / stripWidth) % 2) * 3;
				for(unsigned c = 0; c < 3; ++c)
					pixel[c] = color[c];
			}
		}

		imgAspect->_get() = Image(width, height, Image::Format::FormatRGB8, img.data());
	}

	_defaultTexture = createTexture(texAsset);

}


}
