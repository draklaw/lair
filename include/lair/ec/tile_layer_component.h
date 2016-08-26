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

#include <lair/render_gl2/render_pass.h>

#include <lair/ec/component.h>
#include <lair/ec/dense_component_manager.h>


namespace lair
{


class TileMap;
typedef std::shared_ptr<TileMap> TileMapSP;

class OrthographicCamera;

class SpriteRenderer;

class TileLayerComponentManager;


class TileLayerComponent : public Component {
public:
	typedef TileLayerComponentManager Manager;

public:
	TileLayerComponent(Manager* manager, _Entity* entity);
	TileLayerComponent(const TileLayerComponent&) = delete;
	TileLayerComponent(TileLayerComponent&&)      = default;
	virtual ~TileLayerComponent() = default;

	TileLayerComponent& operator=(const TileLayerComponent&) = delete;
	TileLayerComponent& operator=(TileLayerComponent&&)      = default;

	inline TileMapSP tileMap() const { return _tileMap; }
	inline void setTileMap(TileMapSP tileMap) { _tileMap = tileMap; }

	inline unsigned layerIndex() const { return _layerIndex; }
	inline void setLayerIndex(unsigned index) { _layerIndex = index; }

	inline BlendingMode blendingMode() const { return _blendingMode; }
	inline void setBlendingMode(BlendingMode bm) { _blendingMode = bm; }

	inline unsigned textureFlags() const { return _textureFlags; }
	inline void setTextureFlags(unsigned flags) { _textureFlags = flags; }

protected:
	TileMapSP    _tileMap;
	unsigned     _layerIndex;
	BlendingMode _blendingMode;
	unsigned     _textureFlags;
};


class TileLayerComponentManager : public DenseComponentManager<TileLayerComponent> {
public:
	TileLayerComponentManager(RenderPass* renderPass,
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

	void render(EntityRef entity, float interp, const OrthographicCamera& camera);

protected:
	void _render(EntityRef entity, float interp, const OrthographicCamera& camera);

protected:
	SpriteRenderer*  _spriteRenderer;
	RenderPass*      _renderPass;

	RenderPass::DrawStates _states;
	const ShaderParameter* _params;
};


}


#endif
