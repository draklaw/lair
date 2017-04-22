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

#include <lair/sys_sdl2/image_loader.h>

#include "lair/utils/tile_map.h"


namespace lair
{


TileMap::TileMap() {
}


TileMap::~TileMap() {
}


bool TileMap::isValid() const {
	return _layers.size();
}


unsigned TileMap::nLayers() const {
	return _layers.size();
}


unsigned TileMap::width(unsigned /*layer*/) const {
	return _width;
}


unsigned TileMap::height(unsigned /*layer*/) const {
	return _height;
}


TileMap::TileIndex TileMap::tile(unsigned x, unsigned y, unsigned layer) const {
	unsigned i = y * width(layer) + x;
	lairAssert(i < _layers[layer].size());
	return _layers[layer][i];
}


void TileMap::setTile(unsigned x, unsigned y, unsigned layer, TileIndex tile) {
	unsigned i = y * width(layer) + x;
	lairAssert(i < _layers[layer].size());
	_layers[layer][i] = tile;
}


const Json::Value& TileMap::properties() const {
	return _properties;
}


unsigned TileMap::nObjectLayer() const {
	return _objectLayers.size();
}


const Json::Value& TileMap::objectLayer(unsigned layer) const {
	lairAssert(layer < _objectLayers.size());
	return _objectLayers[layer];
}


const Path& TileMap::tileSetPath() const {
	return _tileSetPath;
}


ImageAspectSP TileMap::tileSet() const {
	return _tileSet;
}


unsigned TileMap::tileSetHTiles() const {
	return _tileSetHTiles;
}


unsigned TileMap::tileSetVTiles() const {
	return _tileSetVTiles;
}


bool TileMap::setFromJson(Logger& log, const Path& path, const Json::Value& value) {
	_width  = value.get("width",  0).asInt();
	_height = value.get("height", 0).asInt();

	if(_width == 0 || _height == 0) {
		log.error("Tile map \"", path, "\" has invalid size.");
		return false;
	}

	const Json::Value& tilesets = value["tilesets"];
	if(!tilesets.isArray() || tilesets.size() != 1) {
		log.error("Tile map \"", path, "\" has invalid number of tilesets.");
		return false;
	}
	_tileSetPath   = makeAbsolute(path.dir(), tilesets[0].get("image", "").asString());
	_tileSetHTiles = tilesets[0].get("imagewidth", 0).asInt()
	               / tilesets[0].get("tilewidth", 1).asInt();
	_tileSetVTiles = tilesets[0].get("imageheight", 0).asInt()
	               / tilesets[0].get("tileheight", 1).asInt();

	_properties = value["properties"];

	_layers.clear();
	_objectLayers.clear();
	for(const Json::Value& layer: value["layers"]) {
		if(layer.get("type", "").asString() == "tilelayer") {
			unsigned width  = layer.get("width",  0).asInt();
			unsigned height = layer.get("height", 0).asInt();

			if(width != _width || height != _height) {
				log.error("Tile map \"", path, "\" has a layer with invalid size");
				return false;
			}

			_layers.emplace_back();
			Layer& map = _layers.back();
			map.reserve(_width * _height);
			for(const Json::Value& v: layer["data"]) {
				if(!v.isIntegral()) {
					log.error("Tile map \"", path, "\": invalid tile index");
					return false;
				}
				map.push_back(v.asInt());
			}
		} else if(layer.get("type", "").asString() == "objectgroup") {
			_objectLayers.push_back(layer);
		}
	}

	return true;
}


void TileMap::_setTileSet(ImageAspectSP tileset) {
	_tileSet = tileset;
}



TileMapLoader::TileMapLoader(LoaderManager* manager, AspectSP aspect)
	: Loader(manager, aspect) {
}


void TileMapLoader::commit() {
	TileMapAspectSP aspect = std::static_pointer_cast<TileMapAspect>(_aspect);
	aspect->_get() = std::move(_tileMap);
	Loader::commit();
}


void TileMapLoader::loadSyncImpl(Logger& log) {
	Json::Value json;
	if(!parseJson(json, realPath(), asset()->logicPath(), log))
		return;

	if(!_tileMap.setFromJson(log, asset()->logicPath(), json))
		return;

	_load<ImageLoader>(_tileMap.tileSetPath(), [this](AspectSP tileSetAspect, Logger& log) {
		ImageAspectSP tileSetImg = std::static_pointer_cast<ImageAspect>(tileSetAspect);
		if(!tileSetImg) {
			log.error("Error while loading TileMap \"", asset()->logicPath(),
			          "\": Failed to load tile set \"", _tileMap.tileSetPath(), "\".");
			return;
		}
		_tileMap._setTileSet(tileSetImg);
	});
}


}
