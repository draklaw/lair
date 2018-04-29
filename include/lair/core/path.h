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


#ifndef _LAIR_CORE_PATH_H
#define _LAIR_CORE_PATH_H


#include <functional>
#include <ostream>
#include <istream>

#if defined(_WIN32) && !defined(_MSC_VER)
#include <ext/stdio_filebuf.h>
#else
#include <fstream>
#endif

#include <lair/core/lair.h>
#include <lair/core/hash.h>

#include <lair/meta/metatype.h>


namespace lair
{


#if defined(_WIN32) && !defined(_MSC_VER)
class WinIFStream : public std::istream {
public:
	WinIFStream(const wchar_t* filename,
	            std::ios_base::openmode mode = std::ios_base::in);
	~WinIFStream();

private:
	__gnu_cxx::stdio_filebuf<char> _buf;
	std::streambuf*                _orig;
};
class WinOFStream : public std::ostream {
public:
	WinOFStream(const wchar_t* filename,
	            std::ios_base::openmode mode = std::ios_base::out);
	~WinOFStream();

private:
	__gnu_cxx::stdio_filebuf<char> _buf;
	std::streambuf*                _orig;
};
#endif


class Path {
public:
	typedef String::const_iterator ConstIterator;

#if defined(_WIN32) && !defined(_MSC_VER)
	typedef WinIFStream   IStream;
	typedef WinOFStream   OStream;
#else
	typedef std::ifstream IStream;
	typedef std::ofstream OStream;
#endif

	static char directory_separator;

public:
	Path();
	Path(const char* utf8Path);
	Path(const std::string& utf8Path);
	Path(const Path& path)            = default;
	Path(Path&& path)                 = default;
	~Path()                           = default;

	Path& operator=(const Path& path) = default;
	Path& operator=(Path&& path)      = default;

	size_t size() const;
	bool empty() const;

	const std::string& utf8String() const;
	const char* utf8CStr() const;
#ifndef _WIN32
	const std::string& native() const;
#else
	std::wstring native() const;
#endif

	bool isAbsolute() const;

	ConstIterator begin() const;
	ConstIterator end() const;

	bool operator==(const Path& other) const { return _path == other._path; }
	bool operator!=(const Path& other) const { return _path != other._path; }
	bool operator< (const Path& other) const { return _path <  other._path; }
	bool operator<=(const Path& other) const { return _path <= other._path; }
	bool operator> (const Path& other) const { return _path >  other._path; }
	bool operator>=(const Path& other) const { return _path >= other._path; }

	void set(const String& utf8Path);

	template<typename It>
	void set(It utf8Begin, It utf8End) {
		_path.assign(utf8Begin, utf8End);
	}

	Path& operator/=(const Path& path);

	void makePreferred();
	void removeTrailingSeparators();
	void removeFilename();
	void replaceExtension(const std::string& newExt);
	bool normalize();

	Path dir() const;
	Path withExtension(const std::string& newExt) const;
	Path normalized() const;

	static bool isDirectorySeparator(char c);

private:
	std::string _path;
};


// Used by boost using Argument Dependant Lookup.
inline std::size_t hash(const Path& path) {
	return hash(path.utf8String());
}


Path operator/(const Path& lp, const Path& rp);
Path makeAbsolute(const Path& cd, const Path& path);

inline void writeRepr(std::ostream& out, const Path& path) {
	out << "<Path \"" << path.utf8String() << "\">";
}

inline std::ostream& operator<<(std::ostream& out, const Path& path) {
	// FIXME: assume that the output stream is utf-8.
	out << path.utf8String();
	return out;
}

extern const Path emptyPath;

}

LAIR_REGISTER_METATYPE(lair::Path, "Path");


#endif
