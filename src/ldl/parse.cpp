/*
 *  Copyright (C) 2017 Simon Boyé
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

#include "lair/ldl/parse.h"


namespace lair
{


unsigned ErrorList::nErrors() const {
	return _errors.size();
}

ErrorList::ErrorType ErrorList::errorType(unsigned index) const {
	lairAssert(index < nErrors());
	return _errors[index].type;
}

const String& ErrorList::errorMessage(unsigned index) const {
	lairAssert(index < nErrors());
	return _errors[index].message;
}

void ErrorList::clear() {
	_errors.clear();
}

void ErrorList::warning(const String& message) {
	_errors.emplace_back(Error{ WARNING, message });
}

void ErrorList::error(const String& message) {
	_errors.emplace_back(Error{ ERROR, message });
}

void ErrorList::log(Logger& log) const {
	for(unsigned i = 0; i < nErrors(); ++i) {
		if(_errors[i].type == ERROR)
			log.error(_errors[i].message);
		else
			log.warning(_errors[i].message);
	}
}


void eatWhitespaces(std::istream& in) {
	in >> std::ws;
}

bool isNextIn(std::istream& in, const char* chars) {
	int next = in.peek();
	while(*chars != '\0') {
		if(*chars == next)
			return true;
		++chars;
	}
	return false;
}

bool parseIdentifier(String& id, std::istream& in, const char* extra) {
	id.clear();

	if(!in.good() || !(std::isalpha(in.peek()) || in.peek() == '_' || isNextIn(in, extra)))
		return false;

	id.push_back(in.get());

	while(in.good() && (std::isalnum(in.peek()) || in.peek() == '_' || isNextIn(in, extra))) {
		id.push_back(in.get());
	}

	return true;
}


ParseBuffer::ParseBuffer(std::istream* stream, const String& streamName, int size)
	: _stream    (stream)
	, _streamName(streamName)
	, _line      (0)
	, _col       (0)
	, _buf       ()
	, _pos       (0) {
	_buf.reserve(size);
}

bool ParseBuffer::parseUntil(const String end) {
	unsigned matchPos = 0;
	while(true) {
		int c = get();
		if(c == -1) {
			return false;
		}

		if(c == end[matchPos]) {
			++matchPos;
			if(matchPos == end.size()) {
				return true;
			}
		}
		else {
			matchPos = 0;
		}
	}
}

void ParseBuffer::flush(size_t i) {
	_read(i);

	const unsigned char* c   = _buf.data();
	const unsigned char* end = c + i;
	for(; c < end; ++c) {
		switch(*c) {
		case '\n':
			++_line;
			_col = 0;
			break;
		default:
			++_col;
			break;
		}
	}

	_buf.erase(_buf.begin(), _buf.begin() + i);
	_pos -= std::min(i, _pos);
}

void ParseBuffer::flush() {
	flush(_pos);
}

bool ParseBuffer::contains(const char* str) const {
	unsigned i = 0;
	for(; i < _pos && str[i] != '\0'; ++i) {
		if(_buf[i] != str[i])
			return false;
	}
	return i == _pos;
}

String ParseBuffer::str() const {
	return String(_buf.data(), _buf.data() + _pos);
}


}
