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


#ifndef _LAIR_RENDER_GL3_PROGRAM_OBJECT_H
#define _LAIR_RENDER_GL3_PROGRAM_OBJECT_H


#include <ostream>

#include <lair/asset/loader.h>

#include <lair/render_gl3/context.h>
#include <lair/render_gl3/shader_object.h>


namespace lair {


class Context;
class Renderer;
class VertexAttribSet;
class ShaderObject;


class ProgramObject {
public:
	ProgramObject(Renderer* renderer=nullptr);
	ProgramObject(const ProgramObject&) = delete;
	ProgramObject(ProgramObject&& other);
	~ProgramObject();

	ProgramObject& operator=(ProgramObject other);

	bool isValid() const;
	bool isGenerated() const;
	bool isLinked() const;

	GLuint id() const;

	void generateObject();
	void deleteObject();

	void attachShader(const ShaderObject& shader);
	void detachShader(const ShaderObject& shader);
	void detachAllShaders();

	void bindAttributeLocation(const GLchar* name, GLuint index) ;

	bool link();
	bool validate();

	void use() const;

	GLint nbActiveAttributes() const;
	GLint nbActiveUniforms() const;

	GLint getAttributeLocation(const GLchar* name) const;
	GLint getUniformLocation(const GLchar* name) const;

	void getLog(std::string& out) const;
	void dumpInfo(std::ostream& out) const;

	friend void swap(ProgramObject& p0, ProgramObject& p1);

private:
	Context*  _context;
	Renderer* _renderer;
	GLuint    _id;
	GLint     _link_status;
};


typedef GenericAspect       <ProgramObject> ShaderAspect;
typedef IntrusivePointer    <ShaderAspect>  ShaderAspectSP;
typedef IntrusiveWeakPointer<ShaderAspect>  ShaderAspectWP;


class ShaderLoader : public Loader {
public:
	typedef ShaderAspect Aspect;

public:
	ShaderLoader(LoaderManager* manager, AspectSP aspect,
	             Renderer* renderer, const VertexAttribSet* attribs);
	ShaderLoader(const ShaderLoader&) = delete;
	ShaderLoader(ShaderLoader&&)      = delete;
	virtual ~ShaderLoader() = default;

	ShaderLoader& operator=(const ShaderLoader&) = delete;
	ShaderLoader& operator=(ShaderLoader&&)      = delete;

	virtual void commit();

protected:
	virtual void loadSyncImpl(Logger& log);
	void loadShader(AspectSP aspect, Logger& log, GLenum type);

protected:
	Renderer*              _renderer;
	const VertexAttribSet* _attribs;
	ShaderObject           _vertexShader;
	ShaderObject           _fragmentShader;
};


}

#endif
