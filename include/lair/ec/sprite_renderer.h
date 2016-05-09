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


#ifndef _LAIR_EC_SPRITE_RENDERER_H
#define _LAIR_EC_SPRITE_RENDERER_H


#include <lair/core/lair.h>

#include <lair/render_gl2/gl.h>
#include <lair/render_gl2/renderer.h>
#include <lair/render_gl2/texture.h>
#include <lair/render_gl2/vertex_buffer.h>


namespace lair
{


enum SpriteVertexAttrib {
	VxPosition,
	VxColor,
	VxTexCoord
};

struct SpriteVertex {
	Vector4 position;
	Vector4 color;
	Vector2 texCoord;
};

struct SpriteShaderParams {
	SpriteShaderParams(const Matrix4& viewMatrix = Matrix4::Identity(),
	                   unsigned texUnit = 0);

	Matrix4  viewMatrix;
	unsigned texUnit;
};

class SpriteShader {
public:
	SpriteShader();
	SpriteShader(const ProgramObject* shader);

	inline const ProgramObject* program() const { return _shader; }
	inline void use() const { if(_shader) _shader->use(); }

	void setParams(Context* glc, const SpriteShaderParams& params);

private:
	const ProgramObject* _shader;
	GLint                _viewMatrixLoc;
	GLint                _textureLoc;
	SpriteShaderParams   _params;
};


enum BlendingMode {
	BLEND_NONE,
	BLEND_ALPHA,
	BLEND_ADD,
	BLEND_MULTIPLY
};


class SpriteRenderer {
public:
	SpriteRenderer(Renderer* renderer);
	SpriteRenderer(const SpriteRenderer&) = delete;
	SpriteRenderer(SpriteRenderer&&)      = delete;
	~SpriteRenderer();

	SpriteRenderer& operator=(const SpriteRenderer&) = delete;
	SpriteRenderer& operator=(SpriteRenderer&&)      = delete;

	unsigned vertexCount() const;
	unsigned indexCount()  const;

	void beginFrame();
	void setDrawCall(TextureSP texture, unsigned texFlags,
	                 BlendingMode blendingMode);
	void addVertex(const Vector4& pos, const Vector4& color, const Vector2& texCoord);
	void addVertex(const Vector3& pos, const Vector4& color, const Vector2& texCoord);
	void addIndex(unsigned index);
	void addSprite(const Matrix4& trans, const Box2& coords,
	               const Vector4& color, const Box2& texCoords);
	void endFrame(Matrix4 viewTransform);

	TextureAspectSP createTexture(AssetSP asset);

protected:
	struct DrawCall {
		TextureWP    tex;
		unsigned     texFlags;
		BlendingMode blendingMode;
		size_t       start;
		size_t       count;
	};
	typedef std::vector<DrawCall> DrawCallVector;

protected:
	Renderer*        _renderer;

	VertexFormat     _spriteFormat;
	ProgramObject    _defaultShaderProg;
	SpriteShader     _defaultShader;

	VertexBuffer     _buffer;
	DrawCallVector   _drawCalls;
};


}


#endif
