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
#include <functional>

#include <lair/ldl/read.h>

#include <lair/render_gl3/context.h>
#include <lair/render_gl3/renderer.h>
#include <lair/render_gl3/glsl_source.h>
#include <lair/render_gl3/shader_object.h>

#include <lair/render_gl3/program_object.h>


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



ShaderLoader::ShaderLoader(LoaderManager* manager, AspectSP aspect,
                           Renderer* renderer, const VertexAttribSet* attribs)
    : Loader(manager, aspect),
      _renderer(renderer),
      _attribs(attribs) {
}


void ShaderLoader::commit() {
	ShaderAspectSP aspect = static_pointer_cast<ShaderAspect>(_aspect);

	if(_vertexShader.isValid() && _fragmentShader.isValid()) {
		std::unique_ptr<ProgramObject> shader(new ProgramObject(
		        _renderer->compileProgram(
		            asset()->logicPath().utf8CStr(),
		            _attribs, &_vertexShader, &_fragmentShader)));
		aspect->_set(std::move(shader));
	}

	Loader::commit();
}


void ShaderLoader::loadSyncImpl(Logger& log) {
	VirtualFile file = this->file();
	if(!file) {
		log.error("Failed to load shader \"", asset()->logicPath(), "\": Invalid file.");
		return;
	}

	std::unique_ptr<std::istream> in;
	Path realPath = file.realPath();
	if(!realPath.empty()) {
		in.reset(new Path::IStream(realPath.native().c_str()));
	}
	else {
		const MemFile* memFile = file.fileBuffer();
		if(memFile) {
			String str((const char*)memFile->data, memFile->size);
			in.reset(new std::istringstream(str));
		}
	}

	Path vertPath;
	Path fragPath;

	ErrorList errors;
	LdlParser parser(in.get(), asset()->logicPath().utf8String(), &errors, LdlParser::CTX_MAP);

	bool success = true;
	if(parser.valueType() == LdlParser::TYPE_MAP) {
		parser.enter();

		while(parser.valueType() != LdlParser::TYPE_END) {
			String key = parser.getKey();
			Path* path = (key == "vert")? &vertPath:
			             (key == "frag")? &fragPath:
			                              nullptr;

			if(!path) {
				parser.error("Unexpected key \"", key, "\".");
				success = false;
			}

			if(success && !path->empty()) {
				parser.error("Duplicated key \"", key, "\".");
				success = false;
			}

			if(success && parser.valueType() != LdlParser::TYPE_STRING) {
				parser.error("Expected shader path (String), got ", parser.valueTypeName());
				success = false;
			}

			if(success) {
				*path = parser.getString();
			}

			parser.next();
		}

		parser.leave();
	}
	else {
		parser.error("Expected shader (VarMap), got ", parser.valueTypeName());
	}

	errors.log(log);

	if(!success) {
		return;
	}

	if(vertPath.empty()) {
		log.error(asset()->logicPath(), ": Missing required vertex shader (vert).");
	}
	if(fragPath.empty()) {
		log.error(asset()->logicPath(), ": Missing required fragment shader (frag).");
	}
	if(vertPath.empty() || fragPath.empty()) {
		return;
	}

	using namespace std::placeholders;

	log.info("Load vertex shader: \"", vertPath, "\"");
	_load<GlslSourceLoader>(vertPath, std::bind(&ShaderLoader::loadShader,
	                                            this, _1, _2, gl::VERTEX_SHADER));

	log.info("Load fragment shader: \"", fragPath, "\"");
	_load<GlslSourceLoader>(fragPath, std::bind(&ShaderLoader::loadShader,
	                                            this, _1, _2, gl::FRAGMENT_SHADER));
}


void ShaderLoader::loadShader(AspectSP aspect, Logger& log, GLenum type) {
	if(aspect->isValid()) {
		GlslSourceAspectSP sourceAspect = static_pointer_cast<GlslSourceAspect>(aspect);

		ShaderObject* shader = (type == gl::VERTEX_SHADER)?   &_vertexShader:
		                       (type == gl::FRAGMENT_SHADER)? &_fragmentShader:
		                                                      nullptr;
		lairAssert(shader);

		*shader = _renderer->compileShader(
		              aspect->asset()->logicPath().utf8CStr(),
		              type, sourceAspect->get());
	}
	else {
		log.error("Error while loading shader \"", asset()->logicPath(),
		          "\": Failed to load \"", aspect->asset()->logicPath(), "\".");
	}
}


}
