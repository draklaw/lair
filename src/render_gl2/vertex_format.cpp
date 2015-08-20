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

#include "lair/render_gl2/vertex_format.h"


namespace lair
{


VertexFormat::VertexFormat(GLsizei sizeInBytes, const VertexAttrib* attribs)
    : _sizeInBytes(sizeInBytes),
      _attribs() {
	const VertexAttrib* end = attribs;
	while(end->name) ++end;
	_attribs.assign(attribs, end);
}


VertexFormat::~VertexFormat() {
}


VertexFormat::AttribIterator VertexFormat::begin() const {
	return _attribs.begin();
}


VertexFormat::AttribIterator VertexFormat::end() const {
	return _attribs.end();
}


void VertexFormat::setup() const {
	for(const VertexAttrib& attrib: _attribs) {
		glEnableVertexAttribArray(attrib.index);
		glVertexAttribPointer(attrib.index, attrib.size, attrib.type,
		                      attrib.normalized, _sizeInBytes,
		                      reinterpret_cast<const void*>(attrib.offset));
	}

	LAIR_THROW_IF_OPENGL_ERROR();
}


void VertexFormat::clear() const {
	for(const VertexAttrib& attrib: _attribs) {
		glDisableVertexAttribArray(attrib.index);
	}
}


}
