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


const Variant& TileMap::properties() const {
	return _properties;
}


unsigned TileMap::nObjectLayer() const {
	return _objectLayers.size();
}


const Variant& TileMap::objectLayer(unsigned layer) const {
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


bool TileMap::setFromLdl(LdlParser& parser) {
	if(parser.valueType() != LdlParser::TYPE_MAP) {
		parser.error("Expected TileMap (VarMap), got ", parser.valueTypeName());
		return false;
	}

	bool success = true;

	parser.enter();
	while(success && parser.valueType() != LdlParser::TYPE_END) {
		String key = parser.getKey();
		if(key == "width") {
			success = ldlRead(parser, _width);
		}
		else if(key == "height") {
			success = ldlRead(parser, _height);
		}
		else if(key == "tilesets") {
			success = _parseTileSets(parser);
		}
		else if(key == "properties") {
			success = ldlRead(parser, _properties);
		}
		else if(key == "layers") {
			success = _parseLayers(parser);
		}
		else {
			parser.warning("Unknown key \"", key, "\" in TileMap, ignoring.");
			parser.skip();
		}
	}

	if(!success) {
		while(parser.valueType() != LdlParser::TYPE_END)
			parser.skip();
		_width = 0;
		_height = 0;
		_layers.clear();
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


bool TileMap::_parseLayers(LdlParser& parser) {
	if(parser.valueType() != LdlParser::TYPE_LIST) {
		parser.error("Expected list of layers (VarList), got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	bool success = true;

	parser.enter();
	while(success && parser.valueType() != LdlParser::TYPE_END) {
		success = _parseLayer(parser);
	}

	while(parser.valueType() != LdlParser::TYPE_END)
		parser.skip();
	parser.leave();

	return success;
}


bool TileMap::_parseLayer(LdlParser& parser) {
	if(parser.valueType() != LdlParser::TYPE_MAP) {
		parser.error("Expected layer (VarMap), got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	bool success = true;

	parser.enter();
	while(success && parser.valueType() != LdlParser::TYPE_END) {
		String key = parser.getKey();
		if(key == "tiles") {
			if(parser.valueType() != LdlParser::TYPE_LIST) {
				parser.error("Expected tile list (VarList), got ", parser.valueTypeName());
				parser.skip();
				success = false;
			}
			else {
				_layers.emplace_back();
				Layer& layer = _layers.back();
				layer.reserve(_width * _height);

				parser.enter();
				while(parser.valueType() != LdlParser::TYPE_END) {
					TileIndex index;
					success = success && ldlRead(parser, index);
					layer.push_back(index);
				}
				parser.leave();

				success = success && layer.size() == _width * _height;
			}
		}
		else if(key == "objects") {
			_objectLayers.emplace_back();
			success = ldlRead(parser, _objectLayers.back());
			success = success && _objectLayers.back().isVarList();
		}
		else {
			parser.warning("Unknown key \"", key, "\" in TileMap layer, ignoring.");
			parser.skip();
		}
	}

	while(parser.valueType() != LdlParser::TYPE_END)
		parser.skip();
	parser.leave();

	return success;
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

	if(_tileMap.setFromLdl(parser)) {
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
