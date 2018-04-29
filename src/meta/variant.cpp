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


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include "lair/meta/variant.h"


namespace lair
{


Variant Variant::null = Variant();


std::ostream& operator<<(std::ostream& out, const Variant& var) {
	if(var.isEmpty())
		out << "null";
	else if(var.type()->writeRepr)
		var.type()->writeRepr(out, var.data());
	else
		out << "<" << var.type()->identifier << " at " << var.data() << ">";
	return out;
}


std::ostream& operator<<(std::ostream& out, const VarList& list) {
	out << "VarList(";
	bool count = 0;
	for(const Variant& var: list) {
		if(count)
			out << ", ";
		out << var;
		count += 1;
	}
	out << ")";
	return out;
}


std::ostream& operator<<(std::ostream& out, const VarMap& map) {
	out << "VarMap(";
	bool count = 0;
	for(const auto& var: map) {
		if(count)
			out << ", ";
		out << var.first << " = " << var.second;
		count += 1;
	}
	out << ")";
	return out;
}


}
