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


#ifndef _LAIR_CORE_LDL_H
#define _LAIR_CORE_LDL_H


#include <lair/core/lair.h>
#include <lair/core/parse.h>

#include <lair/meta/metatype.h>
#include <lair/meta/variant.h>


namespace lair {


class LdlParser : public ErrorOutput {
public:
	enum Context {
		CTX_LIST,
		CTX_MAP,
	};

	enum Type {
		TYPE_ERROR,
		TYPE_END,
		TYPE_NULL,
		TYPE_BOOL,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_STRING,
		TYPE_LIST,
		TYPE_MAP,
	};

public:
	LdlParser(std::istream* stream, const String& streamName, ErrorList* errors, Context context = CTX_LIST);
	LdlParser(const LdlParser&) = delete;
	LdlParser(LdlParser&&) = delete;

	LdlParser& operator=(const LdlParser&) = delete;
	LdlParser& operator=(LdlParser&&) = delete;

	inline Context context() const {
		return _contextStack.back();
	}

	inline bool isValueTyped() const {
		return _valueType.size();
	}

	inline String getValueTypeName() const {
		lairAssert(isValueTyped());
		return _valueType;
	}

	inline String getKey() const {
		lairAssert(!isContextList());
		return _key;
	}

	inline Type valueType() const {
		return _type;
	}

	inline const char* valueTypeName() const {
		return typeName(valueType());
	}

	inline void enter() {
		lairAssert(_type == TYPE_LIST || _type == TYPE_MAP);
		_contextStack.push_back((_type == TYPE_LIST)? CTX_LIST: CTX_MAP);
		_valueType.clear();
		_next();
	}

	void skip();

	inline void leave() {
		lairAssert(_type == TYPE_END);
		lairAssert(_contextStack.size() != 0);
		_contextStack.pop_back();
		_next();
	}

	inline bool getBool() const {
		lairAssert(_type == TYPE_BOOL);
		return _int;
	}

	int64 getInt() const {
		lairAssert(_type == TYPE_INT);
		return _int;
	}

	double getFloat() const {
		lairAssert(_type == TYPE_FLOAT);
		return _float;
	}

	String getString() const {
		lairAssert(_type == TYPE_STRING);
		return _string;
	}

	inline void next() {
		lairAssert(_type != TYPE_LIST && _type != TYPE_MAP && _type != TYPE_END);
		_next();
	}

	inline const String& streamName() const {
		return _buf.streamName();
	}

	inline unsigned line() const {
		return _buf.line();
	}

	inline unsigned col() const {
		return _buf.col();
	}

	inline void error(const String& message) override {
		_errors->error(_buf.streamName(), ": ", _buf.line(), ": ",
		               _buf.col(), ": ", message);
	}

	inline void warning(const String& message) override {
		_errors->warning(_buf.streamName(), ": ", _buf.line(), ": ",
		                 _buf.col(), ": ", message);
	}

	using ErrorOutput::error;
	using ErrorOutput::warning;

	static const char* typeName(Type type);

	inline ErrorList* errorList() {
		return _errors;
	}

private:
	enum Token {
		TOK_INVALID,
		TOK_EOF,
		TOK_NULL,
		TOK_BOOL,
		TOK_INT,
		TOK_FLOAT,
		TOK_STRING,
		TOK_SEP,
		TOK_LIST_OPEN,
		TOK_LIST_CLOSE,
		TOK_MAP_OPEN,
		TOK_MAP_CLOSE,
		TOK_ASSIGN,
	};

	enum State {
		ST_EXPECT_VALUE,
		ST_EXPECT_SEP_END,
		ST_EXPECT_KEY,
		ST_EXPECT_ASSIGN,
		ST_CHECK_IF_TYPE,
		ST_END_NOW,
	};

private:
	// Lexer functions:
	void eatSpaceTabs();
	bool parseComment();
	Token parseNumber();
	Token parseIdentifier();
	Token parseString();

	Token parseToken();

	// Parser functions
	bool isContextList() const;
	bool isParentContextList() const;

