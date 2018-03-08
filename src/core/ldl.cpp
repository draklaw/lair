#define _USE_MATH_DEFINES

#include <cstdlib>
#include <cassert>
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>

#include <lair/core/path.h>
#include <lair/core/text.h>

#include <lair/core/ldl.h>


namespace lair {


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


bool ldlRead(LdlParser& parser, bool& value) {
	switch(parser.valueType()) {
	case LdlParser::TYPE_BOOL:
		value = parser.getBool();
		parser.next();
		return true;
	case LdlParser::TYPE_INT:
		parser.warning("Expected Bool, got ", parser.valueTypeName(), ": implicit conversion");
		value = (parser.getInt() != 0);
		parser.next();
		return true;
	case LdlParser::TYPE_FLOAT:
		parser.warning("Expected Bool, got ", parser.valueTypeName(), ": implicit conversion");
		value = (parser.getFloat() != 0);
		parser.next();
		return true;
	case LdlParser::TYPE_NULL:
	case LdlParser::TYPE_STRING:
	case LdlParser::TYPE_LIST:
	case LdlParser::TYPE_MAP:
		parser.error("Expected Bool, got ", parser.valueTypeName(), ": ignoring");
		parser.skip();
		return false;
	default:
		lairAssert(false);
	}
	lairAssert(false);
	return false;
}

#define DECL_LDL_READ_INT(_type, _typeName) \
bool ldlRead(LdlParser& parser, _type& value) { \
	int64  min = std::numeric_limits<_type>::min(); \
	uint64 max = std::numeric_limits<_type>::max(); \
	switch(parser.valueType()) { \
	case LdlParser::TYPE_BOOL: \
		parser.warning("Expected " _typeName ", got ", parser.valueTypeName(), ": implicit conversion"); \
		value = parser.getBool()? 1: 0; \
		parser.next(); \
		return true; \
	case LdlParser::TYPE_INT: \
		if(parser.getInt() < min) \
			value = min; \
		else if(parser.getInt() > 0 && uint64(parser.getInt()) > max) \
			value = max; \
		else \
			value = parser.getInt(); \
		if(int64(value) != parser.getInt()) \
			parser.warning("Out of bound value of type " _typeName ": ", parser.getInt(), \
			               " clamped to ", value); \
		parser.next(); \
		return true; \
	case LdlParser::TYPE_FLOAT: \
		value = parser.getFloat(); \
		if(double(value) != parser.getFloat()) \
			parser.warning("Expected " _typeName ", got ", parser.valueTypeName(), \
			               ": ", parser.getFloat(), " rounded to ", value); \
		parser.next(); \
		return true; \
	case LdlParser::TYPE_NULL: \
	case LdlParser::TYPE_STRING: \
	case LdlParser::TYPE_LIST: \
	case LdlParser::TYPE_MAP: \
		parser.error("Expected " _typeName ", got ", parser.valueTypeName(), ": ignoring"); \
		parser.skip(); \
		return false; \
	default: \
		lairAssert(false); \
	} \
	lairAssert(false); \
	return false; \
}

DECL_LDL_READ_INT(int8,  "Int8")
DECL_LDL_READ_INT(int16, "Int16")
DECL_LDL_READ_INT(int32, "Int32")
DECL_LDL_READ_INT(int64, "Int64")

DECL_LDL_READ_INT(uint8,  "UInt8")
DECL_LDL_READ_INT(uint16, "UInt16")
DECL_LDL_READ_INT(uint32, "UInt32")
DECL_LDL_READ_INT(uint64, "UInt64")

#define DECL_LDL_READ_FLOAT(_type, _typeName) \
bool ldlRead(LdlParser& parser, _type& value) { \
	switch(parser.valueType()) { \
	case LdlParser::TYPE_BOOL: \
		parser.warning("Expected " _typeName ", got ", parser.valueTypeName(), ": implicit conversion"); \
		value = parser.getBool()? 1: 0; \
		parser.next(); \
		return true; \
	case LdlParser::TYPE_INT: \
		value = parser.getInt(); \
		parser.next(); \
		return true; \
	case LdlParser::TYPE_FLOAT: \
		value = parser.getFloat(); \
		parser.next(); \
		return true; \
	case LdlParser::TYPE_NULL: \
	case LdlParser::TYPE_STRING: \
	case LdlParser::TYPE_LIST: \
	case LdlParser::TYPE_MAP: \
		parser.error("Expected " _typeName ", got ", parser.valueTypeName(), ": ignoring"); \
		parser.skip(); \
		return false; \
	default: \
		lairAssert(false); \
	} \
	lairAssert(false); \
	return false; \
}

DECL_LDL_READ_FLOAT(float,  "Float")
DECL_LDL_READ_FLOAT(double, "Double")


#define DECL_LDL_READ_STRING(_type, _typeName) \
bool ldlRead(LdlParser& parser, _type& value) { \
	switch(parser.valueType()) { \
	case LdlParser::TYPE_STRING: \
		value = parser.getString(); \
		parser.next(); \
		return true; \
	case LdlParser::TYPE_BOOL: \
	case LdlParser::TYPE_INT: \
	case LdlParser::TYPE_FLOAT: \
	case LdlParser::TYPE_NULL: \
	case LdlParser::TYPE_LIST: \
	case LdlParser::TYPE_MAP: \
		parser.error("Expected " _typeName ", got ", parser.valueTypeName(), ": ignoring"); \
		parser.skip(); \
		return false; \
	default: \
		lairAssert(false); \
	} \
	lairAssert(false); \
	return false; \
}

DECL_LDL_READ_STRING(String, "String")
DECL_LDL_READ_STRING(Path,   "Path")

bool ldlRead(LdlParser& parser, Transform& value) {
	if(parser.valueType() != LdlParser::TYPE_LIST) {
		parser.error("Expected Transform (VarList), got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	bool success = true;
	if(!parser.isValueTyped() || parser.getValueTypeName() == "Transform") {
		parser.enter();
		value = Transform::Identity();
		while(parser.valueType() != LdlParser::TYPE_END) {
			Transform trans = Transform::Identity();
			success &= ldlRead(parser, trans);
			value = value * trans;
		}
		parser.leave();
	}
	else if(parser.getValueTypeName() == "translate" || parser.getValueTypeName() == "Vector") {
		Vector3 vec;
		success &= ldlRead(parser, vec, NO_ANNOTATION_WARNING);

		value.setIdentity();
		value.translate(vec);
	}
	else if(parser.getValueTypeName() == "rotate") {
		if(parser.valueType() != LdlParser::TYPE_LIST) {
			parser.error("Rotate transform expected (VarList), got ", parser.valueTypeName());
			parser.skip();
			return false;
		}

		Vector4  vec;
		unsigned index    = 0;
		bool     vSuccess = true;
		parser.enter();
		for(; parser.valueType() != LdlParser::TYPE_END; ++index) {
			if(index < 4) {
				vSuccess &= ldlRead(parser, vec(index));
			}
		}

		if(vSuccess) {
			if(index == 1) {
				value.setIdentity();
				value.rotate(AngleAxis(vec(0) * M_PI / 180, Vector3(0, 0, 1)));
			}
			else if(index == 4) {
				value.setIdentity();
				value.rotate(AngleAxis(vec(0) * M_PI / 180, vec.tail<3>()));
			}
			else {
				parser.error("Invalid number of arguments in rotate transform, expected 1 or 4, got ", index);
				success = false;
			}
		}
		success &= vSuccess;

		parser.leave();
	}
	else if(parser.getValueTypeName() == "scale") {
		if(parser.valueType() != LdlParser::TYPE_LIST) {
			parser.error("Scale transform expected (VarList), got ", parser.valueTypeName());
			parser.skip();
			return false;
		}

		Vector3  vec;
		unsigned index    = 0;
		bool     vSuccess = true;
		parser.enter();
		for(; parser.valueType() != LdlParser::TYPE_END; ++index) {
			if(index < 3) {
				vSuccess &= ldlRead(parser, vec(index));
			}
		}

		if(vSuccess) {
			if(index == 1) {
				value.setIdentity();
				value.scale(vec(0));
			}
			else if(index == 4) {
				value.setIdentity();
				value.scale(vec);
			}
			else {
				parser.error("Invalid number of arguments in scale transform, expected 1 or 4, got ", index);
				success = false;
			}
		}
		success &= vSuccess;

		parser.leave();
	}

	return success;
}


bool ldlWrite(LdlWriter& writer, const bool& value) {
	writer.writeBool(value);
	return true;
}

#define DECL_LDL_WRITE_INT(_type) \
bool ldlWrite(LdlWriter& writer, const _type& value) { \
	uint64 max = std::numeric_limits<_type>::max(); \
	int64  v   = value; \
	if(value > 0 && uint64(value) > max) { \
		writer.warning("Out-of-bound interger: clamping ", value, " to ", max); \
	    v = max; \
	} \
	writer.writeInt(v); \
	return true; \
}

DECL_LDL_WRITE_INT(int8)
DECL_LDL_WRITE_INT(int16)
DECL_LDL_WRITE_INT(int32)
DECL_LDL_WRITE_INT(int64)
DECL_LDL_WRITE_INT(uint8)
DECL_LDL_WRITE_INT(uint16)
DECL_LDL_WRITE_INT(uint32)
DECL_LDL_WRITE_INT(uint64)

#define DECL_LDL_WRITE_FLOAT(_type) \
bool ldlWrite(LdlWriter& writer, const _type& value) { \
	writer.writeFloat(value); \
	return true; \
}

DECL_LDL_WRITE_FLOAT(float)
DECL_LDL_WRITE_FLOAT(double)

bool ldlWrite(LdlWriter& writer, const String& value) {
	writer.writeString(value);
	return true;
}

bool ldlWrite(LdlWriter& writer, const Path& value) {
	writer.writeString(value.utf8String(), LdlWriter::SF_DOUBLE);
	return true;
}


}
