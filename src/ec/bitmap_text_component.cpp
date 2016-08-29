/*
 *  Copyright (C) 2015, 2016 Simon Boyé
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


#include <lair/core/json.h>
#include <lair/core/bitmap_font.h>

#include <lair/sys_sdl2/image_loader.h>

#include <lair/render_gl2/texture.h>

#include <lair/ec/sprite_renderer.h>

#include "lair/ec/bitmap_text_component.h"


namespace lair {


BitmapFontLoader::BitmapFontLoader(LoaderManager* manager, AspectSP aspect)
    : Loader(manager, aspect) {
}


BitmapFontLoader::~BitmapFontLoader() {
}


void BitmapFontLoader::loadSyncImpl(Logger& log) {
	Json::Value json;
	if(!parseJson(json, realPath(), asset()->logicPath(), log)) {
		return;
	}

	Json::Value imgPathValue = json.get("image", Json::nullValue);
	Path imgPath = imgPathValue.isString()? imgPathValue.asString():
	                                        asset()->logicPath().withExtension("png");
	imgPath = make_absolute(asset()->logicPath().dir(), imgPath);

	AssetSP imgAsset = _manager->loadSync<ImageLoader>(imgPath);

	BitmapFontAspectSP aspect = std::static_pointer_cast<BitmapFontAspect>(_aspect);
	aspect->_set(std::make_shared<BitmapFont>(json, imgAsset));

	_success();
}


//---------------------------------------------------------------------------//


BitmapTextComponent::BitmapTextComponent(Manager* manager, _Entity* entity)
    : Component(manager, entity),
      _font(),
      _texture(),
      _text(),
      _color(1, 1, 1, 1),
      _size(0, 0),
      _anchor(0, 0) {
}


BitmapTextComponent::Manager* BitmapTextComponent::manager() {
	return static_cast<Manager*>(_manager);
}


void BitmapTextComponent::setFont(BitmapFontAspectSP font) {
	_font = font;
}


void BitmapTextComponent::setFont(AssetSP font) {
	setFont(font? font->aspect<BitmapFontAspect>(): BitmapFontAspectSP());
}


void BitmapTextComponent::setFont(const Path& logicPath) {
	AssetSP asset = manager()->loader()->loadAsset<BitmapFontLoader>(logicPath);
	setFont(asset);
}


//---------------------------------------------------------------------------//


BitmapTextComponentManager::BitmapTextComponentManager(
        LoaderManager* loaderManager,
        RenderPass* renderPass,
        SpriteRenderer* spriteRenderer,
        size_t componentBlockSize)
	: DenseComponentManager("text", componentBlockSize),
      _loader(loaderManager),
      _renderPass(renderPass),
      _spriteRenderer(spriteRenderer),
      _states() {
	lairAssert(_loader);
	lairAssert(_renderPass);
	lairAssert(_spriteRenderer);
}


BitmapTextComponentManager::~BitmapTextComponentManager() {
}


BitmapTextComponent* BitmapTextComponentManager::addComponentFromJson(
        EntityRef entity, const Json::Value& json, const Path& cd) {
	BitmapTextComponent* comp = addComponent(entity);
	if(json.isMember("font")) {
		comp->setFont(make_absolute(cd, json["font"].asString()));
	}
	if(json.isMember("text")) {
		comp->setText(json["text"].asString());
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
	if(json.isMember("size")) {
		Json::Value size = json["size"];
		if(size.isArray() || size.size() == 2) {
			comp->setSize(Vector2i(size[0].asInt(), size[1].asInt()));
		} /*else {
			log().warning("Invalid anchor field while loading entity \"", entity.name(), "\".");
		}*/
	}
	if(json.isMember("anchor")) {
		Json::Value anchor = json["anchor"];
		if(anchor.isArray() || anchor.size() == 2) {
			comp->setAnchor(Vector2(anchor[0].asFloat(), anchor[1].asFloat()));
		} /*else {
			log().warning("Invalid anchor field while loading entity \"", entity.name(), "\".");
		}*/
	}
	return comp;
}


BitmapTextComponent* BitmapTextComponentManager::cloneComponent(EntityRef base, EntityRef entity) {
	BitmapTextComponent* baseComp = get(base);
	BitmapTextComponent* comp = _addComponent(entity, baseComp);
	comp->setFont( baseComp->font());
	comp->setText( baseComp->text());
	comp->setColor(baseComp->color());
	return comp;
}


//void BitmapTextComponentManager::render(float interp, const OrthographicCamera& camera) {
//	compactArray();
//	//sortArray(TODO);

//	RenderPass::DrawStates states;
//	states.shader = _spriteRenderer->shader().shader;
//	states.buffer = _spriteRenderer->buffer();
//	states.format = _spriteRenderer->format();
//	states.textureFlags = Texture::NEAREST | Texture::CLAMP;
//	states.blendingMode = BLEND_ALPHA;

