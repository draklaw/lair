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

#include <lair/utils/tile_map.h>

#include <lair/render_gl2/orthographic_camera.h>

#include <lair/ec/sprite_renderer.h>

#include "lair/ec/tile_layer_component.h"


namespace lair
{


TileLayerComponent::TileLayerComponent(Manager* manager, _Entity* entity)
    : Component(manager, entity)
    , _tileMap()
    , _tileSet()
    , _layerIndex(0)
    , _blendingMode(BLEND_NONE)
    , _textureFlags(Texture::BILINEAR_NO_MIPMAP | Texture::REPEAT)
    , _buffer(new VertexBuffer(sizeof(SpriteVertex))) {
}


TileLayerComponent::Manager* TileLayerComponent::manager() {
	return static_cast<Manager*>(_manager);
}


void TileLayerComponent::setTileMap(AssetSP tileMap) {
	if(tileMap) {
		TileMapAspectSP aspect = tileMap->aspect<TileMapAspect>();
		setTileMap(aspect);
	}
	else {
		setTileMap(TileMapAspectSP());
	}
}


void TileLayerComponent::setTileMap(const Path& logicPath) {
	AssetSP asset = manager()->loader()->loadAsset<TileMapLoader>(logicPath);
	setTileMap(asset);
}


const PropertyList& TileLayerComponent::properties() {
	static PropertyList props;
	if(props.nProperties() == 0) {
		props.addProperty("tile_map",
		                  &TileLayerComponent::tileMapPath,
		                  &TileLayerComponent::setTileMap);
		props.addProperty("layer_index",
		                  &TileLayerComponent::layerIndex,
		                  &TileLayerComponent::setLayerIndex);
		props.addProperty("blend",
		                  &TileLayerComponent::blendingMode,
		                  &TileLayerComponent::setBlendingMode);
		props.addProperty("texture_flags",
		                  &TileLayerComponent::textureFlags,
		                  &TileLayerComponent::setTextureFlags);
	}
	return props;
}




TileLayerComponentManager::TileLayerComponentManager(
        LoaderManager* loaderManager,
        RenderPass* renderPass,
        SpriteRenderer* spriteRenderer,
        size_t componentBlockSize)
    : DenseComponentManager<TileLayerComponent>("tile_layer", componentBlockSize)
    , _spriteRenderer(spriteRenderer)
    , _loader(loaderManager)
    , _renderPass(renderPass)
    , _states() {
}


TileLayerComponent* TileLayerComponentManager::addComponentFromJson(EntityRef /*entity*/, const Json::Value& /*json*/,
                                  const Path& /*cd*/) {
	lairAssert(false);
  return nullptr;
}


TileLayerComponent* TileLayerComponentManager::cloneComponent(EntityRef base, EntityRef entity) {
	TileLayerComponent* baseComp = get(base);
	TileLayerComponent* comp = _addComponent(entity, baseComp);
	comp->setTileMap(     baseComp->tileMap());
	comp->setLayerIndex(  baseComp->layerIndex());
	comp->setBlendingMode(baseComp->blendingMode());
	comp->setTextureFlags(baseComp->textureFlags());
	return comp;
}


void TileLayerComponentManager::render(EntityRef entity, float interp, const OrthographicCamera& camera) {
	compactArray();

	_states.shader = _spriteRenderer->shader().shader;
	_states.buffer = _spriteRenderer->buffer();
	_states.format = _spriteRenderer->format();

	_render(entity, interp, camera);
}


LoaderManager* TileLayerComponentManager::loader() {
	return _loader;
}


void TileLayerComponentManager::_fillBuffer(VertexBuffer& buffer, const TileMap& tileMap, unsigned layer,
                                            float tileWidth, float tileHeight, const Matrix4& wt) const {
	buffer.clear();

	unsigned width      = tileMap.width (layer);
	unsigned height     = tileMap.height(layer);
	Vector2i nTiles(tileMap.tileSetHTiles(), tileMap.tileSetVTiles());
	for(unsigned y = 0; y < height; ++y) {
		for(unsigned x = 0; x < width; ++x) {
			TileMap::TileIndex tile = tileMap.tile(x, y, layer);
			if(tile == 0)
				continue;

			unsigned index = buffer.vertexCount();
			Box2 tc  = boxView(tileBox(nTiles, tile - 1),
			                   Box2(Vector2(0.001, 0.001), Vector2(0.999, 0.999)));
			for(unsigned vi = 0; vi < 4; ++vi) {
				unsigned x2 = (vi & 0x01)? 1: 0;
				unsigned y2 = (vi & 0x02)? 1: 0;
				Vector4 pos = wt * Vector4((         x + x2) * tileWidth,
				                           (height - y - y2) * tileHeight, 0, 1);
				buffer.addVertex(SpriteVertex{ pos, Vector4::Constant(1),
				                               tc.corner(Box2::CornerType(x2 + y2*2)) });
			}

			buffer.addIndex(index + 0);
			buffer.addIndex(index + 1);
			buffer.addIndex(index + 2);
			buffer.addIndex(index + 2);
			buffer.addIndex(index + 1);
			buffer.addIndex(index + 3);
		}
	}
}


void TileLayerComponentManager::createTextures() {
	for(TileLayerComponent& comp: *this) {
		TileMapAspectSP tileMap = comp.tileMap();
		if(tileMap && !comp.tileSet() && tileMap->isValid())
			comp._setTileSet(_spriteRenderer->createTexture(tileMap->get().tileSet()->asset()));
	}
}


void TileLayerComponentManager::_render(EntityRef entity, float interp, const OrthographicCamera& camera) {
	if ( !entity.isEnabled() )
		return;

	TileLayerComponent* comp = get(entity);
	TileMapAspectSP     tileMapAspect;
	TextureAspectSP     texAspect;

	if(comp && comp->isEnabled()) {
		tileMapAspect = comp->tileMap();
		if(tileMapAspect && !tileMapAspect->isValid()) {
			tileMapAspect->warnIfInvalid(_loader->log());
			tileMapAspect.reset();
		}
	}

	if(tileMapAspect) {
		texAspect = comp->tileSet();
		if(texAspect && !texAspect->isValid()) {
			texAspect->warnIfInvalid(_loader->log());
			texAspect.reset();
		}
	}

	if(texAspect) {
		const TileMap& tileMap = tileMapAspect->get();
		unsigned       layer   = comp->layerIndex();

		Texture& tex = texAspect->_get();

		// FIXME: wt should be applied as a shader parameter
		Matrix4 wt = lerp(interp,
						  comp->_entity()->prevWorldTransform.matrix(),
						  comp->_entity()->worldTransform.matrix());

		if(comp->_bufferDirty) {
			float tileWidth  = float(tex.width())  / float(tileMap.tileSetHTiles());
			float tileHeight = float(tex.height()) / float(tileMap.tileSetVTiles());
			_fillBuffer(*comp->_buffer, tileMap, layer, tileWidth, tileHeight, wt);
			comp->_bufferDirty = false;
		}
		unsigned count = comp->_buffer->indexSize();

		if(count) {
			_states.buffer       = comp->_buffer.get();
			_states.texture      = &tex;
			_states.textureFlags = comp->textureFlags();
			_states.blendingMode = comp->blendingMode();

			Vector4i tileInfo(tileMap.tileSetHTiles(), tileMap.tileSetVTiles(),
			                  tex.width(), tex.height());
			const ShaderParameter* params = _spriteRenderer->addShaderParameters(
			            _spriteRenderer->shader(), camera.transform(), 0, tileInfo);

			float depth = 1.f - normalize(wt(2, 3), camera.viewBox().min()(2),
			                                        camera.viewBox().max()(2));
			_renderPass->addDrawCall(_states, params, depth, 0, count);
		}
	}

	EntityRef child = entity.firstChild();
	while(child.isValid()) {
		render(child, interp, camera);
		child = child.nextSibling();
	}
}


}
