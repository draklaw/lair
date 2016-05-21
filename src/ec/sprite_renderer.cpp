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


static const VertexAttrib _spriteVertexFormat[] = {
    { "vx_position", VxPosition, 4, gl::FLOAT, false,
      offsetof(SpriteVertex, position) },
    { "vx_color",    VxColor,    4, gl::FLOAT, false,
      offsetof(SpriteVertex, color) },
    { "vx_texCoord", VxTexCoord, 2, gl::FLOAT, false,
      offsetof(SpriteVertex, texCoord) },
    { nullptr, 0, 0, 0, false, 0 }
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

	"varying highp   vec4 position;\n"
	"varying lowp    vec4 color;\n"
	"varying mediump vec2 texCoord;\n"

	"void main() {\n"
	"	vec4 fcolor = color * texture2D(texture, texCoord);\n"
	"	if(fcolor.a < .01){\n"
	"		discard;\n"
	"	}\n"
	"	gl_FragColor = fcolor;\n"
	"//	gl_FragColor = vec4(texCoord, 0., 1.);\n"
	"//	gl_FragColor = vec4(1., 0., 0., 1.);\n"
	"}\n";


//---------------------------------------------------------------------------//


SpriteShaderParams::SpriteShaderParams(const Matrix4& viewMatrix, int texUnit)
	: viewMatrix(viewMatrix),
	  texUnit(texUnit) {
	params[0].index = -1;
}


//---------------------------------------------------------------------------//


SpriteShader::SpriteShader()
    : shader       (nullptr),
      viewMatrixLoc(-1),
      textureLoc   (-1) {
}


SpriteShader::SpriteShader(ProgramObject* shader)
    : shader       (shader),
      viewMatrixLoc(shader->getUniformLocation("viewMatrix")),
      textureLoc   (shader->getUniformLocation("texture")) {
}


//---------------------------------------------------------------------------//


SpriteRenderer::SpriteRenderer(Renderer* renderer)
	: _renderer(renderer),
      _spriteFormat(sizeof(SpriteVertex), _spriteVertexFormat),
      _buffer(sizeof(SpriteVertex)) {
	lairAssert(_renderer);

	ShaderObject vert = _renderer->compileShader("sprite", gl::VERTEX_SHADER,
	                                   GlslSource(_spriteVertGlsl));
	ShaderObject frag = _renderer->compileShader("sprite", gl::FRAGMENT_SHADER,
	                                   GlslSource(_spriteFragGlsl));
	if(vert.isCompiled() && frag.isCompiled()) {
		_defaultShaderProg = _renderer->compileProgram(
		                         "sprite", &_spriteFormat, &vert, &frag);
	}
	lairAssert(_defaultShaderProg.isLinked());
	_defaultShader = SpriteShader(&_defaultShaderProg);
}


SpriteRenderer::~SpriteRenderer() {
}


void SpriteRenderer::clear() {
	_buffer.clear();
	_shaderParams.clear();
}


unsigned SpriteRenderer::vertexCount() const {
	return _buffer.vertexCount();
}


unsigned SpriteRenderer::indexCount()  const {
	return _buffer.indexSize();
}


SpriteShader& SpriteRenderer::shader() {
	return _defaultShader;
}


VertexFormat* SpriteRenderer::format() {
	return &_spriteFormat;
}


VertexBuffer* SpriteRenderer::buffer() {
	return &_buffer;
}


void SpriteRenderer::addVertex(const Vector4& pos, const Vector4& color, const Vector2& texCoord) {
	_buffer.addVertex(SpriteVertex{ pos, color, texCoord });
}


void SpriteRenderer::addVertex(const Vector3& pos, const Vector4& color, const Vector2& texCoord) {
	addVertex((Vector4() << pos, 1).finished(), color, texCoord);
}


void SpriteRenderer::addIndex(unsigned index) {
	_buffer.addIndex(index);
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
        const SpriteShader& shader, const Matrix4& viewTransform, int texUnit) {
	_shaderParams.emplace_back(viewTransform, texUnit);
	SpriteShaderParams& sp = _shaderParams.back();
	ShaderParameter* params = sp.params;

	if(shader.viewMatrixLoc >= 0) {
		*(params++) = makeShaderParameter(shader.viewMatrixLoc, sp.viewMatrix);
	}
	if(shader.textureLoc >= 0) {
		*(params++) = makeShaderParameter(shader.textureLoc, &sp.texUnit);
	}
	params->index = -1;

	return sp.params;
}


TextureAspectSP SpriteRenderer::createTexture(AssetSP asset) {
	return _renderer->createTexture(asset);
}


Renderer* SpriteRenderer::renderer() {
	return _renderer;
}


}
