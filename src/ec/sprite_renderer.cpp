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
	"	if(fcolor.a < .5){\n"
	"		discard;\n"
	"	}\n"
	"	gl_FragColor = fcolor;\n"
	"//	gl_FragColor = vec4(texCoord, 0., 1.);\n"
	"//	gl_FragColor = vec4(1., 0., 0., 1.);\n"
	"}\n";


//---------------------------------------------------------------------------//


SpriteShaderParams::SpriteShaderParams(const Matrix4& viewMatrix, unsigned texUnit)
	: viewMatrix(viewMatrix),
	  texUnit(texUnit) {
}


//---------------------------------------------------------------------------//


SpriteShader::SpriteShader()
    : _shader       (nullptr),
      _viewMatrixLoc(-1),
      _textureLoc   (-1) {
}


SpriteShader::SpriteShader(const ProgramObject* shader)
    : _shader       (shader),
      _viewMatrixLoc(shader->getUniformLocation("viewMatrix")),
      _textureLoc   (shader->getUniformLocation("texture")) {
}


void SpriteShader::setParams(Context* glc, const SpriteShaderParams& params) {
	if(_viewMatrixLoc >= 0) {
		glc->uniformMatrix4fv(_viewMatrixLoc, 1, false,
							  const_cast<float*>(params.viewMatrix.data()));
	}
	if(_textureLoc >= 0) {
		glc->uniform1i(_textureLoc, params.texUnit);
	}
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


void SpriteRenderer::beginFrame() {
	_buffer.clear();
	_drawCalls.clear();
	_spriteDepth = 0;
}


void SpriteRenderer::addSprite(const Matrix4& trans, const Box2& coords,
                               const Vector4& color, const Box2& texCoords,
                               TextureSP texture, unsigned texFlags,
                               BlendingMode blendingMode) {
	GLuint index = _buffer.vertexCount();

	for(int corner = 0; corner < 4; ++corner) {
		int tcCorner = corner ^ 0x02; // texCoords are bottom-up.
		Vector4 p;
		p << coords.corner(Box2::CornerType(corner)), _spriteDepth, 1;
		_buffer.addVertex(SpriteVertex{
			trans * p,
			color,
			texCoords.corner(Box2::CornerType(tcCorner))
		});
	}

	_buffer.addIndex(index + 0);
	_buffer.addIndex(index + 1);
	_buffer.addIndex(index + 2);
	_buffer.addIndex(index + 2);
	_buffer.addIndex(index + 1);
	_buffer.addIndex(index + 3);

	++_spriteDepth;

	if(!_drawCalls.empty()
	&&  _drawCalls.back().tex          == texture
	&&  _drawCalls.back().texFlags     == texFlags
	&&  _drawCalls.back().blendingMode == blendingMode) {
		_drawCalls.back().count += 6;
	}
	else {
		_drawCalls.emplace_back(DrawCall{ texture, texFlags, blendingMode,
		                                  _buffer.indexSize() - 6, 6 });
	}
}


void SpriteRenderer::endFrame(Matrix4 viewTransform) {
	Context* glc = _renderer->context();

	_renderer->uploadPendingTextures();

	_buffer.bindAndUpload(glc);
	_spriteFormat.setup(glc);

	_defaultShader.use();
	viewTransform(2, 2) = 1.f / float(_spriteDepth);
	viewTransform(2, 3) = 0;
	SpriteShaderParams params(viewTransform);
	_defaultShader.setParams(glc, params);
	glc->activeTexture(gl::TEXTURE0);

	for(const DrawCall& dc: _drawCalls) {
		if(!dc.tex || !dc.tex->isValid()) {
			continue;
		}

		dc.tex->bind();
		dc.tex->_setFlags(dc.texFlags);

		switch(dc.blendingMode) {
		case BLEND_NONE:
			glc->disable(gl::BLEND);
			break;
		case BLEND_ALPHA:
			glc->enable(gl::BLEND);
			glc->blendEquation(gl::FUNC_ADD);
			glc->blendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
			break;
		case BLEND_ADD:
			glc->enable(gl::BLEND);
			glc->blendEquation(gl::FUNC_ADD);
			glc->blendFunc(gl::ONE, gl::ONE);
			break;
		case BLEND_MULTIPLY:
			glc->enable(gl::BLEND);
			glc->blendEquation(gl::FUNC_ADD);
			glc->blendFunc(gl::DST_COLOR, gl::ZERO);
			break;
		}

		glc->drawElements(gl::TRIANGLES, dc.count, gl::UNSIGNED_INT,
		                  reinterpret_cast<void*>(dc.start*sizeof(unsigned)));
	}
}


TextureAspectSP SpriteRenderer::createTexture(AssetSP asset) {
	return _renderer->createTexture(asset);
}


}
