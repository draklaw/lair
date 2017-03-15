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


#ifndef _LAIR_CORE_LOADER_H
#define _LAIR_CORE_LOADER_H


#include <memory>
#include <typeinfo>
#include <typeindex>

#include <deque>
#include <unordered_map>

#include <string>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/path.h>
#include <lair/core/asset_manager.h>


namespace lair
{


enum {
	MAX_LOADER_THREADS = 8
};


class Loader;
class LoaderManager;

typedef std::shared_ptr<Loader> LoaderSP;


class Loader {
public:
	typedef std::function<void(AspectSP, Logger&)> DoneCallback;

public:
	Loader(LoaderManager* manager, AspectSP aspect);
	Loader(const Loader&) = delete;
	Loader(Loader&&)      = delete;
	virtual ~Loader();

	Loader& operator=(const Loader&) = delete;
	Loader& operator=(Loader&&)      = delete;

	bool               isLoaded();
	bool               isSuccessful();
	AssetSP            asset()    const { return _aspect->asset(); }
	AspectSP           aspect()   const { return _aspect; }
	Path               realPath() const;

	void registerCallback(DoneCallback&& callback);

	void wait();

	void loadSync(Logger& log);

protected:
	typedef std::vector<DoneCallback> FunctionList;

protected:
	virtual void loadSyncImpl(Logger& log) = 0;

	template <typename L>
	LoaderSP _load(const Path& logicPath, DoneCallback callback);

	void _done(Logger& log);
	void _success();

protected:
	LoaderManager*          _manager;
	unsigned                _depCount;
	FunctionList            _onDone;
	AspectSP                _aspect;
	std::mutex              _mutex;
	std::condition_variable _cv;
};


class _LoaderThread {
public:
	_LoaderThread();
	~_LoaderThread();

	void setManager(LoaderManager* manager);

	void start();
	void stop();
	void wait();

private:
	void _run();

private:
	Logger           _logger;
	LoaderManager*   _manager;
	std::thread      _thread;
	std::atomic_bool _running;
};


class LoaderManager {
public:
	LoaderManager(AssetManager* assetManager,
	              unsigned nThread = 1,
	              Logger& log = noopLogger);
	LoaderManager(const LoaderManager&) = delete;
	LoaderManager(LoaderManager&&)      = delete;
	~LoaderManager();

	LoaderManager& operator=(const LoaderManager&) = delete;
	LoaderManager& operator=(LoaderManager&&)      = delete;

	unsigned    nThread() const { return _nThread; }
	unsigned    nToLoad();
	const Path& basePath() const { return _basePath; }

	void setNThread(unsigned count);
	void setBasePath(const Path& path);
	Path realFromLogic(const Path& path) const;

	/// Load an aspect, even if already loaded.
	template < typename L, typename... Args >
	std::shared_ptr<L> load(std::shared_ptr<typename L::Aspect> aspect, Args&&... args) {
		auto loader = std::make_shared<L>(this, aspect,
										  std::forward<Args>(args)...);
		if(!loader->isLoaded()) {
			log().info("Request \"", aspect->asset()->logicPath(), "\" loading...");
			_enqueueLoader(loader);
		}
		return loader;
	}

	/// Load an aspect if it does not exist in the asset.
	template < typename L, typename... Args >
	std::shared_ptr<L> load(AssetSP asset, Args&&... args) {
		auto aspect = asset->aspect<typename L::Aspect>();
		if(!aspect) {
			aspect = asset->createAspect<typename L::Aspect>();
		}
		return load<L>(aspect, std::forward<Args>(args)...);
	}

	/// Same as above, but create the Asset if necessary.
	template < typename L, typename... Args >
	std::shared_ptr<L> load(const Path& logicPath, Args&&... args) {
		AssetSP asset = _assets->getAsset(logicPath);
		if(!asset) {
			asset = _assets->createAsset(logicPath);
		}
		return load<L>(asset, std::forward<Args>(args)...);
	}

	/// Same as above, but return the Asset.
	template < typename L, typename... Args >
	AssetSP loadAsset(const Path& logicPath, Args&&... args) {
		return load<L>(logicPath, std::forward<Args>(args)...)->asset();
	}

	template < typename L, typename... Args >
	void loadSync(std::shared_ptr<typename L::Aspect> aspect, Args&&... args) {
		log().log("Loading \"", aspect->asset()->logicPath(), "\" from thread ",
		          std::this_thread::get_id(), " (sync)...");
		L loader(this, aspect, std::forward<Args>(args)...);
		loader.loadSync(log());
	}

	template < typename L, typename... Args >
	void loadSync(AssetSP asset, Args&&... args) {
		auto aspect = asset->aspect<typename L::Aspect>();
		if(!aspect) {
			aspect = asset->createAspect<typename L::Aspect>();
			loadSync<L>(aspect, std::forward<Args>(args)...);
		}
	}

	template < typename L, typename... Args >
	AssetSP loadSync(const Path& logicPath, Args&&... args) {
		AssetSP asset = _assets->getAsset(logicPath);
		if(!asset) {
			asset = _assets->createAsset(logicPath);
		}
		loadSync<L>(asset, std::forward<Args>(args)...);
		return asset;
	}

	void waitAll();

	Logger& log() { return *_logger; }

	void _enqueueLoader(LoaderSP loader);
	LoaderSP _popLoader();
	LoaderSP _getAnyPendingLoader();

private:
	typedef std::deque<LoaderSP> Queue;
	typedef std::list <LoaderSP> LoaderList;

private:
	Logger*       _logger;
	AssetManager* _assets;

	std::mutex    _queueLock;
	std::condition_variable
	              _queueCv;
	Queue         _queue;
	LoaderList    _wipList;

	unsigned      _nThread;
	_LoaderThread _threadPool[MAX_LOADER_THREADS];

	Path          _basePath;
};


template <typename L>
LoaderSP Loader::_load(const Path& logicPath, DoneCallback callback) {
	auto loader = _manager->load<L>(logicPath);
	loader->registerCallback([this, callback](AspectSP aspect, Logger& log) {
		callback(aspect, log);
		_done(log);
	});

	std::unique_lock<std::mutex> lk(_mutex);
	++_depCount;

	return loader;
}


}


#endif
