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

#include "lair/core/text.h"


namespace lair
{


Utf8CodepointIterator::Utf8CodepointIterator()
	: _begin(nullptr)
	, _end  (nullptr) {
}

Utf8CodepointIterator::Utf8CodepointIterator(const char* begin, const char* end)
	: _begin(begin)
	, _end  (end) {
	assert(_begin <= _end);
}

Utf8CodepointIterator::Utf8CodepointIterator(const std::string& utf8String)
	: _begin(utf8String.data())
	, _end  (utf8String.data() + utf8String.size()) {
	assert(_begin <= _end);
}


}
