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


#include <functional>

#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/loader.h>

#include "lair/ec/sprite_renderer.h"


namespace lair
{


static const VertexAttribInfo _spriteVertexAttribSet[] = {
    { "vx_position", VxPosition },
    { "vx_color",    VxColor },
    { "vx_texCoord", VxTexCoord },
    LAIR_VERTEX_ATTRIB_INFO_END
};


static const TextureUnit _texColor = { 0, "sprite_color" };
const TextureUnit* TexColor = &_texColor;


//---------------------------------------------------------------------------//


SpriteShaderParams::SpriteShaderParams(const Matrix4& viewMatrix, int texUnit, const Vector4i& tileInfo)
	: viewMatrix(viewMatrix),
	  texUnit(texUnit),
	  tileInfo(tileInfo.cast<float>()) {
	params[0].index = -1;
}


//---------------------------------------------------------------------------//


SpriteShader::SpriteShader()
    : shader       (nullptr),
      viewMatrixLoc(-1),
      textureLoc   (-1),
      tileInfoLoc (-1) {
}


SpriteShader::SpriteShader(ShaderAspectSP shader)
    : shader       (shader),
      finalized    (false),
      viewMatrixLoc(-1),
      textureLoc   (-1),
      tileInfoLoc  (-1) {
}

bool SpriteShader::finalize() {
	if(!finalized && shader->isValid()) {
		const ProgramObject& s = shader->get();
		finalized     = true;
		viewMatrixLoc = s.getUniformLocation("viewMatrix");
		textureLoc    = s.getUniformLocation("texture");
		tileInfoLoc   = s.getUniformLocation("tileInfo");
	}
	return finalized;
}


ProgramObject* SpriteShader::get() {
	return &shader->_get();
}


//---------------------------------------------------------------------------//


SpriteRenderer::SpriteRenderer(LoaderManager* loader, Renderer* renderer,
                               Size vBufferSize, Size iBufferSize)
    : _loader(loader),
      _renderer(renderer),
      _attribSet(_spriteVertexAttribSet),
      _vertexArray(),
      _vertexBufferSize(vBufferSize),
      _indexBufferSize(iBufferSize),
      _vertexBuffer(renderer),
      _indexBuffer(renderer) {
	lairAssert(_renderer);

	_renderer->registerTextureUnit(TexColor);

	const VertexAttrib spriteVertexAttribs[] = {
	    { &_vertexBuffer, VxPosition, 4, gl::FLOAT, false,
	      offsetof(SpriteVertex, position) },
	    { &_vertexBuffer, VxColor,    4, gl::FLOAT, false,
	      offsetof(SpriteVertex, color) },
	    { &_vertexBuffer, VxTexCoord, 2, gl::FLOAT, false,
	      offsetof(SpriteVertex, texCoord) },
	    LAIR_VERTEX_ATTRIB_END
	};

	_vertexArray = renderer->createVertexArray(
	                sizeof(SpriteVertex), spriteVertexAttribs, &_indexBuffer);

	_defaultShader = loadShader("shader/sprite.ldl");
}


SpriteRenderer::~SpriteRenderer() {
}


unsigned SpriteRenderer::vertexCount() const {
	return _vertexBuffer.pos() / sizeof(SpriteVertex);
}


unsigned SpriteRenderer::indexCount()  const {
	return _indexBuffer.pos() / sizeof(unsigned);
}


SpriteShaderSP SpriteRenderer::shader() {
	return _defaultShader;
}


VertexAttribSet* SpriteRenderer::attribSet() {
	return &_attribSet;
}


VertexArray* SpriteRenderer::vertexArray() {
	return _vertexArray.get();
}


BufferObject* SpriteRenderer::vertexBuffer() {
	return &_vertexBuffer;
}


BufferObject* SpriteRenderer::indexBuffer() {
	return &_indexBuffer;
}


void SpriteRenderer::beginRender() {
	_vertexBuffer.beginWrite(_vertexBufferSize);
	_indexBuffer.beginWrite(_indexBufferSize);
}


bool SpriteRenderer::endRender() {
	bool success = true;

	// Check for overflow and grow buffers if required
	if(_vertexBuffer.pos() > _vertexBufferSize) {
		dbgLogger.warning("SpriteRenderer: Vertex buffer too small ! Actual size: ",
		                  _vertexBufferSize, ", required size: ", _vertexBuffer.pos());
		_vertexBufferSize = _vertexBuffer.pos() * 2;
		success = false;
	}
	if(_indexBuffer.pos() > _indexBufferSize) {
		dbgLogger.warning("SpriteRenderer: Index buffer too small ! Actual size: ",
		                  _indexBufferSize, ", required size: ", _indexBuffer.pos());
		_indexBufferSize = _indexBuffer.pos() * 2;
		success = false;
	}

	success &= _vertexBuffer.endWrite();
	success &= _indexBuffer.endWrite();

	return success;
}


void SpriteRenderer::addVertex(const Vector4& pos, const Vector4& color, const Vector2& texCoord) {
	_vertexBuffer.write(SpriteVertex{ pos, color, texCoord });
}


void SpriteRenderer::addVertex(const Vector3& pos, const Vector4& color, const Vector2& texCoord) {
	addVertex((Vector4() << pos, 1).finished(), color, texCoord);
}


void SpriteRenderer::addIndex(unsigned index) {
	_indexBuffer.write(index);
}


void SpriteRenderer::addSprite(const Matrix4& trans, const Box2& coords,
                               const Vector4& color, const Box2& texCoords) {
	GLuint index = vertexCount();

	for(int corner = 0; corner < 4; ++corner) {
		int tcCorner = corner ^ 0x02; // texCoords are bottom-up.
		Vector4 p;
		p << coords.corner(Box2::CornerType(corner)), 0, 1;
		p = trans * p;
		addVertex(p, linearFromSrgb(color), texCoords.corner(Box2::CornerType(tcCorner)));
	}

	addIndex(index + 0);
	addIndex(index + 1);
	addIndex(index + 2);
	addIndex(index + 2);
	addIndex(index + 1);
	addIndex(index + 3);
}


SpriteShaderSP SpriteRenderer::loadShader(const Path& logicPath) {
	auto loader = _loader->load<ShaderLoader>(logicPath, _renderer, &_attribSet);
	auto shader = std::make_shared<SpriteShader>(loader->asset()->aspect<ShaderAspect>());
	_unfinalizedShaders.push_back(shader);
	return shader;
}


void SpriteRenderer::finalizeShaders() {
	using namespace std::placeholders;
	std::remove_if(_unfinalizedShaders.begin(), _unfinalizedShaders.end(),
	               std::bind(&SpriteShader::finalize, _1));
}


const ShaderParameter* SpriteRenderer::addShaderParameters(
        const SpriteShaderSP shader, const Matrix4& viewTransform, int texUnit, const Vector4i& tileInfo) {
	if(!_shaderParams.empty()) {
		SpriteShaderParams& sp = _shaderParams.back();
		if(sp.viewMatrix == viewTransform && sp.texUnit == texUnit && sp.tileInfo == tileInfo.cast<float>())
			return sp.params;
	}

	_shaderParams.emplace_back(viewTransform, texUnit, tileInfo);
	SpriteShaderParams& sp = _shaderParams.back();
	ShaderParameter* params = sp.params;

	if(shader->viewMatrixLoc >= 0) {
		*(params++) = makeShaderParameter(shader->viewMatrixLoc, sp.viewMatrix);
	}
	if(shader->textureLoc >= 0) {
		*(params++) = makeShaderParameter(shader->textureLoc, &sp.texUnit);
	}
	if(shader->tileInfoLoc >= 0) {
		*(params++) = makeShaderParameter(shader->tileInfoLoc, sp.tileInfo);
	}
	params->index = -1;

	return sp.params;
}


TextureAspectSP SpriteRenderer::createTexture(AssetSP asset) {
	return _renderer->createTexture(asset);
}


TextureAspectSP SpriteRenderer::defaultTexture() const {
	return _renderer->defaultTexture();
}


TextureSetCSP SpriteRenderer::getTextureSet(const TextureSet& textureSet) {
	return _renderer->getTextureSet(textureSet);
}


TextureSetCSP SpriteRenderer::getTextureSet(const TextureUnit* unit, TextureAspectSP texture, SamplerSP sampler) {
	return _renderer->getTextureSet(unit, texture, sampler);
}


TextureSetCSP SpriteRenderer::getTextureSet(const TextureUnit* unit, AssetSP textureAsset, SamplerSP sampler) {
	return _renderer->getTextureSet(unit, textureAsset, sampler);
}


SamplerSP SpriteRenderer::defaultSampler() {
	if(!_defaultSampler) {
		_defaultSampler = _renderer->getSampler(
		        SamplerParams(SamplerParams::BILINEAR_NO_MIPMAP
		                    | SamplerParams::CLAMP));
	}
	return _defaultSampler;
}


TextureSetCSP SpriteRenderer::defaultTextureSet() {
	if(!_defaultTextureSet) {
		_defaultTextureSet = getTextureSet(TexColor, defaultTexture(), defaultSampler());
	}
	return _defaultTextureSet;
}


Renderer* SpriteRenderer::renderer() {
	return _renderer;
}


}
