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


#ifndef _LAIR_EC_TILE_LAYER_COMPONENT_H
#define _LAIR_EC_TILE_LAYER_COMPONENT_H


#include <lair/core/lair.h>

#include <lair/render_gl3/render_pass.h>
#include <lair/render_gl3/texture.h>

#include <lair/ec/component.h>
#include <lair/ec/dense_component_manager.h>

#include <lair/utils/tile_map.h>


namespace lair
{


class OrthographicCamera;

class SpriteRenderer;

class TileLayerComponentManager;



class TileLayerComponent : public Component, WithProperties<TileLayerComponent> {
public:
	typedef TileLayerComponentManager Manager;

public:
	TileLayerComponent(Manager* manager, _Entity* entity);
	TileLayerComponent(const TileLayerComponent&) = delete;
	TileLayerComponent(TileLayerComponent&&)      = default;
	virtual ~TileLayerComponent() = default;

	TileLayerComponent& operator=(const TileLayerComponent&) = delete;
	TileLayerComponent& operator=(TileLayerComponent&&)      = default;

	Manager* manager();

	inline TileMapAspectSP tileMap() const { return _tileMap; }
	inline const Path& tileMapPath() const {
		return tileMap()? tileMap()->asset()->logicPath(): emptyPath;
	}
	inline void setTileMap(TileMapAspectSP tileMap) { _tileMap = tileMap; _bufferDirty = true; }
	void setTileMap(AssetSP tileMap);
	void setTileMap(const Path& logicPath);

	inline TextureAspectSP tileSet() const { return _tileSet.lock(); }
	inline void _setTileSet(TextureAspectSP tileSet) { _tileSet = tileSet; }

	inline unsigned layerIndex() const { return _layerIndex; }
	inline void setLayerIndex(unsigned index) { _layerIndex = index; _bufferDirty = true; }

	inline BlendingMode blendingMode() const { return _blendingMode; }
	inline void setBlendingMode(BlendingMode bm) { _blendingMode = bm; }

	inline unsigned textureFlags() const { return _textureFlags; }
	inline void setTextureFlags(unsigned flags) { _textureFlags = flags; }

	static const PropertyList& properties();

protected:
	TileMapAspectSP _tileMap;
	TextureAspectWP _tileSet;
	unsigned        _layerIndex;
	BlendingMode    _blendingMode;
	unsigned        _textureFlags;

public:
	bool         _bufferDirty;
	std::unique_ptr<VertexBuffer> _buffer;
};


class TileLayerComponentManager : public DenseComponentManager<TileLayerComponent> {
public:
	TileLayerComponentManager(LoaderManager* loaderManager,
	                          RenderPass* renderPass,
	                          SpriteRenderer* spriteRenderer,
	                          size_t componentBlockSize = 16);

	TileLayerComponentManager(const TileLayerComponentManager&) = delete;
	TileLayerComponentManager(TileLayerComponentManager&&)      = delete;

	virtual ~TileLayerComponentManager() = default;

	TileLayerComponentManager& operator=(const TileLayerComponentManager&) = delete;
	TileLayerComponentManager& operator=(TileLayerComponentManager&&)      = delete;

	virtual TileLayerComponent* addComponentFromJson(EntityRef entity, const Json::Value& json,
	                                  const Path& cd=Path());
	virtual TileLayerComponent* cloneComponent(EntityRef base, EntityRef entity);

	void createTextures();

	void render(EntityRef entity, float interp, const OrthographicCamera& camera);

	LoaderManager* loader();

protected:
	void _fillBuffer(VertexBuffer& buffer, const TileMap& tileMap, unsigned layer,
	                 float tileWidth, float tileHeight, const Matrix4& wt) const;
	void _render(EntityRef entity, float interp, const OrthographicCamera& camera);

protected:
	SpriteRenderer*  _spriteRenderer;
	LoaderManager*   _loader;
	RenderPass*      _renderPass;

	RenderPass::DrawStates _states;
};


}


#endif
