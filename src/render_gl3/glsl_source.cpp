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


GlslSource::GlslSource() {
}


GlslSource::GlslSource(const std::string& source, const Path& filename) {
	loadFromString(source, filename);
}


GlslSource::~GlslSource() {
	clear();
}


bool GlslSource::isValid() const {
	return _string.size();
}


void GlslSource::loadFromString(const std::string& source, const Path& filename) {
	clear();

	addHeader();
	addString(source, SourceInfo{filename, 0});
}


void GlslSource::loadFromFile(const std::string& filename) {
	loadFromFile(filename.c_str());
}


void GlslSource::loadFromFile(const char* filename) {
	std::ifstream in(filename);
	if(!in.good())
		throw std::runtime_error(
				"Unable to read the file \""+std::string(filename)+"\".");

	loadFromStream(in, filename);
}


void GlslSource::loadFromStream(std::istream& in, const Path& filename) {
	clear();

	addHeader();

	std::istream::sentry se(in, true);
	std::streambuf* sb = in.rdbuf();

	Size size = int(sb->pubseekoff(0, std::ios_base::end));
	sb->pubseekoff(0, std::ios_base::beg);

	char* string = new GLchar[size];

	GLchar* p = string;
	while(true) {
		int c = sb->sbumpc();
		switch(c) {
		case '\r':
			if(sb->sgetc() == '\n')
				sb->sbumpc();
			c = '\n';
			break;
		case EOF:
			size = p - string;
			return;
		}
		*(p++) = c;
	}

	addString(string, size, SourceInfo{filename, 0});
}


void GlslSource::clear() {
	for(char* string: _string)
		delete[] string;
}


const GLchar *const* GlslSource::string() const {
	return _string.data();
}


const GLint* GlslSource::length() const {
	return _length.data();
}


GLsizei GlslSource::count() const {
	return _length.size();
}


void GlslSource::addHeader() {
	// TODO: Generate header based on OpenGL version.
	static const char* header =
	    "#version 330 core\n"
	    "#define lowp\n"
	    "#define mediump\n"
	    "#define highp\n";

	addString(header, std::strlen(header), SourceInfo{"<gl_3_3_header>", 0});
}


void GlslSource::addString(const String& string, const SourceInfo& info) {
	addString(string.data(), string.size(), info);
}


void GlslSource::addString(const char* string, Size size, const SourceInfo& info) {
	_length.push_back(size);

	char* s = new GLchar[size];
	std::strncpy(s, string, size);
	_string.push_back(s);

	_sourceInfo.push_back(info);
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
		_source.loadFromStream(in, asset()->logicPath());
	}
	else {
		const MemFile* memFile = file.fileBuffer();
		if(memFile) {
			String code((const char*)memFile->data, memFile->size);
			_source.loadFromString(code, asset()->logicPath());
		}
	}
}


}
