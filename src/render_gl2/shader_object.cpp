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
#include <memory>
#include <vector>

#include <lair/render_gl2/glsl_source.h>
#include <lair/render_gl2/renderer.h>

#include <lair/render_gl2/shader_object.h>


namespace lair {


ShaderObject::ShaderObject(Renderer* renderer, GLenum type)
    : _context       (renderer? renderer->context(): nullptr),
      _renderer      (renderer),
      _type          (type),
      _id            (0),
      _compile_status(gl::FALSE) {
}


ShaderObject::ShaderObject(ShaderObject&& other)
    : _context       (other._context),
      _renderer      (other._renderer),
      _type          (other._type),
      _id            (other._id),
      _compile_status(other._compile_status) {
	other._context        = nullptr;
	other._renderer       = nullptr;
	other._type           = 0;
	other._id             = 0;
	other._compile_status = gl::FALSE;
}


ShaderObject::~ShaderObject() {
	if(_id != 0)
		deleteObject();
}


ShaderObject& ShaderObject::operator=(ShaderObject other) {
	swap(*this, other);
	return *this;
}


bool ShaderObject::isValid() const {
	return _renderer;
}


bool ShaderObject::isGenerated() const {
	return _id != 0;
}


bool ShaderObject::isCompiled() const {
	return _compile_status == gl::TRUE;
}


GLuint ShaderObject::id() const {
	return _id;
}


void ShaderObject::generateObject(GLenum type) {
	lairAssert(_id == 0);
	if(type != 0)
		_type = type;
	lairAssert(_type != 0);

	_id = _context->createShader(_type);
}


void ShaderObject::deleteObject() {
	lairAssert(_id != 0);
	_context->deleteShader(_id);
	_id = 0;
}


bool ShaderObject::compile(const GlslSource& source) {
	lairAssert(_id != 0);
	_context->shaderSource(_id, source.count(), (GLchar* const*)source.string(), (int*)source.length());
	_context->compileShader(_id);

	_context->getShaderiv(_id, gl::COMPILE_STATUS, &_compile_status);
	return _compile_status == gl::TRUE;
}


bool ShaderObject::compileFromFile(const std::string& filename) {
	GlslSource source;
	source.loadFromFile(filename);
	return compile(source);
}


bool ShaderObject::compileFromStream(std::istream& in) {
	GlslSource source;
	source.loadFromStream(in);
	return compile(source);
}


void ShaderObject::getLog(std::string& out) const {
	lairAssert(_id != 0);
	GLint log_size;
	_context->getShaderiv(_id, gl::INFO_LOG_LENGTH, &log_size);

	std::unique_ptr<GLchar[]> buffer(new GLchar[log_size]);
	_context->getShaderInfoLog(_id, log_size, NULL, buffer.get());

	out.assign(buffer.get());
}


void swap(ShaderObject& s0, ShaderObject& s1) {
	std::swap(s0._context,        s1._context);
	std::swap(s0._renderer,       s1._renderer);
	std::swap(s0._type,           s1._type);
	std::swap(s0._id,             s1._id);
	std::swap(s0._compile_status, s1._compile_status);
}


}
