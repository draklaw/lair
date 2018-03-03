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
#include <lair/render_gl3/buffer_object.h>
#include <lair/render_gl3/renderer.h>

#include "lair/render_gl3/vertex_array.h"


namespace lair
{


VertexArray::VertexArray(Renderer* renderer, unsigned index, GLsizei sizeInBytes,
                         const VertexAttrib* attribs, BufferObject* indices)
    : _context(renderer->context()),
      _renderer(renderer),
      _index(index),
      _vao(0),
      _sizeInBytes(sizeInBytes),
      _attribs(),
      _indices(indices) {
	const VertexAttrib* end = attribs;
	while(end->buffer) ++end;
	_attribs.assign(attribs, end);

	// Sort by buffer, to avoid useless re-binding.
	std::sort(_attribs.begin(), _attribs.end());
}


VertexArray::~VertexArray() {
	_release();
}


VertexArray::AttribIterator VertexArray::begin() const {
	return _attribs.begin();
}


VertexArray::AttribIterator VertexArray::end() const {
	return _attribs.end();
}


void VertexArray::setup() {
	if(!_vao) {
		_context->genVertexArrays(1, &_vao);

		_context->bindVertexArray(_vao);

		BufferObject* buffer = nullptr;
		for(const VertexAttrib& attrib: _attribs) {
			if(attrib.buffer != buffer) {
				buffer = attrib.buffer;
				buffer->bind();
			}
			_context->enableVertexAttribArray(attrib.index);
			_context->vertexAttribPointer(attrib.index, attrib.size, attrib.type,
			                              attrib.normalized, _sizeInBytes,
			                              reinterpret_cast<const void*>(attrib.offset));
		}

		if(_indices) {
			_indices->bind(gl::ELEMENT_ARRAY_BUFFER);
		}

		return;
	}

	_context->bindVertexArray(_vao);
}


void VertexArray::_release() {
	if(_vao) {
		_context->deleteVertexArrays(1, &_vao);
		_vao = 0;
	}
}

}
