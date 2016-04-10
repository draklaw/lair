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

#include <lair/render_gl2/context.h>
#include <lair/render_gl2/renderer.h>

#include "lair/render_gl2/texture.h"


namespace lair
{


Texture::Texture(Renderer* renderer)
	: _context (renderer? renderer->context(): nullptr),
	  _renderer(renderer),
	  _id      (0),
	  _flags   (0),
	  _width   (0),
	  _height  (0) {
}


Texture::Texture(Texture&& other)
	: _context (other._context),
	  _renderer(other._renderer),
	  _id      (other._id),
	  _flags   (other._flags),
	  _width   (other._width),
	  _height  (other._height) {
	other._context  = nullptr;
	other._renderer = nullptr;
	other._id       = 0;
	other._flags    = 0;
	other._width    = 0;
	other._height   = 0;
}


Texture::~Texture() {
	_release();
}


Texture& Texture::operator=(Texture other) {
	swap(*this, other);
	return *this;
}


void Texture::bind() const {
	lairAssert(_id);
	_context->bindTexture(gl::TEXTURE_2D, _id);
}


bool Texture::_upload(const Image& image) {
	lairAssert(image.isValid());

	if(!_id) {
		_context->genTextures(1, &_id);
	}

	GLenum iformat;
	GLenum format;
	switch(image.format()) {
	case Image::FormatRGB8:
		iformat = gl::RGB;
		format  = gl::RGB;
		break;
	case Image::FormatRGBA8:
		iformat = gl::RGBA;
		format  = gl::RGBA;
		break;
	default:
		return false;
	}

	_context->bindTexture(gl::TEXTURE_2D, _id);
	_context->texImage2D(gl::TEXTURE_2D, 0, iformat, image.width(), image.height(), 0,
	                     format, gl::UNSIGNED_BYTE, image.data());
	_context->generateMipmap(gl::TEXTURE_2D);

	_width  = image.width();
	_height = image.height();

	return true;
}


void swap(Texture& t0, Texture& t1) {
	std::swap(t0._context,  t1._context);
	std::swap(t0._renderer, t1._renderer);
	std::swap(t0._id,       t1._id);
	std::swap(t0._flags,    t1._flags);
	std::swap(t0._width,    t1._width);
	std::swap(t0._height,   t1._height);
}


void Texture::_setFlags(uint32 flags) {
	_flags = flags;

	GLenum mag;
	switch(magFilter()) {
	case MAG_NEAREST: mag = gl::NEAREST; break;
	case MAG_LINEAR:  mag = gl::LINEAR; break;
	}

	GLenum min;
	switch(minFilter() | mipmapMode()) {
	case MIN_NEAREST | MIPMAP_NONE:    min = gl::NEAREST; break;
	case MIN_LINEAR  | MIPMAP_NONE:    min = gl::LINEAR; break;
	case MIN_NEAREST | MIPMAP_NEAREST: min = gl::NEAREST_MIPMAP_NEAREST; break;
	case MIN_LINEAR  | MIPMAP_NEAREST: min = gl::LINEAR_MIPMAP_NEAREST; break;
	case MIN_NEAREST | MIPMAP_LINEAR:  min = gl::NEAREST_MIPMAP_LINEAR; break;
	case MIN_LINEAR  | MIPMAP_LINEAR:  min = gl::LINEAR_MIPMAP_LINEAR; break;
	}

	GLenum wraps;
	switch(wrapS()) {
	case REPEAT_S: wraps = gl::REPEAT; break;
	case CLAMP_S:  wraps = gl::CLAMP_TO_EDGE; break;
	case MIRROR_S: wraps = gl::MIRRORED_REPEAT; break;
	}

	GLenum wrapt;
	switch(wrapT()) {
	case REPEAT_T: wrapt = gl::REPEAT; break;
	case CLAMP_T:  wrapt = gl::CLAMP_TO_EDGE; break;
	case MIRROR_T: wrapt = gl::MIRRORED_REPEAT; break;
	}

	_context->bindTexture(gl::TEXTURE_2D, _id);
	_context->texParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, mag);
	_context->texParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, min);
	_context->texParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S,     wraps);
	_context->texParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T,     wrapt);
}


void Texture::_release() {
	if(isValid()) {
		_context->deleteTextures(1, &_id);
	}
}


TextureAspect::TextureAspect(AssetSP asset, Renderer* renderer)
	: Aspect(asset),
	  _tex(renderer) {
}


}
