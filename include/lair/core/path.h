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


#ifndef _LAIR_UTILS_PATH_H
#define _LAIR_UTILS_PATH_H


#include <functional>
#include <ostream>

#include <boost/functional/hash.hpp>

#include <lair/core/lair.h>


namespace lair
{


class Path {
public:
	static char directory_separator;

public:
	Path()                            = default;
	Path(const char* utf8Path);
	Path(const std::string& utf8Path);
	Path(const Path& path)            = default;
	Path(Path&& path)                 = default;
	~Path()                           = default;

	Path& operator=(const Path& path) = default;
	Path& operator=(Path&& path)      = default;

	size_t size() const;

	const std::string& utf8String() const;
	const char* utf8CStr() const;
//#ifndef _WIN32
#if 1
	const std::string& native() const;
#else
	const std::wstring native() const;
#endif

	bool operator==(const Path& other) const { return _path == other._path; }
	bool operator!=(const Path& other) const { return _path != other._path; }
	bool operator< (const Path& other) const { return _path <  other._path; }
	bool operator<=(const Path& other) const { return _path <= other._path; }
	bool operator> (const Path& other) const { return _path >  other._path; }
	bool operator>=(const Path& other) const { return _path >= other._path; }

	Path& operator/=(const Path& path);

	void make_preferred();

private:
	std::string _path;
};


// Used by boost using Argument Dependant Lookup.
inline std::size_t hash_value(const Path& path) {
	return boost::hash_value<std::string>(path.utf8String());
}


Path operator/(const Path& lp, const Path& rp);

inline std::ostream& operator<<(std::ostream& out, const Path& path) {
	// FIXME: assume that the output stream is utf-8.
	out << path.utf8String();
	return out;
}


}


namespace std {
template <>
class hash<lair::Path> {
public:
	inline size_t operator()(const lair::Path& path) const {
		return hash<string>()(path.utf8String());
	}
};
}


#endif
