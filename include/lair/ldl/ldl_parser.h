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


#ifndef LAIR_LDL_LDL_PARSER_H
#define LAIR_LDL_LDL_PARSER_H


#include <lair/core/lair.h>
#include <lair/core/text.h>
#include <lair/core/parse.h>


namespace lair
{


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

	Context context() const;

	bool isValueTyped() const;
	String getValueTypeName() const;

	String getKey() const;
	Type valueType() const;
	const char* valueTypeName() const;

	void enter();
	void leave();
	void skip();

	bool getBool() const;
	int64 getInt() const;
	double getFloat() const;
	String getString() const;

	void next();

	const String& streamName() const;
	unsigned line() const;
	unsigned col() const;

	void error(const String& message) override;
	void warning(const String& message) override;

	using ErrorOutput::error;
	using ErrorOutput::warning;

	static const char* typeName(Type type);

	ErrorList* errorList();

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
	void _next();

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


enum {
	NO_ANNOTATION_WARNING   = 0x00,
	WITH_ANNOTATION_WARNING = 0x01,
};


}


#endif
