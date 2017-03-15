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

#include "lair/core/loader.h"


namespace lair
{


Loader::Loader(LoaderManager* manager, AspectSP aspect)
    : _manager(manager),
      _depCount(aspect->isValid()? 0: 1),
      _aspect(aspect),
      _mutex(),
      _cv() {
}


Loader::~Loader() {
}


bool Loader::isLoaded() {
	std::unique_lock<std::mutex> lk(_mutex);
	return _depCount == 0;
}


bool Loader::isSuccessful() {
	return _aspect->isValid();
}


Path Loader::realPath() const {
	return _manager->realFromLogic(asset()->logicPath());
}


void Loader::registerCallback(DoneCallback&& callback) {
	_onDone.push_back(callback);
}


void Loader::wait() {
	std::unique_lock<std::mutex> lk(_mutex);
	if(_depCount != 0) {
		_cv.wait(lk, [this]{ return _depCount == 0; });
	}
}


void Loader::loadSync(Logger& log) {
	lairAssert(!isLoaded());

	// It is *very* important to catch exceptions here, or waiting threads
	// could wait forever. Moreover, it would unexpectedly stop a worker
	// thread.
	try {
		loadSyncImpl(log);
	} catch(std::exception e) {
		log.error("Exception caught while loading \"", asset()->logicPath(), "\": ", e.what());
	}

	_done(log);
}


void Loader::_done(Logger& log) {
	bool finalize = false;

	{
		std::unique_lock<std::mutex> lk(_mutex);

		if(_depCount > 0) {
			log.warning(asset()->logicPath(), " done: ", _depCount, " -> ", _depCount - 1);
			--_depCount;
			finalize = (_depCount == 0);
		}
	}

	if(finalize) {
		for(auto& callback: _onDone) {
			callback(_aspect, log);
		}
		_cv.notify_all();
	}
}


void Loader::_success() {
	_aspect->_setValid(true);
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
	_logger.setFrom(_manager->log());
}


void _LoaderThread::start() {
	lairAssert(!_running && _manager);
	if(_thread.joinable()) {
		wait();
	}
	_manager->log().debug("Starting loader thread...");
	_running = true;
	_thread = std::thread(&_LoaderThread::_run, this);
}


void _LoaderThread::stop() {
	lairAssert(_running && _thread.joinable());
	_manager->log().debug("Stopping loader thread ", _thread.get_id(), "...");
	_running = false;
}


void _LoaderThread::wait() {
	lairAssert(_thread.joinable());
	if(_running) {
		stop();
	}
	auto id = _thread.get_id();
	_manager->log().debug("Joinning loader thread ", id, "...");
	_thread.join();
	_manager->log().debug("Joined loader thread ", id, ".");
}


void _LoaderThread::_run() {
	_logger.log("Start loader thread ", _thread.get_id(), ".");
	while(_running) {
		LoaderSP loader = _manager->_popLoader();
		// Loader can be null to signal the thread it may be stopped
		if(loader) {
			_logger.log("Loading \"", loader->asset()->logicPath(), "\" from thread ", _thread.get_id(), "...");
			loader->loadSync(_logger);
			_logger.info("Done loading \"", loader->asset()->logicPath(), "\" from thread ", _thread.get_id(), ".");
		}
	}
	_logger.log("Stop loader thread ", _thread.get_id(), ".");
}


//---------------------------------------------------------------------------//


LoaderManager::LoaderManager(AssetManager* assetManager, unsigned nThread, Logger& logger)
    : _logger(&logger),
      _assets(assetManager),
      _queueLock(),
      _queueCv(),
      _queue(),
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


void LoaderManager::setBasePath(const Path& path) {
	_basePath = path;
}


Path LoaderManager::realFromLogic(const Path& path) const {
	// TODO: use a FileSystem to access file
	return _basePath / path;
}


void LoaderManager::waitAll() {
	while(LoaderSP loader = _getAnyPendingLoader()) {
		loader->wait();
	}
}


void LoaderManager::_enqueueLoader(LoaderSP loader) {
	{
		std::unique_lock<std::mutex> lk(_queueLock);
		_queue.push_back(loader);
	}
	_queueCv.notify_one();
}


LoaderSP LoaderManager::_popLoader() {
	std::unique_lock<std::mutex> lk(_queueLock);
	if(!_queue.size()) {
		_queueCv.wait(lk);
	}

	LoaderSP loader;
	if(_queue.size()) {
		loader = _queue.front();
		_queue.pop_front();
		_wipList.push_back(loader);
	}
	return loader;
}


LoaderSP LoaderManager::_getAnyPendingLoader() {
	std::unique_lock<std::mutex> lk(_queueLock);
	if(_queue.empty()) {
		while(!_wipList.empty() && _wipList.front()->isLoaded()) {
			_wipList.pop_front();
		}
		return _wipList.empty()? LoaderSP(): _wipList.front();
	}
	return _queue.back();
}


}
