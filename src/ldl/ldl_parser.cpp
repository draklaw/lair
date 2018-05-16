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


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include "lair/ldl/ldl_parser.h"


namespace lair
{


LdlParser::LdlParser(std::istream* stream, const String& streamName, ErrorList* errors, Context context)
    : _buf         (stream, streamName)
    , _errors      (errors)
    , _contextStack()
    , _state       ((context == CTX_LIST)? ST_EXPECT_VALUE: ST_EXPECT_KEY)
    , _token       (TOK_INVALID)
    , _type        ((context == CTX_LIST)? TYPE_LIST: TYPE_MAP)
    , _int         ()
    , _float       ()
    , _string      ()
    , _key         ()
    , _valueType   ()
{
}


LdlParser::Context LdlParser::context() const {
	return _contextStack.back();
}


bool LdlParser::isValueTyped() const {
	return _valueType.size();
}


String LdlParser::getValueTypeName() const {
	lairAssert(isValueTyped());
	return _valueType;
}


String LdlParser::getKey() const {
	lairAssert(!isContextList());
	return _key;
}


LdlParser::Type LdlParser::valueType() const {
	return _type;
}


const char* LdlParser::valueTypeName() const {
	return typeName(valueType());
}


void LdlParser::enter() {
	lairAssert(_type == TYPE_LIST || _type == TYPE_MAP);
	_contextStack.push_back((_type == TYPE_LIST)? CTX_LIST: CTX_MAP);
	_valueType.clear();
	_next();
}


void LdlParser::leave() {
	lairAssert(_type == TYPE_END);
	lairAssert(_contextStack.size() != 0);
	_contextStack.pop_back();
	_next();
}


void LdlParser::skip() {
	if(_type == TYPE_LIST || _type == TYPE_MAP) {
		enter();
		while(_type != TYPE_END)
			skip();
		leave();
	}
	else if(_type != TYPE_END) {
		next();
	}
}


bool LdlParser::getBool() const {
	lairAssert(_type == TYPE_BOOL);
	return _int;
}


int64 LdlParser::getInt() const {
	lairAssert(_type == TYPE_INT);
	return _int;
}


double LdlParser::getFloat() const {
	lairAssert(_type == TYPE_FLOAT);
	return _float;
}


String LdlParser::getString() const {
	lairAssert(_type == TYPE_STRING);
	return _string;
}


void LdlParser::next() {
	lairAssert(_type != TYPE_LIST && _type != TYPE_MAP && _type != TYPE_END);
	_next();
}


const String& LdlParser::streamName() const {
	return _buf.streamName();
}


unsigned LdlParser::line() const {
	return _buf.line();
}


unsigned LdlParser::col() const {
	return _buf.col();
}


void LdlParser::error(const String& message) {
	_errors->error(_buf.streamName(), ": ", _buf.line(), ": ",
	               _buf.col(), ": ", message);
}


void LdlParser::warning(const String& message) {
	_errors->warning(_buf.streamName(), ": ", _buf.line(), ": ",
	                 _buf.col(), ": ", message);
}


const char* LdlParser::typeName(Type type) {
	static const char* typeName[] = {
	"<Error>",
	"<End>",
	"Null",
	"Bool",
	"Int64",
	"Double",
	"String",
	"VarList",
	"VarMap",
	};
	return typeName[type];
}


ErrorList* LdlParser::errorList() {
	return _errors;
}


void LdlParser::eatSpaceTabs() {
	while(true) {
		switch(_buf.peek()) {
		case ' ':
		case '\t':
			_buf.get();
			continue;
		default:
			_buf.flush();
			return;
		}
	}
}


bool LdlParser::parseComment() {
	_buf.get();
	if((_buf[0] == '/' && _buf[1] == '/') || _buf[0] == '#') {
		_buf.get();
		_buf.parseUntil("\n");
		if(_buf.eof() && _buf[-1] != '\n')
			unexpectedEofError();
		return true;
	}
	if(_buf[0] == '/' && _buf[1] == '*') {
		_buf.get();
		_buf.parseUntil("*/");
		if(_buf.eof() && (_buf.pos() < 4 && _buf[-2] != '*' && _buf[-1] != '/'))
			unexpectedEofError();
		return true;
	}
	return false;
}


LdlParser::Token LdlParser::parseNumber() {
	// Is this a non-decimal litteral ?
	if(_buf.peek() == '0') {
		switch(_buf[1]) {
		case 'B':
		case 'b':
			_buf.flush(2);
			while(asciiBinDigit(_buf.peek()))
				_buf.get();
			if(!_buf.pos())
				return TOK_INVALID;
			_int = std::stoll(_buf.str(), 0, 2);
			return TOK_INT;
		case 'O':
		case 'o':
			_buf.flush(2);
			while(asciiOctDigit(_buf.peek()))
				_buf.get();
			if(!_buf.pos())
				return TOK_INVALID;
			_int = std::stoll(_buf.str(), 0, 8);
			return TOK_INT;
		case 'X':
		case 'x':
			_buf.flush(2);
			while(asciiHexDigit(_buf.peek()))
				_buf.get();
			if(!_buf.pos())
				return TOK_INVALID;
			_int = std::stoll(_buf.str(), 0, 16);
			return TOK_INT;
		}
	}

	if(_buf.peek() == '+' || _buf.peek() == '-')
		_buf.get();

	while(asciiDigit(_buf.peek()))
		_buf.get();

	Token token = TOK_INT;
	if(_buf.peek() == '.') {
		token = TOK_FLOAT;
		_buf.get();
		if(!asciiDigit(_buf[-2]) && !asciiDigit(_buf.peek()))
			return TOK_INVALID;
		while(asciiDigit(_buf.peek()))
			_buf.get();
	}

	if(_buf.peek() == 'e' || _buf.peek() == 'E') {
		token = TOK_FLOAT;
		_buf.get();
		if(_buf.peek() == '+' || _buf.peek() == '-')
			_buf.get();
		if(!asciiDigit(_buf.peek()))
			return TOK_INVALID;
		while(asciiDigit(_buf.peek()))
			_buf.get();
	}

	if(!_buf.pos()) {
		_buf.get();
		return token;
	}

	if(token == TOK_INT)
		_int = std::stoll(_buf.str(), 0, 10);
	else
		_float = std::stod(_buf.str());

	return token;
}


LdlParser::Token LdlParser::parseIdentifier() {
	_buf.get();
	if(!asciiAlpha(_buf[0]) && _buf[0] != '_')
		return TOK_INVALID;

	while(asciiAlpha(_buf.peek()) || asciiDigit(_buf.peek()) || _buf.peek() == '_')
		_buf.get();

	if(_buf.contains("null"))
		return TOK_NULL;
	if(_buf.contains("true")) {
		_int = 1;
		return TOK_BOOL;
	}
	if(_buf.contains("false")) {
		_int = 0;
		return TOK_BOOL;
	}

	_string = _buf.str();

	return TOK_STRING;
}


LdlParser::Token LdlParser::parseString() {
	_buf.get();
	if(_buf[0] != '\'' && _buf[0] != '"')
		return TOK_INVALID;

	if(_buf[1] == _buf[0] && _buf[2] == _buf[0])
		_buf.get(2);

	String end = _buf.str();
	_buf.flush();

	_string.clear();
	unsigned matchCount = 0;
	while(matchCount < end.size()) {
		int c0 = _buf.get();
		if(c0 == '\\') {
			int c1 = _buf.get();
			switch(c1) {
			case '\n':
				break;
			case '\\':
			case '\'':
			case '"':
				_string.push_back(c1);
				break;
			case 'a':
				_string.push_back('\a');
				break;
			case 'b':
				_string.push_back('\b');
				break;
			case 'f':
				_string.push_back('\f');
				break;
			case 'n':
				_string.push_back('\n');
				break;
			case 'r':
				_string.push_back('\r');
				break;
			case 't':
				_string.push_back('\t');
				break;
			case 'v':
				_string.push_back('\v');
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7': {
				unsigned c = c1 - '0';
				for(int j = 1; j < 3 && asciiOctDigit(_buf.peek()); ++j)
					c = (c << 3) | (_buf.get() - '0');
				if(c > 255)
					warning("Invalid octal escape sequence (overflow)");
				_string.push_back(c);
				break;
			}
			case 'x':
			case 'u':
			case 'U': {
				unsigned  count = (c1 == 'x')? 2: (c1 == 'u')? 4: 8;
				Codepoint cp    = 0;
				unsigned  j     = 0;
				for(; j < count && asciiHexDigit(_buf.peek()); ++j)
					cp = (cp << 4) | asciiHexValue(_buf.get());

				if(j != count)
					warning("Invalid hexadecimal escape sequence: expected ", count, " digits");

				char utf8[4];
				char* utf8End = utf8FromCodepoint(utf8, cp);
				for(char* b = utf8; b < utf8End; ++b)
					_string.push_back(*b);
				break;
			}
			default:
				_string.push_back(c0);
				_buf.back();
				break;
			}
		}
		else if(c0 == '\r' && _buf[1] == '\n') {
			// Ignore \r
		}
		else if(c0 == -1) {
			unexpectedEofError();
			break;
		}
		else {
			_string.push_back(c0);
			matchCount = (c0 == end[matchCount])? matchCount+1: 0;
		}
	}

