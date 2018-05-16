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

#include <lair/asset/image.h>

#include <lair/render_gl3/context.h>
#include <lair/render_gl3/renderer.h>

#include "lair/render_gl3/texture.h"


namespace lair
{


Texture::Texture(Renderer* renderer)
    : _context       (renderer? renderer->context(): nullptr),
      _renderer      (renderer),
      _target        (0),
      _id            (0),
      _width         (0),
      _height        (0),
      _format        (0),
      _maxMipmapLevel(0) {
}


Texture::Texture(Texture&& other)
    : _context       (other._context),
      _renderer      (other._renderer),
      _target        (other._target),
      _id            (other._id),
      _width         (other._width),
      _height        (other._height),
      _format        (other._format),
      _maxMipmapLevel(other._maxMipmapLevel) {
	other._context        = nullptr;
	other._renderer       = nullptr;
	other._target         = 0;
	other._id             = 0;
	other._width          = 0;
	other._height         = 0;
	other._format         = 0;
	other._maxMipmapLevel = 0;
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
	_context->bindTexture(_target, _id);
}


bool Texture::_upload(const Image& image, unsigned maxMipmapLevel, bool linear) {
	lairAssert(image.isValid());

	// Compute GL parameters from image

	GLenum target = gl::TEXTURE_2D;

	GLenum format;
	GLenum imgFormat;
	switch(image.format()) {
	case Image::FormatRGB8:
		format    = linear? gl::RGB8: gl::SRGB8;
		imgFormat = gl::RGB;
		break;
	case Image::FormatRGBA8:
		format    = linear? gl::RGBA8: gl::SRGB8_ALPHA8;
		imgFormat = gl::RGBA;
		break;
	default:
		return false;
	}

	unsigned nMipmaps = 0;
	unsigned maxSize = std::max(image.width(), image.height());
	while(maxSize) {
		nMipmaps += 1;
		maxSize = maxSize >> 1;
	}
	// Note: maxMipmapLevel is *not* the number of mipmap, it is the index of
	// the last one.
	maxMipmapLevel = std::min(maxMipmapLevel, nMipmaps - 1);

	// We use immutable texture storage, so if the texture size / format changed,
	// we recreate a new texture.
	bool allocateStorage = !_id
	                    || target         != _target
	                    || image.width()  != _width
	                    || image.height() != _height
	                    || format         != _format
	                    || maxMipmapLevel != _maxMipmapLevel;

	_target         = target;
	_width          = image.width();
	_height         = image.height();
	_format         = format;
	_maxMipmapLevel = maxMipmapLevel;

	if(allocateStorage) {
		_release();
		_context->genTextures(1, &_id);
	}

	_context->bindTexture(_target, _id);

	if(_context->_gl_arb_texture_storage) {
		_context->texStorage2D(_target, _maxMipmapLevel + 1, _format,
		                       _width, _height);
	}
	else {
		_context->texImage2D(_target, 0, _format, _width, _height, 0,
		                     imgFormat, gl::UNSIGNED_BYTE, nullptr);
	}

	_context->texSubImage2D(_target, 0, 0, 0, _width, _height,
	                        imgFormat, gl::UNSIGNED_BYTE, image.data());
	_context->texParameteri(_target, gl::TEXTURE_MAX_LEVEL, _maxMipmapLevel);

	if(_maxMipmapLevel > 0) {
		_context->generateMipmap(_target);
	}

	return true;
}


void swap(Texture& t0, Texture& t1) {
	std::swap(t0._context,        t1._context);
	std::swap(t0._renderer,       t1._renderer);
	std::swap(t0._target,         t1._target);
	std::swap(t0._id,             t1._id);
	std::swap(t0._width,          t1._width);
	std::swap(t0._height,         t1._height);
	std::swap(t0._format,         t1._format);
	std::swap(t0._maxMipmapLevel, t1._maxMipmapLevel);
}


void Texture::_release() {
	if(isValid()) {
		_context->deleteTextures(1, &_id);
	}
}


}
