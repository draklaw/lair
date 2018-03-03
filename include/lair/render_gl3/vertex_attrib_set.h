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


#ifndef LAIR_RENDER_GL3_VERTEX_ATTRIB_SET_H
#define LAIR_RENDER_GL3_VERTEX_ATTRIB_SET_H


#include <lair/core/lair.h>

#include <lair/render_gl3/context.h>


namespace lair
{


struct VertexAttribInfo {
	const char* name;
	GLuint      index;
};

#define LAIR_VERTEX_ATTRIB_INFO_END { nullptr, 0 }


/**
 * \brief A set of vertex attribute info (vx_id, index).
 *
 * This describe the attributes of a vertex, and more specifically how attribute
 * names in shaders map to the application indices. The main goal of this class
 * is to be given to program objects to bind vertex attributes.
 */
class VertexAttribSet {
protected:
	typedef std::vector<VertexAttribInfo> AttribList;

public:
	typedef AttribList::const_iterator AttribIterator;

public:
	VertexAttribSet(const VertexAttribInfo* attribs);
	VertexAttribSet(const VertexAttribSet&) = delete;
	VertexAttribSet(VertexAttribSet&&)      = delete;
	~VertexAttribSet();

	VertexAttribSet& operator=(const VertexAttribSet&) = delete;
	VertexAttribSet& operator=(VertexAttribSet&&)      = delete;

	AttribIterator begin() const;
	AttribIterator end() const;

protected:
	AttribList _attribs;
};


}


#endif
