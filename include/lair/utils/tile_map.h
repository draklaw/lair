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


#ifndef _LAIR_UTILS_TILE_MAP_H
#define _LAIR_UTILS_TILE_MAP_H


#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/json.h>
#include <lair/core/loader.h>
#include <lair/core/image.h>


namespace lair
{


class TileMap {
public:
	typedef unsigned TileIndex;

public:
	TileMap();
	TileMap(const TileMap&) = delete;
	TileMap(TileMap&&)      = delete;
	~TileMap();

	TileMap& operator=(const TileMap&) = delete;
	TileMap& operator=(TileMap&&)      = delete;

	unsigned nLayers() const;
	unsigned width(unsigned layer) const;
	unsigned height(unsigned layer) const;
	TileIndex tile(unsigned x, unsigned y, unsigned layer) const;
	void setTile(unsigned x, unsigned y, unsigned layer, TileIndex tile);

	unsigned nObjectLayer() const;
	const Json::Value& objectLayer(unsigned layer) const;

	const Path&   tileSetPath() const;
	ImageAspectSP tileSet() const;
	unsigned      tileSetHTiles() const;
	unsigned      tileSetVTiles() const;

	bool setFromJson(Logger& log, const Path& path, const Json::Value& value);

	void _setTileSet(ImageAspectSP tileset);

protected:
	typedef std::vector<TileIndex> Layer;
	typedef std::vector<Layer>     LayerList;

	typedef std::vector<Json::Value> ObjectLayerList;

protected:
	unsigned  _width;
	unsigned  _height;
	LayerList _layers;

	ObjectLayerList _objectLayers;

	Path          _tileSetPath;
	ImageAspectSP _tileSet;
	unsigned      _tileSetHTiles;
	unsigned      _tileSetVTiles;
};

typedef std::shared_ptr<TileMap> TileMapSP;


typedef GenericAspect<TileMap>         TileMapAspect;
typedef std::shared_ptr<TileMapAspect> TileMapAspectSP;
typedef std::weak_ptr  <TileMapAspect> TileMapAspectWP;

class TileMapLoader : public Loader {
public:
	typedef TileMapAspect Aspect;

public:
	TileMapLoader(LoaderManager* manager, AspectSP aspect);
	TileMapLoader(const TileMapLoader&) = delete;
	TileMapLoader(TileMapLoader&&)      = delete;
	virtual ~TileMapLoader() = default;

	TileMapLoader& operator=(const TileMapLoader&) = delete;
	TileMapLoader& operator=(TileMapLoader&&)      = delete;

protected:
	virtual void loadSyncImpl(Logger& log);
};



}


#endif
