/*
 *  Copyright (C) 2015, 2016 Simon Boyé
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


#include <iostream>
#include <functional>

#include <SDL_mixer.h>

#include <lair/utils/game_state.h>

#include "lair/utils/game_base.h"


#define DEFAULT_LOG_LEVEL LogLevel::Debug


namespace lair {


GameBase::GameBase(int /*argc*/, char** /*argv*/)
    : _mlogger(),
      _logStream("log.txt"),
//#ifndef _WIN32
      _stdlogBackend(std::clog, true),
//#endif
      _fileBackend(_logStream, false),
      _logger("game", &_mlogger, DEFAULT_LOG_LEVEL),

      _dataPath(),

      _sys(nullptr),
      _window(nullptr),

      _renderModule(nullptr),
      _renderer(nullptr),

      _audio(nullptr),

      _assets(nullptr),
      _loader(nullptr),

      _nextState(nullptr),
      _currentState(nullptr) {

	// TODO: Parse arguments to override some config values.

//#ifndef _WIN32
	_mlogger.addBackend(&_stdlogBackend);
//#endif
	_mlogger.addBackend(&_fileBackend);
	dbgLogger.setMaster(&_mlogger);
	dbgLogger.setDefaultModuleName("DEBUG");
	dbgLogger.setLevel(LogLevel::Debug);
}


GameBase::~GameBase() {
	log().log("Stopping game...");
}


Path GameBase::dataPath() const {
	return _dataPath;
}


SysModule* GameBase::sys() {
	return _sys.get();
}


Window* GameBase::window() {
	return _window;
}


AssetManager* GameBase::assets() {
	return _assets.get();
}


LoaderManager* GameBase::loader() {
	return _loader.get();
}


RenderModule* GameBase::renderModule() {
	return _renderModule.get();
}


Renderer* GameBase::renderer() {
	return _renderer;
}


AudioModule* GameBase::audio() {
	return _audio.get();
}


void GameBase::initialize() {
	log().log("Starting game...");

	_sys.reset(new SysModule(&_mlogger, LogLevel::Log));
	_sys->initialize();
	_sys->onQuit = std::bind(&GameBase::quit, this);

	Path configLogicPath = "config.json";
	Path configRealPath = sys()->basePath() / configLogicPath;
	parseJson(_config, configRealPath, configLogicPath, log());

	const char* envPath = std::getenv("LAIR_DATA_DIR");
	if (envPath) {
		_dataPath = envPath;
	} else {
		_dataPath = _sys->basePath() / "assets";
	}
	log().log("Data directory: ", _dataPath);

	_assets = make_unique(new AssetManager);
	_loader = make_unique(new LoaderManager(_assets.get(), 1, _logger));
	_loader->setBasePath(dataPath());

	_window = _sys->createWindow("Lair", 1280, 720);
	if(_config.get("fullscreen", false).asBool()) {
		_window->setFullscreen(true);
	}
	//_sys->setVSyncEnabled(false);
	log().info("VSync: ", _sys->isVSyncEnabled()? "on": "off");

	_renderModule.reset(new RenderModule(sys(), assets(), &_mlogger, DEFAULT_LOG_LEVEL));
	_renderModule->initialize();
	_renderer = _renderModule->createRenderer();

	_audio.reset(new AudioModule(&_mlogger));
	_audio->initialize();
	_audio->setMusicVolume(.35);
}


void GameBase::shutdown() {
	_audio->shutdown();

	_renderModule->shutdown();
	_renderModule.reset();

	_window->destroy();
	_sys->shutdown();
	_sys.reset();
}


void GameBase::setNextState(GameState* state) {
	if(_nextState) {
		log().warning("Setting next state while an other state is enqueued.");
	}
	_nextState = state;
}


void GameBase::run() {
	while(_nextState) {
		_currentState = _nextState;
		_nextState    = nullptr;
		_currentState->run();
	}
}


void GameBase::quit() {
	if(_currentState) {
		_currentState->quit();
	}
}


}
