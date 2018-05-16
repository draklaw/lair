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

#include <lair/ldl/read.h>
#include <lair/ldl/write.h>

#include <lair/utils/game_state.h>

#include "lair/utils/game_base.h"


#define DEFAULT_LOG_LEVEL LogLevel::Debug


namespace lair {


// This function is declared in an auto-generated file
void registerLairResource(MemoryFileSystem& fs);


GameConfigBase::GameConfigBase()
    : fullscreen(false)
    , vSync     (true)
    , soundVolume(.25)
    , musicVolume(.35)
    , windowSize(1280, 720)
    , debugGl(false)
{
}

void GameConfigBase::setFromArgs(int& argc, char** argv) {
	int last = 1;
	for(int argi = 1; argi < argc; ++ argi) {
		char* arg = argv[argi];
		if(std::strcmp(arg, "-f") == 0 || std::strcmp(arg, "--fullscreen") == 0) {
			fullscreen = true;
		}
		else if(std::strcmp(arg, "--no-fullscreen") == 0) {
			fullscreen = false;
		}
		else if(std::strcmp(arg, "--vsync") == 0) {
			vSync = true;
		}
		else if(std::strcmp(arg, "--no-vsync") == 0) {
			vSync = false;
		}
		else if(std::strcmp(arg, "--debug-gl") == 0) {
			debugGl = true;
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
		                  &GameConfigBase::fullscreen);
		props.addProperty("vsync",
		                  &GameConfigBase::vSync);
		props.addProperty("sound_volume",
		                  &GameConfigBase::soundVolume);
		props.addProperty("music_volume",
		                  &GameConfigBase::musicVolume);
		props.addProperty("window_size",
		                  &GameConfigBase::windowSize);
		props.addProperty("debug_gl",
		                  &GameConfigBase::debugGl);
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


FileSystemSP GameBase::fileSystem() const {
	return _fileSystem;
}


PropertySerializer& GameBase::serializer() {
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


int GameBase::argc() const {
	return _argc;
}


char** GameBase::argv() const {
	return _argv;
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
		Variant confVar;
		parseLdl(confVar, configRealPath, configLogicPath, log());
		_serializer.read(config, confVar, log());
	}
	else {
		log().info("Config not found, create one: \"", configLogicPath, "\"...");
		ErrorList errors;
		Path::OStream out(configRealPath.native().c_str());
		if(out.good()) {
			LdlWriter writer(&out, configLogicPath.utf8String(), &errors);
			Variant confVar;
			_serializer.write(confVar, config, log());
			ldlWrite(writer, confVar);
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

	// File systems

	_realFs.reset(new RealFileSystem(_dataPath));
	_memoryFs.reset(new MemoryFileSystem);

	registerLairResource(*_memoryFs);

	_fileSystem = std::make_shared<FileSystem>();
	_fileSystem->addFileSystem("/", _realFs);
	_fileSystem->addFileSystem("/", _memoryFs);

	// Assets

	_assets = make_unique(new AssetManager);
	_loader = make_unique(new LoaderManager(_assets.get(), 1, _logger));
	_loader->setFileSystem(fileSystem());

	// Window

	_window = _sys->createWindow("Lair", config.windowSize(0), config.windowSize(1));
	_window->setFullscreen(config.fullscreen);
	_sys->setVSyncEnabled(config.vSync);
	log().info("VSync: ", _sys->isVSyncEnabled()? "on": "off");

	// Render

	_renderModule.reset(new RenderModule(sys(), assets(), &_mlogger, DEFAULT_LOG_LEVEL));
	_renderModule->initialize(config.debugGl);
	_renderer = _renderModule->createRenderer();

	// Audio

	_audio.reset(new AudioModule(&_mlogger));
	_audio->initialize();
	_audio->setMusicVolume(config.musicVolume);
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
