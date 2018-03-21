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

#include <fstream>
#include <stdexcept>
#include <vector>
#include <cstring>

#include <lair/render_gl3/glsl_source.h>


namespace lair {


GlslSource::GlslSource()
    : _string(nullptr),
      _length(nullptr),
	  _count(0) {
}

GlslSource::GlslSource(const std::string& source)
    : _string(nullptr),
      _length(nullptr),
	  _count(0) {
	loadFromString(source);
}

GlslSource::GlslSource(GlslSource&& other)
    : _string(other._string),
      _length(other._length),
      _count (other._count) {
	other._string = nullptr;
	other._length = nullptr;
	other._count  = 0;
}

GlslSource::~GlslSource() {
	clear();
}

GlslSource& GlslSource::operator=(GlslSource&& other) {
	if(this != &other) {
		clear();

		_string = other._string;
		_length = other._length;
		_count  = other._count;

		other._string = nullptr;
		other._length = nullptr;
		other._count  = 0;
	}
	return *this;
}

bool GlslSource::isValid() const {
	return _string;
}

void GlslSource::loadFromString(const std::string& source) {
	clear();

	char* header = createHeader();

	_count = 2;

	_length = new GLint[_count];
	_length[0] = std::strlen(header);
	_length[1] = source.size();

	_string = new GLchar*[_count];
	_string[0] = header;
	_string[1] = new GLchar[_length[1]+1];

	std::strncpy(_string[1], source.c_str(), _length[1]+1);
//	GLchar* p = _string[0];
//	for(auto c: source)
//		*(p++) = c;
}

void GlslSource::loadFromFile(const std::string& filename) {
	loadFromFile(filename.c_str());
}

void GlslSource::loadFromFile(const char* filename) {
	std::ifstream in(filename);
	if(!in.good())
		throw std::runtime_error(
				"Unable to read the file \""+std::string(filename)+"\".");

	loadFromStream(in);
}

void GlslSource::loadFromStream(std::istream& in) {
	clear();

	std::istream::sentry se(in, true);
	std::streambuf* sb = in.rdbuf();

	char* header = createHeader();

	_count = 2;

	_length = new GLint[_count];
	_length[0] = std::strlen(header);
	_length[1] = int(sb->pubseekoff(0, std::ios_base::end));
	sb->pubseekoff(0, std::ios_base::beg);

	_string = new GLchar*[_count];
	_string[0] = header;
	_string[1] = new GLchar[_length[1]];

	GLchar* p = _string[1];
	while(true) {
		int c = sb->sbumpc();
		switch(c) {
		case '\r':
			if(sb->sgetc() == '\n')
				sb->sbumpc();
			c = '\n';
			break;
		case EOF:
			_length[1] = p - _string[1];
			return;
		}
		*(p++) = c;
	}
}

void GlslSource::clear() {
	for(GLsizei i=0; i<_count; ++i)
		delete[] _string[i];
	delete[] _string;
	delete[] _length;
	_count = 0;
}

const GLchar *const* GlslSource::string() const {
	return _string;
}

const GLint* GlslSource::length() const {
	return _length;
}

GLsizei GlslSource::count() const {
	return _count;
}

GLchar* GlslSource::createHeader() const {
	// TODO: Generate header based on OpenGL version.
	static const char* header =
	    "#version 330 core\n"
	    "#define lowp\n"
	    "#define mediump\n"
	    "#define highp\n";

	Size size = std::strlen(header) + 1;
	GLchar* h = new GLchar[size];
	std::strncpy(h, header, size);
	return h;
}



GlslSourceLoader::GlslSourceLoader(LoaderManager* manager, AspectSP aspect)
    : Loader(manager, aspect) {
}


void GlslSourceLoader::commit() {
	GlslSourceAspectSP aspect = std::static_pointer_cast<GlslSourceAspect>(_aspect);
	aspect->_get() = std::move(_source);
	Loader::commit();
}


void GlslSourceLoader::loadSyncImpl(Logger& log) {
	VirtualFile file = this->file();
	if(!file) {
		log.error("Failed to load GLSL source \"", asset()->logicPath(), "\": Invalid file.");
		return;
	}

	Path realPath = file.realPath();
	if(!realPath.empty()) {
		Path::IStream in(realPath.native().c_str());
		_source.loadFromStream(in);
	}
	else {
		const MemFile* memFile = file.fileBuffer();
		if(memFile) {
			String code((const char*)memFile->data, memFile->size);
			_source.loadFromString(code);
		}
	}
}


}
