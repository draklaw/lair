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
      _height(0),
      _loader() {
}


Texture::Texture(Texture&& other)
    : _id(other._id),
      _flags(other._flags),
      _width(other._width),
      _height(other._height),
      _loader() {
	other._id = 0;
	other._flags = 0;
	other._width = 0;
	other._height = 0;
	other._loader.reset();
}


Texture::~Texture() {
	_release();
}


Texture& Texture::operator=(Texture other) {
	swap(*this, other);
	return *this;
}


void Texture::_load(ImageLoaderPtr loader) {
	lairAssert(!isValid() && !_loader);
	_loader = loader;
}


bool Texture::_uploadNow() {
	if(isValid()) {
		// Valid textures should not hold a reference to a loader.
		lairAssert(!_loader);
		return true;
	}
	if(!_loader) {
		// We likely failed loading previously.
		return false;
	}

	_loader->wait();
	bool ok = false;
	if(_loader->getImage().isValid()) {
		ok = _upload(_loader->getImage());
	}
	_loader.reset();

	return ok;
}


bool Texture::_upload(const Image& image) {
	lairAssert(image.isValid());

	if(!_id) {
		glGenTextures(1, &_id);
	}

	glBindTexture(GL_TEXTURE_2D, _id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, image.data());
	glGenerateMipmap(GL_TEXTURE_2D);
	LAIR_THROW_IF_OPENGL_ERROR();

	_width  = image.width();
	_height = image.height();

	return true;
}


void swap(Texture& t0, Texture& t1) {
	std::swap(t0._id,     t1._id);
	std::swap(t0._flags,  t1._flags);
	std::swap(t0._width,  t1._width);
	std::swap(t0._height, t1._height);
	std::swap(t0._loader, t1._loader);
}


void Texture::_setFlags(uint32 flags) {
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

	LAIR_THROW_IF_OPENGL_ERROR();
}


void Texture::_release() {
	if(isValid()) {
		glDeleteTextures(1, &_id);
	}
}


}
