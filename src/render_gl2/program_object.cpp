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

#include <cassert>
#include <algorithm>
#include <memory>

#include <lair/render_gl2/gl.h>
#include <lair/render_gl2/shader_object.h>

#include <lair/render_gl2/program_object.h>


namespace lair {


ProgramObject::ProgramObject()
    : _id(0),
      _link_status(GL_FALSE) {
}


ProgramObject::ProgramObject(ProgramObject&& other)
    : _id(other._id),
      _link_status(other._link_status) {
	other._id = 0;
	other._link_status = GL_FALSE;
}


ProgramObject::~ProgramObject() {
	if(isGenerated())
		deleteObject();
}


bool ProgramObject::isGenerated() const {
	return _id != 0;
}


bool ProgramObject::isLinked() const {
	return _link_status == GL_TRUE;
}


GLuint ProgramObject::id() const {
	return _id;
}


void ProgramObject::generateObject() {
	assert(!isGenerated());
	_id = glCreateProgram(); LAIR_THROW_IF_OPENGL_ERROR();
}


void ProgramObject::deleteObject() {
	assert(isGenerated());
	glDeleteProgram(_id); LAIR_THROW_IF_OPENGL_ERROR();
	_id = 0;
}


void ProgramObject::attachShader(const ShaderObject& shader) {
	assert(isGenerated() && shader.isGenerated());
	glAttachShader(_id, shader.id()); LAIR_THROW_IF_OPENGL_ERROR();
}


void ProgramObject::detachShader(const ShaderObject& shader) {
	assert(isGenerated() && shader.isGenerated());
	glDetachShader(_id, shader.id()); LAIR_THROW_IF_OPENGL_ERROR();
}


void ProgramObject::detachAllShaders() {
	assert(isGenerated());

	GLint nb_shaders;
	glGetProgramiv(_id, GL_ATTACHED_SHADERS, &nb_shaders);
	LAIR_THROW_IF_OPENGL_ERROR();

	std::unique_ptr<GLuint[]> shader_id(new GLuint[nb_shaders]);
	glGetAttachedShaders(_id, nb_shaders, NULL, shader_id.get());
	LAIR_THROW_IF_OPENGL_ERROR();

	for(int i=0; i<nb_shaders; ++i)
		glDetachShader(_id, shader_id[i]); LAIR_THROW_IF_OPENGL_ERROR();
}


void ProgramObject::bindAttributeLocation(const GLchar* name, GLuint index) {
	assert(isGenerated());
	glBindAttribLocation(_id, index, name);
	LAIR_THROW_IF_OPENGL_ERROR();
}


bool ProgramObject::link() {
	assert(isGenerated());

	glLinkProgram(_id); LAIR_THROW_IF_OPENGL_ERROR();
	glGetProgramiv(_id, GL_LINK_STATUS, &_link_status);
	LAIR_THROW_IF_OPENGL_ERROR();
	return _link_status == GL_TRUE;
}


bool ProgramObject::validate() {
	assert(isLinked());

	glValidateProgram(_id);
	LAIR_THROW_IF_OPENGL_ERROR();

	GLint validate_status;
	glGetProgramiv(_id, GL_VALIDATE_STATUS, &validate_status);
	LAIR_THROW_IF_OPENGL_ERROR();

	return validate_status == GL_TRUE;
}


void ProgramObject::use() const {
	assert(isLinked());
	glUseProgram(_id); LAIR_THROW_IF_OPENGL_ERROR();
}


GLint ProgramObject::nbActiveAttributes() const {
	assert(isLinked());
	GLint nb_active_attributes;
	glGetProgramiv(_id, GL_ACTIVE_ATTRIBUTES, &nb_active_attributes);
	LAIR_THROW_IF_OPENGL_ERROR();
	return nb_active_attributes;
}


GLint ProgramObject::nbActiveUniforms() const {
	assert(isLinked());
	GLint nb_active_uniforms;
	glGetProgramiv(_id, GL_ACTIVE_UNIFORMS, &nb_active_uniforms);
	LAIR_THROW_IF_OPENGL_ERROR();
	return nb_active_uniforms;
}


GLint ProgramObject::getAttributeLocation(const GLchar* name) const {
	assert(isLinked());
	GLint res = glGetAttribLocation(_id, name); LAIR_THROW_IF_OPENGL_ERROR();
	return res;
}


GLint ProgramObject::getUniformLocation(const GLchar* name) const {
	assert(isLinked());
	GLint res = glGetUniformLocation(_id, name); LAIR_THROW_IF_OPENGL_ERROR();
	return res;
}


void ProgramObject::dumpLog(std::ostream& out) const {
	GLint log_size;
	glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &log_size);
	LAIR_THROW_IF_OPENGL_ERROR();

	std::unique_ptr<GLchar[]> buffer(new GLchar[log_size]);
	glGetProgramInfoLog(_id, log_size, NULL, buffer.get());
	LAIR_THROW_IF_OPENGL_ERROR();

	out << buffer.get();
}

void ProgramObject::dumpInfo(std::ostream& out) const {
	assert(isLinked());

	//  checkGlErrors();

	GLint max_attrib_name_length;
	GLint max_uniform_name_length;
	glGetProgramiv(_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
				   &max_attrib_name_length);
	LAIR_THROW_IF_OPENGL_ERROR();
	glGetProgramiv(_id, GL_ACTIVE_UNIFORM_MAX_LENGTH,
				   &max_uniform_name_length);
	LAIR_THROW_IF_OPENGL_ERROR();

	GLsizei buf_size = std::max(max_attrib_name_length,
	                            max_uniform_name_length);
	std::unique_ptr<GLchar[]> buffer(new GLchar[buf_size]);

	out << "Program attributes:\n";
	GLint nb_attributes = nbActiveAttributes();
	for(int i=0; i<nb_attributes; ++i) {
		GLint size;
		GLenum type;
		glGetActiveAttrib(_id, i, buf_size, NULL, &size, &type, buffer.get());
		LAIR_THROW_IF_OPENGL_ERROR();
		out << getAttributeLocation(buffer.get()) << ": " << buffer.get() << " - "
			<< /*typeString(type)*/type << "[" << size << "]\n";
	}

	out << "Program uniforms:\n";
	GLint nb_uniforms = nbActiveUniforms();
	for(int i=0; i<nb_uniforms; ++i) {
		GLint size;
		GLenum type;
		glGetActiveUniform(_id, i, buf_size, NULL, &size, &type, buffer.get());
		LAIR_THROW_IF_OPENGL_ERROR();
		out << getUniformLocation(buffer.get()) << ": " << buffer.get() << " - "
			<< /*typeString(type)*/type << "[" << size << "]\n";
	}
}


void swap(ProgramObject& p0, ProgramObject& p1) {
	std::swap(p0._id,          p1._id);
	std::swap(p0._link_status, p1._link_status);
}


}
