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


#include "lair/core/json.h"


namespace lair {


Matrix4 parseMatrix4(const Json::Value& json, bool* ok) {
	Matrix4 m = Matrix4::Identity();
	if(!json.isArray() || (json.size() != 12 && json.size() != 16)) {
		if(ok) *ok = false;
		return m;
	}

	unsigned index = 0;
	for(const Json::Value& v: json) {
		if(!v.isNumeric()) {
			if(ok) *ok = false;
			return Matrix4::Identity();
		}
		m(index / 4, index % 4) = v.asFloat();
		++index;
	}

	if(ok) *ok = true;
	return m;
}


}
