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


#ifndef _LAIR_RENDER_GL2_SPRITE_H
#define _LAIR_RENDER_GL2_SPRITE_H


#include <lair/core/lair.h>

#include <lair/utils/loader.h>


namespace lair
{


class Texture;
class Renderer;


class Sprite {
public:
	Sprite(Texture* texture = nullptr, unsigned hTiles = 1, unsigned vTiles = 1,
	       const Box2& region = Box2(Vector2(0, 0), Vector2(1, 1)));
	Sprite(const Sprite&) = delete;
	Sprite(Sprite&&)      = default;
	~Sprite();

	Sprite& operator=(const Sprite&) = delete;
	Sprite& operator=(Sprite&&)      = default;

	inline Texture*   texture() const { return _texture; }
	inline const Box2 region()  const { return _region; }
	inline unsigned   hTiles()  const { return _hTiles; }
	inline unsigned   vTiles()  const { return _vTiles; }

	inline unsigned width()  const { return _texture->width()  * _region.sizes().x() / _hTiles; }
	inline unsigned height() const { return _texture->height() * _region.sizes().y() / _vTiles; }

	Box2 tileBox(unsigned index) const;
	Box2 tileBox(unsigned tx, unsigned ty) const;

public:
	Texture* _texture;
	Box2     _region;
	unsigned _hTiles;
	unsigned _vTiles;
};


class SpriteLoader : public Loader {
public:
	SpriteLoader(LoaderManager* manager, const std::string& path, Renderer* renderer);
	SpriteLoader(const SpriteLoader&) = delete;
	SpriteLoader(SpriteLoader&&)      = delete;
	~SpriteLoader();

	SpriteLoader& operator=(const SpriteLoader&) = delete;
	SpriteLoader& operator=(SpriteLoader&&)      = delete;

protected:
	virtual void loadSyncImpl(Logger& log);

public:
	Renderer*   _renderer;
	Sprite      _sprite;
	std::string _texture;
	uint32      _textureFlags;
};


}


#endif
