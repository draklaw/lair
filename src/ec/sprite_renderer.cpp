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

#include "lair/ec/sprite_renderer.h"


namespace lair
{


static const VertexAttribInfo _spriteVertexAttribSet[] = {
    { "vx_position", VxPosition },
    { "vx_color",    VxColor },
    { "vx_texCoord", VxTexCoord },
    LAIR_VERTEX_ATTRIB_INFO_END
};


const char* _spriteVertGlsl =
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


const char* _spriteFragGlsl =
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"

	"uniform sampler2D texture;\n"
	"uniform vec4      tileInfo;\n"

	"varying highp   vec4 position;\n"
	"varying lowp    vec4 color;\n"
	"varying mediump vec2 texCoord;\n"

	"void main() {\n"
	"	// Is this useful ?\n"
	"	vec2 nTiles    = tileInfo.xy;\n"
	"	vec2 texSize   = tileInfo.zw;\n"
	"	vec2 texCoord2 = texCoord * nTiles;\n"
	"	vec2 margin    = nTiles / (2.0 * texSize);\n"
	"	vec2 tileCoord = fract(texCoord2);\n"
	"	vec2 tile      = texCoord2 - tileCoord;\n"
	"	tileCoord      = clamp(tileCoord, margin, vec2(1.0) - margin);\n"
	"	texCoord2      = (tile + tileCoord) / nTiles;\n"
	"	\n"
	"	vec4 fcolor = color * texture2D(texture, texCoord2);\n"
	"	if(fcolor.a < .01){\n"
	"		discard;\n"
	"	}\n"
	"	gl_FragColor = fcolor;\n"
	"//	gl_FragColor = vec4(texCoord, 0., 1.);\n"
	"//	gl_FragColor = vec4(1., 0., 0., 1.);\n"
	"//	gl_FragColor = vec4(tileCoord, 0., 1.);\n"
	"}\n";


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


SpriteShader::SpriteShader(ProgramObject* shader)
    : shader       (shader),
      viewMatrixLoc(shader->getUniformLocation("viewMatrix")),
      textureLoc   (shader->getUniformLocation("texture")),
      tileInfoLoc  (shader->getUniformLocation("tileInfo")) {
}


//---------------------------------------------------------------------------//


SpriteRenderer::SpriteRenderer(Renderer* renderer, Size vBufferSize, Size iBufferSize)
	: _renderer(renderer),
      _attribSet(_spriteVertexAttribSet),
      _vertexArray(),
      _vertexBufferSize(vBufferSize),
      _indexBufferSize(iBufferSize),
      _vertexBuffer(renderer),
      _indexBuffer(renderer) {
	lairAssert(_renderer);

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

	ShaderObject vert = _renderer->compileShader("sprite", gl::VERTEX_SHADER,
	                                   GlslSource(_spriteVertGlsl));
	ShaderObject frag = _renderer->compileShader("sprite", gl::FRAGMENT_SHADER,
	                                   GlslSource(_spriteFragGlsl));
	if(vert.isCompiled() && frag.isCompiled()) {
		_defaultShaderProg = _renderer->compileProgram(
		                         "sprite", &_attribSet, &vert, &frag);
	}
	lairAssert(_defaultShaderProg.isLinked());
	_defaultShader = SpriteShader(&_defaultShaderProg);
}


SpriteRenderer::~SpriteRenderer() {
}


unsigned SpriteRenderer::vertexCount() const {
	return _vertexBuffer.pos() / sizeof(SpriteVertex);
}


unsigned SpriteRenderer::indexCount()  const {
	return _indexBuffer.pos() / sizeof(unsigned);
}


SpriteShader& SpriteRenderer::shader() {
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
		addVertex(p, color, texCoords.corner(Box2::CornerType(tcCorner)));
	}

	addIndex(index + 0);
	addIndex(index + 1);
	addIndex(index + 2);
	addIndex(index + 2);
	addIndex(index + 1);
	addIndex(index + 3);
}


const ShaderParameter* SpriteRenderer::addShaderParameters(
        const SpriteShader& shader, const Matrix4& viewTransform, int texUnit, const Vector4i& tileInfo) {
	if(!_shaderParams.empty()) {
		SpriteShaderParams& sp = _shaderParams.back();
		if(sp.viewMatrix == viewTransform && sp.texUnit == texUnit && sp.tileInfo == tileInfo.cast<float>())
			return sp.params;
	}

	_shaderParams.emplace_back(viewTransform, texUnit, tileInfo);
	SpriteShaderParams& sp = _shaderParams.back();
	ShaderParameter* params = sp.params;

	if(shader.viewMatrixLoc >= 0) {
		*(params++) = makeShaderParameter(shader.viewMatrixLoc, sp.viewMatrix);
	}
	if(shader.textureLoc >= 0) {
		*(params++) = makeShaderParameter(shader.textureLoc, &sp.texUnit);
	}
	if(shader.tileInfoLoc >= 0) {
		*(params++) = makeShaderParameter(shader.tileInfoLoc, sp.tileInfo);
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


TextureSetCSP SpriteRenderer::getTextureSet(unsigned unit, TextureAspectSP texture, SamplerSP sampler) {
	return _renderer->getTextureSet(unit, texture, sampler);
}

TextureSetCSP SpriteRenderer::getTextureSet(unsigned unit, AssetSP textureAsset, SamplerSP sampler) {
	return _renderer->getTextureSet(unit, textureAsset, sampler);
}


TextureSetCSP SpriteRenderer::defaultTextureSet() {
	if(!_defaultTextureSet) {
		_defaultTextureSet = getTextureSet(TexColor, defaultTexture(), nullptr);
	}
	return _defaultTextureSet;
}


Renderer* SpriteRenderer::renderer() {
	return _renderer;
}


}
