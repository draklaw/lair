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
#include <lair/core/asset_manager.h>
#include <lair/core/loader.h>
#include <lair/core/bitmap_font.h>

#include <lair/render_gl2/orthographic_camera.h>
#include <lair/render_gl2/texture.h>
#include <lair/render_gl2/render_pass.h>

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

protected:
	virtual void loadSyncImpl(Logger& log);
};


class BitmapTextComponent : public Component {
public:
	typedef BitmapTextComponentManager Manager;

public:
	BitmapTextComponent(Manager* manager, _Entity* entity);

	Manager* manager();

	inline BitmapFontAspectSP font() const { return _font.lock(); }
	void setFont(BitmapFontAspectSP font);
	void setFont(AssetSP font);
	void setFont(const Path& logicPath);

	inline TextureAspectSP texture() const { return _texture.lock(); }
	inline void _setTexture(TextureAspectSP texture) { _texture = texture; }

	inline const std::string& text() const { return _text; }
	inline void setText(const std::string& text) { _text = text; }

	inline const Vector4& color() const { return _color; }
	inline void setColor(const Vector4& color) { _color = color; }

	inline const Vector2i& size() const { return _size; }
	inline void setSize(const Vector2i& size) { _size = size; }

	inline const Vector2& anchor() const { return _anchor; }
	inline void setAnchor(const Vector2& anchor) { _anchor = anchor; }

public:
	BitmapFontAspectWP _font;
	TextureAspectWP    _texture;
	std::string        _text;
	Vector4            _color;
	Vector2i           _size;
	Vector2            _anchor;
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

	virtual BitmapTextComponent* addComponentFromJson(EntityRef entity, const Json::Value& json,
	                                  const Path& cd=Path());
	virtual BitmapTextComponent* cloneComponent(EntityRef base, EntityRef entity);

	void render(float interp, const OrthographicCamera& camera);

	LoaderManager* loader();
	SpriteRenderer* spriteRenderer() const;

private:
	LoaderManager*  _loader;
	RenderPass*     _renderPass;
	SpriteRenderer* _spriteRenderer;
};


}

#endif
