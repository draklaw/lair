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

#include <lair/render_gl3/orthographic_camera.h>

#include <lair/ec/sprite_renderer.h>

#include "lair/ec/tile_layer_component.h"


namespace lair
{


TileLayerComponent::TileLayerComponent(Manager* manager, _Entity* entity)
    : Component(manager, entity)
    , _tileMap()
    , _layerIndex(0)
    , _blendingMode(BLEND_NONE)
    , _vBuffer(new BufferObject(manager->spriteRenderer()->renderer()))
    , _iBuffer(new BufferObject(manager->spriteRenderer()->renderer())) {

	const VertexAttrib spriteVertexAttribs[] = {
	    { _vBuffer.get(), VxPosition, 4, gl::FLOAT, false,
	      offsetof(SpriteVertex, position) },
	    { _vBuffer.get(), VxColor,    4, gl::FLOAT, false,
	      offsetof(SpriteVertex, color) },
	    { _vBuffer.get(), VxTexCoord, 2, gl::FLOAT, false,
	      offsetof(SpriteVertex, texCoord) },
	    LAIR_VERTEX_ATTRIB_END
	};

	_vertexArray = manager->spriteRenderer()->renderer()->createVertexArray(
	                sizeof(SpriteVertex), spriteVertexAttribs, _iBuffer.get());
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


void TileLayerComponent::setTextureSet(const TextureSet& textureSet) {
	_textureSet = manager()->spriteRenderer()->getTextureSet(textureSet);
}


TextureAspectSP TileLayerComponent::tileSet() const {
	return _textureSet? _textureSet->getTextureAspect(TexColor): nullptr;
}


void TileLayerComponent::_setTileSet(TextureAspectSP texture) {
	SamplerSP sampler = _textureSet? _textureSet->getSampler(TexColor):
	                                 manager()->spriteRenderer()->defaultSampler();
	_textureSet = manager()->spriteRenderer()->getTextureSet(
	                  TexColor, texture, sampler);
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
		props.addProperty("blend", blendingModeInfo(),
		                  &TileLayerComponent::blendingMode,
		                  &TileLayerComponent::setBlendingMode);
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


void TileLayerComponentManager::render(EntityRef entity, float interp, const OrthographicCamera& camera) {
	compactArray();

	_states.shader = _spriteRenderer->shader()->get();

	_render(entity, interp, camera);
}


SpriteRenderer* TileLayerComponentManager::spriteRenderer() {
	return _spriteRenderer;
}


LoaderManager* TileLayerComponentManager::loader() {
	return _loader;
}


unsigned TileLayerComponentManager::_fillBuffer(BufferObject& vBuffer, BufferObject& iBuffer,
                                                const TileMap& tileMap, unsigned layerIndex,
                                                const Matrix4& wt) const {
	TileLayerCSP layer = tileMap.tileLayer(layerIndex);

	unsigned offsetX = layer->offsetInTiles()(0);
	unsigned offsetY = layer->offsetInTiles()(1);

	unsigned width   = layer->widthInTiles();
	unsigned height  = layer->heightInTiles();

	unsigned tileWidth  = layer->tileWidthInPixels();
	unsigned tileHeight = layer->tileHeightInPixels();

	unsigned nVertices = width * height * 4;
	unsigned nIndices  = width * height * 6;

	vBuffer.beginWrite(nVertices * sizeof(SpriteVertex));
	iBuffer.beginWrite(nIndices * sizeof(unsigned));

	Vector2i nTiles(tileMap.tileSetHTiles(), tileMap.tileSetVTiles());
	for(unsigned y = 0; y < height; ++y) {
		for(unsigned x = 0; x < width; ++x) {
			TileMap::TileIndex tile = layer->tile(x, y);
			TileMap::TileIndex gid  = tile & TileMap::GID_MASK;
			if(gid == 0)
				continue;

			unsigned index = vBuffer.pos() / sizeof(SpriteVertex);
			Box2 tc  = boxView(tileBox(nTiles, gid - 1),
			                   Box2(Vector2(0.001, 0.001), Vector2(0.999, 0.999)));
			for(unsigned vi = 0; vi < 4; ++vi) {
				unsigned x2 = (vi & 0x01)? 1: 0;
				unsigned y2 = (vi & 0x02)? 1: 0;

				unsigned tx = x2;
				unsigned ty = y2;
				tx ^= (tile & TileMap::HFLIP_FLAG) >> 31;
				ty ^= (tile & TileMap::VFLIP_FLAG) >> 30;
				if(tile & TileMap::DFLIP_FLAG) {
					std::swap(tx, ty);
				}

				Vector4 pos = wt * Vector4((         x + x2 + offsetX) * tileWidth,
				                           (height - y - y2 - offsetY) * tileHeight, 0, 1);
				vBuffer.write(SpriteVertex{ pos, Vector4::Constant(1),
				                            tc.corner(Box2::CornerType(tx + ty*2)) });
			}

			iBuffer.write(index + 0);
			iBuffer.write(index + 1);
			iBuffer.write(index + 2);
			iBuffer.write(index + 2);
			iBuffer.write(index + 1);
			iBuffer.write(index + 3);
		}
	}

	// FIXME: Check return value and retry.
	vBuffer.endWrite();
	iBuffer.endWrite();

	return nIndices;
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
	TextureSetCSP       textureSet;
	const Texture*      texColor = nullptr;

	if(comp && comp->isEnabled()) {
		tileMapAspect = comp->tileMap();
		if(tileMapAspect && !tileMapAspect->isValid()) {
			tileMapAspect->warnIfInvalid(_loader->log());
			tileMapAspect.reset();
		}
	}

	if(tileMapAspect) {
		textureSet = comp->textureSet();
		texColor = textureSet? textureSet->getTextureOrWarn(TexColor, _loader->log()): nullptr;
		if(!texColor) {
			textureSet = _spriteRenderer->defaultTextureSet();
			texColor = textureSet->getTexture(TexColor);
		}
	}

	if(texColor) {
		const TileMap& tileMap = tileMapAspect->get();
		unsigned       layer   = comp->layerIndex();

		// FIXME: wt should be applied as a shader parameter
		Matrix4 wt = lerp(interp,
						  comp->_entity()->prevWorldTransform.matrix(),
						  comp->_entity()->worldTransform.matrix());

		if(comp->_bufferDirty) {
			comp->_vertexCount = _fillBuffer(*comp->_vBuffer, *comp->_iBuffer,
			                                 tileMap, layer, wt);
			comp->_bufferDirty = false;
		}

		if(comp->_vertexCount) {
			_states.vertices     = comp->_vertexArray.get();
			_states.textureSet   = textureSet;
			_states.blendingMode = comp->blendingMode();

			Vector4i tileInfo(tileMap.tileSetHTiles(), tileMap.tileSetVTiles(),
			                  texColor->width(), texColor->height());
			const ShaderParameter* params = _spriteRenderer->addShaderParameters(
			            _spriteRenderer->shader(), camera.transform(), 0, tileInfo);

			float depth = 1.f - normalize(wt(2, 3), camera.viewBox().min()(2),
			                                        camera.viewBox().max()(2));
			_renderPass->addDrawCall(_states, params, depth, 0, comp->_vertexCount);
		}
	}

	EntityRef child = entity.firstChild();
	while(child.isValid()) {
		render(child, interp, camera);
		child = child.nextSibling();
	}
}


}
