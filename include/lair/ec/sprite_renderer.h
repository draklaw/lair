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
#include <lair/render_gl2/texture.h>
#include <lair/render_gl2/vertex_buffer.h>
#include <lair/render_gl2/render_pass.h>
#include <lair/render_gl2/renderer.h>


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
	                   int texUnit = 0, const Vector4i& tileInfo = Vector4i(1, 1, 65536, 65536));

	ShaderParameter params[4];
	Matrix4         viewMatrix;
	int             texUnit;
	Vector4         tileInfo;
};

class SpriteShader {
public:
	SpriteShader();
	SpriteShader(ProgramObject* shader);

public:
	ProgramObject* shader;
	GLint          viewMatrixLoc;
	GLint          textureLoc;
	GLint          tileInfoLoc;
};


inline Box2 tileBox(const Vector2i& nTiles, const Vector2& tile) {
	return Box2(Vector2i(tile(0)  , tile(1)  ).cast<Scalar>().cwiseQuotient(nTiles.cast<Scalar>()),
	            Vector2i(tile(0)+1, tile(1)+1).cast<Scalar>().cwiseQuotient(nTiles.cast<Scalar>()));
}

inline Box2 tileBox(const Vector2i& nTiles, int tileIndex) {
	std::div_t tile = std::div(tileIndex, nTiles(0));
	return tileBox(nTiles, Vector2(tile.rem, tile.quot));
}

inline Box2 boxView(const Box2& box, const Box2& view) {
	Vector2 sizes = box.sizes();
	return Box2(box.min() + view.min().cwiseProduct(sizes),
	            box.min() + view.max().cwiseProduct(sizes));
}


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

	SpriteShader& shader();
	VertexFormat* format();
	VertexBuffer* buffer();

	void clear();

	void addVertex(const Vector4& pos, const Vector4& color, const Vector2& texCoord);
	void addVertex(const Vector3& pos, const Vector4& color, const Vector2& texCoord);
	void addIndex(unsigned index);
	void addSprite(const Matrix4& trans, const Box2& coords,
	               const Vector4& color, const Box2& texCoords);

	const ShaderParameter* addShaderParameters(
	        const SpriteShader& shader, const Matrix4& viewTransform, int texUnit, const Vector4i& tileInfo);

	TextureAspectSP createTexture(AssetSP asset);
	TextureAspectSP defaultTexture() const;

	Renderer* renderer();

protected:
	typedef std::list<SpriteShaderParams> ShaderParamList;

protected:
	Renderer*        _renderer;

	VertexFormat     _spriteFormat;
	ProgramObject    _defaultShaderProg;
	SpriteShader     _defaultShader;

	VertexBuffer     _buffer;
	ShaderParamList  _shaderParams;
};


}


#endif
