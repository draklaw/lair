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


#include <functional>

#include <lair/core/lair.h>
#include <lair/core/log.h>

#include "lair/utils/loader.h"


namespace lair
{


Loader::Loader(LoaderManager* manager, const std::string& path)
    : _manager(manager),
      _isLoaded(false),
      _size(0),
      _path(path),
      _mutex(),
      _cv() {
}


Loader::~Loader() {
}


bool Loader::isLoaded() {
	std::unique_lock<std::mutex> lk(_mutex);
	return _isLoaded;
}


size_t Loader::size() {
	std::unique_lock<std::mutex> lk(_mutex);
	return _size;
}


void Loader::wait() {
	std::unique_lock<std::mutex> lk(_mutex);
	if(!_isLoaded) {
		_cv.wait(lk, [this]{ return _isLoaded; });
	}
}


void Loader::loadSync(Logger& /*log*/) {
}


void Loader::_done(Logger& /*log*/, size_t size) {
	{
		std::unique_lock<std::mutex> lk(_mutex);
		lairAssert(!_isLoaded);
		_isLoaded = true;
		_size = size;
		_manager->_doneLoading(this, size);
	}
	_cv.notify_all();
}


//---------------------------------------------------------------------------//


_LoaderThread::_LoaderThread()
	: _manager(nullptr),
      _thread (),
      _running(false) {
}


_LoaderThread::~_LoaderThread() {
	if(_thread.joinable()) {
		wait();
	}
}


void _LoaderThread::setManager(LoaderManager* manager) {
	lairAssert(manager);
	_manager = manager;
	_logger.setFrom(_manager->logger());
}


void _LoaderThread::start() {
	lairAssert(!_running && _manager);
	if(_thread.joinable()) {
		wait();
	}
	_manager->logger().debug("Starting loader thread...");
	_running = true;
	_thread = std::thread(&_LoaderThread::_run, std::ref(*this));
}


void _LoaderThread::stop() {
	lairAssert(_running && _thread.joinable());
	_manager->logger().debug("Stopping loader thread ", _thread.get_id(), "...");
	_running = false;
}


void _LoaderThread::wait() {
	lairAssert(_thread.joinable());
	if(_running) {
		stop();
	}
	auto id = _thread.get_id();
	_manager->logger().debug("Joinning loader thread ", id, "...");
	_thread.join();
	_manager->logger().debug("Joined loader thread ", id, ".");
}


void _LoaderThread::_run() {
	_logger.info("Start loader thread ", _thread.get_id(), ".");
	while(_running) {
		std::shared_ptr<Loader> loader = _manager->_popLoader();
		// Loader can be null to signal the thread it may be stopped
		if(loader) {
			_logger.log("Loading ", loader->path(), " from thread ", _thread.get_id(), "...");
			loader->loadSync(_logger);
			_logger.info("Done loading ", loader->path(), " from thread ", _thread.get_id(), ".");
		}
	}
	_logger.info("Stop loader thread ", _thread.get_id(), ".");
}


//---------------------------------------------------------------------------//


LoaderManager::LoaderManager(size_t maxCacheSize, unsigned nThread,
                             MasterLogger* logger, LogLevel logLevel)
    : _logger("loader", logger, logLevel),
      _queueLock(),
      _queueCv(),
      _queue(),
      _cacheLock(),
      _cache(),
      _cacheSize(0),
      _maxCacheSize(maxCacheSize),
      _nThread(0),
      _threadPool() {
	for(int i = 0; i < MAX_LOADER_THREADS; ++i) {
		_threadPool[i].setManager(this);
	}
	setNThread(nThread);
}


LoaderManager::~LoaderManager() {
	setNThread(0);
	for(unsigned i = 0; i < _nThread; ++i) {
		_threadPool[i].wait();
	}
}


unsigned LoaderManager::nToLoad() {
	std::unique_lock<std::mutex> lk(_queueLock);
	return _queue.size();
}


size_t LoaderManager::cacheSize() {
	std::unique_lock<std::mutex> lk(_cacheLock);
	return _cacheSize;
}


void LoaderManager::setNThread(unsigned count) {
	count = std::min(count, unsigned(MAX_LOADER_THREADS));
	if(count < _nThread) {
		while(_nThread != count) {
			--_nThread;
			_threadPool[_nThread].stop();
		}
		// Wake-up all thread so they return if stopped
		_queueCv.notify_all();
	} else {
		while(_nThread != count) {
			_threadPool[_nThread].start();
			++_nThread;
		}
	}
}


void LoaderManager::clearCache() {
	_cache.clear();
}


void LoaderManager::_enqueueLoader(LoaderPtr loader) {
	{
		std::unique_lock<std::mutex> lk(_queueLock);
		_queue.push_back(loader);
	}
	_queueCv.notify_one();
}

LoaderManager::LoaderPtr LoaderManager::_popLoader() {
	std::unique_lock<std::mutex> lk(_queueLock);
	if(!_queue.size()) {
		_queueCv.wait(lk);
	}
	LoaderPtr loader;
	if(_queue.size()) {
		loader = _queue.front();
		_queue.pop_front();
	}
	return loader;
}


void LoaderManager::_doneLoading(Loader* /*loader*/, size_t size) {
	std::unique_lock<std::mutex> lk(_cacheLock);
	_cacheSize += size;
}


}
