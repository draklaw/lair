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


#ifndef LAIR_LDL_LDL_WRITER_H
#define LAIR_LDL_LDL_WRITER_H


#include <lair/core/lair.h>


namespace lair
{


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


}


#endif