	if(matchCount == end.size())
		_string.resize(_string.size() - end.size());

	return TOK_STRING;
}


LdlParser::Token LdlParser::parseToken() {
	while(true) {
		eatSpaceTabs();

		int c = _buf.peek();

		switch(c) {
		case -1:
			return TOK_EOF;
		case '#':
		case '/':
		case '*':
			parseComment();
			continue;
		case '.':
		case '+':
		case '-':
			return parseNumber();
		case '_':
			return parseIdentifier();
		case '"':
		case '\'':
			return parseString();
		case '\r':
			if(_buf[_buf.pos() + 1] != '\n')
				break;
			_buf.get();
			// Fall-trough
		case ',':
		case '\n':
			_buf.get();
			return TOK_SEP;
		case '[':
		case '(':
			_buf.get();
			return TOK_LIST_OPEN;
		case ']':
		case ')':
			_buf.get();
			return TOK_LIST_CLOSE;
		case '{':
			_buf.get();
			return TOK_MAP_OPEN;
		case '}':
			_buf.get();
			return TOK_MAP_CLOSE;
		case '=':
			_buf.get();
			return TOK_ASSIGN;
		}

		if(asciiDigit(c)) {
			return parseNumber();
		}

		if(asciiAlpha(c)) {
			return parseIdentifier();
		}

		_buf.get();
		return TOK_INVALID;
	}
}


