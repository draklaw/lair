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


#ifndef _LAIR_CORE_ASSET_MANAGER_H
#define _LAIR_CORE_ASSET_MANAGER_H


#include <memory>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <mutex>

#include <lair/core/lair.h>
#include <lair/core/path.h>


namespace lair
{


class Aspect;
class Asset;
class AssetManager;

typedef std::shared_ptr<Aspect> AspectSP;
typedef std::weak_ptr  <Aspect> AspectWP;
typedef std::shared_ptr<Asset>  AssetSP;
typedef std::weak_ptr  <Asset>  AssetWP;


class Aspect {
public:
	Aspect(AssetSP asset);
	Aspect(const Aspect&)  = delete;
	Aspect(      Aspect&&) = delete;
	virtual ~Aspect() = default;

	Aspect& operator=(const Aspect&)  = delete;
	Aspect& operator=(      Aspect&&) = delete;

	inline bool isValid() const {
		std::lock_guard<std::mutex> lock(_lock);
		return _isValid;
	}
//	virtual std::string aspectName() const = 0;

//	virtual Size cpuMemory() const = 0;
//	virtual Size gpuMemory() const = 0;

	inline AssetSP asset() { return _asset; }

//	inline const AspectWP nextAspect() const {
//		return _nextAspect;
//	}


	void _setValid(bool valid) {
		std::lock_guard<std::mutex> lock(_lock);
		_isValid = valid;
	}

//	inline void _setNextAspect(const AspectWP& aspect) {
//		_nextAspect = aspect;
//	}

	inline std::mutex& _getLock() { return _lock; }

protected:
	AssetSP    _asset;      // Asset must not be destroyed before its aspects.
	bool       _isValid;
//	AspectWP   _nextAspect;
	mutable std::mutex _lock;
};


template<typename T>
class GenericAspect : public Aspect {
public:
	typedef T Data;

public:
	GenericAspect(AssetSP asset)
		: Aspect(asset),
	      _data() {
	}

	GenericAspect(const GenericAspect&)  = delete;
	GenericAspect(      GenericAspect&&) = delete;
	~GenericAspect() = default;

	GenericAspect& operator=(const GenericAspect&)  = delete;
	GenericAspect& operator=(      GenericAspect&&) = delete;

	const Data* get() const {
		std::lock_guard<std::mutex> lock(_lock);
		return _isValid? &_data: nullptr;
	}

	Data* _get() {
		return _isValid? &_data: nullptr;
	}

private:
	Data _data;
};


class Asset : public std::enable_shared_from_this<Asset> {
public:
	Asset(AssetManager* manager, const Path& logicPath);
	Asset(const Asset&)  = delete;
	Asset(      Asset&&) = delete;
	~Asset() = default;

	Asset& operator=(const Asset&)  = delete;
	Asset& operator=(      Asset&&) = delete;

	inline const Path& logicPath() const {
		return _logicPath;
	}

//	Size cpuMemory() const;
//	Size gpuMemory() const;

	template<typename _Aspect>
	std::shared_ptr<_Aspect> aspect();

	template<typename _Aspect, typename... Args>
	std::shared_ptr<_Aspect> createAspect(Args&&... args);

	inline AssetManager* manager() {
		return _manager;
	}

	// FIXME: This may cause race condition if aspects are set from an other
	// thread.
//	inline const AspectWP firstAspect() const {
//		return _firstAspect;
//	}


//	inline void _setFirstAspect(const AspectWP& aspect) {
//		_firstAspect = aspect;
//	}

private:
	AssetManager* _manager;
	Path          _logicPath;
//	AspectWP      _firstAspect;
};


class AssetManager {
public:
	AssetManager();
	AssetManager(const AssetManager&)  = delete;
	AssetManager(      AssetManager&&) = delete;
	~AssetManager() = default;

	AssetManager& operator=(const AssetManager&)  = delete;
	AssetManager& operator=(      AssetManager&&) = delete;

	AssetSP createAsset(const Path& logicPath);
	AssetSP getAsset   (const Path& logicPath);

	AspectSP getAspect(const std::type_info& aspectTypeInfo, AssetSP asset);
	void setAspect(AssetSP asset, AspectSP aspect);

	template<typename _Aspect>
	std::shared_ptr<_Aspect> getAspect(AssetSP asset);

	template<typename _Aspect>
	std::shared_ptr<_Aspect> createAspect(AssetSP asset);

	void releaseAll();

private:
	typedef std::unordered_map<Path,            AssetSP, boost::hash<Path>>
	                                                            AssetPathMap;
	typedef std::unordered_map<const Asset*,    AspectSP>       AssetAspectMap;
	typedef std::unordered_map<std::type_index, AssetAspectMap> AspectMap;

private:
	AssetPathMap _assetMap;
	AspectMap    _aspects;

	mutable std::mutex _lock;
};



template<typename _Aspect>
inline std::shared_ptr<_Aspect> Asset::aspect() {
	AssetSP asset = shared_from_this();
	return _manager->getAspect<_Aspect>(asset);
}


template<typename _Aspect, typename... Args>
std::shared_ptr<_Aspect> Asset::createAspect(Args&&... args) {
	AssetSP asset = shared_from_this();
	auto aspect = std::make_shared<_Aspect>(asset, std::forward<Args>(args)...);
	_manager->setAspect(asset, aspect);
	return aspect;
}



template<typename _Aspect>
inline std::shared_ptr<_Aspect> AssetManager::getAspect(AssetSP asset) {
	return std::static_pointer_cast<_Aspect>(getAspect(typeid(_Aspect), asset));
}


}


#endif
