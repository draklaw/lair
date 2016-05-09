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


#if defined(_WIN32) && !defined(_MSC_VER)
#include <cstdio>
#endif

#ifdef _WIN32
#include <locale>
#include <codecvt>
#endif

#include "lair/core/path.h"


namespace lair {


#if defined(_WIN32) && !defined(_MSC_VER)

WinFStream::WinFStream(const wchar_t* filename)
	: std::istream(),
	  _buf(_wfopen(filename, L"r"), std::ios_base::in),
	  _orig(basic_ios::rdbuf(&_buf)){
}


WinFStream::~WinFStream() {
	basic_ios::rdbuf(_orig);
	_buf.close();
}

#endif


#ifdef _WIN32
std::wstring utf16LEFromUtf8(const std::string& utf8) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::little_endian>, wchar_t> utf16conv;
	return utf16conv.from_bytes(utf8);
}
#endif


#ifndef _WIN32
char Path::directory_separator = '/';
#else
char Path::directory_separator = '\\';
#endif

Path::Path(const char* utf8Path)
    : _path(utf8Path) {
}


Path::Path(const std::string& utf8Path)
    : _path(utf8Path) {
}


size_t Path::size() const {
	return _path.size();
}


bool Path::empty() const {
	return !_path.size();
}


const std::string& Path::utf8String() const {
	return _path;
}


const char* Path::utf8CStr() const {
	return _path.c_str();
}


#ifndef _WIN32
const std::string& Path::native() const {
	return _path;
}
#else
std::wstring Path::native() const {
	return utf16LEFromUtf8(_path);
}
#endif


bool Path::isAbsolute() const {
	return !empty() && isDirectorySeparator(_path[0]);
}


Path& Path::operator/=(const Path& path) {
	if(!path.empty()) {
		bool needSep = (!empty() && !isDirectorySeparator(_path.back()));
		_path.reserve(_path.size() + needSep + path.size());
		if(needSep) {
			_path.push_back(directory_separator);
		}
		_path.append(path._path);
	}
	return *this;
}


void Path::makePreferred() {
#ifdef _WIN32
	for(char& c: _path) {
		if(isDirectorySeparator(c)) {
			c = directory_separator;
		}
	}
#endif
}


void Path::removeTrailingSeparators() {
	int end = size() - 1;
	while(end > 0 && isDirectorySeparator(_path[end])) --end;
	_path.resize(end + 1);
}


void Path::removeFilename() {
	int end = size() - 1;
	while(end > 0 && !isDirectorySeparator(_path[end])) --end;
	while(end > 1 && isDirectorySeparator(_path[end-1])) --end; // If there is several slashes...
	_path.resize(end);
}


void Path::replaceExtension(const std::string& newExt) {
	int i = _path.size() - 1;
	while(i >= 0 && _path[i] != '.') --i;
	if(i >= 0) {
		_path.resize(i+1);
		_path.append(newExt);
	}
}


Path Path::dir() const {
	Path p = *this;
	p.removeFilename();
	return p;
}


Path Path::withExtension(const std::string& newExt) const {
	Path p = *this;
	p.replaceExtension(newExt);
	return p;
}


Path operator/(const Path& lp, const Path& rp) {
	Path p(lp);
	p /= rp;
	return p;
}


Path make_absolute(const Path& cd, const Path& path) {
	if(path.isAbsolute())
		return path;
	return cd / path;
}


bool Path::isDirectorySeparator(char c) {
#ifndef _WIN32
	return c == '/';
#else
	return (c == '/') || (c == '\\');
#endif
}

}
