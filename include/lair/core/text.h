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


#ifndef _LAIR_CORE_TEXT_H
#define _LAIR_CORE_TEXT_H


#include <lair/core/lair.h>


namespace lair
{


typedef int32 Codepoint;


class Utf8CodepointIterator {
public:
	Utf8CodepointIterator();
	Utf8CodepointIterator(const char* begin, const char* end);
	Utf8CodepointIterator(const std::string& utf8String);
	Utf8CodepointIterator(const Utf8CodepointIterator&) = default;
	Utf8CodepointIterator(Utf8CodepointIterator&&)      = default;
	~Utf8CodepointIterator()                            = default;

	Utf8CodepointIterator& operator=(const Utf8CodepointIterator&) = default;
	Utf8CodepointIterator& operator=(Utf8CodepointIterator&&)      = default;

	inline bool hasNext() const {
		return _begin < _end;
	}

	inline Codepoint next() {
		assert(hasNext());

		Codepoint cp;
		int nCont = 0;

		if((*_begin & 0x80) == 0) {
			cp = *_begin;
			++_begin;
		}
		else if((*_begin & 0xe0) == 0xc0) {
			cp = *_begin & 0x1f;
			nCont = 1;
			++_begin;
		}
		else if((*_begin & 0xf0) == 0xe0) {
			cp = *_begin & 0x0f;
			nCont = 2;
			++_begin;
		}
		else if((*_begin & 0xf8) == 0xf0) {
			cp = *_begin & 0x07;
			nCont = 3;
			++_begin;
		}
		else {
			while(_begin != _end && (*_begin & 0xc0) == 0x80) {
				++_begin;
			}
			return -1;
		}

		for(; _begin != _end && nCont != 0 && (*_begin & 0xc0) == 0x80; --nCont) {
			cp = (cp << 6) | (*_begin & 0x3f);
			++_begin;
		}

		return (nCont != 0)? -1: cp;
	}


protected:
	const char* _begin;
	const char* _end;
};


}


#endif
