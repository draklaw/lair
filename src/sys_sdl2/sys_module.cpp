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


#include <chrono>
#include <thread>

#include <SDL.h>
#include <SDL_filesystem.h>
#include <SDL_image.h>

#include <lair/core/lair.h>

#include "lair/sys_sdl2/window.h"

#include "lair/sys_sdl2/sys_module.h"


namespace lair
{


SysModule::SysModule(MasterLogger* logger, LogLevel level)
    : onQuit(nullptr),
      _log("sys_sdl2", logger, level),
      _initialized(false),
      _windowMap(),
      _basePath() {
}


SysModule::~SysModule() {
	if(_initialized) {
		shutdown();
	}
}


bool SysModule::initialize() {
	lairAssert(!_initialized);

	log().log("System initialization...");

	SDL_version compileVersion;
	SDL_version linkVersion;
	SDL_VERSION(&compileVersion);
	SDL_GetVersion(&linkVersion);

	if(compileVersion.major == linkVersion.major &&
	        compileVersion.minor == linkVersion.minor &&
	        compileVersion.patch == linkVersion.patch) {
		log().info("SDL version: ", int(linkVersion.major), ".",
		           int(linkVersion.minor), ".", int(linkVersion.patch));
	}
	else {
		log().warning("SDL version mismatch: compiled against ",
		              int(compileVersion.major), ".",
		              int(compileVersion.minor), ".",
		              int(compileVersion.patch),
		              ", linked against ",
		              int(linkVersion.major), ".",
		              int(linkVersion.minor), ".",
		              int(linkVersion.patch));
	}


	int err = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if(err < 0) {
		log().error("SDL initialization failed: ", SDL_GetError());
		return false;
	}

	int imgErr = IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	if(imgErr < 0) {
		log().error("SDL image initialization failed: ", IMG_GetError());
		return false;
	}

	{
		auto basePath = make_unique(SDL_GetBasePath(), SDL_free);
		_basePath = Path(basePath.get());
	}
	log().info("System base path: ", _basePath.utf8String());

	_initialized = true;

	setScreensaverEnabled(true);

	return true;
}


void SysModule::shutdown() {
	lairAssert(_initialized);

	log().log("System shutdown...");
	destroyAllWindows();

	_initialized = false;

	SDL_Quit();
}


bool SysModule::isScreensaverEnabled() const {
	lairAssert(_initialized);

	return SDL_IsScreenSaverEnabled() == SDL_TRUE;
}


void SysModule::setScreensaverEnabled(bool enableScreensaver) {
	lairAssert(_initialized);
	if(enableScreensaver)
		SDL_EnableScreenSaver();
	else
		SDL_DisableScreenSaver();
}


bool SysModule::isVSyncEnabled() const {
	int si = SDL_GL_GetSwapInterval();
	return si == 1;
}


void SysModule::setVSyncEnabled(bool enableVSync) {
	int res;
	if(enableVSync) {
		//        res = SDL_GL_SetSwapInterval(-1);
		//        if(res == -1) res = SDL_GL_SetSwapInterval(1);
		res = SDL_GL_SetSwapInterval(1);
	}
	else {
		res = SDL_GL_SetSwapInterval(0);
	}
	if(res < 0) {
		log().error("Failed to set VSync.");
	}
}


Window* SysModule::createWindow(const char* utf8Title, int width, int height) {
	lairAssert(_initialized);

	log().log("Create window: \"", utf8Title, "\", ", width, "x", height);
	WindowPtr window(new Window(this));
	SDL_Window* sdlWindow = window->_create(utf8Title, width, height);
	if(!sdlWindow) {
		return 0;
	}

	Window* ptr = window.get();
	_windowMap.emplace(window->_windowID(), std::move(window));

	return ptr;
}


void SysModule::destroyAllWindows() {
	lairAssert(_initialized);

	while(!_windowMap.empty()) {
		_windowMap.begin()->second->destroy();
	}
}


void SysModule::waitAndDispatchSystemEvents() {
	SDL_Event event;
	SDL_WaitEvent(&event);
	_dispatchSystemEvent(event);
	dispatchPendingSystemEvents();
}


void SysModule::dispatchPendingSystemEvents() {
	SDL_Event event;
	while(SDL_PollEvent(&event))
		_dispatchSystemEvent(event);
}


uint8 SysModule::getKeyState(unsigned scancode) {
	int size = 0;
	const uint8* states = SDL_GetKeyboardState(&size);
	return (scancode < unsigned(size))? states[scancode]: 0;
}


const Path& SysModule::basePath() {
	return _basePath;
}


const Path SysModule::getPrefPath(const char* org, const char* app) {
	Path p;
	{
		auto path = make_unique(SDL_GetPrefPath(org, app), SDL_free);
		_basePath = Path(path.get());
	}
	log().info("Preference path (", org, ", ", app, "): ", p);
	return p;
}


uint64 SysModule::getTimeNs() const {
//	return double(SDL_GetPerformanceCounter())
//	     / double(SDL_GetPerformanceFrequency())
//	     * 1e9;
//	return SDL_GetTicks() * 1000000;
	uint64 time = std::chrono::time_point_cast<std::chrono::nanoseconds>(
	            std::chrono::high_resolution_clock::now()).time_since_epoch().count();
	static uint64 start = time;
	return time - start;
}


void SysModule::waitNs(uint64 ns) const {
	if(ns == 0) return;
	// integer division rounded up.
//	SDL_Delay((ns-1) / 1000000000 + 1);
//	SDL_Delay(double(ns) / 1.6);
//	SDL_Delay((ns-1) / 1000000 + 1);
	std::this_thread::sleep_for(std::chrono::nanoseconds(ns));
}


void SysModule::_removeWindow(unsigned windowID) {
	lairAssert(_initialized);

	int removed = _windowMap.erase(windowID);
	lairAssert(removed == 1);
}


const char* SysModule::_sdlEventName(unsigned type) const {
	switch(type) {
	case SDL_QUIT: return "SDL_QUIT";
	case SDL_APP_TERMINATING: return "SDL_APP_TERMINATING";
	case SDL_APP_LOWMEMORY: return "SDL_APP_LOWMEMORY";
	case SDL_APP_WILLENTERBACKGROUND: return "SDL_APP_WILLENTERBACKGROUND";
	case SDL_APP_DIDENTERBACKGROUND: return "SDL_APP_DIDENTERBACKGROUND";
	case SDL_APP_WILLENTERFOREGROUND: return "SDL_APP_WILLENTERFOREGROUND";
	case SDL_APP_DIDENTERFOREGROUND: return "SDL_APP_DIDENTERFOREGROUND";
	case SDL_WINDOWEVENT: return "SDL_WINDOWEVENT";
	case SDL_SYSWMEVENT: return "SDL_SYSWMEVENT";
	case SDL_KEYDOWN: return "SDL_KEYDOWN";
	case SDL_KEYUP: return "SDL_KEYUP";
	case SDL_TEXTEDITING: return "SDL_TEXTEDITING";
	case SDL_TEXTINPUT: return "SDL_TEXTINPUT";
	case SDL_MOUSEMOTION: return "SDL_MOUSEMOTION";
	case SDL_MOUSEBUTTONDOWN: return "SDL_MOUSEBUTTONDOWN";
	case SDL_MOUSEBUTTONUP: return "SDL_MOUSEBUTTONUP";
	case SDL_MOUSEWHEEL: return "SDL_MOUSEWHEEL";
	case SDL_JOYAXISMOTION: return "SDL_JOYAXISMOTION";
	case SDL_JOYBALLMOTION: return "SDL_JOYBALLMOTION";
	case SDL_JOYHATMOTION: return "SDL_JOYHATMOTION";
	case SDL_JOYBUTTONDOWN: return "SDL_JOYBUTTONDOWN";
	case SDL_JOYBUTTONUP: return "SDL_JOYBUTTONUP";
	case SDL_JOYDEVICEADDED: return "SDL_JOYDEVICEADDED";
	case SDL_JOYDEVICEREMOVED: return "SDL_JOYDEVICEREMOVED";
	case SDL_CONTROLLERAXISMOTION: return "SDL_CONTROLLERAXISMOTION";
	case SDL_CONTROLLERBUTTONDOWN: return "SDL_CONTROLLERBUTTONDOWN";
	case SDL_CONTROLLERBUTTONUP: return "SDL_CONTROLLERBUTTONUP";
	case SDL_CONTROLLERDEVICEADDED: return "SDL_CONTROLLERDEVICEADDED";
	case SDL_CONTROLLERDEVICEREMOVED: return "SDL_CONTROLLERDEVICEREMOVED";
	case SDL_CONTROLLERDEVICEREMAPPED: return "SDL_CONTROLLERDEVICEREMAPPED";
	case SDL_FINGERDOWN: return "SDL_FINGERDOWN";
	case SDL_FINGERUP: return "SDL_FINGERUP";
	case SDL_FINGERMOTION: return "SDL_FINGERMOTION";
	case SDL_DOLLARGESTURE: return "SDL_DOLLARGESTURE";
	case SDL_DOLLARRECORD: return "SDL_DOLLARRECORD";
	case SDL_MULTIGESTURE: return "SDL_MULTIGESTURE";
	case SDL_CLIPBOARDUPDATE: return "SDL_CLIPBOARDUPDATE";
	case SDL_DROPFILE: return "SDL_DROPFILE";
	default: return "UNKNOWN_EVENT";
	}
}


void SysModule::_dispatchSystemEvent(const SDL_Event& event)
{
	log().debug("System event: ", _sdlEventName(event.type));

	switch(event.type) {
	/* Application events */
	case SDL_QUIT:
		if(onQuit) onQuit();
		break;

		/* These application events have special meaning on iOS, see README-ios.txt for details */
	case SDL_APP_TERMINATING:
	case SDL_APP_LOWMEMORY:
	case SDL_APP_WILLENTERBACKGROUND:
	case SDL_APP_DIDENTERBACKGROUND:
	case SDL_APP_WILLENTERFOREGROUND:
	case SDL_APP_DIDENTERFOREGROUND:
		break;

		/* Window events */
	case SDL_WINDOWEVENT: {
		Window* w = _windowFromID(event.window.windowID);
		if(w) w->_processEvent(event.window);
		else log().warning("Unknown window ID: ", event.window.windowID);
		break;
	}
	case SDL_SYSWMEVENT:
		break;

		/* Keyboard events */
	case SDL_KEYDOWN:
	case SDL_KEYUP:
	case SDL_TEXTEDITING:
	case SDL_TEXTINPUT:
		break;

		/* Mouse events */
	case SDL_MOUSEMOTION:
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEWHEEL:
		break;

		/* Joystick events */
	case SDL_JOYAXISMOTION:
	case SDL_JOYBALLMOTION:
	case SDL_JOYHATMOTION:
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
	case SDL_JOYDEVICEADDED:
	case SDL_JOYDEVICEREMOVED:
		break;

		/* Game controller events */
	case SDL_CONTROLLERAXISMOTION:
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
	case SDL_CONTROLLERDEVICEADDED:
	case SDL_CONTROLLERDEVICEREMOVED:
	case SDL_CONTROLLERDEVICEREMAPPED:
		break;

		/* Touch events */
	case SDL_FINGERDOWN:
	case SDL_FINGERUP:
	case SDL_FINGERMOTION:
		break;

		/* Gesture events */
	case SDL_DOLLARGESTURE:
	case SDL_DOLLARRECORD:
	case SDL_MULTIGESTURE:
		break;

		/* Clipboard events */
	case SDL_CLIPBOARDUPDATE:
		break;

		/* Drag and drop events */
	case SDL_DROPFILE:
		break;

	default:
		log().info("Unknown SDL system event: ", event.type);
		break;
	}
}


Window* SysModule::_windowFromID(unsigned windowID) {
	auto it = _windowMap.find(windowID);
	if(it != _windowMap.end())
		return it->second.get();
	else
		return 0;
}


}
