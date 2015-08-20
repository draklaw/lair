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


namespace lair
{


class Texture;


class Sprite {
public:
	Sprite(Texture* texture, unsigned hTiles = 1, unsigned vTiles = 1,
	       const Box2& region = Box2(Vector2(0, 0), Vector2(1, 1)));
	Sprite(const Sprite&) = delete;
	Sprite(Sprite&&)      = delete;
	~Sprite();

	Sprite& operator=(const Sprite&) = delete;
	Sprite& operator=(Sprite&&)      = delete;

	inline Texture*   texture() const { return _texture; }
	inline const Box2 region()  const { return _region; }
	inline unsigned   hTiles()  const { return _hTiles; }
	inline unsigned   vTiles()  const { return _vTiles; }

	Box2 tileBox(unsigned index) const;
	Box2 tileBox(unsigned tx, unsigned ty) const;

protected:
	Texture* _texture;
	Box2     _region;
	unsigned _hTiles;
	unsigned _vTiles;
};


}


#endif
