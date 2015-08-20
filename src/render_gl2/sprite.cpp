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

#include "lair/render_gl2/sprite.h"


namespace lair
{


Sprite::Sprite(Texture* texture, unsigned hTiles, unsigned vTiles, const Box2& region)
    : _texture(texture),
      _region(region),
      _hTiles(hTiles),
      _vTiles(vTiles) {
}


Sprite::~Sprite() {
}


Box2 Sprite::tileBox(unsigned index) const {
	index %= _hTiles * _vTiles;
	unsigned tx = index % _hTiles;
	unsigned ty = index / _hTiles;
	return tileBox(tx, ty);
}


Box2 Sprite::tileBox(unsigned tx, unsigned ty) const {
	return Box2(Vector2(lerp(float(tx)   / float(_hTiles), _region.min().x(), _region.max().x()),
	                    lerp(float(ty)   / float(_vTiles), _region.min().y(), _region.max().y())),
	            Vector2(lerp(float(tx+1) / float(_hTiles), _region.min().x(), _region.max().x()),
	                    lerp(float(ty+1) / float(_vTiles), _region.min().y(), _region.max().y())));
}


}
