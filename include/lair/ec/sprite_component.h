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


#ifndef _LAIR_EC_SPRITE_COMPONENT_H
#define _LAIR_EC_SPRITE_COMPONENT_H


#include <cstring>
#include <vector>
#include <list>

#include <lair/core/lair.h>

#include <lair/sys_sdl2/image_loader.h>

#include <lair/render_gl2/gl.h>
#include <lair/render_gl2/batch.h>
#include <lair/render_gl2/renderer.h>
#include <lair/render_gl2/texture.h>

#include <lair/ec/entity.h>
#include <lair/ec/component.h>
#include <lair/ec/component_manager.h>


namespace lair
{


class OrthographicCamera;
class Texture;

class _Entity;
class EntityManager;

class SpriteComponentManager;


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


class SpriteComponent : public Component {
public:
	SpriteComponent(_Entity* entity, ComponentManager<SpriteComponent>* manager);
	SpriteComponent(const SpriteComponent&) = delete;
	SpriteComponent(SpriteComponent&&)      = default;
	~SpriteComponent();

	SpriteComponent& operator=(const SpriteComponent&) = delete;
	SpriteComponent& operator=(SpriteComponent&&)      = default;

	inline TextureAspectSP texture() const { return _texture.lock(); }
	inline void setTexture(TextureAspectSP texture) {
		_texture = texture;
	}
	void setTexture(AssetSP texture);
	void setTexture(const Path& logicPath);

	inline const Vector2& anchor() const { return _anchor; }
	inline void setAnchor(const Vector2& anchor) { _anchor = anchor; }

	inline const Vector4& color() const { return _color; }
	inline void setColor(const Vector4& color) { _color = color; }

	inline const Vector2i& tileGridSize() const { return _tileGridSize; }
	inline void setTileGridSize(const Vector2i& size) { _tileGridSize = size; }

	inline unsigned tileIndex() const { return _tileIndex; }
	inline void setTileIndex(unsigned index) { _tileIndex = index; }

	inline const Box2& view() const { return _view; }
	inline void setView(const Box2& view) { _view = view; }

	inline BlendingMode blendingMode() const { return _blendingMode; }
	inline void setBlendingMode(BlendingMode bm) { _blendingMode = bm; }

	inline unsigned textureFlags() const { return _textureFlags; }
	inline void setTextureFlags(unsigned flags) { _textureFlags = flags; }

	virtual void destroy();
	virtual void clone(EntityRef& target);

	Box2 _texCoords() const;

	static inline SpriteComponent*& _getEntityComponent(_Entity* entity) {
		return entity->sprite;
	}

protected:
	SpriteComponentManager* _manager;
	TextureAspectWP _texture;
	Vector2         _anchor;
	Vector4         _color;
	Vector2i        _tileGridSize;
	unsigned        _tileIndex;
	Box2            _view;
	BlendingMode    _blendingMode;
	unsigned        _textureFlags;
};


class SpriteComponentManager : public ComponentManager<SpriteComponent> {
public:
	SpriteComponentManager(Renderer* renderer, AssetManager* assetManager,
	                       LoaderManager* loaderManager,
	                       size_t componentBlockSize = 1024);
	SpriteComponentManager(const SpriteComponentManager&) = delete;
	SpriteComponentManager(SpriteComponentManager&&)      = delete;
	~SpriteComponentManager();

	SpriteComponentManager& operator=(const SpriteComponentManager&) = delete;
	SpriteComponentManager& operator=(SpriteComponentManager&&)      = delete;

	virtual void addComponentFromJson(EntityRef entity, const Json::Value& json,
	                                  const Path& cd=Path());
	virtual void cloneComponent(EntityRef base, EntityRef entity);

	void render(float interp, const OrthographicCamera& camera);

	Renderer* renderer();
	AssetManager* assets();
	LoaderManager* loader();

protected:
	Renderer*        _renderer;
	AssetManager*    _assets;
	LoaderManager*   _loader;

	VertexFormat     _spriteFormat;
	ProgramObject    _defaultShaderProg;
	SpriteShader     _defaultShader;

	VertexBuffer     _buffer;
};


}


#endif
