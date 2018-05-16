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

	void writeKey(const String& key, StringFormat format = SF_AUTO);

	void writeNull();
	void writeBool(bool b);
	void writeInt(int64 i, IntFormat format = IF_DECIMAL);
	void writeFloat(double d);
	void writeString(const String& str, StringFormat format = SF_AUTO);

	void openList(CompoundFormat format = CF_MULTI_LINE,
	                     const String& type = String(),
	              StringFormat typeFormat = SF_AUTO);

	void openMap(CompoundFormat format = CF_MULTI_LINE,
	                    const String& type = String(),
	             StringFormat typeFormat = SF_AUTO);

	void close();

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
	bool _acceptKey() const;
	bool _acceptValue() const;

	void _nextState();

	void _writeSepIfRequired();
	void _writeNewLine(int offset = 0);
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
