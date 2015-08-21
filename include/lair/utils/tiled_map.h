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


#ifndef _LAIR_UTILS_TILED_MAP_H
#define _LAIR_UTILS_TILED_MAP_H


#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/json.h>


namespace lair
{


class Renderer;
class Sprite;


class TiledMap {
public:
	typedef unsigned TileIndex;

public:
	TiledMap();
	TiledMap(const TiledMap&) = delete;
	TiledMap(TiledMap&&)      = delete;
	~TiledMap();

	TiledMap& operator=(const TiledMap&) = delete;
	TiledMap& operator=(TiledMap&&)      = delete;

	unsigned nLayers() const;
	unsigned width(unsigned layer) const;
	unsigned height(unsigned layer) const;
	TileIndex tile(unsigned x, unsigned y, unsigned layer) const;
	void setTile(unsigned x, unsigned y, unsigned layer, TileIndex tile);

	unsigned nObjectLayer() const;
	const Json::Value& objectLayer(unsigned layer) const;

	const std::string& tileSetFile() const;
	unsigned tileSetHTiles() const;
	unsigned tileSetVTiles() const;

	void setTileset(Sprite* tileset);

	bool setFromJson(Logger& log, const std::string& name, const Json::Value& value);

	void render(Renderer* renderer) const;

protected:
	typedef std::vector<TileIndex> TileMap;
	typedef std::vector<TileMap>   LayerList;

	typedef std::vector<Json::Value> ObjectLayerList;

protected:
	unsigned  _width;
	unsigned  _height;
	LayerList _layers;

	ObjectLayerList _objectLayers;

	std::string _tileSetFile;
	unsigned    _tileSetHTiles;
	unsigned    _tileSetVTiles;

	Sprite* _tileset;
};


}


#endif