//	const ShaderParameter* params = _spriteRenderer->addShaderParameters(
//	            _spriteRenderer->shader(), camera.transform(), 0);

//	for(auto& entityComp: *this) {
//		BitmapTextComponent& comp = entityComp;

//		if(!comp.isEnabled()
//		|| !comp.font()    || !comp.font()   ->get() || !comp.font()   ->get()->isValid()
//		/*|| !comp.texture() || !comp.texture()->get() || !comp.texture()->get()->isValid()*/) {
//			continue;
//		}
//		BitmapFontSP font = comp.font()->get();

//		Matrix4 wt = lerp(interp,
//						  comp._entity()->prevWorldTransform.matrix(),
//						  comp._entity()->worldTransform.matrix());

//		// FIXME: Find a way to get rid of this - we should not upload stuff here.
//		if(!comp.texture()) {
//			comp._setTexture(_spriteRenderer->createTexture(font->image()));
//			_spriteRenderer->renderer()->uploadPendingTextures();
//		}
//		TextureSP tex = comp.texture()->_get();

//		int width = (comp.size()(0) > 0)? comp.size()(0): 999999;
//		TextLayout layout = font->layoutText(comp.text(), width);
//		unsigned index = _spriteRenderer->indexCount();
//		for(unsigned i = 0; i < layout.nGlyphs(); ++i) {
//			unsigned cp = layout.glyph(i).codepoint;
//			Vector2 pos = layout.glyph(i).pos;
//			const BitmapFont::Glyph& glyph = font->glyph(cp);

//			Vector2 size = glyph.size;

//			pos(0) += glyph.offset(0);
//			pos(1) += font->height() - size(1) - glyph.offset(1)
//			        + layout.box().sizes()(1);
//			pos -= layout.box().sizes().cwiseProduct(comp.anchor());
//			Box2 coords(pos, pos + size);

//			_spriteRenderer->addSprite(wt, coords, comp.color(), glyph.region);
//		}
//		unsigned count = _spriteRenderer->indexCount() - index;

//		if(count) {
//			states.texture      = tex;

//			float depth = 1.f - normalize(wt(2, 3), camera.viewBox().min()(2),
//			                                        camera.viewBox().max()(2));
//			_renderPass->addDrawCall(states, params, depth, index, count);
//		}
//	}
//}


void BitmapTextComponentManager::render(EntityRef entity, float interp, const OrthographicCamera& camera) {
	compactArray();

	_states.shader = _spriteRenderer->shader().shader;
	_states.buffer = _spriteRenderer->buffer();
	_states.format = _spriteRenderer->format();
	_states.textureFlags = Texture::BILINEAR_NO_MIPMAP | Texture::CLAMP;
	_states.blendingMode = BLEND_ALPHA;

	_render(entity, interp, camera);
}


LoaderManager* BitmapTextComponentManager::loader() {
	return _loader;
}


SpriteRenderer* BitmapTextComponentManager::spriteRenderer() const {
	return _spriteRenderer;
}


void BitmapTextComponentManager::_render(EntityRef entity, float interp, const OrthographicCamera& camera) {
	if ( !entity.isEnabled() )
		return;

	BitmapTextComponent* comp = get(entity);
	if(comp && comp->isEnabled()
	&& comp->font() && comp->font()->get() && comp->font()->get()->isValid()) {
		BitmapFontSP font = comp->font()->get();

		Matrix4 wt = lerp(interp,
						  comp->_entity()->prevWorldTransform.matrix(),
						  comp->_entity()->worldTransform.matrix());

		// FIXME: Find a way to get rid of this - we should not upload stuff here.
		if(!comp->texture()) {
			comp->_setTexture(_spriteRenderer->createTexture(font->image()));
			_spriteRenderer->renderer()->uploadPendingTextures();
		}
		TextureSP tex = comp->texture()->_get();

		int width = (comp->size()(0) > 0)? comp->size()(0): 999999;
		TextLayout layout = font->layoutText(comp->text(), width);
		unsigned index = _spriteRenderer->indexCount();
		for(unsigned i = 0; i < layout.nGlyphs(); ++i) {
			unsigned cp = layout.glyph(i).codepoint;
			Vector2 pos = layout.glyph(i).pos;
			const BitmapFont::Glyph& glyph = font->glyph(cp);

			Vector2 size = glyph.size;

			pos(0) += glyph.offset(0);
			pos(1) += font->height() - size(1) - glyph.offset(1)
			        + layout.box().sizes()(1);
			pos -= layout.box().sizes().cwiseProduct(comp->anchor());
			Box2 coords(pos, pos + size);

			_spriteRenderer->addSprite(wt, coords, comp->color(), glyph.region);
		}
		unsigned count = _spriteRenderer->indexCount() - index;

		if(count) {
			_states.texture      = tex;

			const ShaderParameter* params = _spriteRenderer->addShaderParameters(
			            _spriteRenderer->shader(), camera.transform(), 0, Vector4i(1, 1, 65536, 65536));

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
