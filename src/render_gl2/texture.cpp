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
#include <lair/core/image.h>

#include "lair/render_gl2/texture.h"


namespace lair
{


Texture::Texture()
    : _id(0),
      _flags(0),
      _width(0),
      _height(0) {
}


Texture::~Texture() {
	release();
}


bool Texture::upload(const Image& image, uint32 flags) {
	if(!_id) {
		glGenTextures(1, &_id);
	}

	glBindTexture(GL_TEXTURE_2D, _id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, image.data());
	LAIR_THROW_IF_OPENGL_ERROR();

	_width  = image.width();
	_height = image.height();
	setFlags(flags);
	return true;
}


void Texture::setFlags(uint32 flags) {
	_flags = flags;

	GLenum mag;
	switch(magFilter()) {
	case MAG_NEAREST: mag = GL_NEAREST;
	case MAG_LINEAR:  mag = GL_LINEAR;
	}

	GLenum min;
	switch(minFilter() | mipmapMode()) {
	case MIN_NEAREST | MIPMAP_NONE:    min = GL_NEAREST;
	case MIN_LINEAR  | MIPMAP_NONE:    min = GL_LINEAR;
	case MIN_NEAREST | MIPMAP_NEAREST: min = GL_NEAREST_MIPMAP_NEAREST;
	case MIN_LINEAR  | MIPMAP_NEAREST: min = GL_LINEAR_MIPMAP_NEAREST;
	case MIN_NEAREST | MIPMAP_LINEAR:  min = GL_NEAREST_MIPMAP_LINEAR;
	case MIN_LINEAR  | MIPMAP_LINEAR:  min = GL_LINEAR_MIPMAP_LINEAR;
	}

	GLenum wraps;
	switch(wrapS()) {
	case REPEAT_S: wraps = GL_REPEAT;
	case CLAMP_S:  wraps = GL_CLAMP_TO_EDGE;
	case MIRROR_S: wraps = GL_MIRRORED_REPEAT;
	}

	GLenum wrapt;
	switch(wrapT()) {
	case REPEAT_T: wrapt = GL_REPEAT;
	case CLAMP_T:  wrapt = GL_CLAMP_TO_EDGE;
	case MIRROR_T: wrapt = GL_MIRRORED_REPEAT;
	}

	glBindTexture(GL_TEXTURE_2D, _id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     wraps);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     wrapt);

	if(mipmapMode() != MIPMAP_NONE) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	LAIR_THROW_IF_OPENGL_ERROR();
}


void Texture::release() {
	if(isValid()) {
		glDeleteTextures(1, &_id);
	}
}


}
