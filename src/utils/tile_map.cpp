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
#include <lair/core/ldl.h>

#include <lair/sys_sdl2/image_loader.h>

#include "lair/utils/tile_map.h"


namespace lair
{


TileLayer::TileLayer()
    : _offsetInTiles(0, 0)
    , _sizeInTiles(0, 0)
    , _tileSizeInPixels(0, 0)
{
}


TileLayer::~TileLayer() {
}


const Vector2i& TileLayer::offsetInTiles() const {
	return _offsetInTiles;
}


Vector2i TileLayer::offsetInPixels() const {
	return _offsetInTiles.cwiseProduct(_tileSizeInPixels);
}


const Vector2i& TileLayer::sizeInTiles() const {
	return _sizeInTiles;
}


unsigned TileLayer::widthInTiles() const {
	return _sizeInTiles(0);
}


unsigned TileLayer::heightInTiles() const {
	return _sizeInTiles(1);
}


Box2i TileLayer::boxInTiles() const {
	return Box2i(offsetInTiles(), offsetInTiles() + sizeInTiles());
}


Box2i TileLayer::boxInPixels() const {
	return Box2i(offsetInPixels(), offsetInPixels() + sizeInPixels());
}


Vector2i TileLayer::sizeInPixels() const {
	return _sizeInTiles.cwiseProduct(_tileSizeInPixels);
}


unsigned TileLayer::widthInPixels() const {
	return _sizeInTiles(0) * _tileSizeInPixels(0);
}


unsigned TileLayer::heightInPixels() const {
	return _sizeInTiles(1) * _tileSizeInPixels(1);
}


const Vector2i& TileLayer::tileSizeInPixels() const {
	return _tileSizeInPixels;
}


unsigned TileLayer::tileWidthInPixels() const {
	return _tileSizeInPixels(0);
}


unsigned TileLayer::tileHeightInPixels() const {
	return _tileSizeInPixels(1);
}


TileLayer::TileIndex TileLayer::tile(unsigned x, unsigned y) const {
	lairAssert(x < widthInTiles() && y < widthInTiles());
	return _tiles[x + y * widthInTiles()];
}


void TileLayer::setTile(unsigned x, unsigned y, TileIndex tile) {
	lairAssert(x < widthInTiles() && y < widthInTiles());
	_tiles[x + y * widthInTiles()] = tile;
}


bool TileLayer::setFromLdl(LdlParser& parser) {
	if(parser.valueType() != LdlParser::TYPE_MAP) {
		parser.error("Expected tile layer (VarMap), got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	bool success = true;

	parser.enter();
	while(success && parser.valueType() != LdlParser::TYPE_END) {
		String key = parser.getKey();
		if(key == "offset") {
			success = ldlRead(parser, _offsetInTiles);
		}
		else if(key == "size") {
			success = ldlRead(parser, _sizeInTiles);
		}
		else if(key == "tile_size") {
			success = ldlRead(parser, _tileSizeInPixels);
		}
		else if(key == "tiles") {
			if(parser.valueType() != LdlParser::TYPE_LIST) {
				parser.error("Expected tile layer (VarMap), got ", parser.valueTypeName());
				parser.skip();
				success = false;
			}
			else {
				unsigned size = _sizeInTiles.prod();
				_tiles.reserve(size);

				parser.enter();
				for(unsigned i = 0; i < size && success && parser.valueType() != LdlParser::TYPE_END; ++i) {
					TileIndex tile;
					success = ldlRead(parser, tile);
					_tiles.push_back(tile);
				}

				if(success && _tiles.size() != size) {
					parser.error("Not enough tiles it tile layer.");
					success = false;
				}

				if(parser.valueType() != LdlParser::TYPE_END) {
					if(success)
						parser.warning("Too much tiles in tile layer, ignoring.");
					while(parser.valueType() != LdlParser::TYPE_END)
						parser.skip();
				}
				parser.leave();
			}
		}
		else {
			parser.warning("Unknown key \"", key, "\" in TileLayer, ignoring.");
			parser.skip();
		}
	}

	while(parser.valueType() != LdlParser::TYPE_END)
		parser.skip();
	parser.leave();

	return success;
}




TileMap::TileMap() {
}


TileMap::~TileMap() {
}


bool TileMap::isValid() const {
	return nLayers();
}


unsigned TileMap::nLayers() const {
	return _tileLayers.size();
}


TileLayerCSP TileMap::tileLayer(unsigned layerIndex) const {
	return _tileLayers[layerIndex];
}


TileLayerSP TileMap::tileLayer(unsigned layerIndex) {
	return _tileLayers[layerIndex];
}


const Variant& TileMap::properties() const {
	return _properties;
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


bool TileMap::setFromLdl(LdlParser& parser, const ObjectsLoader& loadObjects) {
	if(parser.valueType() != LdlParser::TYPE_MAP) {
		parser.error("Expected TileMap (VarMap), got ", parser.valueTypeName());
		return false;
	}

	bool success = true;

	parser.enter();
	while(success && parser.valueType() != LdlParser::TYPE_END) {
		String key = parser.getKey();
		if(key == "width") {
//			success = ldlRead(parser, _width);
			parser.skip();
		}
		else if(key == "height") {
//			success = ldlRead(parser, _height);
			parser.skip();
		}
		else if(key == "properties") {
			success = ldlRead(parser, _properties);
		}
		else if(key == "tilesets") {
			success = _parseTileSets(parser);
		}
		else if(key == "tile_layers") {
			success = _parseTileLayers(parser);
		}
		else if(key == "objects") {
			if(loadObjects)
				success = loadObjects(parser);
			else
				parser.skip();
		}
		else {
			parser.warning("Unknown key \"", key, "\" in TileMap, ignoring.");
			parser.skip();
		}
	}

	if(!success) {
		while(parser.valueType() != LdlParser::TYPE_END)
			parser.skip();
		_tileLayers.clear();
	}
	parser.leave();

	return success;
}

void TileMap::setTileSet(AssetSP tileset, unsigned nHTiles, unsigned nVTiles) {
	_tileSetPath   = tileset->logicPath();
	_tileSetHTiles = nHTiles;
	_tileSetVTiles = nVTiles;
	_tileSet = tileset->aspect<ImageAspect>();
}


void TileMap::_setTileSet(ImageAspectSP tileset) {
	_tileSet = tileset;
}


bool TileMap::_parseTileSets(LdlParser& parser) {
	if(parser.valueType() != LdlParser::TYPE_LIST) {
		parser.error("Expected list of tilesets (VarList), got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	bool success = true;

	parser.enter();
	while(success && parser.valueType() != LdlParser::TYPE_END) {
		success = _parseTileSet(parser);

		// FIXME: Support more than one tileset. Currently, only take the first
		// one that should be used with the tile map.
		break;
	}

	while(parser.valueType() != LdlParser::TYPE_END)
		parser.skip();
	parser.leave();

	return success;
}


bool TileMap::_parseTileSet(LdlParser& parser) {
	if(parser.valueType() != LdlParser::TYPE_MAP) {
		parser.error("Expected tileset (VarMap), got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	bool success = true;

	// TODO: Support multiple tilesets
	parser.enter();
	while(success && parser.valueType() != LdlParser::TYPE_END) {
		String key = parser.getKey();
		if(key == "image") {
			success = ldlRead(parser, _tileSetPath);
		}
		else if(key == "h_tiles") {
			success = ldlRead(parser, _tileSetHTiles);
		}
		else if(key == "v_tiles") {
			success = ldlRead(parser, _tileSetVTiles);
		}
		else {
			parser.warning("Unknown key \"", key, "\" in TileSet, ignoring.");
			parser.skip();
		}
	}

	while(parser.valueType() != LdlParser::TYPE_END)
		parser.skip();
	parser.leave();

	return success;
}


bool TileMap::_parseTileLayers(LdlParser& parser) {
	if(parser.valueType() != LdlParser::TYPE_LIST) {
		parser.error("Expected list of tile layers (VarList), got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	bool success = true;

	parser.enter();
	while(success && parser.valueType() != LdlParser::TYPE_END) {
		TileLayerSP tileLayer = std::make_shared<TileLayer>();
		success = tileLayer->setFromLdl(parser);

		if(success) {
			_tileLayers.push_back(tileLayer);
		}
	}

	while(parser.valueType() != LdlParser::TYPE_END)
		parser.skip();
	parser.leave();

	return success;
}



TileMapLoader::TileMapLoader(LoaderManager* manager, AspectSP aspect,
                             const ObjectsLoader& loadObjects)
    : Loader(manager, aspect, loadObjects? Loader::LOAD_FROM_MAIN_THREAD:
                                           Loader::LOAD_FROM_ANY_THREAD),
      _loadObjects(loadObjects) {
}


void TileMapLoader::commit() {
	TileMapAspectSP aspect = std::static_pointer_cast<TileMapAspect>(_aspect);
	aspect->_get() = std::move(_tileMap);
	Loader::commit();
}


void TileMapLoader::loadSyncImpl(Logger& log) {
	const VirtualFile& vFile = file();

	Path realPath = vFile.realPath();
	if(!realPath.empty()) {
		Path::IStream in(realPath.native().c_str());
		if(!in.good()) {
			log.error("Unable to read \"", asset()->logicPath(), "\".");
			return;
		}
		parseMap(in, log);
		return;
	}

	const MemFile* memFile = vFile.fileBuffer();
	if(memFile) {
		String buffer((const char*)memFile->data, memFile->size);
		std::istringstream in(buffer);
		parseMap(in, log);
		return;
	}
}


void TileMapLoader::parseMap(std::istream& in, Logger& log) {
	ErrorList errors;
	LdlParser parser(&in, asset()->logicPath().utf8String(), &errors, LdlParser::CTX_MAP);

	if(_tileMap.setFromLdl(parser, _loadObjects)) {
		_load<ImageLoader>(_tileMap.tileSetPath(), [this](AspectSP tileSetAspect, Logger& log) {
			ImageAspectSP tileSetImg = std::static_pointer_cast<ImageAspect>(tileSetAspect);
			if(!tileSetImg) {
				log.error("Error while loading TileMap \"", asset()->logicPath(),
				          "\": Failed to load tile set \"", _tileMap.tileSetPath(), "\".");
				return;
			}
			_tileMap._setTileSet(tileSetImg);
		}, log);
	}

	errors.log(log);
}


}
