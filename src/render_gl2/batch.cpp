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


#include <boost/functional/hash.hpp>

#include <lair/core/lair.h>
#include <lair/core/log.h>

#include <lair/render_gl2/vertex_format.h>
#include <lair/render_gl2/program_object.h>
#include <lair/render_gl2/texture.h>

#include "lair/render_gl2/batch.h"


namespace lair
{


Batch::Batch()
    : _buffers() {
}


Batch::~Batch() {
}


VertexBuffer& Batch::getBuffer(const ProgramObject* shader, Texture* texture,
                        const VertexFormat* format) {
	BufferId id(shader, texture, format);
	auto buffIt = _buffers.find(id);
	if(buffIt == _buffers.end()) {
		buffIt = _buffers.emplace_hint(buffIt, id, VertexBuffer(format->sizeInBytes()));
	}
	return buffIt->second;
}


void Batch::clearBuffers() {
	for(auto& idBuff: _buffers) {
		idBuff.second.clear();
	}
}


void Batch::clearAll() {
	_buffers.clear();
}


void Batch::render(Context* glc) {
	glc->activeTexture(gl::TEXTURE0);
	for(auto& idBuff: _buffers) {
		idBuff.first.shader->use();
		glc->bindTexture(gl::TEXTURE_2D, idBuff.first.texture->_glId());
		idBuff.second.bindAndUpload(glc);
		idBuff.first.format->setup(glc);

		glc->drawElements(gl::TRIANGLES, idBuff.second.indexSize(), gl::UNSIGNED_INT, 0);

		idBuff.first.format->clear(glc);
	}
}


size_t Batch::HashBufferId::operator()(const BufferId& bid) const {
	size_t h = 0;
	boost::hash_combine(h, bid.shader);
	boost::hash_combine(h, bid.texture);
	boost::hash_combine(h, bid.format);
	return h;
}


}
