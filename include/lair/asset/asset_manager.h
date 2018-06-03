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


#ifndef _LAIR_ASSET_ASSET_MANAGER_H
#define _LAIR_ASSET_ASSET_MANAGER_H


#include <memory>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <mutex>

#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/intrusive_pointer.h>
#include <lair/core/path.h>


namespace lair
{


class Aspect;
class Asset;
class AssetManager;
class Loader;

using AspectSP = IntrusivePointer    <Aspect>;
using AspectWP = IntrusiveWeakPointer<Aspect>;
using AssetSP  = IntrusivePointer    <Asset>;
using AssetWP  = IntrusiveWeakPointer<Asset>;

typedef std::shared_ptr<Loader> LoaderSP;


class Aspect : public IntrusiveBlock<Aspect> {
public:
	enum {
		WARNED_INVALID = 0x01,
	};

public:
	Aspect(AssetSP asset);
	Aspect(const Aspect&)  = delete;
	Aspect(      Aspect&&) = delete;
	virtual ~Aspect();

	Aspect& operator=(const Aspect&)  = delete;
	Aspect& operator=(      Aspect&&) = delete;

	virtual bool isValid() const = 0;
	inline bool isLoading() const { return bool(_loader); }

	void warnIfInvalid(Logger& log);

//	virtual std::string aspectName() const = 0;

//	virtual Size cpuMemory() const = 0;
//	virtual Size gpuMemory() const = 0;

	inline AssetSP asset() { return _asset; }

//	inline const AspectWP nextAspect() const {
//		return _nextAspect;
//	}

//	inline void _setNextAspect(const AspectWP& aspect) {
//		_nextAspect = aspect;
//	}

	inline LoaderSP _getLoader() { return _loader; }
	inline void _setLoader(LoaderSP loader) { _loader = loader; }

	void _destroy();
	void _delete();

protected:
	AssetSP    _asset;      // Asset must not be destroyed before its aspects.
	unsigned   _flags;
	LoaderSP   _loader;
//	AspectWP   _nextAspect;
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
	virtual ~GenericAspect() = default;

	GenericAspect& operator=(const GenericAspect&)  = delete;
	GenericAspect& operator=(      GenericAspect&&) = delete;

	virtual bool isValid() const {
		return _data.isValid();
	}

	const Data& get() const {
		return _data;
	}

	Data& _get() {
		return _data;
	}

private:
	Data _data;
};


class Asset : public IntrusiveBlock<Asset> {
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
	IntrusivePointer<_Aspect> aspect();

	template<typename _Aspect, typename... Args>
	IntrusivePointer<_Aspect> createAspect(Args&&... args);

	template<typename _Aspect, typename... Args>
	IntrusivePointer<_Aspect> getOrCreateAspect(Args&&... args);

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

	void _destroy();
	void _delete();

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

	AssetSP createAsset     (const Path& logicPath);
	AssetSP getAsset        (const Path& logicPath);
	AssetSP getOrCreateAsset(const Path& logicPath);

	AspectSP getAspect(const std::type_info& aspectTypeInfo, AssetSP asset);
	void setAspect(AssetSP asset, AspectSP aspect);
	AspectSP getOrSetAspect(AssetSP asset, AspectSP aspect);

	template<typename _Aspect>
	IntrusivePointer<_Aspect> getAspect(AssetSP asset);

	template<typename _Aspect, typename... Args>
	IntrusivePointer<_Aspect> createAspect(AssetSP asset, Args&&... args);

	template<typename _Aspect, typename... Args>
	IntrusivePointer<_Aspect> getOrCreateAspect(AssetSP asset, Args&&... args);

	void releaseAll();

	void _destroy(Asset* asset);
	void _delete(Asset* asset);

	void _destroy(Aspect* aspect);
	void _delete(Aspect* aspect);

private:
	typedef std::unordered_map<Path,            AssetSP, Hash<Path>>
	                                                            AssetPathMap;
	typedef std::unordered_map<const Asset*,    AspectSP>       AssetAspectMap;
	typedef std::unordered_map<std::type_index, AssetAspectMap> AspectMap;

private:
	AssetPathMap _assetMap;
	AspectMap    _aspects;

	mutable std::mutex _lock;
};



template<typename _Aspect>
inline IntrusivePointer<_Aspect> Asset::aspect() {
	return _manager->getAspect<_Aspect>(pointer());
}


template<typename _Aspect, typename... Args>
IntrusivePointer<_Aspect> Asset::createAspect(Args&&... args) {
	return _manager->createAspect<_Aspect>(pointer(), std::forward<Args>(args)...);
}


template<typename _Aspect, typename... Args>
IntrusivePointer<_Aspect> Asset::getOrCreateAspect(Args&&... args) {
	return _manager->getOrCreateAspect<_Aspect>(pointer(), std::forward<Args>(args)...);
}



template<typename _Aspect>
inline IntrusivePointer<_Aspect> AssetManager::getAspect(AssetSP asset) {
	return static_pointer_cast<_Aspect>(getAspect(typeid(_Aspect), asset));
}


template<typename _Aspect, typename... Args>
inline IntrusivePointer<_Aspect> AssetManager::createAspect(AssetSP asset, Args&&... args) {
	auto aspect = makeIntrusive<_Aspect>(asset, std::forward<Args>(args)...);
	setAspect(asset, aspect);
	return aspect;
}


template<typename _Aspect, typename... Args>
inline IntrusivePointer<_Aspect> AssetManager::getOrCreateAspect(AssetSP asset, Args&&... args) {
	auto aspect = makeIntrusive<_Aspect>(asset, std::forward<Args>(args)...);
	return static_pointer_cast<_Aspect>(getOrSetAspect(asset, aspect));
}


}


#endif
