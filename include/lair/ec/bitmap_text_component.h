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


#ifndef _LAIR_EC_BITMAP_TEXT_COMPONENT_H
#define _LAIR_EC_BITMAP_TEXT_COMPONENT_H


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include <lair/meta/property.h>
#include <lair/meta/property_list.h>
#include <lair/meta/with_properties.h>

#include <lair/asset/asset_manager.h>
#include <lair/asset/loader.h>
#include <lair/asset/bitmap_font.h>

#include <lair/render_gl3/orthographic_camera.h>
#include <lair/render_gl3/texture.h>
#include <lair/render_gl3/texture_set.h>
#include <lair/render_gl3/render_pass.h>

#include <lair/ec/component.h>
#include <lair/ec/component_manager.h>
#include <lair/ec/dense_component_manager.h>


namespace lair {


class SpriteRenderer;
class BitmapTextComponentManager;


class BitmapFontLoader : public Loader {
public:
	typedef BitmapFontAspect Aspect;

public:
	BitmapFontLoader(LoaderManager* manager, AspectSP aspect);
	BitmapFontLoader(const BitmapFontLoader&) = delete;
	BitmapFontLoader(BitmapFontLoader&&)      = delete;
	~BitmapFontLoader();

	BitmapFontLoader& operator=(const BitmapFontLoader&) = delete;
	BitmapFontLoader& operator=(BitmapFontLoader&&)      = delete;

	virtual void commit();

protected:
	virtual void loadSyncImpl(Logger& log);

protected:
	Json::Value _fontDesc;
	BitmapFont  _font;
};


class BitmapTextComponent : public Component, WithProperties<BitmapTextComponent> {
public:
	typedef BitmapTextComponentManager Manager;

public:
	BitmapTextComponent(Manager* manager, _Entity* entity);

	Manager* manager();

	inline BitmapFontAspectSP font() const { return _font.lock(); }
	inline const Path& fontPath() const {
		BitmapFontAspectSP aspect = font();
		return aspect? aspect->asset()->logicPath(): emptyPath;
	}
	void setFont(BitmapFontAspectSP font);
	void setFont(AssetSP font);
	void setFont(const Path& logicPath);

	inline TextureSetCSP textureSet() const { return _textureSet; }
	inline void setTextureSet(TextureSetCSP textureSet) { _textureSet = textureSet; }
	void setTextureSet(const TextureSet& textureSet);

	inline TextureAspectSP texture() const;
	inline void _setTexture(TextureAspectSP texture);

	inline const std::string& text() const { return _text; }
	inline void setText(const std::string& text) { _text = text; }

	inline const Vector4& color() const { return _color; }
	inline void setColor(const Vector4& color) { _color = color; }

	inline const Vector2& size() const { return _size; }
	inline void setSize(const Vector2& size) { _size = size; }

	inline const Vector2& anchor() const { return _anchor; }
	inline void setAnchor(const Vector2& anchor) { _anchor = anchor; }

	inline BlendingMode blendingMode() const { return _blendingMode; }
	inline void setBlendingMode(BlendingMode bm) { _blendingMode = bm; }

	static const PropertyList& properties();

public:
	BitmapFontAspectWP _font;
	TextureSetCSP      _textureSet;
	std::string        _text;
	Vector4            _color;
	Vector2            _size;
	Vector2            _anchor;
	BlendingMode       _blendingMode;
};


class BitmapTextComponentManager : public DenseComponentManager<BitmapTextComponent> {
public:
	BitmapTextComponentManager(LoaderManager* loaderManager,
	                           RenderPass* renderPass,
	                           SpriteRenderer* spriteRenderer,
	                           size_t componentBlockSize = 1024);

	BitmapTextComponentManager(const BitmapTextComponentManager&) = delete;
	BitmapTextComponentManager(BitmapTextComponentManager&&)      = delete;

	virtual ~BitmapTextComponentManager();

	void createTextures();

//	void render(float interp, const OrthographicCamera& camera);
	void render(EntityRef entity, float interp, const OrthographicCamera& camera);

	LoaderManager* loader();
	SpriteRenderer* spriteRenderer() const;

protected:
	void _render(EntityRef entity, float interp, const OrthographicCamera& camera);

private:
	LoaderManager*  _loader;
	RenderPass*     _renderPass;
	SpriteRenderer* _spriteRenderer;

	RenderPass::DrawStates _states;
	const ShaderParameter* _params;
};


void renderBitmapText(RenderPass* pass, SpriteRenderer* renderer,
                      const BitmapFont& font, const TextureSetCSP& textureSet,
                      const Matrix4& transform, float depth,
                      const TextLayout& layout, const Vector2& anchor,
                      const Vector4& color, const Matrix4& viewTransform,
                      BlendingMode blendingMode);
}

#endif
