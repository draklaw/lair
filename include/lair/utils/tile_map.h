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


#include <functional>

#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/metatype.h>
#include <lair/core/json.h>
#include <lair/core/ldl.h>
#include <lair/core/loader.h>
#include <lair/core/image.h>


namespace lair
{


class TileMap {
public:
	// TODO: Support flipped / rotated tiles.
	typedef unsigned TileIndex;

	typedef std::function<bool(LdlParser&)> ObjectsLoader;

public:
	TileMap();
	TileMap(const TileMap&) = delete;
	TileMap(TileMap&&)      = default;
	~TileMap();

	TileMap& operator=(const TileMap&) = delete;
	TileMap& operator=(TileMap&&)      = default;

	bool isValid() const;
	unsigned nLayers() const;
	unsigned width(unsigned layer) const;
	unsigned height(unsigned layer) const;
	TileIndex tile(unsigned x, unsigned y, unsigned layer) const;
	void setTile(unsigned x, unsigned y, unsigned layer, TileIndex tile);

	const Variant& properties() const;

	const Path&   tileSetPath() const;
	ImageAspectSP tileSet() const;
	unsigned      tileSetHTiles() const;
	unsigned      tileSetVTiles() const;

	bool setFromLdl(LdlParser& parser, const ObjectsLoader& loadObjects = ObjectsLoader());
	void setTileSet(AssetSP tileset, unsigned nHTiles, unsigned nVTiles);

	void _setTileSet(ImageAspectSP tileset);

protected:
	typedef std::vector<TileIndex> Layer;
	typedef std::vector<Layer>     LayerList;

protected:
	bool _parseTileSets(LdlParser& parser);
	bool _parseTileSet(LdlParser& parser);
	bool _parseTileLayers(LdlParser& parser);
	bool _parseTileLayer(LdlParser& parser);

protected:
	unsigned  _width;
	unsigned  _height;
	LayerList _layers;

	Variant _properties;

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

	typedef TileMap::ObjectsLoader ObjectsLoader;

public:
	TileMapLoader(LoaderManager* manager, AspectSP aspect,
	              const ObjectsLoader& loadObjects = ObjectsLoader());
	TileMapLoader(const TileMapLoader&) = delete;
	TileMapLoader(TileMapLoader&&)      = delete;
	virtual ~TileMapLoader() = default;

	TileMapLoader& operator=(const TileMapLoader&) = delete;
	TileMapLoader& operator=(TileMapLoader&&)      = delete;

	virtual void commit();

protected:
	virtual void loadSyncImpl(Logger& log);

	void parseMap(std::istream& in, Logger& log);

protected:
	ObjectsLoader _loadObjects;
	TileMap       _tileMap;
};



}


#endif
