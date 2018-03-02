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
	GlslSource(const std::string& source);
	// TODO: implement the copy constructor of glsl_source.
	GlslSource(const GlslSource&) = delete;
	/**
	 * \brief Destroy a glsl_source object.
	 */
	virtual ~GlslSource();

	// TODO: implement the assignement operator of glsl_source.
	GlslSource& operator=(const GlslSource&) = delete;

	/**
	 * \brief Loads a shader from a string object.
	 * \param source A string containing GLSL code.
	 */
	void loadFromString(const std::string& source);

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
	void loadFromStream(std::istream& in);

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
	GLchar const* const* string() const;

	/**
	 * \brief Returns an array of count() int describing the lengths of strings
	 * returned by string().
	 *
	 * The pointer returned by this function can be directly given to
	 * glShaderSource(...).
	 *
	 * \return An array of int.
	 */
	const GLint* length() const;

	/**
	 * \brief Returns the number of elements in the arrays returned by string()
	 * and length()
	 * \return
	 */
	GLsizei count() const;

private:
	GLchar** _string;
	GLint* _length;
	GLsizei _count;
};


}

#endif