	bool nextState();
	void _next() {
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

	// Errors:
	void unexpectedEofError();
	void unexpectedTokenError();

private:
	ParseBuffer          _buf;
	ErrorList*           _errors;

	std::vector<Context> _contextStack;
	State                _state;
	Token                _token;

	Type                 _type;
	int64                _int;
	double               _float;
	String               _string;
	String               _key;
	String               _valueType;
};


class LdlWriter {
public:
	enum IntFormat {
		IF_DECIMAL,
		IF_BINARY,
		IF_OCTAL,
		IF_HEX,
	};

	enum StringFormat {
		SF_AUTO,
		SF_ID,
		SF_SINGLE,
		SF_DOUBLE,
		SF_TRIPLE_SINGLE,
		SF_TRIPLE_DOUBLE,
	};

	enum CompoundFormat {
		CF_MULTI_LINE,
		CF_SINGLE_LINE,
	};

public:
	LdlWriter(std::ostream* out, const String& streamName, ErrorList* errors);

	inline void writeKey(const String& key, StringFormat format = SF_AUTO) {
		lairAssert(_acceptKey());
		_writeSepIfRequired();
		_writeString(key, format);
		*_out << " = ";
		_state = ST_POST_KEY;
	}

	inline void writeNull() {
		lairAssert(_acceptValue());
		_writeSepIfRequired();
		*_out << "null";
		_nextState();
	}

	inline void writeBool(bool b) {
		lairAssert(_acceptValue());
		_writeSepIfRequired();
		if(b)
			*_out << "true";
		else
			*_out << "false";
		_nextState();
	}

	inline void writeInt(int64 i, IntFormat format = IF_DECIMAL) {
		lairAssert(_acceptValue());
		_writeSepIfRequired();
		switch(format) {
		case IF_BINARY:
//			*_out << "0b" << std::bin << i;
//			break;
		case IF_DECIMAL:
			*_out << std::dec << i;
			break;
		case IF_OCTAL:
			*_out << "0o" << std::oct << i;
			break;
		case IF_HEX:
			*_out << "0x" << std::hex << i;
			break;
		}
		_nextState();
	}

	inline void writeFloat(double d) {
		lairAssert(_acceptValue());
		_writeSepIfRequired();
		*_out << d;
		_nextState();
	}

	inline void writeString(const String& str, StringFormat format = SF_AUTO) {
		lairAssert(_acceptValue());
		_writeSepIfRequired();
		_writeString(str, format);
		_nextState();
	}

	inline void openList(CompoundFormat format = CF_MULTI_LINE,
	                     const String& type = String(), StringFormat typeFormat = SF_AUTO) {
		lairAssert(_state == ST_START || _acceptValue());

		char closeChar = '\0';
		if(_contextStack.size()) {
			_writeSepIfRequired();

			if(type.size()) {
				_writeString(type, typeFormat);
				*_out << "(";
				closeChar = ')';
			}
			else {
				*_out << "[ ";
				closeChar = ']';
			}
			if(format == CF_MULTI_LINE)
				_writeNewLine(1);
		}

		_contextStack.push_back(CInfo{ CTX_LIST, format, closeChar });
		_state = ST_BEGIN_BLOCK;
	}

	inline void openMap(CompoundFormat format = CF_MULTI_LINE,
	                    const String& type = String(), StringFormat typeFormat = SF_AUTO) {
		lairAssert(_state == ST_START || _acceptValue());

		char closeChar = '\0';
		if(_contextStack.size()) {
			_writeSepIfRequired();

			if(type.size()) {
				_writeString(type, typeFormat);
			}
			*_out << "{ ";
			if(format == CF_MULTI_LINE)
				_writeNewLine(1);
			closeChar = '}';
		}

		_contextStack.push_back(CInfo{ CTX_MAP, format, closeChar });
		_state = ST_BEGIN_BLOCK;
	}

	inline void close() {
		lairAssert(_contextStack.size() != 0);
		lairAssert(_state == ST_BEGIN_BLOCK || _state == ST_POST_VALUE);

		if(_contextStack.size() > 1) {
			char closeChar = _contextStack.back().closeChar;
			if(_contextStack.back().format == CF_MULTI_LINE)
				_writeNewLine(-1);
			else if(closeChar != ')')
				*_out << " ";
			*_out << closeChar;
			_state = ST_POST_VALUE;
		}
		else
			_state = ST_CLOSED;

		_contextStack.pop_back();
	}

