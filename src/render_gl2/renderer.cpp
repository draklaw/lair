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


static const VertexAttrib _spriteVertexFormat[] = {
    { "vx_position", Renderer::VxPosition, 4, GL_FLOAT, false,
      offsetof(SpriteVertex, position) },
    { "vx_color",    Renderer::VxColor, 4, GL_FLOAT, false,
      offsetof(SpriteVertex, color) },
    { "vx_texCoord", Renderer::VxTexCoord, 2, GL_FLOAT, false,
      offsetof(SpriteVertex, texCoord) },
    { nullptr, 0, 0, 0, false, 0 }
};


const char* defaultVertGlsl =
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"

	"uniform highp mat4 viewMatrix;\n"

	"attribute highp   vec4 vx_position;\n"
	"attribute lowp    vec4 vx_color;\n"
	"attribute mediump vec2 vx_texCoord;\n"

	"varying highp   vec4 position;\n"
	"varying lowp    vec4 color;\n"
	"varying mediump vec2 texCoord;\n"

	"void main() {\n"
	"	gl_Position = viewMatrix * vx_position;\n"
	"	position    = vx_position;\n"
	"	color       = vx_color;\n"
	"	texCoord    = vx_texCoord;\n"
	"}\n";


const char* defaultFragGlsl =
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"

	"uniform sampler2D texture;\n"

	"varying mediump vec2 texCoord;\n"

	"void main() {\n"
	"	gl_FragColor = texture2D(texture, texCoord);\n"
	"//	gl_FragColor = vec4(texCoord, 0., 1.);\n"
	"//	gl_FragColor = vec4(1., 0., 0., 1.);\n"
	"}\n";


//---------------------------------------------------------------------------//


SpriteShader::SpriteShader()
    : _shader       (nullptr),
      _viewMatrixLoc(-1),
      _textureLoc   (-1) {
}


SpriteShader::SpriteShader(const ProgramObject* shader)
    : _shader       (shader),
      _viewMatrixLoc(glGetUniformLocation(_shader->id(), "viewMatrix")),
      _textureLoc   (glGetUniformLocation(_shader->id(), "texture")) {
}


//---------------------------------------------------------------------------//


Renderer::Renderer(RenderModule* module)
    : _module(module),
      _spriteFormat(sizeof(SpriteVertex), _spriteVertexFormat),
      _defaultTexture(),
      _textures(),
      _spriteShaderProg(),
      _spriteShader() {
	lairAssert(_module);

	_createDefaultTexture();

	ShaderObject vert = _compileShader("sprite", GL_VERTEX_SHADER,
	                                   GlslSource(defaultVertGlsl));
	ShaderObject frag = _compileShader("sprite", GL_FRAGMENT_SHADER,
	                                   GlslSource(defaultFragGlsl));
	if(vert.isCompiled() && frag.isCompiled()) {
		_spriteShaderProg = _compileProgram("sprite",
		        &_spriteFormat, &vert, &frag);
	}
	lairAssert(_spriteShaderProg.isLinked());
	_spriteShader = SpriteShader(&_spriteShaderProg);
}


Renderer::~Renderer() {
}


void Renderer::preloadTexture(const std::string& file, uint32 flags) {
	TexId id(file, flags);
	std::unique_lock<std::mutex> lk(_textureLock);
	auto texIt = _textures.find(id);
	if(texIt == _textures.end()) {
		texIt = _textures.emplace_hint(texIt, id, Texture());
		texIt->second._load(_module->sys()->loader().loadImage(file));
	}
}


void Renderer::preloadSprite(const std::string& file) {
	std::unique_lock<std::mutex> lk(_spriteLock);
	auto spriteIt = _sprites.find(file);
	if(spriteIt == _sprites.end()) {
		spriteIt = _sprites.emplace_hint(spriteIt, file,
		                _module->sys()->loader().load<SpriteLoader>(file, this));
	}
}


Texture* Renderer::getTexture(const std::string& file, uint32 flags) {
	TexId id(file, flags);
	std::unique_lock<std::mutex> lk(_textureLock);
	auto texIt = _textures.find(id);
	if(texIt == _textures.end()) {
		log().warning("Texture \"", file, "\" not preloaded.");
		preloadTexture(file, flags);
		texIt = _textures.find(id);
	}
	lairAssert(texIt != _textures.end());
	texIt->second._uploadNow();
	texIt->second._setFlags(flags);
	return &texIt->second;
}


Sprite* Renderer::getSprite(const std::string& file) {
	std::unique_lock<std::mutex> lk(_spriteLock);
	auto spriteIt = _sprites.find(file);
	if(spriteIt == _sprites.end()) {
		log().warning("Sprite \"", file, "\" not preloaded.");
		preloadSprite(file);
		spriteIt = _sprites.find(file);
	}
	lairAssert(spriteIt != _sprites.end());
	spriteIt->second->wait();
	if(spriteIt->second->isSuccessful()) {
		spriteIt->second->_sprite._texture = getTexture(spriteIt->second->_texture,
														spriteIt->second->_textureFlags);
	} else {
		spriteIt->second->_sprite._texture = defaultTexture();
	}
	return &spriteIt->second->_sprite;
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


ShaderObject Renderer::_compileShader(const char* name, GLenum type,
                                      const GlslSource& source) {
	ShaderObject shader(type);
	shader.generateObject();
	if(!shader.compile(source)) {
		std::string sLog;
		shader.getLog(sLog);
		log().error("Failed to compile ",
		            (type == GL_VERTEX_SHADER)? "vertex": "fragment",
		            " shader object \"", name, "\":\n", sLog);
	}
	return shader;
}

ProgramObject Renderer::_compileProgram(const char* name,
		const VertexFormat* format,
		const ShaderObject* vert, const ShaderObject* frag) {
	ProgramObject prog;
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


}
