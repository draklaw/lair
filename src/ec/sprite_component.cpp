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
      _texture(),
      _anchor(0, 0),
      _color(1, 1, 1, 1),
      _tileGridSize(1, 1),
      _tileIndex(0),
      _view(Vector2(0, 0), Vector2(1, 1)),
      _blendingMode(BLEND_NONE),
      _textureFlags(Texture::BILINEAR_NO_MIPMAP | Texture::REPEAT) {
}


SpriteComponent::~SpriteComponent() {
}


SpriteComponent::Manager* SpriteComponent::manager() {
	return static_cast<Manager*>(_manager);
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
		props.addProperty("texture_flags", Texture::flagsInfo(),
		                  &SpriteComponent::textureFlags,
		                  &SpriteComponent::setTextureFlags);
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
	       && (     c0->_texture.owner_before(c1->_texture)
	           || (!c1->_texture.owner_before(c0->_texture)
	               && c0->_textureFlags < c1->_textureFlags)));
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


SpriteComponent* SpriteComponentManager::addComponentFromJson(EntityRef entity, const Json::Value& json,
                                                  const Path& cd) {
	SpriteComponent* comp = addComponent(entity);
	if(json.isMember("sprite")) {
		comp->setTexture(makeAbsolute(cd, json["sprite"].asString()));
	}
	if(json.isMember("anchor")) {
		Json::Value anchor = json["anchor"];
		if(anchor.isArray() || anchor.size() == 2) {
			comp->setAnchor(Vector2(anchor[0].asFloat(), anchor[1].asFloat()));
		} /*else {
			log().warning("Invalid anchor field while loading entity \"", entity.name(), "\".");
		}*/
	}
	if(json.isMember("color")) {
		Json::Value color = json["color"];
		if(color.isArray() || color.size() == 4) {
			comp->setColor(Vector4(color[0].asFloat(), color[1].asFloat(),
			               color[2].asFloat(), color[3].asFloat()));
		} /*else {
			log().warning("Invalid anchor field while loading entity \"", entity.name(), "\".");
		}*/
	}
	comp->setTileGridSize(Vector2i(json.get("h_tiles", 1).asInt(),
	                               json.get("v_tiles", 1).asInt()));
	comp->setTileIndex(json.get("index", 0).asInt());
	if(json.isMember("view")) {
		Json::Value view = json["view"];
		if(view.isArray() || view.size() == 4) {
			comp->setView(Box2(Vector2(view[0].asFloat(), view[1].asFloat()),
			        Vector2(view[2].asFloat(), view[3].asFloat())));
		} /*else {
			log().warning("Invalid anchor field while loading entity \"", entity.name(), "\".");
		}*/
	}
	if(json.isMember("blend")) {
		std::string blend = json["blend"].asString();
		if(blend == "none") {
			comp->setBlendingMode(BLEND_NONE);
		}
		else if(blend == "alpha") {
			comp->setBlendingMode(BLEND_ALPHA);
		}
		else if(blend == "add") {
			comp->setBlendingMode(BLEND_ADD);
		}
		else if(blend == "multiply") {
			comp->setBlendingMode(BLEND_MULTIPLY);
		}
	}
	if(json.isMember("texture_flags")) {
		std::string flags = json["texture_flags"].asString();
		if(flags == "nearest") {
			comp->setTextureFlags(Texture::NEAREST | Texture::REPEAT);
		}
		if(flags == "bilinear_no_mipmap") {
			comp->setTextureFlags(Texture::BILINEAR_NO_MIPMAP | Texture::REPEAT);
		}
		if(flags == "bilinear_mipmap") {
			comp->setTextureFlags(Texture::BILINEAR_MIPMAP | Texture::REPEAT);
		}
		if(flags == "trilinear") {
			comp->setTextureFlags(Texture::TRILINEAR | Texture::REPEAT);
		}
	}
	return comp;
}


void SpriteComponentManager::render(EntityRef entity, float interp, const OrthographicCamera& camera) {
//	compactArray();

	_states.shader = _spriteRenderer->shader().shader;
	_states.buffer = _spriteRenderer->buffer();
	_states.format = _spriteRenderer->format();

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
	TextureAspectSP  texAspect;

	if(sc && sc->isEnabled()) {
		texAspect = sc->texture();
		if(texAspect && !texAspect->isValid()) {
			texAspect->warnIfInvalid(_loader->log());
			texAspect = _spriteRenderer->defaultTexture();
		}
	}

	if(texAspect) {
		const Texture& tex = texAspect->get();

		Matrix4 wt = lerp(interp,
		                  sc->_entity()->prevWorldTransform.matrix(),
		                  sc->_entity()->worldTransform.matrix());

		Box2 texCoords = sc->_texCoords();
		Scalar w = tex.width()  * texCoords.sizes()(0);
		Scalar h = tex.height() * texCoords.sizes()(1);
		Vector2 offset(-w * sc->anchor().x(),
		               -h * sc->anchor().y());
		Box2 coords(offset, Vector2(w, h) + offset);

		texCoords = boxView(texCoords, Box2(Vector2(0.001, 0.001), Vector2(0.999, 0.999)));

		unsigned index = _spriteRenderer->indexCount();
		_spriteRenderer->addSprite(wt, coords, sc->color(), texCoords);
		unsigned count = _spriteRenderer->indexCount() - index;

		if(count) {
			_states.texture      = &texAspect->_get();
			_states.textureFlags = sc->textureFlags();
			_states.blendingMode = sc->blendingMode();

			Vector4i tileInfo;
			tileInfo << sc->tileGridSize(), tex.width(), tex.height();
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