	template<typename... Args>
	inline void error(const Args&... args) {
		if(_errors) {
			_errors->error(_streamName, ": ", args...);
		}
	}

	template<typename... Args>
	inline void warning(const Args&... args) {
		if(_errors) {
			_errors->warning(_streamName, ": ", args...);
		}
	}


private:
	enum Context {
		CTX_LIST,
		CTX_MAP,
	};

	enum State {
		ST_START,
		ST_BEGIN_BLOCK,
		ST_POST_VALUE,
		ST_POST_KEY,
		ST_CLOSED,
	};

	struct CInfo {
		Context        context;
		CompoundFormat format;
		char           closeChar;
	};

private:
	inline bool _acceptKey() const {
		return _contextStack.size()
		    && _contextStack.back().context == CTX_MAP
		    && (_state == ST_BEGIN_BLOCK || _state == ST_POST_VALUE);
	}

	inline bool _acceptValue() const {
		return _contextStack.size()
		    && (_contextStack.back().context == CTX_LIST
		        || _state == ST_POST_KEY);
	}

	inline void _nextState() {
		lairAssert(_contextStack.size());
		_state = (_contextStack.back().context == CTX_LIST || _state != ST_POST_VALUE)?
		             ST_POST_VALUE:
		             ST_POST_KEY;
	}

	inline void _writeSepIfRequired() {
		if(_state == ST_POST_VALUE) {
			if(_contextStack.back().format == CF_MULTI_LINE) {
				_writeNewLine();
			}
			else {
				*_out << ", ";
			}
		}
	}

	inline void _writeNewLine(int offset = 0) {
		*_out << "\n";
		for(int i = 0; i < int(_contextStack.size()) - 1 + offset; ++i)
			*_out << "\t";
	}

	void _writeString(const String& str, StringFormat format);

private:
	std::ostream* _out;
	String        _streamName;
	ErrorList*    _errors;

