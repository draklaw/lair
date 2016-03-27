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


#ifndef _LAIR_UTILS_LOADER_H
#define _LAIR_UTILS_LOADER_H


#include <memory>

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
	Loader(LoaderManager* manager, AssetSP asset);
	Loader(const Loader&) = delete;
	Loader(Loader&&)      = delete;
	virtual ~Loader();

	Loader& operator=(const Loader&) = delete;
	Loader& operator=(Loader&&)      = delete;

	bool               isLoaded();
	bool               isSuccessful();
	AssetSP            asset() { return _asset; }
	Path               realPath() const;

	void wait();

	void loadSync(Logger& log);

protected:
	virtual void loadSyncImpl(Logger& log) = 0;

	void _success();

protected:
	LoaderManager*          _manager;
	bool                    _isLoaded;
	bool                    _isSuccessful;
	AssetSP                 _asset;
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
	LoaderManager(unsigned nThread = 1,
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

	template < typename L, typename... Args >
	std::shared_ptr<L> load(AssetSP asset, Args&&... args) {
		log().info("Request ", asset->logicPath(), " loading...");
		auto loader = std::make_shared<L>(this, asset,
		                                  std::forward<Args>(args)...);
		_enqueueLoader(loader);
		return loader;
	}

	Logger& log() { return *_logger; }

	void _enqueueLoader(LoaderSP loader);
	LoaderSP _popLoader();

private:
	typedef std::deque<LoaderSP> Queue;
	typedef std::list <LoaderSP> LoaderList;

private:
	Logger*       _logger;

	std::mutex    _queueLock;
	std::condition_variable
	              _queueCv;
	Queue         _queue;
	LoaderList    _wipList;

	unsigned      _nThread;
	_LoaderThread _threadPool[MAX_LOADER_THREADS];

	Path          _basePath;
};


}


#endif
