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


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include "lair/render_gl2/gl.h"


namespace lair
{


static std::string _formatGlError(GLenum error, const char *file, int line) {
	std::ostringstream out;
	out << file << ":" << line << ": OpenGL error: " << glErrorString(error);
	return out.str();
}


GLError::GLError(GLenum error, const char *file, int line)
    : std::logic_error(_formatGlError(error, file, line)) {
}


GLOutOfMemory::GLOutOfMemory()
    : std::runtime_error("out of graphic memory") {
}


const char* glErrorString(GLenum error) {
	switch(error) {
	case gl::NO_ERROR:          return "no error";
	case gl::INVALID_ENUM:      return "invalid enum";
	case gl::INVALID_FRAMEBUFFER_OPERATION:
		return "invalid framebuffer operation";
	case gl::INVALID_VALUE:     return "invalid value";
	case gl::INVALID_OPERATION: return "invalid operation";
	case gl::OUT_OF_MEMORY:     return "out of memory";
	}
	return "unknown error";
}


}
