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


#include <cstdlib>

#if defined(__linux__)
#  include <unistd.h>
#elif defined(_WIN32)
#  include <Windows.h>
#endif

#include <lair/core/lair.h>
#include <lair/core/log.h>

#include "lair/utils/path.h"


using namespace boost::filesystem;

namespace lair
{

Path exePath(const char* progName) {
	Path p;
#if defined(__linux__)
	{
		char buff[4096];
		ssize_t res = readlink("/proc/self/exe", buff, 4096);
		if(res != -1) {
			p = buff;
		}
	}
#elif defined(_WIN32)
	{
		CHAR buff[4096];
		GetModuleFileName(NULL, buff, 4096);
		p = buff;
	}
#endif

	if(p.empty()) {
		p = current_path();
		p /= progName;
	}

	p = canonical(p.parent_path());

	return p;
}


}
