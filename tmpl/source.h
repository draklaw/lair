/*
 *  Copyright (C) {{ year }} Simon Boyé
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


#ifndef LAIR_{{ module|upper }}_{{ class_snake|upper }}_H
#define LAIR_{{ module|upper }}_{{ class_snake|upper }}_H


#include <lair/core/lair.h>


namespace lair
{


class {{ class_camel }} {
public:
	{{ class_camel }}();
	{{ class_camel }}(const {{ class_camel }}&) = delete;
	{{ class_camel }}({{ class_camel }}&&)      = delete;
	~{{ class_camel }}();

	{{ class_camel }}& operator=(const {{ class_camel }}&) = delete;
	{{ class_camel }}& operator=({{ class_camel }}&&)      = delete;


protected:
};


}


#endif
