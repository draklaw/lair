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

#include "lair/asset/asset_manager.h"


namespace lair
{


Aspect::Aspect(AssetSP asset)
    : _asset(asset),
      _flags(0) {
}


Aspect::~Aspect() {
}


void Aspect::warnIfInvalid(Logger& log) {
	if(!isValid() && !(_flags & WARNED_INVALID)) {
		log.warning("Try to use invalid asset \"", asset()->logicPath(), "\". Not loaded yet ?");
		_flags |= WARNED_INVALID;
	}
}


Asset::Asset(AssetManager* manager, const Path& logicPath)
    : _manager(manager),
      _logicPath   (logicPath) {
}


AssetManager::AssetManager() {
}


AssetSP AssetManager::createAsset(const Path& logicPath) {
	std::unique_lock<std::mutex> lock(_lock);
	assert(!_assetMap.count(logicPath));
	auto it = _assetMap.emplace(logicPath, std::make_shared<Asset>(this, logicPath)).first;
	return it->second;
}


AssetSP AssetManager::getAsset(const Path& logicPath) {
	std::unique_lock<std::mutex> lock(_lock);
	auto it = _assetMap.find(logicPath);
	if(it == _assetMap.end()) {
		return AssetSP();
	}
	return it->second;
}


AssetSP AssetManager::getOrCreateAsset(const Path& logicPath) {
	std::unique_lock<std::mutex> lock(_lock);
	auto it = _assetMap.find(logicPath);
	if(it == _assetMap.end()) {
		it = _assetMap.emplace(logicPath, std::make_shared<Asset>(this, logicPath)).first;
	}
	return it->second;
}


AspectSP AssetManager::getAspect(const std::type_info& aspectTypeInfo, AssetSP asset) {
	std::unique_lock<std::mutex> lock(_lock);
	auto typeIt = _aspects.find(aspectTypeInfo);
	if(typeIt != _aspects.end()) {
		auto aspectIt = typeIt->second.find(asset.get());
		if(aspectIt != typeIt->second.end()) {
			return aspectIt->second;
		}
	}
	return AspectSP();
}


void AssetManager::setAspect(AssetSP asset, AspectSP aspect) {
	assert(asset);
	assert(aspect);
	assert(asset->manager() == this);

	std::unique_lock<std::mutex> lock(_lock);

	Aspect& aspectRef = *aspect;
	std::type_index index(typeid(aspectRef));
	auto mapIt = _aspects.find(index);
	if(mapIt == _aspects.end()) {
		mapIt = _aspects.emplace(index, AssetAspectMap()).first;
	}
	AssetAspectMap& assetMap = mapIt->second;

	assert(!assetMap.count(asset.get()));
	assetMap.emplace(asset.get(), aspect);

//	aspect->_setNextAspect(asset->firstAspect());
//	asset->_setFirstAspect(aspect);
}


AspectSP AssetManager::getOrSetAspect(AssetSP asset, AspectSP aspect) {
	assert(asset);
	assert(aspect);
	assert(asset->manager() == this);

	std::unique_lock<std::mutex> lock(_lock);

	Aspect& aspectRef = *aspect;
	std::type_index index(typeid(aspectRef));
	auto mapIt = _aspects.find(index);
	if(mapIt == _aspects.end()) {
		mapIt = _aspects.emplace(index, AssetAspectMap()).first;
	}
	AssetAspectMap& assetMap = mapIt->second;

	auto assetIt = assetMap.find(asset.get());
	if(assetIt != assetMap.end())
		return assetIt->second;

	assetMap.emplace_hint(assetIt, asset.get(), aspect);
	return aspect;
}


void AssetManager::releaseAll() {
	_aspects.clear();
	_assetMap.clear();
}


}
