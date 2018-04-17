/*
 *  Copyright (C) 2018 Simon Boyé
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


#include <lair/core/log.h>

#include "lair/core/lair.h"


namespace lair {


AssertionFailedError::AssertionFailedError(const char* testCode, const char* file, int line)
    : std::logic_error(_formatWhat(testCode, file, line)) {
}

std::string AssertionFailedError::_formatWhat(
        const char* testCode, const char* file, int line) {
	std::ostringstream out;
	out << file << ":" << line << ": Assertion failed: '" << testCode << "'.";
	return out.str();
}


bool throwOnAssert = false;


void _assertCrash(const char* testCode, const char* file, int line) {
	dbgLogger.fatal(AssertionFailedError::_formatWhat(testCode, file, line));
	abort();
}


}
