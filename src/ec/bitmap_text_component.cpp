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

#include <lair/render_gl3/texture.h>

#include <lair/ec/sprite_renderer.h>

#include "lair/ec/bitmap_text_component.h"


namespace lair {


BitmapFontLoader::BitmapFontLoader(LoaderManager* manager, AspectSP aspect)
    : Loader(manager, aspect) {
}


BitmapFontLoader::~BitmapFontLoader() {
}


void BitmapFontLoader::commit() {
	BitmapFontAspectSP aspect = std::static_pointer_cast<BitmapFontAspect>(_aspect);
	aspect->_get() = std::move(_font);
	Loader::commit();
}


void BitmapFontLoader::loadSyncImpl(Logger& log) {
	if(!parseJson(_fontDesc, realPath(), asset()->logicPath(), log)) {
		return;
	}

	Json::Value imgPathValue = _fontDesc.get("image", Json::nullValue);
	Path imgPath = imgPathValue.isString()? imgPathValue.asString():
	                                        asset()->logicPath().withExtension("png");

	_load<ImageLoader>(imgPath, [this](AspectSP imgAspect, Logger& log) {
		if(imgAspect->isValid()) {
			_font = std::move(BitmapFont(_fontDesc, imgAspect->asset()));
		}
		else {
			log.error("Error while loading BitmapFont \"", asset()->logicPath(),
			          "\": failed to load image \"", imgAspect->asset()->logicPath(), "\".");
		}
	});
}


//---------------------------------------------------------------------------//


BitmapTextComponent::BitmapTextComponent(Manager* manager, _Entity* entity)
    : Component(manager, entity),
      _font(),
      _textureSet(),
      _text(),
      _color(1, 1, 1, 1),
      _size(0, 0),
      _anchor(0, 0),
      _blendingMode(BLEND_NONE) {
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


void BitmapTextComponent::setTextureSet(const TextureSet& textureSet) {
	_textureSet = manager()->spriteRenderer()->getTextureSet(textureSet);
}


TextureAspectSP BitmapTextComponent::texture() const {
	return _textureSet? _textureSet->getTextureAspect(TexColor): nullptr;
}


void BitmapTextComponent::_setTexture(TextureAspectSP texture) {
	SamplerSP sampler = _textureSet? _textureSet->getSampler(TexColor): nullptr;
	_textureSet = manager()->spriteRenderer()->getTextureSet(
	                  TexColor, texture, sampler);
}


const PropertyList& BitmapTextComponent::properties() {
	static PropertyList props;
	if(props.nProperties() == 0) {
		props.addProperty("font",
		                  &BitmapTextComponent::fontPath,
		                  &BitmapTextComponent::setFont);
		props.addProperty("text",
		                  &BitmapTextComponent::text,
		                  &BitmapTextComponent::setText);
		props.addProperty("color",
		                  &BitmapTextComponent::color,
		                  &BitmapTextComponent::setColor);
		props.addProperty("size",
		                  &BitmapTextComponent::size,
		                  &BitmapTextComponent::setSize);
		props.addProperty("anchor",
		                  &BitmapTextComponent::anchor,
		                  &BitmapTextComponent::setAnchor);
		props.addProperty("blend", blendingModeInfo(),
		                  &BitmapTextComponent::blendingMode,
		                  &BitmapTextComponent::setBlendingMode);
	}
	return props;
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


void BitmapTextComponentManager::createTextures() {
	for(BitmapTextComponent& comp: *this) {
		BitmapFontAspectSP font = comp.font();
		if(font && !comp.texture() && font->isValid())
			comp._setTexture(_spriteRenderer->createTexture(font->get().image()));
	}
}


void BitmapTextComponentManager::render(EntityRef entity, float interp, const OrthographicCamera& camera) {
	compactArray();

	_states.shader = _spriteRenderer->shader().shader;
	_states.vertices = _spriteRenderer->vertexArray();

	_render(entity, interp, camera);
}


LoaderManager* BitmapTextComponentManager::loader() {
	return _loader;
}


SpriteRenderer* BitmapTextComponentManager::spriteRenderer() const {
	return _spriteRenderer;
}


void BitmapTextComponentManager::_render(EntityRef entity, float interp, const OrthographicCamera& camera) {
	if(!entity.isEnabled())
		return;

	BitmapTextComponent* comp = get(entity);
	BitmapFontAspectSP   fontAspect;
	TextureSetCSP        textureSet;

	if(comp && comp->isEnabled()) {
		fontAspect = comp->font();
		if(fontAspect && !fontAspect->isValid()) {
			fontAspect->warnIfInvalid(_loader->log());
			fontAspect.reset();
		}
	}

	if(fontAspect) {
		textureSet = comp->textureSet();
		if(!textureSet || !textureSet->getTextureAspectOrWarn(TexColor, _loader->log())) {
			textureSet = _spriteRenderer->defaultTextureSet();
		}
	}

	if(textureSet) {
		const BitmapFont& font = fontAspect->get();

		Matrix4 wt = lerp(interp,
						  comp->_entity()->prevWorldTransform.matrix(),
						  comp->_entity()->worldTransform.matrix());

		unsigned width = (comp->size()(0) > 0)? comp->size()(0): 999999;
		TextLayout layout = font.layoutText(comp->text(), width);

		float depth = 1.f - normalize(wt(2, 3), camera.viewBox().min()(2),
		                                        camera.viewBox().max()(2));
		renderBitmapText(_renderPass, _spriteRenderer, font, textureSet,
		                 wt, depth, layout, comp->anchor(), comp->color(),
		                 camera.transform(), comp->blendingMode());
	}

	EntityRef child = entity.firstChild();
	while(child.isValid()) {
		render(child, interp, camera);
		child = child.nextSibling();
	}
}


void renderBitmapText(RenderPass* pass, SpriteRenderer* renderer,
                      const BitmapFont& font, const TextureSetCSP& textureSet,
                      const Matrix4& transform, float depth,
                      const TextLayout& layout, const Vector2& anchor,
                      const Vector4& color, const Matrix4& viewTransform,
                      BlendingMode blendingMode) {
	unsigned index = renderer->indexCount();
	for(unsigned i = 0; i < layout.nGlyphs(); ++i) {
		unsigned cp = layout.glyph(i).codepoint;
		Vector2 pos = layout.glyph(i).pos;
		const BitmapFont::Glyph& glyph = font.glyph(cp);

		Vector2 size = glyph.size;

		pos(0) += glyph.offset(0);
		pos(1) += font.height() - size(1) - glyph.offset(1)
		        + layout.box().sizes()(1);
		pos -= layout.box().sizes().cwiseProduct(anchor);
		Box2 coords(pos, pos + size);

		renderer->addSprite(transform, coords, color, glyph.region);
	}
	unsigned count = renderer->indexCount() - index;

	if(count) {
		RenderPass::DrawStates states;
		states.shader       = renderer->shader().shader;
		states.vertices     = renderer->vertexArray();
		states.textureSet   = textureSet;
		states.blendingMode = blendingMode;

		const ShaderParameter* params = renderer->addShaderParameters(
		            renderer->shader(), viewTransform, 0, Vector4i(1, 1, 65536, 65536));

		pass->addDrawCall(states, params, depth, index, count);
	}
}

}
