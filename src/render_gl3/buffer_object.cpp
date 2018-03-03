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

#include <lair/render_gl3/context.h>
#include <lair/render_gl3/renderer.h>

#include "lair/render_gl3/buffer_object.h"


namespace lair
{


BufferObject::BufferObject(Renderer* renderer, GLenum target, GLenum usage)
    : _context(renderer->context()),
      _renderer(renderer),
      _target(target),
      _usage(usage),
      _size(0),
      _buffer(0),
      _begin(nullptr),
      _pos(nullptr) {
}


BufferObject::~BufferObject() {
	_release();
}


void BufferObject::bind(GLenum target) {
	if(!_buffer) {
		_context->genBuffers(1, &_buffer);
	}

	if(!target) {
		target = _target;
	}

	_context->bindBuffer(target, _buffer);
}


void BufferObject::beginWrite(Size size) {
	lairAssert(!_begin);

	bind();

	if(size != _size) {
		_context->bufferData(_target, size, nullptr, _usage);
		_size = size;
	}

	_begin = reinterpret_cast<Byte*>(
	            _context->mapBufferRange(
	                    _target, 0, _size,
	                    gl::MAP_WRITE_BIT | gl::MAP_INVALIDATE_RANGE_BIT |
	                    gl::MAP_FLUSH_EXPLICIT_BIT));
	_pos = _begin;
}

bool BufferObject::endWrite() {
	lairAssert(_begin);

	bind();

	if(pos() <= _size) {
		_context->flushMappedBufferRange(_target, 0, pos());
	}

	_begin = nullptr;
	return _context->unmapBuffer(_target);
}

bool BufferObject::write(const void* data, Size size) {
	lairAssert(_begin);

	bool fitIn = (_pos + size <= _begin + _size);
	if(fitIn) {
		memcpy(_pos, data, size);
	}
	_pos += size;

	return fitIn;
}


void BufferObject::_release() {
	if(_buffer) {
		_context->deleteBuffers(1, &_buffer);
		_buffer = 0;
	}
}


}
