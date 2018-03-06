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
#include <lair/core/json.h>
#include <lair/core/log.h>

#include <lair/render_gl3/orthographic_camera.h>
#include <lair/render_gl3/texture.h>
#include <lair/render_gl3/renderer.h>

#include "lair/ec/entity.h"
#include "lair/ec/entity_manager.h"

#include "lair/ec/sprite_component.h"


namespace lair
{


SpriteComponent::SpriteComponent(Manager* manager,_Entity* entity)
    : Component(manager, entity),
      _textureSet(),
      _anchor(0, 0),
      _color(1, 1, 1, 1),
      _tileGridSize(1, 1),
      _tileIndex(0),
      _view(Vector2(0, 0), Vector2(1, 1)),
      _blendingMode(BLEND_NONE) {
}


SpriteComponent::~SpriteComponent() {
}


SpriteComponent::Manager* SpriteComponent::manager() {
	return static_cast<Manager*>(_manager);
}


void SpriteComponent::setTextureSet(const TextureSet& textureSet) {
	_textureSet = manager()->spriteRenderer()->getTextureSet(textureSet);
}


TextureAspectSP SpriteComponent::texture() const {
	return _textureSet? _textureSet->getTextureAspect(TexColor): nullptr;
}


void SpriteComponent::setTexture(TextureAspectSP texture) {
	SamplerSP sampler = _textureSet? _textureSet->getSampler(TexColor): nullptr;
	_textureSet = manager()->spriteRenderer()->getTextureSet(
	                  TexColor, texture, sampler);
}


void SpriteComponent::setTexture(AssetSP texture) {
	if(texture) {
		TextureAspectSP ta = texture->aspect<TextureAspect>();
		if(!ta) {
			ta = manager()->spriteRenderer()->createTexture(texture);
		}
		setTexture(ta);
	}
	else {
		setTexture(TextureAspectSP());
	}
}


void SpriteComponent::setTexture(const Path& logicPath) {
	AssetSP asset = manager()->loader()->loadAsset<ImageLoader>(logicPath);
	setTexture(asset);
}


const PropertyList& SpriteComponent::properties() {
	static PropertyList props;
	if(props.nProperties() == 0) {
		// TODO: Support texture sets
		props.addProperty("texture",
		                  &SpriteComponent::texturePath,
		                  &SpriteComponent::setTexture);
		props.addProperty("anchor",
		                  &SpriteComponent::anchor,
		                  &SpriteComponent::setAnchor);
		props.addProperty("color",
		                  &SpriteComponent::color,
		                  &SpriteComponent::setColor);
		props.addProperty("tile_grid",
		                  &SpriteComponent::tileGridSize,
		                  &SpriteComponent::setTileGridSize);
		props.addProperty("tile_index",
		                  &SpriteComponent::tileIndex,
		                  &SpriteComponent::setTileIndex);
		props.addProperty("view",
		                  &SpriteComponent::view,
		                  &SpriteComponent::setView);
		props.addProperty("blend", blendingModeInfo(),
		                  &SpriteComponent::blendingMode,
		                  &SpriteComponent::setBlendingMode);
	}
	return props;
}


Box2 SpriteComponent::_texCoords() const {
	Vector2i nTiles = _tileGridSize.cwiseMax(Vector2i(1, 1));
	return boxView(tileBox(nTiles, _tileIndex), _view);
}

inline bool SpriteComponent::_renderCompare(SpriteComponent* c0, SpriteComponent* c1) {
	return c0->_blendingMode <  c1->_blendingMode
	   || (c0->_blendingMode == c1->_blendingMode
	       && c0->_textureSet.owner_before(c1->_textureSet));
}

//---------------------------------------------------------------------------//


SpriteComponentManager::SpriteComponentManager(AssetManager* assetManager,
                                               LoaderManager* loaderManager,
                                               RenderPass *renderPass,
                                               SpriteRenderer* spriteRenderer,
                                               size_t componentBlockSize)
    : DenseComponentManager("sprite", componentBlockSize),
      _assets(assetManager),
      _loader(loaderManager),
      _spriteRenderer(spriteRenderer),
	  _renderPass(renderPass),
      _states() {
	lairAssert(_assets);
	lairAssert(_loader);
	lairAssert(_spriteRenderer);
}


SpriteComponentManager::~SpriteComponentManager() {
}


void SpriteComponentManager::render(EntityRef entity, float interp, const OrthographicCamera& camera) {
//	compactArray();

	_states.shader   = _spriteRenderer->shader().shader;
	_states.vertices = _spriteRenderer->vertexArray();

	_render(entity, interp, camera);
}


AssetManager* SpriteComponentManager::assets() {
	return _assets;
}


LoaderManager* SpriteComponentManager::loader() {
	return _loader;
}


SpriteRenderer* SpriteComponentManager::spriteRenderer() {
	return _spriteRenderer;
}


void SpriteComponentManager::_render(EntityRef entity, float interp, const OrthographicCamera& camera) {
	if ( !entity.isEnabled() )
		return;

	SpriteComponent* sc = get(entity);
	TextureSetCSP    textureSet;
	const Texture*   texColor = nullptr;

	if(sc && sc->isEnabled()) {
		textureSet = sc->textureSet();
		texColor   = textureSet? textureSet->getTextureOrWarn(TexColor, _loader->log()): nullptr;
		if(!texColor) {
			textureSet = _spriteRenderer->defaultTextureSet();
			texColor = textureSet->getTexture(TexColor);
		}
	}

	if(texColor) {
		Matrix4 wt = lerp(interp,
		                  sc->_entity()->prevWorldTransform.matrix(),
		                  sc->_entity()->worldTransform.matrix());

		Box2 texCoords = sc->_texCoords();
		Scalar w = texColor->width()  * texCoords.sizes()(0);
		Scalar h = texColor->height() * texCoords.sizes()(1);
		Vector2 offset(-w * sc->anchor().x(),
		               -h * sc->anchor().y());
		Box2 coords(offset, Vector2(w, h) + offset);

		texCoords = boxView(texCoords, Box2(Vector2(0.001, 0.001), Vector2(0.999, 0.999)));

		unsigned index = _spriteRenderer->indexCount();
		_spriteRenderer->addSprite(wt, coords, sc->color(), texCoords);
		unsigned count = _spriteRenderer->indexCount() - index;

		if(count) {
			_states.textureSet   = textureSet;
			_states.blendingMode = sc->blendingMode();

			Vector4i tileInfo;
			tileInfo << sc->tileGridSize(), texColor->width(), texColor->height();
			const ShaderParameter* params = _spriteRenderer->addShaderParameters(
			            _spriteRenderer->shader(), camera.transform(), 0, tileInfo);

			float depth = 1.f - normalize(wt(2, 3), camera.viewBox().min()(2),
			                                        camera.viewBox().max()(2));
			_renderPass->addDrawCall(_states, params, depth, index, count);
		}
	}

	EntityRef child = entity.firstChild();
	while(child.isValid()) {
		render(child, interp, camera);
		child = child.nextSibling();
	}
}


}
