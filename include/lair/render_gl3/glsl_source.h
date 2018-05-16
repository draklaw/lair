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


#ifndef _LAIR_RENDER_GL3_GLSL_SOURCE_H
#define _LAIR_RENDER_GL3_GLSL_SOURCE_H


#include <istream>

#include <lair/asset/loader.h>

#include <lair/render_gl3/context.h>


namespace lair {


/**
 * \brief Encapsulate GLSL code.
 *
 * The class \c glsl_source represent a piece of GLSL code that can be used
 * to create shaders.
 *
 * \sa shader_object
 */
class GlslSource {
public:
	/**
	 * \brief Creates a glsl_source object.
	 */
	GlslSource();
	/**
	 * \brief Creates a glsl_source object from a string.
	 */
	GlslSource(const std::string& source, const Path& filename);
	// TODO: implement the copy constructor of glsl_source.
	GlslSource(const GlslSource&  other) = delete;
	GlslSource(      GlslSource&& other) = default;

	/**
	 * \brief Destroy a glsl_source object.
	 */
	virtual ~GlslSource();

	// TODO: implement the assignement operator of glsl_source.
	GlslSource& operator=(const GlslSource&  other) = delete;
	GlslSource& operator=(      GlslSource&& other) = default;

	bool isValid() const;

	/**
	 * \brief Loads a shader from a string object.
	 * \param source A string containing GLSL code.
	 */
	void loadFromString(const std::string& source, const Path& filename);

	/**
	 * \brief Loads a shader from a file.
	 * \param filename The file name.
	 */
	void loadFromFile(const std::string& filename);
	/**
	 * \brief Loads a shader from a file.
	 * \param filename The file name.
	 */
	void loadFromFile(const char* filename);

	/**
	 * \brief Loads a shader from an input stream.
	 * \param in The input stream.
	 */
	void loadFromStream(std::istream& in, const Path& filename);

	void addBlock(const String& code, const Path& filename, int line);

	/**
	 * \brief Clear the object, deleting its content.
	 */
	void clear();

	/**
	 * \brief Returns an array of count() strings containing the shader.
	 *
	 * The pointer returned by this function can be directly given to
	 * glShaderSource(...).
	 *
	 * \return an array of string containing the shader.
	 */
	GLchar const* const* strings() const;

	/**
	 * \brief Returns an array of count() int describing the lengths of strings
	 * returned by strings().
	 *
	 * The pointer returned by this function can be directly given to
	 * glShaderSource(...).
	 *
	 * \return An array of int.
	 */
	const GLint* length() const;

	/**
	 * \brief Returns the number of elements in the arrays returned by strings()
	 * and length()
	 * \return
	 */
	GLsizei count() const;

	const GLchar* string(unsigned i) const;
	const Path& filename(unsigned i) const;
	int line(unsigned i) const;

private:
	typedef std::vector<GLint> IntVector;
	typedef std::vector<GLchar*> CStringVector;

	struct SourceInfo {
		Path filename;
		int  line;
	};
	typedef std::vector<SourceInfo> SourceInfoVector;

private:
	void addHeader();

	void addString(const String& string, const SourceInfo& info);
	void addString(const char* string, Size size, const SourceInfo& info);

private:
	CStringVector _string;
	IntVector     _length;

	SourceInfoVector _sourceInfo;
};


typedef GenericAspect  <GlslSource>       GlslSourceAspect;
typedef std::shared_ptr<GlslSourceAspect> GlslSourceAspectSP;
typedef std::weak_ptr  <GlslSourceAspect> GlslSourceAspectWP;


class GlslSourceLoader : public Loader {
public:
	typedef GlslSourceAspect Aspect;

public:
	GlslSourceLoader(LoaderManager* manager, AspectSP aspect);
	GlslSourceLoader(const GlslSourceLoader&) = delete;
	GlslSourceLoader(GlslSourceLoader&&)      = delete;
	virtual ~GlslSourceLoader() = default;

	GlslSourceLoader& operator=(const GlslSourceLoader&) = delete;
	GlslSourceLoader& operator=(GlslSourceLoader&&)      = delete;

	virtual void commit();

protected:
	virtual void loadSyncImpl(Logger& log);

protected:
	enum ChunkType {
		CODE,
		INCLUDE,
	};

	struct Chunk {
		ChunkType type;
		char* begin;
		char* end;
		int line;
		GlslSourceAspectSP include;
	};

	typedef std::vector<Chunk> ChunkVector;

protected:
	String      _code;
	ChunkVector _chunks;
};


}

#endif
