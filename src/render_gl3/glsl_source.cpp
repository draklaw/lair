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
	addString(source, SourceInfo{filename, 1});
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

	addString(string, size, SourceInfo{filename, 1});
}


void GlslSource::addBlock(const String& code, const Path& filename, int line) {
	if(!_string.size()) {
		addHeader();
	}

	addString(code, SourceInfo{filename, line});
}


void GlslSource::clear() {
	for(char* string: _string)
		delete[] string;
}


const GLchar *const* GlslSource::strings() const {
	return _string.data();
}


const GLint* GlslSource::length() const {
	return _length.data();
}


GLsizei GlslSource::count() const {
	return _length.size();
}


const GLchar* GlslSource::string(unsigned i) const {
	lairAssert(i < unsigned(count()));
	return _string[i];
}


const Path& GlslSource::filename(unsigned i) const {
	lairAssert(i < unsigned(count()));
	return _sourceInfo[i].filename;
}


int GlslSource::line(unsigned i) const {
	lairAssert(i < unsigned(count()));
	return _sourceInfo[i].line;
}


void GlslSource::addHeader() {
	// TODO: Generate header based on OpenGL version.
	static const char* header =
	    "#version 330 core\n"
	    "#define lowp\n"
	    "#define mediump\n"
	    "#define highp\n";

	addString(header, std::strlen(header), SourceInfo{"<gl_3_3_header>", 1});
}


void GlslSource::addString(const String& string, const SourceInfo& info) {
	addString(string.data(), string.size(), info);
}


void GlslSource::addString(const char* string, Size size, const SourceInfo& info) {
	_length.push_back(size);

	char* s = new GLchar[size + 1];
	std::strncpy(s, string, size);
	s[size] = '\0';
	_string.push_back(s);

	_sourceInfo.push_back(info);
}


GlslSourceLoader::GlslSourceLoader(LoaderManager* manager, AspectSP aspect)
    : Loader(manager, aspect) {
}


void GlslSourceLoader::commit() {
	GlslSource source;
	for(const Chunk& chunk: _chunks) {
		bool fail = false;
		switch(chunk.type) {
		case CODE:
			source.addBlock(String(chunk.begin, chunk.end),
			                asset()->logicPath(), chunk.line);
			break;
		case INCLUDE: {
			if(!chunk.include->isValid()) {
				_manager->log().error(asset()->logicPath(), ": ", chunk.line,
				                      ": Failed to load include \"",
				                      chunk.include->asset()->logicPath(), "\"");
				source.clear();
				fail = true;
				break;
			}
			const GlslSource& include = chunk.include->get();
			// Start at 1 to skip the header block
			for(unsigned i = 1; i < unsigned(include.count()); ++i) {
				source.addBlock(include.string(i), include.filename(i), include.line(i));
			}
			break;
		}
		}

		if(fail)
			break;
	}

	GlslSourceAspectSP aspect = static_pointer_cast<GlslSourceAspect>(_aspect);
	aspect->_get() = std::move(source);
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
		in.seekg(0, std::ios::end);
		_code.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&_code[0], _code.size());
	}

	if(_code.empty()) {
		const MemFile* memFile = file.fileBuffer();
		if(memFile) {
			_code.assign((const char*)memFile->data, memFile->size);
		}
	}

	char* p   = &_code[0];
	char* end = p + _code.size();
	int line = 1;

	_chunks.emplace_back(Chunk{CODE, p, p, line, GlslSourceAspectSP()});

	static const char* includeKeyword = "include";
	while(p != end) {
		char* lineStart = p;
		char closeChar = 0;
		char* begin = nullptr;
		char* end   = nullptr;

		while(p != end && (*p == ' ' || *p == '\t'))
			++p;

		bool match = (*p == '#');

		if(match) {
			++p;

			while(p != end && (*p == ' ' || *p == '\t'))
				++p;

			match = (strncmp(p, includeKeyword, strlen(includeKeyword)) == 0);
		}

		if(match) {
			p += strlen(includeKeyword);

			while(p != end && (*p == ' ' || *p == '\t'))
				++p;

			switch(*p) {
			case '<': closeChar = '>'; break;
			case '"': closeChar = '"'; break;
			default:
				match = false;
				log.error(asset()->logicPath(), ": ", line, ": Syntax error");
				break;
			}
		}

		if(match) {
			++p;
			begin = p;
			while(p != end && *p != closeChar && *p != '\n')
				++p;

			match = (*p == closeChar);
		}

		if(match) {
			end = p;
			++p;
			while(p != end && (*p == ' ' || *p == '\t'))
				++p;

			if(*p != '\n' && *p != '\0') {
				match = false;
				log.error(asset()->logicPath(), ": ", line, ": Syntax error");
			}
		}

		if(match) {
			_chunks.back().end = lineStart;
			auto loader = _load<GlslSourceLoader>(Path(String(begin, end)),
			                                      [](AspectSP, Logger&){});
			auto aspect = static_pointer_cast<GlslSourceAspect>(loader->aspect());
			_chunks.push_back(Chunk{INCLUDE, begin, end, line, aspect});
			_chunks.push_back(Chunk{CODE, p, p, line, GlslSourceAspectSP()});
		}

		while(p != end && *p != '\n')
			++p;
		if(p != end)
			++p;
		line += 1;
	}
	_chunks.back().end = p;
}


}
