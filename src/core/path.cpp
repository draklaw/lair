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


#include <locale>

#include "lair/core/path.h"


namespace lair {


std::wstring utf16FromUtf8(const std::string& utf8) {
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt;
	// FIXME: NOT thread safe.
#ifndef _WIN32
	static std::locale loc = std::locale("C.UTF-8");
#else
	static std::locale loc = std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>);
#endif
	const codecvt& cvt = std::use_facet<codecvt>(loc);
	std::wstring utf16;
	std::mbstate_t mb;
	const char* fromNext =  &utf8[0];
	wchar_t*      toNext = &utf16[0];
	std::codecvt_base::result res = std::codecvt_base::result::partial;
	while(res == std::codecvt_base::result::partial) {
		PtrDiff offset = toNext - &utf16[0];
		utf16.resize(std::min(utf16.size() * 2, Size(4096)));
		toNext = &utf16[0] + offset;
		res = cvt.in(mb,
			         fromNext, & utf8[ utf8.size()], fromNext,
			         toNext,   &utf16[utf16.size()], toNext);
	}
	return utf16;
}


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
const std::wstring Path::native() const {
	return utf16FromUtf8(_path);
}
#endif


Path& Path::operator/=(const Path& path) {
	bool needSep = (_path.back() != directory_separator);
	_path.reserve(_path.size() + needSep + path.size());
	if(needSep) {
		_path.push_back(directory_separator);
	}
	_path.append(path._path);
	return *this;
}


inline void Path::make_preferred() {
#ifdef _WIN32
	for(char& c: _path) {
		if(c == '/') {
			c = directory_separator;
		}
	}
#endif
}


Path operator/(const Path& lp, const Path& rp) {
	Path p(lp);
	p /= rp;
	return p;
}


}
