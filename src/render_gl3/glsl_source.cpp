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
	: _string(NULL),
	  _length(NULL),
	  _count(0) {
}

GlslSource::GlslSource(const std::string& source)
	: _string(NULL),
	  _length(NULL),
	  _count(0) {
	loadFromString(source);
}

GlslSource::~GlslSource() {
	clear();
}

void GlslSource::loadFromString(const std::string& source) {
	clear();

	_count = 1;

	_length = new GLint[1];
	_length[0] = source.size();

	_string = new GLchar*[1];
	_string[0] = new GLchar[_length[0]+1];

	std::strncpy(_string[0], source.c_str(), _length[0]+1);
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

	_count = 1;

	_length = new GLint[1];
	_length[0] = int(sb->pubseekoff(0, std::ios_base::end));
	sb->pubseekoff(0, std::ios_base::beg);

	_string = new GLchar*[1];
	_string[0] = new GLchar[_length[0]];

	GLchar* p = _string[0];
	while(true) {
		int c = sb->sbumpc();
		switch(c) {
		case '\r':
			if(sb->sgetc() == '\n')
				sb->sbumpc();
			c = '\n';
			break;
		case EOF:
			_length[0] = p - _string[0];
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


}
