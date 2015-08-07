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

#ifndef _LAIR_RENDER_GL2_SHADER_OBJECT_H
#define _LAIR_RENDER_GL2_SHADER_OBJECT_H


#include <string>
#include <istream>
#include <ostream>

#include <lair/render_gl2/gl.h>


namespace lair {


class GlslSource;

class ShaderObject {
public:
	ShaderObject(GLenum type=0);
	ShaderObject(const ShaderObject&) = delete;
	~ShaderObject();

	ShaderObject& operator=(const ShaderObject&) = delete;

	bool isGenerated() const;
	bool isCompiled() const;

	GLuint id() const;

	void generateObject(GLenum type=0);
	void deleteObject();

	bool compile(const GlslSource& source);
	bool compileFromFile(const std::string& filename);
	bool compileFromStream(std::istream& in);

	void dumpLog(std::ostream& out) const;

private:
	GLenum _type;
	GLuint _id;
	GLint _compile_status;
};


}

#endif
