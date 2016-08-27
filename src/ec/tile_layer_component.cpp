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
	, _layerIndex(0)
	, _blendingMode(BLEND_NONE)
	, _textureFlags(Texture::NEAREST | Texture::REPEAT) {
}



TileLayerComponentManager::TileLayerComponentManager(RenderPass* renderPass,
                          SpriteRenderer* spriteRenderer,
                          size_t componentBlockSize)
	: DenseComponentManager<TileLayerComponent>("tile_layer", componentBlockSize)
	, _spriteRenderer(spriteRenderer)
	, _renderPass(renderPass)
	, _states()
	, _params(nullptr) {
}


TileLayerComponent* TileLayerComponentManager::addComponentFromJson(EntityRef entity, const Json::Value& json,
                                  const Path& cd) {
	lairAssert(false);
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

	_params = _spriteRenderer->addShaderParameters(
	            _spriteRenderer->shader(), camera.transform(), 0);

	_render(entity, interp, camera);

	_params = nullptr;
}


void TileLayerComponentManager::_render(EntityRef entity, float interp, const OrthographicCamera& camera) {
	if ( !entity.isEnabled() )
		return;

	TileLayerComponent* comp = get(entity);
	if(comp && comp->isEnabled()
	&& comp->tileMap() && comp->tileMap()->tileSet() && comp->tileMap()->tileSet().get()
	&& comp->tileMap()->tileSet()->get()->isValid()) {
		TileMapSP tileMap = comp->tileMap();
		unsigned  layer   = comp->layerIndex();

		// FIXME: Find a way to get rid of this - we should not upload stuff here.
		TextureAspectSP texAspect = tileMap->tileSet()->asset()->aspect<TextureAspect>();
		if(!texAspect || !texAspect->get()) {
			texAspect = _spriteRenderer->createTexture(tileMap->tileSet()->asset());
			_spriteRenderer->renderer()->uploadPendingTextures();
		}
		TextureSP tex = texAspect->get();

		Matrix4 wt = lerp(interp,
		                  comp->_entity()->prevWorldTransform.matrix(),
		                  comp->_entity()->worldTransform.matrix());

		unsigned width      = tileMap->width (layer);
		unsigned height     = tileMap->height(layer);
		Vector2i nTiles(tileMap->tileSetHTiles(), tileMap->tileSetVTiles());
		float    tileWidth  = float(tex->width())  / float(nTiles(0));
		float    tileHeight = float(tex->height()) / float(nTiles(1));
		unsigned first = _spriteRenderer->indexCount();
		for(unsigned y = 0; y < height; ++y) {
			for(unsigned x = 0; x < width; ++x) {
				TileMap::TileIndex tile = tileMap->tile(x, y, layer);
				if(tile == 0)
					continue;

				unsigned index = _spriteRenderer->vertexCount();
				Box2 tc  = tileBox(nTiles, tile - 1);
				for(unsigned vi = 0; vi < 4; ++vi) {
					unsigned x2 = (vi & 0x01)? 1: 0;
					unsigned y2 = (vi & 0x02)? 1: 0;
					Vector4 pos = wt * Vector4((         x + x2) * tileWidth,
					                           (height - y - y2) * tileHeight, 0, 1);
					_spriteRenderer->addVertex(pos, Vector4::Constant(1),
					                           tc.corner(Box2::CornerType(x2 + y2*2)));
				}

				_spriteRenderer->addIndex(index + 0);
				_spriteRenderer->addIndex(index + 1);
				_spriteRenderer->addIndex(index + 2);
				_spriteRenderer->addIndex(index + 2);
				_spriteRenderer->addIndex(index + 1);
				_spriteRenderer->addIndex(index + 3);
			}
		}
		unsigned count = _spriteRenderer->indexCount() - first;

		if(count) {
			_states.texture      = tex;
			_states.textureFlags = comp->textureFlags();
			_states.blendingMode = comp->blendingMode();

			float depth = 1.f - normalize(wt(2, 3), camera.viewBox().min()(2),
			                                        camera.viewBox().max()(2));
			_renderPass->addDrawCall(_states, _params, depth, first, count);
		}
	}

	EntityRef child = entity.firstChild();
	while(child.isValid()) {
		render(child, interp, camera);
		child = child.nextSibling();
	}
}


}
