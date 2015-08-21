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


#ifndef _LAIR_RENDER_GL2_BATCH_H
#define _LAIR_RENDER_GL2_BATCH_H


#include <unordered_map>

#include <lair/core/lair.h>

#include <lair/render_gl2/vertex_buffer.h>


namespace lair
{


class ProgramObject;
class Texture;
class VertexFormat;

class Batch {
public:
	Batch();
	Batch(const Batch&) = delete;
	Batch(Batch&&)      = delete;
	~Batch();

	Batch& operator=(const Batch&) = delete;
	Batch& operator=(Batch&&)      = delete;

	VertexBuffer& getBuffer(const ProgramObject* shader, Texture* texture,
	                        const VertexFormat* format);

	void clearBuffers();
	void clearAll();

	void render();

protected:
	struct BufferId {
		inline BufferId(const ProgramObject* shader, Texture* texture,
		                const VertexFormat* format)
		    : shader(shader), texture(texture), format(format) {
		}
		inline bool operator==(const BufferId& other) const {
			return shader == other.shader && texture == other.texture
			        && format == other.format;
		}
		inline bool operator!=(const BufferId& other) const {
			return !(*this == other);
		}

		const ProgramObject* shader;
		Texture*             texture;
		const VertexFormat*        format;
	};

	struct HashBufferId {
		size_t operator()(const BufferId& bid) const;
	};

	typedef std::unordered_map<BufferId, VertexBuffer, HashBufferId> BufferMap;

protected:
	BufferMap _buffers;
};


}


#endif