bool LdlParser::isContextList() const {
	return _contextStack.empty() || _contextStack.back() == CTX_LIST;
}


bool LdlParser::isParentContextList() const {
	unsigned size = _contextStack.size();
	return size < 2 || _contextStack[size - 2] == CTX_LIST;
}


bool LdlParser::nextState() {
	if(_state == ST_END_NOW) {
		_type = TYPE_END;
		_state = ST_EXPECT_SEP_END;//isContextList()? ST_EXPECT_VALUE: ST_EXPECT_KEY;
		return true;
	}

	_buf.flush();
	_token = parseToken();

	switch(_state) {
	case ST_EXPECT_VALUE: {
		switch(_token) {
		case TOK_NULL:
			_type = TYPE_NULL;
			_state = ST_EXPECT_SEP_END;
			return true;
		case TOK_BOOL:
			_type  = TYPE_BOOL;
			_state = ST_EXPECT_SEP_END;
			return true;
		case TOK_INT:
			_type  = TYPE_INT;
			_state = ST_EXPECT_SEP_END;
			return true;
		case TOK_FLOAT:
			_type  = TYPE_FLOAT;
			_state = ST_EXPECT_SEP_END;
			return true;
		case TOK_STRING:
			_type   = TYPE_STRING;
			_state  = ST_CHECK_IF_TYPE;
			return false;
		case TOK_LIST_OPEN:
			_type  = TYPE_LIST;
			_state = ST_EXPECT_VALUE;
			return true;
		case TOK_MAP_OPEN:
			_type  = TYPE_MAP;
			_state = ST_EXPECT_KEY;
			return true;

		// Maybe errors:
		case TOK_SEP:
			if(!isContextList()) {
				unexpectedTokenError();
				_state = ST_EXPECT_SEP_END;
			}
			return false;
		case TOK_LIST_CLOSE:
		case TOK_MAP_CLOSE:
		case TOK_EOF:
			if(_token == TOK_EOF && _contextStack.size() > 1)
				unexpectedEofError();
			else if(!isContextList())
				unexpectedTokenError();
			_type = TYPE_END;
			_state = isParentContextList()? ST_EXPECT_VALUE: ST_EXPECT_KEY;
			return true;

		// Errors:
		default:
			unexpectedTokenError();
			_state = ST_EXPECT_SEP_END;
			return false;
		}
	}
	case ST_EXPECT_SEP_END: {
		switch(_token) {
		case TOK_SEP:
			_state = isContextList()? ST_EXPECT_VALUE: ST_EXPECT_KEY;
			return false;
		case TOK_LIST_CLOSE:
		case TOK_MAP_CLOSE:
		case TOK_EOF:
			if(_token == TOK_EOF && _contextStack.size() > 1)
				unexpectedEofError();
			_type = TYPE_END;
			_state = isParentContextList()? ST_EXPECT_VALUE: ST_EXPECT_KEY;
			return true;

		// Errors:
		default:
			unexpectedTokenError();
			_state = ST_EXPECT_SEP_END;
			return false;
		}
	}
	case ST_EXPECT_KEY: {
		switch(_token) {
		case TOK_STRING:
			_type   = TYPE_STRING;
			_state  = ST_EXPECT_ASSIGN;
			_key    = _string;
			return false;
		case TOK_SEP:
			_state = isContextList()? ST_EXPECT_VALUE: ST_EXPECT_KEY;
			return false;
		case TOK_LIST_CLOSE:
		case TOK_MAP_CLOSE:
		case TOK_EOF:
			if(_token == TOK_EOF && _contextStack.size() > 1)
				unexpectedEofError();
			_type = TYPE_END;
			_state = isParentContextList()? ST_EXPECT_VALUE: ST_EXPECT_KEY;
			return true;

		// Errors:
		default:
			unexpectedTokenError();
			_state = ST_EXPECT_SEP_END;
			return false;
		}
	}
	case ST_EXPECT_ASSIGN: {
		switch(_token) {
		case TOK_ASSIGN:
			_state  = ST_EXPECT_VALUE;
			return false;

		// Errors:
		case TOK_LIST_CLOSE:
		case TOK_MAP_CLOSE:
		case TOK_EOF:
			if(_token == TOK_EOF)
				unexpectedEofError();
			else
				unexpectedTokenError();
			_type = TYPE_END;
			_state = isParentContextList()? ST_EXPECT_VALUE: ST_EXPECT_KEY;
			return true;
		case TOK_SEP:
			unexpectedTokenError();
			_state = isContextList()? ST_EXPECT_VALUE: ST_EXPECT_KEY;
			return false;
		default:
			unexpectedTokenError();
			_state = ST_EXPECT_SEP_END;
			return false;
		}
	}
	case ST_CHECK_IF_TYPE: {
		switch(_token) {
		case TOK_LIST_OPEN:
			_type      = TYPE_LIST;
			_state     = ST_EXPECT_VALUE;
			_valueType = _string;
			return true;
		case TOK_MAP_OPEN:
			_type      = TYPE_MAP;
			_state     = ST_EXPECT_KEY;
			_valueType = _string;
			return true;
		case TOK_SEP:
			_state = isContextList()? ST_EXPECT_VALUE: ST_EXPECT_KEY;
			return true;
		case TOK_LIST_CLOSE:
		case TOK_MAP_CLOSE:
		case TOK_EOF:
			if(_token == TOK_EOF && _contextStack.size() > 1)
				unexpectedEofError();
			_state = ST_END_NOW;
			return true;

		// Errors:
		default:
			unexpectedTokenError();
			_state = ST_EXPECT_SEP_END;
			return false;
		}
	}
	case ST_END_NOW:
		lairAssert(false);
	}

	lairAssert(false);
	return false;
}


