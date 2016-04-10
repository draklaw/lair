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

#include <lair/render_gl2/context.h>
#include <lair/render_gl2/renderer.h>
#include <lair/render_gl2/shader_object.h>

#include <lair/render_gl2/program_object.h>


namespace lair {


ProgramObject::ProgramObject(Renderer* renderer)
	: _context    (renderer? renderer->context(): nullptr),
	  _renderer   (renderer),
	  _id         (0),
	  _link_status(gl::FALSE) {
}


ProgramObject::ProgramObject(ProgramObject&& other)
	: _context    (other._context),
	  _renderer   (other._renderer),
	  _id         (other._id),
	  _link_status(other._link_status) {
	other._context     = nullptr;
	other._renderer    = nullptr;
	other._id          = 0;
	other._link_status = gl::FALSE;
}


ProgramObject::~ProgramObject() {
	if(isGenerated())
		deleteObject();
}


ProgramObject& ProgramObject::operator=(ProgramObject other) {
	swap(*this, other);
	return *this;
}


bool ProgramObject::isValid() const {
	return _renderer;
}

bool ProgramObject::isGenerated() const {
	return _id != 0;
}


bool ProgramObject::isLinked() const {
	return _link_status == gl::TRUE;
}


GLuint ProgramObject::id() const {
	return _id;
}


void ProgramObject::generateObject() {
	assert(!isGenerated());
	_id = _context->createProgram();
}


void ProgramObject::deleteObject() {
	assert(isGenerated());
	_context->deleteProgram(_id);
	_id = 0;
}


void ProgramObject::attachShader(const ShaderObject& shader) {
	assert(isGenerated() && shader.isGenerated());
	_context->attachShader(_id, shader.id());
}


void ProgramObject::detachShader(const ShaderObject& shader) {
	assert(isGenerated() && shader.isGenerated());
	_context->detachShader(_id, shader.id());
}


void ProgramObject::detachAllShaders() {
	assert(isGenerated());

	GLint nb_shaders;
	_context->getProgramiv(_id, gl::ATTACHED_SHADERS, &nb_shaders);

	std::unique_ptr<GLuint[]> shader_id(new GLuint[nb_shaders]);
	_context->getAttachedShaders(_id, nb_shaders, NULL, shader_id.get());

	for(int i=0; i<nb_shaders; ++i)
		_context->detachShader(_id, shader_id[i]);
}


void ProgramObject::bindAttributeLocation(const GLchar* name, GLuint index) {
	assert(isGenerated());
	_context->bindAttribLocation(_id, index, (GLchar*)name);
}


bool ProgramObject::link() {
	assert(isGenerated());

	_context->linkProgram(_id);
	_context->getProgramiv(_id, gl::LINK_STATUS, &_link_status);
	return _link_status == gl::TRUE;
}


bool ProgramObject::validate() {
	assert(isLinked());

	_context->validateProgram(_id);

	GLint validate_status;
	_context->getProgramiv(_id, gl::VALIDATE_STATUS, &validate_status);

	return validate_status == gl::TRUE;
}


void ProgramObject::use() const {
	assert(isLinked());
	_context->useProgram(_id);
}


GLint ProgramObject::nbActiveAttributes() const {
	assert(isLinked());
	GLint nb_active_attributes;
	_context->getProgramiv(_id, gl::ACTIVE_ATTRIBUTES, &nb_active_attributes);
	return nb_active_attributes;
}


GLint ProgramObject::nbActiveUniforms() const {
	assert(isLinked());
	GLint nb_active_uniforms;
	_context->getProgramiv(_id, gl::ACTIVE_UNIFORMS, &nb_active_uniforms);
	return nb_active_uniforms;
}


GLint ProgramObject::getAttributeLocation(const GLchar* name) const {
	assert(isLinked());
	GLint res = _context->getAttribLocation(_id, (GLchar*)name);
	return res;
}


GLint ProgramObject::getUniformLocation(const GLchar* name) const {
	assert(isLinked());
	GLint res = _context->getUniformLocation(_id, (GLchar*)name);
	return res;
}


void ProgramObject::getLog(std::string& out) const {
	GLint log_size;
	_context->getProgramiv(_id, gl::INFO_LOG_LENGTH, &log_size);

	std::unique_ptr<GLchar[]> buffer(new GLchar[log_size]);
	_context->getProgramInfoLog(_id, log_size, NULL, buffer.get());

	out.assign(buffer.get());
}

void ProgramObject::dumpInfo(std::ostream& out) const {
	assert(isLinked());

	//  checkGlErrors();

	GLint max_attrib_name_length;
	GLint max_uniform_name_length;
	_context->getProgramiv(_id, gl::ACTIVE_ATTRIBUTE_MAX_LENGTH,
	                       &max_attrib_name_length);
	_context->getProgramiv(_id, gl::ACTIVE_UNIFORM_MAX_LENGTH,
	                       &max_uniform_name_length);

	GLsizei buf_size = std::max(max_attrib_name_length,
	                            max_uniform_name_length);
	std::unique_ptr<GLchar[]> buffer(new GLchar[buf_size]);

	out << "Program attributes:\n";
	GLint nb_attributes = nbActiveAttributes();
	for(int i=0; i<nb_attributes; ++i) {
		GLint size;
		GLenum type;
		_context->getActiveAttrib(_id, i, buf_size, NULL, &size, &type, buffer.get());
		out << getAttributeLocation(buffer.get()) << ": " << buffer.get() << " - "
		    << /*typeString(type)*/type << "[" << size << "]\n";
	}

	out << "Program uniforms:\n";
	GLint nb_uniforms = nbActiveUniforms();
	for(int i=0; i<nb_uniforms; ++i) {
		GLint size;
		GLenum type;
		_context->getActiveUniform(_id, i, buf_size, NULL, &size, &type, buffer.get());
		out << getUniformLocation(buffer.get()) << ": " << buffer.get() << " - "
		    << /*typeString(type)*/type << "[" << size << "]\n";
	}
}


void swap(ProgramObject& p0, ProgramObject& p1) {
	std::swap(p0._context,     p1._context);
	std::swap(p0._renderer,    p1._renderer);
	std::swap(p0._id,          p1._id);
	std::swap(p0._link_status, p1._link_status);
}


}
