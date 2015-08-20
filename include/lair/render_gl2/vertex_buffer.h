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


#ifndef _LAIR_RENDER_GL2_VERTEX_BUFFER_H
#define _LAIR_RENDER_GL2_VERTEX_BUFFER_H


#include <vector>

#include <lair/core/lair.h>


namespace lair
{


class VertexBuffer {
public:
	VertexBuffer(size_t vertexSize);
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer(VertexBuffer&&)      = default;
	~VertexBuffer();

	VertexBuffer& operator=(const VertexBuffer&) = delete;
	VertexBuffer& operator=(VertexBuffer&&)      = default;

	inline size_t vertexSize() const { return _vxBuffer.size(); }
	inline const Byte* vertexData() const { return _vxBuffer.data(); }

	inline size_t indexSize() const { return _indexBuffer.size(); }
	inline const unsigned* indexData() const { return _indexBuffer.data(); }

	template < typename T >
	inline void addVertex(const T& data) {
		lairAssert(sizeof(T) == _vertexSize);
		const Byte* begin = reinterpret_cast<const Byte*>(&data);
		_vxBuffer.insert(_vxBuffer.end(), begin, begin + sizeof(T));
	}

	void addIndex(unsigned index);

	void clear();

	void bindAndUpload();

protected:
	typedef std::vector<Byte> Buffer;
	typedef std::vector<unsigned> IndexBuffer;

protected:
	size_t      _vertexSize;
	Buffer      _vxBuffer;
	IndexBuffer _indexBuffer;

	GLuint      _vxBufferId;
	GLuint      _indexBufferId;
};


}


#endif