void LdlParser::_next() {
//		static const char* s[] = {
//				"ST_EXPECT_VALUE",
//				"ST_EXPECT_SEP_END",
//				"ST_EXPECT_KEY",
//				"ST_EXPECT_ASSIGN",
//				"ST_CHECK_IF_TYPE",
//				"ST_END_NOW"
//		};

// 		dbgLogger.error("State from: ", s[_state], ", ", typeName(_type), ", \"", _buf.str(), "\"");
	while(!nextState()) {
// 			dbgLogger.error("State step: ", s[_state], ", ", typeName(_type), ", \"", _buf.str(), "\"");
	}
// 		dbgLogger.error("State end : ", s[_state], ", ", typeName(_type), ", \"", _buf.str(), "\"");
// 		dbgLogger.warning(_buf.line(), ": ", _buf.col(), ": ", typeName(_type));
}


void LdlParser::unexpectedEofError() {
	_buf.flush();
	error("Unexpected end-of-file");
}


void LdlParser::unexpectedTokenError() {
	switch(_token) {
	case TOK_INVALID:
		error("Invalid token: \"", _buf.str(), "\"");
		return;
	case TOK_EOF:
		_buf.flush();
		error("Unexpected end-of-file");
		return;
	case TOK_NULL:
	case TOK_BOOL:
	case TOK_INT:
	case TOK_FLOAT:
	case TOK_STRING:
	case TOK_SEP:
	case TOK_LIST_OPEN:
	case TOK_LIST_CLOSE:
	case TOK_MAP_OPEN:
	case TOK_MAP_CLOSE:
	case TOK_ASSIGN:
		error("Unexpected token: \"", _buf.str(), "\"");
		return;
	}
}


}
