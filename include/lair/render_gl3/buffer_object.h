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


#ifndef LAIR_RENDER_GL3_BUFFER_OBJECT_H
#define LAIR_RENDER_GL3_BUFFER_OBJECT_H


#include <lair/core/lair.h>

#include <lair/render_gl3/context.h>


namespace lair
{


class Renderer;


/**
 * \brief An OpenGL buffer object.
 *
 * This class provides a subset of the features of real buffer objects. It only
 * allows to write linearly in it (like a stream) and you must do so between
 * beginWrite() and endWrite(). Behind the scene, it tries to upload the data
 * efficiently.
 */
class BufferObject {
public:
	BufferObject(Renderer* renderer, GLenum target = gl::ARRAY_BUFFER,
	             GLenum usage = gl::STREAM_DRAW);
	BufferObject(const BufferObject&) = delete;
	BufferObject(BufferObject&&)      = delete;
	~BufferObject();

	BufferObject& operator=(const BufferObject&) = delete;
	BufferObject& operator=(BufferObject&&)      = delete;

	void bind(GLenum target = 0);

	void beginWrite(Size size);
	bool endWrite();

	inline Size pos() const {
		lairAssert(_begin);
		return Size(_pos - _begin);
	}

	/**
	 * \brief Write data in the buffer. *Must* be called between beginWrite() /
	 * endWrite().
	 *
	 * If the write leads to an overflow, return false. However, pos() keeps
	 * growing, so it is possible to finish writing and then check how much was
	 * written.
	 */
	bool write(const void* data, Size size);

	template<typename T>
	bool write(const T& obj) {
		return write(reinterpret_cast<const void*>(&obj), sizeof(T));
	}

	template<typename T>
	bool write(const T* objs, unsigned count = 1) {
		return write(reinterpret_cast<const void*>(objs), sizeof(T) * count);
	}

	void _release();

protected:
	Context*  _context;
	Renderer* _renderer;
	GLenum    _target;
	GLenum    _usage;
	Size      _size;
	GLuint    _buffer;
	Byte*     _begin;
	Byte*     _pos;
};


}


#endif
