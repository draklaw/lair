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


#ifndef _LAIR_UTILS_ASSERT_H
#define _LAIR_UTILS_ASSERT_H


#include <cstdint>
#include <stdexcept>
#include <sstream>


namespace lair {

class AssertionFailedError : public std::logic_error {
public:
	inline AssertionFailedError(const char* testCode, const char* file, int line)
	    : std::logic_error(_formatWhat(testCode, file, line)) {
	}

private:
	inline static std::string _formatWhat(
	        const char* testCode, const char* file, int line) {
		std::ostringstream out;
		out << file << ":" << line << ": Assertion failed: '" << testCode << "'.\n";
		return out.str();
	}
};


inline void lairAssert(bool result, const char* testCode, const char* file, int line) {
	if(!result) {
		throw AssertionFailedError(testCode, file, line);
	}
}

#define lairAssert(_test) lair::lairAssert(_test, #_test, __FILE__, __LINE__)


typedef std::uint8_t  uint8;
typedef std:: int8_t   int8;
typedef std::uint16_t uint16;
typedef std:: int16_t  int16;
typedef std::uint32_t uint32;
typedef std:: int32_t  int32;
typedef std::uint64_t uint64;
typedef std:: int64_t  int64;

typedef unsigned ScanCode;


}

#endif
