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


#ifndef _LAIR_RENDER_GL2_GL_H
#define _LAIR_RENDER_GL2_GL_H


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include <lair/render_gl2/context.h>


#define LAIR_THROW_IF_OPENGL_ERROR(_glc) lair::throwGlError(_glc, __FILE__, __LINE__)
#define LAIR_LOG_OPENGL_ERRORS(_glc) lair::logGlError(_glc, log(), __FILE__, __LINE__)
#define LAIR_LOG_OPENGL_ERRORS_TO(_glc, _log) lair::logGlError(_glc, _log, __FILE__, __LINE__)


namespace lair {


struct GLError : public std::logic_error {
	GLError(GLenum error, const char* file, int line);
};


struct GLOutOfMemory : public std::runtime_error {
	GLOutOfMemory();
};

const char* glErrorString(GLenum error);


inline void logGlError(Context* glc, Logger& log, const char* file, int line) {
	GLenum error;
	while((error = glc->getError()) != gl::NO_ERROR) {
		if(error == gl::OUT_OF_MEMORY)
			throw GLOutOfMemory();

		log.error(file, ":", line, ": OpenGL error: ", glErrorString(error));
	}
}

inline void throwGlError(Context* glc, const char* file, int line) {
	GLenum error = glc->getError();
	if(error != gl::NO_ERROR) {
		if(error == gl::OUT_OF_MEMORY)
			throw GLOutOfMemory();
		throw GLError(error, file, line);
	}
}


}


#endif
