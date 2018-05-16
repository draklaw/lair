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


#include <iomanip>

#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/parse.h>

#include "lair/ldl/ldl_writer.h"


namespace lair
{


LdlWriter::LdlWriter(std::ostream* out, const String& streamName, ErrorList* errors)
    : _out         (out)
    , _streamName  (streamName)
    , _errors      (errors)
    , _state       (ST_START)
    , _contextStack()
{
}

void LdlWriter::_writeString(const String& str, StringFormat format) {
	if(format == SF_AUTO || format == SF_ID) {
		bool hasNonId  = str.empty() || asciiDigit(str[0]);
		bool hasSingle = false;
		bool hasDouble = false;
		for(unsigned i = 0; i < str.size(); ++i) {
			int c = str[i];
			if(c == '\'')
				hasSingle = true;
			else if(c == '"')
				hasDouble = true;
			else if(!asciiAlpha(c) && !asciiDigit(c) && c != '_')
				hasNonId = true;
		}
		if(!hasNonId)
			format = SF_ID;
		else if(hasDouble && !hasSingle)
			format = SF_SINGLE;
		else
			format = SF_DOUBLE;
	}

	switch(format) {
	case SF_ID:
		break;
	case SF_SINGLE:
		*_out << "'";
		break;
	case SF_DOUBLE:
		*_out << "\"";
		break;
	case SF_TRIPLE_SINGLE:
		*_out << "'''";
		break;
	case SF_TRIPLE_DOUBLE:
		*_out << "\"\"\"";
		break;
	default:
		lairAssert(false);
	}

	unsigned numSingle = 0;
	unsigned numDouble = 0;
	for(unsigned i = 0; i < str.size(); ++i) {
		char c = str[i];

		if(c == '\'')
			++numSingle;
		else
			numSingle = 0;
		if(c == '"')
			++numDouble;
		else
			numDouble = 0;

		switch(c) {
		case '\\':
			*_out << "\\\\";
			break;
		case '\'':
			if(format == SF_SINGLE || (format == SF_TRIPLE_SINGLE && numSingle == 3)) {
				*_out << "\\";
				numSingle = 0;
			}
			*_out << "'";
			break;
		case '"':
			if(format == SF_DOUBLE || (format == SF_TRIPLE_DOUBLE && numDouble == 3)) {
				*_out << "\\";
				numDouble = 0;
			}
			*_out << "\"";
			break;
		case '\a':
			*_out << "\\a";
			break;
		case '\b':
			*_out << "\\b";
			break;
		case '\f':
			*_out << "\\f";
			break;
		case '\n':
			*_out << "\\n";
			break;
		case '\r':
			*_out << "\\r";
			break;
		case '\t':
			*_out << "\\t";
			break;
		case '\v':
			*_out << "\\v";
			break;
		case '\0':
			*_out << "\\0";
		default:
			if(c < 0x20)
				*_out << "\\x" << std::hex << std::setw(2) << std::setfill('0') << int(c);
			else
				*_out << c;
		}
	}

	switch(format) {
	case SF_ID:
		break;
	case SF_SINGLE:
		*_out << "'";
		break;
	case SF_DOUBLE:
		*_out << "\"";
		break;
	case SF_TRIPLE_SINGLE:
		*_out << "'''";
		break;
	case SF_TRIPLE_DOUBLE:
		*_out << "\"\"\"";
		break;
	default:
		lairAssert(false);
	}
}


}
