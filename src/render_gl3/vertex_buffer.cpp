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

#include "lair/render_gl3/vertex_buffer.h"


namespace lair
{


VertexBuffer::VertexBuffer(size_t vertexSize)
	: _vertexSize(vertexSize),
	  _vxBuffer(),
	  _indexBuffer(),
	  _vxBufferId(0),
	  _indexBufferId(0) {
}


VertexBuffer::~VertexBuffer() {
}


void VertexBuffer::addIndex(unsigned index) {
	_indexBuffer.push_back(index);
}

void VertexBuffer::clear() {
	_vxBuffer.clear();
	_indexBuffer.clear();
}


void VertexBuffer::bindAndUpload(Context* glc) {
	if(!_vxBufferId) {
		glc->genBuffers(1, &_vxBufferId);
	}
	glc->bindBuffer(gl::ARRAY_BUFFER, _vxBufferId);
	glc->bufferData(gl::ARRAY_BUFFER, _vxBuffer.size(), _vxBuffer.data(),
	                gl::STREAM_DRAW);

	if(!_indexBufferId) {
		glc->genBuffers(1, &_indexBufferId);
	}
	glc->bindBuffer(gl::ELEMENT_ARRAY_BUFFER, _indexBufferId);
	glc->bufferData(gl::ELEMENT_ARRAY_BUFFER, _indexBuffer.size() * sizeof(unsigned),
	                _indexBuffer.data(), gl::STREAM_DRAW);
}


}
