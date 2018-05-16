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
#include <lair/core/json.h>
#include <lair/core/ldl.h>
#include <lair/core/loader.h>
#include <lair/core/image.h>

#include <lair/meta/metatype.h>


namespace lair
{


class TileLayer {
public:
	typedef uint32 TileIndex;

	enum {
		HFLIP_FLAG = 0x80000000,
		VFLIP_FLAG = 0x40000000,
		DFLIP_FLAG = 0x20000000,
		GID_MASK   = 0x1fffffff,
	};

public:
	TileLayer();
	TileLayer(const TileLayer&) = delete;
	TileLayer(TileLayer&&)      = default;
	~TileLayer();

	TileLayer& operator=(const TileLayer&) = delete;
	TileLayer& operator=(TileLayer&&)      = default;

	const Vector2i& offsetInTiles() const;
	Vector2i offsetInPixels() const;

	const Vector2i& sizeInTiles() const;
	unsigned widthInTiles() const;
	unsigned heightInTiles() const;

	Box2i boxInTiles() const;
	Box2i boxInPixels() const;

	Vector2i sizeInPixels() const;
	unsigned widthInPixels() const;
	unsigned heightInPixels() const;

	const Vector2i& tileSizeInPixels() const;
	unsigned tileWidthInPixels() const;
	unsigned tileHeightInPixels() const;

	TileIndex tile(unsigned x, unsigned y) const;
	void setTile(unsigned x, unsigned y, TileIndex tile);

	bool setFromLdl(LdlParser& parser);

protected:
	typedef std::vector<TileIndex> TileVector;

protected:
	Vector2i   _offsetInTiles;
	Vector2i   _sizeInTiles;
	Vector2i   _tileSizeInPixels;
	TileVector _tiles;
};


typedef std::shared_ptr<      TileLayer> TileLayerSP;
typedef std::shared_ptr<const TileLayer> TileLayerCSP;
typedef std::weak_ptr  <      TileLayer> TileLayerWP;
typedef std::weak_ptr  <const TileLayer> TileLayerCWP;


class TileMap {
public:
	typedef uint32 TileIndex;

	enum {
		HFLIP_FLAG = 0x80000000,
		VFLIP_FLAG = 0x40000000,
		DFLIP_FLAG = 0x20000000,
		GID_MASK   = 0x1fffffff,
	};

public:
	TileMap();
	TileMap(const TileMap&) = delete;
	TileMap(TileMap&&)      = default;
	~TileMap();

	TileMap& operator=(const TileMap&) = delete;
	TileMap& operator=(TileMap&&)      = default;

	bool isValid() const;
	unsigned nLayers() const;
	TileLayerCSP tileLayer(unsigned layerIndex) const;
	TileLayerSP tileLayer(unsigned layerIndex);

//	const Vector2i& minOffsetInTiles() const;
//	const Vector2i& maxSizeInTiles() const;

	const Variant& properties() const;
	const Variant& objects() const;

	const Path&   tileSetPath() const;
	ImageAspectSP tileSet() const;
	unsigned      tileSetHTiles() const;
	unsigned      tileSetVTiles() const;

	bool setFromLdl(LdlParser& parser);
	void setTileSet(AssetSP tileset, unsigned nHTiles, unsigned nVTiles);

	void _setTileSet(ImageAspectSP tileset);

protected:
	typedef std::vector<TileLayerSP> LayerVector;

protected:
	bool _parseTileSets(LdlParser& parser);
	bool _parseTileSet(LdlParser& parser);
	bool _parseTileLayers(LdlParser& parser);

protected:
	LayerVector _tileLayers;

	Variant _properties;
	Variant _objects;

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

	virtual void commit();

protected:
	virtual void loadSyncImpl(Logger& log);

	void parseMap(std::istream& in, Logger& log);

protected:
	TileMap       _tileMap;
};



}


#endif