	unsigned      _indent;
	State         _state;
	std::vector<CInfo> _contextStack;
};


enum {
	NO_ANNOTATION_WARNING   = 0x00,
	WITH_ANNOTATION_WARNING = 0x01,
};

bool ldlRead(LdlParser& parser, bool&   value);
bool ldlRead(LdlParser& parser, uint8&  value);
bool ldlRead(LdlParser& parser,  int8&  value);
bool ldlRead(LdlParser& parser, uint16& value);
bool ldlRead(LdlParser& parser,  int16& value);
bool ldlRead(LdlParser& parser, uint32& value);
bool ldlRead(LdlParser& parser,  int32& value);
bool ldlRead(LdlParser& parser, uint64& value);
bool ldlRead(LdlParser& parser,  int64& value);
bool ldlRead(LdlParser& parser, float&  value);
bool ldlRead(LdlParser& parser, double& value);
bool ldlRead(LdlParser& parser, String& value);
bool ldlRead(LdlParser& parser, Path&   value);


template<typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
bool ldlRead(LdlParser& parser, Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& value, unsigned flags) {
	typedef Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols> Matrix;

	if(parser.valueType() != LdlParser::TYPE_LIST) {
		parser.error("Expected Vector/Matrix (VarList), got ", parser.valueTypeName(), ": ignoring");
		parser.skip();
		return false;
	}

	lairAssert(value.rows() > 0 && value.cols() > 0);
	if(value.rows() == 1 || value.cols() == 1) {
		if(parser.isValueTyped()) {
			if((flags & WITH_ANNOTATION_WARNING)
			&& parser.getValueTypeName() != "Vector") {
				parser.warning("Unexpected type annotation: expected Vector, got ", parser.getValueTypeName());
			}
		}
		else {
//			parser.warning("Expected type annotation \"Vector\"");
		}

		unsigned dim = std::max(value.cols(), value.rows());
		Matrix tmp(value.rows(), value.cols());

		parser.enter();

		bool success = true;
		for(unsigned i = 0; success && i < dim; ++i) {
			if(parser.valueType() != LdlParser::TYPE_END)
				success &= ldlRead(parser, tmp(i));
			else
				tmp(i) = (i == 3)? Scalar(1): Scalar(0);
		}

		if(success && parser.valueType() != LdlParser::TYPE_END) {
			parser.warning("To much coefficient in Vector", dim);
		}
		while(parser.valueType() != LdlParser::TYPE_END)
			parser.skip();

		parser.leave();

		if(success)
			value = tmp;
		return success;
	}
	else {
		if(parser.isValueTyped()) {
			if((flags & WITH_ANNOTATION_WARNING)
			&& parser.getValueTypeName() != "Matrix") {
				parser.warning("Unexpected type annotation: expected Matrix, got ", parser.getValueTypeName());
			}
		}
		else {
//			parser.warning("Expected type annotation \"Matrix\"");
		}

		unsigned rows = value.rows();
		unsigned cols = value.cols();
		Matrix tmp;

		parser.enter();

		bool success = true;
		for(unsigned r = 0; success && r < rows; ++r) {
			if(parser.valueType() != LdlParser::TYPE_END) {
				if(parser.valueType() == LdlParser::TYPE_LIST) {
					parser.enter();

					for(unsigned c = 0; success && c < cols; ++c) {
						if(parser.valueType() != LdlParser::TYPE_END)
							success &= ldlRead(parser, tmp(r, c));
						else
							tmp(r, c) = (r == c)? Scalar(1): Scalar(0);
					}

					if(success && parser.valueType() != LdlParser::TYPE_END) {
						parser.warning("To much coefficient in Matrix", rows, "x", cols, " row");
					}

					while(parser.valueType() != LdlParser::TYPE_END)
						parser.skip();

					parser.leave();
				}
				else {
					parser.error("Expected Matrix row (VarList), got ", parser.valueTypeName(), ": ignoring");
					parser.skip();
					success = false;
				}
			}
			else {
				for(unsigned c = 0; c < cols; ++c)
					tmp(r, c) = (r == c)? Scalar(1): Scalar(0);
			}
		}

		if(success && parser.valueType() != LdlParser::TYPE_END) {
			parser.warning("To much rows in Matrix", rows, "x", cols);
		}

		while(parser.valueType() != LdlParser::TYPE_END)
			parser.skip();

		parser.leave();

		if(success)
			value = tmp;
		return success;
	}

	return true;
}

template<typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
bool ldlRead(LdlParser& parser, Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& value) {
	return ldlRead(parser, value, WITH_ANNOTATION_WARNING);
}

bool ldlRead(LdlParser& parser, Transform& value);

template<typename Scalar, int Dim>
bool ldlRead(LdlParser& parser, Eigen::AlignedBox<Scalar, Dim>& value) {
	typedef Eigen::AlignedBox<Scalar, Dim> Box;

	if(parser.valueType() == LdlParser::TYPE_NULL) {
		value = Box();
		parser.next();
		return true;
	}

	if(parser.valueType() != LdlParser::TYPE_MAP) {
		parser.error("Expected Box (VarMap), got ", parser.valueTypeName(), ": ignoring");
		parser.skip();
		return false;
	}

	if(parser.isValueTyped()) {
		if(parser.getValueTypeName() != "Box") {
			parser.warning("Unexpected type annotation: expected Box, got ", parser.getValueTypeName());
		}
	}
	else {
//		parser.warning("Expected type annotation \"Box\"");
	}

	static unsigned minIndex  = 0;
	static unsigned maxIndex  = 1;
	static unsigned sizeIndex = 2;
	Eigen::Matrix<Scalar, Dim, 1> vec[3];
	bool success = true;
	bool has[] = { false, false, false };

	parser.enter();
	while(parser.valueType() != LdlParser::TYPE_END) {
		int index = -1;
		if(success && parser.getKey() == "min")
			index = 0;
		else if(success && parser.getKey() == "max")
			index = 1;
		else if(success && parser.getKey() == "size")
			index = 2;

		if(index >= 0) {
			if(has[index]) {
				parser.warning("Duplicate key \"", parser.getKey(), "\": ignoring");
			}
			else {
				has[index] = ldlRead(parser, vec[index]);
				success    = success && has[index];
			}
		}
		else {
			parser.warning("Unexpected key \"", parser.getKey(), "\": ignoring");
			parser.skip();
		}
	}

	if(!success) {
		parser.leave();
		return false;
	}

	if(!has[minIndex] || (!has[maxIndex] && !has[sizeIndex])) {
		parser.error("Box require attributes \"min\" and either \"max\" or \"size\"");
		parser.leave();
		return false;
	}
	if(has[maxIndex] && has[sizeIndex]) {
		parser.warning("Both \"max\" and \"size\" set for a Box: size will be ignored");
	}

	value.min() = vec[minIndex];
	if(has[maxIndex])
		value.max() = vec[maxIndex];
	else
		value.max() = vec[minIndex] + vec[sizeIndex];

	for(int i = 0; i < Dim; ++i) {
		if(value.max()(i) < value.min()(i)) {
			parser.warning("Box has a max coefficient smaller than min: the box will be empty");
			break;
		}
	}

	parser.leave();
	return true;
}

bool ldlRead(LdlParser& parser, Variant& value);
bool ldlRead(LdlParser& parser, VarList& value);
bool ldlRead(LdlParser& parser, VarMap&  value);


bool ldlWrite(LdlWriter& writer, const bool&   value);
bool ldlWrite(LdlWriter& writer, const int8&   value);
bool ldlWrite(LdlWriter& writer, const int16&  value);
bool ldlWrite(LdlWriter& writer, const int32&  value);
bool ldlWrite(LdlWriter& writer, const int64&  value);
bool ldlWrite(LdlWriter& writer, const uint8&  value);
bool ldlWrite(LdlWriter& writer, const uint16& value);
bool ldlWrite(LdlWriter& writer, const uint32& value);
bool ldlWrite(LdlWriter& writer, const uint64& value);
bool ldlWrite(LdlWriter& writer, const float&  value);
bool ldlWrite(LdlWriter& writer, const double& value);
bool ldlWrite(LdlWriter& writer, const String& value);
bool ldlWrite(LdlWriter& writer, const Path&   value);

template<typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
bool ldlWrite(LdlWriter& writer, const Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& value) {
	bool success = true;

	if(value.rows() == 1 || value.cols() == 1) {
		unsigned size = std::max(value.rows(), value.cols());

		writer.openList(LdlWriter::CF_SINGLE_LINE, "Vector");
		for(unsigned i = 0; i < size; ++i)
			success &= ldlWrite(writer, value(i));
		writer.close();
	}
	else {
		unsigned rows = value.rows();
		unsigned cols = value.cols();

		writer.openList(LdlWriter::CF_MULTI_LINE, "Matrix");
		for(unsigned r = 0; r < rows; ++r) {
			writer.openList(LdlWriter::CF_SINGLE_LINE);
			for(unsigned c = 0; c < cols; ++c)
				success &= ldlWrite(writer, value(r, c));
			writer.close();
		}
		writer.close();
	}

	return success;
}

bool ldlWrite(LdlWriter& writer, const Transform& value);

template<typename Scalar, int Dim>
bool ldlWrite(LdlWriter& writer, const Eigen::AlignedBox<Scalar, Dim>& value) {
	typedef Eigen::AlignedBox<Scalar, Dim> Box;
	typedef typename Box::VectorType Vector;

	writer.openMap(LdlWriter::CF_SINGLE_LINE, "Box");

	writer.writeKey("min");
	ldlWrite(writer, value.min());

	Vector size = value.max() - value.min();
	writer.writeKey("size");
	ldlWrite(writer, size);

	writer.close();

	return true;
}

template<typename T>
String ldlToString(const T& value) {
	std::ostringstream out;
	LdlWriter writer(&out, "<string>", nullptr);
	writer.openList();
	ldlWrite(writer, value);
	writer.close();
	return out.str();
}

}

#endif
