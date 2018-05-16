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


#ifndef _LAIR_LDL_PARSE_H
#define _LAIR_LDL_PARSE_H


#include <lair/core/lair.h>
#include <lair/core/log.h>


namespace lair
{


template<typename Arg0>
inline void streamCat(std::ostream& out, const Arg0& arg0) {
	out << arg0;
}

template<typename Arg0, typename... Args>
inline void streamCat(std::ostream& out, const Arg0& arg0, const Args&... args) {
	out << arg0;
	streamCat(out, args...);
}

template<typename... Args>
inline String cat(const Args&... args) {
	std::ostringstream out;
	streamCat(out, args...);
	return out.str();
}


class ErrorOutput {
public:
	virtual void warning(const String& message) = 0;
	virtual void error(const String& message) = 0;

	template<typename... Args>
	void warning(Args... args) {
		warning(cat(args...));
	}

	template<typename... Args>
	void error(Args... args) {
		error(cat(args...));
	}
};


class ErrorList : public ErrorOutput {
public:
	enum ErrorType {
		WARNING,
		ERROR,
	};

	struct Error {
		ErrorType type;
		String    message;
	};

public:
	unsigned nErrors() const;
	ErrorType errorType(unsigned index) const;
	const String& errorMessage(unsigned index) const;

	void clear();
	void warning(const String& message) override;
	void error(const String& message) override;

	using ErrorOutput::warning;
	using ErrorOutput::error;

	template<typename... Args>
	void prepend(unsigned from, Args... args) {
		for(unsigned i = from; i < nErrors(); ++i) {
			_errors[i].message = cat(args..., _errors[i].message);
		}
	}

	void log(Logger& log) const;

private:
	typedef std::vector<Error> ErrorVector;

private:
	ErrorVector _errors;
};


void eatWhitespaces(std::istream& in);
bool isNextIn(std::istream& in, const char* chars);
bool parseIdentifier(String& id, std::istream& in, const char* extra = "");


/**
 * \brief Read a stream into an extensible buffer and allow to move the read
 * position back and forth.
 *
 * Also keeps track of line and column numbers.
 */
class ParseBuffer {
public:
	ParseBuffer(std::istream* stream, const String& streamName, int size = 1024);

	inline bool eof() const {
		return _stream->eof();
	}

	inline bool error() const {
		return _stream->bad();
	}

	inline const String& streamName() const {
		return _streamName;
	}

	/// Returns the line number at the beginning of the buffer.
	inline unsigned line() const {
		return _line + 1;
	}

	/// \brief Returns the column number at the beginning of the buffer.
	inline unsigned col() const {
		return _col + 1;
	}

	/// \brief Get the current cursor position.
	inline size_t pos() const {
		return _pos;
	}

	/// \brief Read `count` bytes from the cursor and return the last one.
	inline int get(unsigned count = 1) {
		int c;
		for(unsigned i = 0; i < count; ++i) {
			c = _read(_pos);
			if(c >= 0)
				++_pos;
		}
		return c;
	}

	inline void back(unsigned count = 1) {
		lairAssert(count <= _pos);
		_pos -= count;
	}

	/// \brief Move the cursor forward until it reads `end`.
	bool parseUntil(const String end);

	/// \brief Return the next byte without moving the cursor.
	inline int peek() {
		return _read(_pos);
	}

	/// \brief Returns the `i`th bytes from the beginning of the buffer.
	///
	/// Accepts negative indices to access character `i + pos()`.
	inline int operator[](int i) {
		if(i < 0)
			i += _pos;
		return (i >= 0)? _read(i): -1;
	}

	/// \brief Remove the `i`th first bytes from the buffer.
	void flush(size_t i);

	/// \brief Remove all bytes up to the currsor position.
	void flush();

	/// \brief Returns true if the buffer contains `str`.
	bool contains(const char* str) const;

	/// \brief Returns the content of the buffer up to the cursor.
	String str() const;

private:
	typedef std::vector<unsigned char> Buffer;

private:
	/// \brief Read enough characters so that `_buf[i]` is valid. Do not move
	/// the cursor.
	inline int _read(unsigned i) {
		while(_buf.size() <= i) {
			int c = _stream->get();
			if(c == std::istream::traits_type::eof())
				return -1;
			_buf.push_back(c);
		}
		return _buf[i];
	}

private:
	std::istream* _stream;

	String        _streamName;
	unsigned      _line;
	unsigned      _col;

	Buffer        _buf;
	size_t        _pos;
};


inline bool asciiDigit(int c) {
	return c >= '0' && c <= '9';
}

inline bool asciiBinDigit(int c) {
	return c >= '0' && c <= '1';
}

inline bool asciiOctDigit(int c) {
	return c >= '0' && c <= '7';
}

inline bool asciiHexDigit(int c) {
	return asciiDigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

inline bool asciiAlpha(int c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

inline int asciiHexValue(int c) {
	if(c >= '0' && c <= '9')
		return c - '0';
	if(c >= 'A' && c <= 'F')
		return 10 + c - 'A';
	if(c >= 'a' && c <= 'f')
		return 10 + c - 'a';
	return -1;
}

}


#endif
