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


GameConfigBase::GameConfigBase()
    : _fullscreen(false)
    , _vSync     (true)
{
}

bool GameConfigBase::fullscreen() const {
	return _fullscreen;
}

void GameConfigBase::setFullscreen(bool enable) {
	_fullscreen = enable;
}

bool GameConfigBase::vSync() const {
	return _vSync;
}

void GameConfigBase::setVSync(bool enable) {
	_vSync = enable;
}

void GameConfigBase::setFromArgs(int& argc, char** argv) {
	int last = 1;
	for(int argi = 1; argi < argc; ++ argi) {
		char* arg = argv[argi];
		if(std::strcmp(arg, "--fullscreen") == 0) {
			_fullscreen = true;
		}
		else if(std::strcmp(arg, "--no-fullscreen") == 0) {
			_fullscreen = false;
		}
		else if(std::strcmp(arg, "--vsync") == 0) {
			_vSync = true;
		}
		else if(std::strcmp(arg, "--no-vsync") == 0) {
			_vSync = false;
		}
		else {
			argv[last++] = arg;
		}
	}
	argv[last] = 0;
	argc = last;
}

const PropertyList& GameConfigBase::properties() const {
	return GameConfigBase::staticProperties();
}

const PropertyList& GameConfigBase::staticProperties() {
	static PropertyList props;
	if(!props.nProperties()) {
		props.addProperty("fullscreen",
		                  &GameConfigBase::fullscreen,
		                  &GameConfigBase::setFullscreen);
		props.addProperty("vsync",
		                  &GameConfigBase::vSync,
		                  &GameConfigBase::setVSync);
	}
	return props;
}



GameBase::GameBase(int argc, char** argv)
    : _mlogger(),
      _logStream("log.txt"),
//#ifndef _WIN32
      _stdlogBackend(std::clog, true),
//#endif
      _fileBackend(_logStream, false),
      _logger("game", &_mlogger, DEFAULT_LOG_LEVEL),

      _argc(argc),
      _argv(argv),

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


LdlPropertySerializer& GameBase::serializer() {
	return _serializer;
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


void GameBase::initialize(GameConfigBase& config) {
	log().log("Starting game...");

	_sys.reset(new SysModule(&_mlogger, LogLevel::Log));
	_sys->initialize();
	_sys->onQuit = std::bind(&GameBase::quit, this);

	// Config

	Path configLogicPath = "config.ldl";
	Path configRealPath = sys()->basePath() / configLogicPath;

	Path::IStream in(configRealPath.native().c_str());
	if(in.good()) {
		log().info("Read config \"", configRealPath, "\"...");
		ErrorList errors;
		LdlParser parser(&in, configLogicPath.utf8String(), &errors, LdlParser::CTX_MAP);
		_serializer.read(parser, config);
		errors.log(log());
	}
	else {
		log().info("Config not found, create one: \"", configLogicPath, "\"...");
		ErrorList errors;
		Path::OStream out(configRealPath.native().c_str());
		if(out.good()) {
			LdlWriter writer(&out, configLogicPath.utf8String(), &errors);
			_serializer.write(writer, config);
			errors.log(log());
		}
		else {
			log().warning("Unable to create the config file \"", configRealPath, "\".");
		}
	}

	// Command-line arguments override config files.
	config.setFromArgs(_argc, _argv);

	// Data path

	const char* envPath = std::getenv("LAIR_DATA_DIR");
	if(envPath) {
		_dataPath = envPath;
	} else {
		_dataPath = _sys->basePath() / "assets";
	}
	log().log("Data directory: ", _dataPath);

	// Assets

	_assets = make_unique(new AssetManager);
	_loader = make_unique(new LoaderManager(_assets.get(), 1, _logger));
	_loader->setBasePath(dataPath());

	// Window

	_window = _sys->createWindow("Lair", 1280, 720);
	_window->setFullscreen(config.fullscreen());
	_sys->setVSyncEnabled(config.vSync());
	log().info("VSync: ", _sys->isVSyncEnabled()? "on": "off");

	// Render

	_renderModule.reset(new RenderModule(sys(), assets(), &_mlogger, DEFAULT_LOG_LEVEL));
	_renderModule->initialize();
	_renderer = _renderModule->createRenderer();

	// Audio

	_audio.reset(new AudioModule(&_mlogger));
	_audio->initialize();
	_audio->setMusicVolume(.35);
}


void GameBase::shutdown() {
	_assets->releaseAll();

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
