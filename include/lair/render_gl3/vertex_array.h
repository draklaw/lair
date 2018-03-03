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


#ifndef LAIR_RENDER_GL3_VERTEX_ARRAY_H
#define LAIR_RENDER_GL3_VERTEX_ARRAY_H


#include <lair/core/lair.h>

#include <lair/render_gl3/context.h>


namespace lair
{


class Renderer;
class BufferObject;

struct VertexAttrib {
	BufferObject* buffer;
	GLuint        index;
	GLint         size;
	GLenum        type;
	GLboolean     normalized;
	GLsizei       offset;

	/// Arbitrary order so that sort() group attribs by buffer.
	inline bool operator<(const VertexAttrib& other) const {
		return buffer < other.buffer;
	}
};

#define LAIR_VERTEX_ATTRIB_END { nullptr, 0, 0, 0, 0, 0 }


class VertexArray {
protected:
	typedef std::vector<VertexAttrib> AttribList;

public:
	typedef AttribList::const_iterator AttribIterator;

public:
	VertexArray(Renderer* renderer, unsigned index, GLsizei sizeInBytes,
	            const VertexAttrib* attribs, BufferObject* indices = nullptr);
	VertexArray(const VertexArray&) = delete;
	VertexArray(VertexArray&&)      = delete;
	~VertexArray();

	VertexArray& operator=(const VertexArray&) = delete;
	VertexArray& operator=(VertexArray&&)      = delete;

	inline unsigned index() const { return _index; }
	inline GLsizei sizeInBytes() const { return _sizeInBytes; }

	AttribIterator begin() const;
	AttribIterator end() const;

	inline BufferObject* indices() const { return _indices; }

	/// Bind the vao (and set it if required).
	void setup();

	void _release();

protected:
	Context*      _context;
	Renderer*     _renderer;
	unsigned      _index;
	GLuint        _vao;
	GLsizei       _sizeInBytes;
	AttribList    _attribs;
	BufferObject* _indices;
};

typedef std::shared_ptr<VertexArray> VertexArraySP;


}


#endif
