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


#include <SDL.h>

#include <lair/core/lair.h>

#include "lair/sys_sdl2/sys_module.h"

#include "lair/sys_sdl2/window.h"


namespace lair {


Window::Window(SysModule* sys)
    : _sys(sys),
      _window(nullptr),
      _glContext(nullptr) {
	lairAssert(sys);
}


Window::~Window() {
	if(isValid()) {
		destroy();
	}
}


int Window::width() const {
	lairAssert(isValid());
	int w;
	SDL_GetWindowSize(_window, &w, 0);
	return w;
}


int Window::height() const {
	lairAssert(isValid());
	int h;
	SDL_GetWindowSize(_window, 0, &h);
	return h;
}


bool Window::isValid() const {
	return _window;
}


bool Window::isFullscreen() const {
	lairAssert(isValid());
	return SDL_GetWindowFlags(_window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
}


bool Window::isVisible() const {
	lairAssert(isValid());
	return SDL_GetWindowFlags(_window) & SDL_WINDOW_SHOWN;
}


const char* Window::utf8Title() const {
	lairAssert(isValid());
	return SDL_GetWindowTitle(_window);
}


void Window::destroy() {
	lairAssert(isValid());

	log().log("Destroy window: \"", utf8Title(), "\"");

//	onDestroy();

	if(_sys) {
		_sys->_removeWindow(_windowID());
	}

//	SDL_GL_DeleteContext(_glContext);
//	_glContext = 0;

	SDL_DestroyWindow(_window);
	_window = 0;

	_sys = 0;
}


void Window::swapBuffers() {
	SDL_GL_SwapWindow(_window);
}


void Window::resize(int width, int height) {
	lairAssert(isValid());
	SDL_SetWindowSize(_window, width, height);
}


void Window::setFullscreen(bool fullscreen) {
	lairAssert(isValid());

	unsigned flag = 0;
	if(fullscreen) {
		flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	int r = SDL_SetWindowFullscreen(_window, flag);
	if(r != 0) {
		// TODO
	}
}


void Window::setVisible(bool visible) {
	lairAssert(isValid());
	if(visible)
		SDL_ShowWindow(_window);
	else
		SDL_HideWindow(_window);
}


void Window::setUtf8Title(const char* utf8Title) {
	lairAssert(isValid());
	SDL_SetWindowTitle(_window, utf8Title);
}


SDL_Window* Window::_create(const char* utf8Title, int width, int height) {
	lairAssert(!isValid());

//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	unsigned windowFlags = 0
	        | SDL_WINDOW_OPENGL
//	        | SDL_WINDOW_FULLSCREEN_DESKTOP
	        ;
	_window = SDL_CreateWindow(
	            utf8Title,
	            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	            width, height,
	            windowFlags);
	if(!_window) {
		log().error("Failed to create SDL window.", SDL_GetError());
		return 0;
	}

	_glContext = SDL_GL_CreateContext(_window);
	if(!_glContext) {
		log().error("failed to create OpenGL context: ", SDL_GetError());
		SDL_DestroyWindow(_window);
		return 0;
	}

	typedef std::pair<const char*, SDL_GLattr> GLAttr;
	GLAttr glAttrs[] =
	{
	    std::make_pair("SDL_GL_RED_SIZE = ", SDL_GL_RED_SIZE),
	    std::make_pair("SDL_GL_GREEN_SIZE = ", SDL_GL_GREEN_SIZE),
	    std::make_pair("SDL_GL_BLUE_SIZE = ", SDL_GL_BLUE_SIZE),
	    std::make_pair("SDL_GL_ALPHA_SIZE = ", SDL_GL_ALPHA_SIZE),
	    std::make_pair("SDL_GL_BUFFER_SIZE = ", SDL_GL_BUFFER_SIZE),
	    std::make_pair("SDL_GL_DOUBLEBUFFER = ", SDL_GL_DOUBLEBUFFER),
	    std::make_pair("SDL_GL_DEPTH_SIZE = ", SDL_GL_DEPTH_SIZE),
	    std::make_pair("SDL_GL_STENCIL_SIZE = ", SDL_GL_STENCIL_SIZE),
	    std::make_pair("SDL_GL_ACCUM_RED_SIZE = ", SDL_GL_ACCUM_RED_SIZE),
	    std::make_pair("SDL_GL_ACCUM_GREEN_SIZE = ", SDL_GL_ACCUM_GREEN_SIZE),
	    std::make_pair("SDL_GL_ACCUM_BLUE_SIZE = ", SDL_GL_ACCUM_BLUE_SIZE),
	    std::make_pair("SDL_GL_ACCUM_ALPHA_SIZE = ", SDL_GL_ACCUM_ALPHA_SIZE),
	    std::make_pair("SDL_GL_STEREO = ", SDL_GL_STEREO),
	    std::make_pair("SDL_GL_MULTISAMPLEBUFFERS = ", SDL_GL_MULTISAMPLEBUFFERS),
	    std::make_pair("SDL_GL_MULTISAMPLESAMPLES = ", SDL_GL_MULTISAMPLESAMPLES),
	    std::make_pair("SDL_GL_ACCELERATED_VISUAL = ", SDL_GL_ACCELERATED_VISUAL),
	    //        std::make_pair("SDL_GL_RETAINED_BACKING = ", SDL_GL_RETAINED_BACKING),
	    std::make_pair("SDL_GL_CONTEXT_MAJOR_VERSION = ", SDL_GL_CONTEXT_MAJOR_VERSION),
	    std::make_pair("SDL_GL_CONTEXT_MINOR_VERSION = ", SDL_GL_CONTEXT_MINOR_VERSION),
	    //        std::make_pair("SDL_GL_CONTEXT_EGL = ", SDL_GL_CONTEXT_EGL),
	    std::make_pair("SDL_GL_CONTEXT_FLAGS = ", SDL_GL_CONTEXT_FLAGS),
	    std::make_pair("SDL_GL_CONTEXT_PROFILE_MASK = ", SDL_GL_CONTEXT_PROFILE_MASK),
	    std::make_pair("SDL_GL_SHARE_WITH_CURRENT_CONTEXT = ", SDL_GL_SHARE_WITH_CURRENT_CONTEXT),
	    //        std::make_pair("SDL_GL_FRAMEBUFFER_SRGB_CAPABLE = ", SDL_GL_FRAMEBUFFER_SRGB_CAPABLE),
	};
	for(unsigned i = 0; i < (sizeof(glAttrs) / sizeof(GLAttr)); ++i) {
		int attr;
		SDL_GL_GetAttribute(glAttrs[i].second, &attr);
		log().info(glAttrs[i].first, attr);
	}

	return _window;
}


unsigned Window::_windowID() const {
	return SDL_GetWindowID(_window);
}


SDL_Window* Window::_sdlWindow() {
	return _window;
}


const char* Window::_sdlWindowEventName(unsigned type) const {
	switch(type) {
	case SDL_WINDOWEVENT_SHOWN: return "SDL_WINDOWEVENT_SHOWN";
	case SDL_WINDOWEVENT_HIDDEN: return "SDL_WINDOWEVENT_HIDDEN";
	case SDL_WINDOWEVENT_EXPOSED: return "SDL_WINDOWEVENT_EXPOSED";
	case SDL_WINDOWEVENT_MOVED: return "SDL_WINDOWEVENT_MOVED";
	case SDL_WINDOWEVENT_RESIZED: return "SDL_WINDOWEVENT_RESIZED";
	case SDL_WINDOWEVENT_SIZE_CHANGED: return "SDL_WINDOWEVENT_SIZE_CHANGED";
	case SDL_WINDOWEVENT_MINIMIZED: return "SDL_WINDOWEVENT_MINIMIZED";
	case SDL_WINDOWEVENT_MAXIMIZED: return "SDL_WINDOWEVENT_MAXIMIZED";
	case SDL_WINDOWEVENT_RESTORED: return "SDL_WINDOWEVENT_RESTORED";
	case SDL_WINDOWEVENT_ENTER: return "SDL_WINDOWEVENT_ENTER";
	case SDL_WINDOWEVENT_LEAVE: return "SDL_WINDOWEVENT_LEAVE";
	case SDL_WINDOWEVENT_FOCUS_GAINED: return "SDL_WINDOWEVENT_FOCUS_GAINED";
	case SDL_WINDOWEVENT_FOCUS_LOST: return "SDL_WINDOWEVENT_FOCUS_LOST";
	case SDL_WINDOWEVENT_CLOSE: return "SDL_WINDOWEVENT_CLOSE";
	default: return "UNKNOWN_WINDOW_EVENT";
	}
}


void Window::_processEvent(const SDL_WindowEvent& event) {
	log().debug("Window event: ", _sdlWindowEventName(event.event));

	switch(event.event) {
	case SDL_WINDOWEVENT_SHOWN:
	case SDL_WINDOWEVENT_HIDDEN:
	case SDL_WINDOWEVENT_EXPOSED:
	case SDL_WINDOWEVENT_MOVED:
	case SDL_WINDOWEVENT_RESIZED:
	case SDL_WINDOWEVENT_SIZE_CHANGED:
	case SDL_WINDOWEVENT_MINIMIZED:
	case SDL_WINDOWEVENT_MAXIMIZED:
	case SDL_WINDOWEVENT_RESTORED:
	case SDL_WINDOWEVENT_ENTER:
	case SDL_WINDOWEVENT_LEAVE:
	case SDL_WINDOWEVENT_FOCUS_GAINED:
	case SDL_WINDOWEVENT_FOCUS_LOST:
		break;
	case SDL_WINDOWEVENT_CLOSE:
		break;

	default:
		log().warning("Unknown SDL window event: ", event.type);
		break;
	}
}


Logger& Window::log() {
	lairAssert(_sys);
	return _sys->log();
}


}
