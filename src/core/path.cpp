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


#ifdef _WIN32
#include <locale>
#include <codecvt>
#endif

#include "lair/core/path.h"


namespace lair {


//#ifdef _WIN32
#if 0
std::wstring utf16FromUtf8(const std::string& utf8) {
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16conv;
	return utf16conv.from_bytes(utf8);
}
#endif


//#ifndef _WIN32
#if 1
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


//#ifndef _WIN32
#if 1
const std::string& Path::native() const {
	return _path;
}
#else
const std::wstring Path::native() const {
	return utf16FromUtf8(_path);
}
#endif


bool Path::isAbsolute() const {
	return !empty() && _path[0] == '/';
}


Path& Path::operator/=(const Path& path) {
	if(!path.empty()) {
		bool needSep = (!empty() && _path.back() != '/');
		_path.reserve(_path.size() + needSep + path.size());
		if(needSep) {
			_path.push_back('/');
		}
		_path.append(path._path);
	}
	return *this;
}


void Path::removeTrailingSeparators() {
	int end = size() - 1;
	while(end > 0 && _path[end] == '/') --end;
	_path.resize(end + 1);
}


Path Path::dir() {
	removeTrailingSeparators();
	int end = size() - 1;
	while(end > 0 && _path[end] != '/') --end;
	while(end > 1 && _path[end-1] == '/') --end; // If there is several slashes...
	Path p;
	p._path.reserve(end);
	p._path.append(_path.begin(), _path.begin() + end);
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


}
