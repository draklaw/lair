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

#include <list>

#include <lair/core/lair.h>

#include <lair/geometry/shape_2d.h>

#include <lair/render_gl3/context.h>
#include <lair/render_gl3/texture.h>
#include <lair/render_gl3/buffer_object.h>
#include <lair/render_gl3/vertex_attrib_set.h>
#include <lair/render_gl3/vertex_array.h>
#include <lair/render_gl3/render_pass.h>
#include <lair/render_gl3/renderer.h>


namespace lair
{


class LoaderManager;


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

extern const TextureUnit* TexColor;

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
	SpriteShader(ShaderAspectSP shader);

	bool finalize();

	ProgramObject* get();

public:
	ShaderAspectSP shader;
	bool           finalized;
	GLint          viewMatrixLoc;
	GLint          textureLoc;
	GLint          tileInfoLoc;
};

typedef std::shared_ptr<SpriteShader> SpriteShaderSP;


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
	SpriteRenderer(LoaderManager* manager,
	               Renderer* renderer,
	               Size vBufferSize = (1 << 20) * sizeof(SpriteVertex),
	               Size iBufferSize = (1 << 20) * sizeof(unsigned));
	SpriteRenderer(const SpriteRenderer&) = delete;
	SpriteRenderer(SpriteRenderer&&)      = delete;
	~SpriteRenderer();

	SpriteRenderer& operator=(const SpriteRenderer&) = delete;
	SpriteRenderer& operator=(SpriteRenderer&&)      = delete;

	unsigned vertexCount() const;
	unsigned indexCount()  const;

	SpriteShaderSP   shader();
	VertexAttribSet* attribSet();
	VertexArray*     vertexArray();
	BufferObject*    vertexBuffer();
	BufferObject*    indexBuffer();

	void beginRender();
	bool endRender();

	void addVertex(const Vector4& pos, const Vector4& color, const Vector2& texCoord);
	void addVertex(const Vector3& pos, const Vector4& color, const Vector2& texCoord);
	void addIndex(unsigned index);
	void addSprite(const Matrix4& trans, const Box2& coords,
	               const Vector4& color, const Box2& texCoords);

	void addShape(const Matrix4& trans, const Sphere2& sphere, const Vector4& color);
	void addShape(const Matrix4& trans, const AlignedBox2& box, const Vector4& color);
	void addShape(const Matrix4& trans, const OrientedBox2& box, const Vector4& color);
	void addShape(const Matrix4& trans, const Shape2D& shape, const Vector4& color);

	SpriteShaderSP loadShader(const Path& logicPath);
	void finalizeShaders();

	const ShaderParameter* addShaderParameters(
	        const SpriteShaderSP shader, const Matrix4& viewTransform, int texUnit, const Vector4i& tileInfo);

	TextureAspectSP createTexture(AssetSP asset);
	TextureAspectSP defaultTexture() const;

	TextureSetCSP getTextureSet(const TextureSet& textureSet);
	TextureSetCSP getTextureSet(const TextureUnit* unit, TextureAspectSP texture, SamplerSP sampler);
	TextureSetCSP getTextureSet(const TextureUnit* unit, AssetSP textureAsset, SamplerSP sampler);

	SamplerSP defaultSampler();
	TextureSetCSP defaultTextureSet();

	Renderer* renderer();

protected:
	typedef std::list<SpriteShaderParams> ShaderParamList;
	typedef std::list<SpriteShaderSP> ShaderList;

protected:
	LoaderManager*   _loader;
	Renderer*        _renderer;

	VertexAttribSet  _attribSet;
	VertexArraySP    _vertexArray;
	ProgramObject    _defaultShaderProg;
	SpriteShaderSP   _defaultShader;
	ShaderList       _unfinalizedShaders;

	Size             _vertexBufferSize;
	Size             _indexBufferSize;
	BufferObject     _vertexBuffer;
	BufferObject     _indexBuffer;
	ShaderParamList  _shaderParams;

	SamplerSP        _defaultSampler;
	TextureSetCSP    _defaultTextureSet;
};


}


#endif
