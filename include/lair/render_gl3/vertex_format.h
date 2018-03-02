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


#ifndef _LAIR_RENDER_GL3_VERTEX_FORMAT_H
#define _LAIR_RENDER_GL3_VERTEX_FORMAT_H


#include <vector>

#include <lair/core/lair.h>

#include <lair/render_gl3/context.h>


namespace lair
{


struct VertexAttrib {
	const char* name;
	GLuint      index;
	GLint       size;
	GLenum      type;
	GLboolean   normalized;
	GLsizei     offset;
};


class VertexFormat {
protected:
	typedef std::vector<VertexAttrib> AttribList;

public:
	typedef AttribList::const_iterator AttribIterator;

public:
	VertexFormat(GLsizei sizeInBytes, const VertexAttrib* attribs);
	VertexFormat(const VertexFormat&) = delete;
	VertexFormat(VertexFormat&&)      = delete;
	~VertexFormat();

	VertexFormat& operator=(const VertexFormat&) = delete;
	VertexFormat& operator=(VertexFormat&&)      = delete;

	inline GLsizei sizeInBytes() const { return _sizeInBytes; }

	AttribIterator begin() const;
	AttribIterator end() const;

	void setup(Context* glc) const;
	void clear(Context* glc) const;

protected:
	GLsizei    _sizeInBytes;
	AttribList _attribs;
};


}


#endif
