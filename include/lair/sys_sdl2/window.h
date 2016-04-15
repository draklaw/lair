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


#ifndef _LAIR_SYS_SDL2_WINDOW_H
#define _LAIR_SYS_SDL2_WINDOW_H


#include <string>

#include <SDL.h>

#include <lair/core/lair.h>

#include <lair/core/signal.h>


namespace lair
{

class Logger;
class SysModule;

class Window {
public:
	Window(SysModule* sys);
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	~Window();

	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) = delete;

	int width() const;
	int height() const;

	bool isValid() const;
	bool isFullscreen() const;
	bool isVisible() const;

	const char* utf8Title() const;

	void resize(int width, int height);
	void setFullscreen(bool fullscreen);
	void setVisible(bool visible);
	void setUtf8Title(const char* utf8Title);
	void destroy();

	void swapBuffers();

	// Input grabbing.
	// maximize, minimize and resize stuff.

	SDL_Window* _create(const char* utf8Title, int width, int height);
	unsigned _windowID() const;
	SDL_Window* _sdlWindow();

	const char* _sdlWindowEventName(unsigned type) const;
	void _processEvent(const SDL_WindowEvent& event);

	Logger& log();

	Signal<> onResize;

private:
	SysModule* _sys;
	SDL_Window* _window;
	SDL_GLContext _glContext;
};


}


#endif
