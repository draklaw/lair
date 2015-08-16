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


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include "lair/core/python_logger.h"


namespace lair
{


PythonLogger::PythonLogger(PyObject* out)
    : _out(out),
      _terminalColor(false) {
	lairAssert(_out);
	PyObject* isatty = PyObject_CallMethod(_out, (char*)"isatty", (char*)"");
	if(isatty) {
		_terminalColor = PyBool_Check(isatty);
	}
}


PythonLogger::~PythonLogger() {
	Py_XDECREF(_out);
}


void PythonLogger::write(LogLevel level, const char* moduleName,
                          std::stringbuf* msgBuf) {
	if(_terminalColor) {
		std::ostringstream code;
		switch(level) {
		case LogLevel::Fatal:   code << "\033[38;5;" << 16 + 5*36 + 0*6 + 0*1 << ";1m"; break;
		case LogLevel::Error:   code << "\033[38;5;" << 16 + 4*36 + 0*6 + 0*1 << ";1m"; break;
		case LogLevel::Warning: code << "\033[38;5;" << 16 + 4*36 + 2*6 + 0*1 << ";1m"; break;
		case LogLevel::Log:     code << "\033[38;5;" << 16 + 0*36 + 4*6 + 0*1 << "m"; break;
		case LogLevel::Info:    code << "\033[38;5;" << 16 + 1*36 + 2*6 + 3*1 << "m"; break;
		case LogLevel::Debug:   code << "\033[38;5;" << 232 + 12 << "m"; break;
		}
		PyObject_CallMethod(_out, (char*)"write", (char*)"s", code.str().c_str());
	}
	PyObject_CallMethod(_out, (char*)"write", (char*)"s", moduleName);
	PyObject_CallMethod(_out, (char*)"write", (char*)"s", (char*)": ");
	PyObject_CallMethod(_out, (char*)"write", (char*)"s", msgBuf->str().c_str());
	if(_terminalColor) {
		PyObject_CallMethod(_out, (char*)"write", (char*)"s", (char*)"\033[0m");
	}
	PyObject_CallMethod(_out, (char*)"write", (char*)"s", (char*)"\n");
	PyObject_CallMethod(_out, (char*)"flush", (char*)"");
}


void PythonLogger::write(LogLevel level, const char* moduleName,
                          const char* msg) {
	if(_terminalColor) {
		std::ostringstream code;
		switch(level) {
		case LogLevel::Fatal:   code << "\033[38;5;" << 16 + 5*36 + 0*6 + 0*1 << ";1m"; break;
		case LogLevel::Error:   code << "\033[38;5;" << 16 + 4*36 + 0*6 + 0*1 << ";1m"; break;
		case LogLevel::Warning: code << "\033[38;5;" << 16 + 4*36 + 2*6 + 0*1 << ";1m"; break;
		case LogLevel::Log:     code << "\033[38;5;" << 16 + 0*36 + 4*6 + 0*1 << "m"; break;
		case LogLevel::Info:    code << "\033[38;5;" << 16 + 1*36 + 2*6 + 3*1 << "m"; break;
		case LogLevel::Debug:   code << "\033[38;5;" << 232 + 12 << "m"; break;
		}
		PyObject_CallMethod(_out, (char*)"write", (char*)"s", code.str().c_str());
	}
	PyObject_CallMethod(_out, (char*)"write", (char*)"s", moduleName);
	PyObject_CallMethod(_out, (char*)"write", (char*)"s", (char*)": ");
	PyObject_CallMethod(_out, (char*)"write", (char*)"s", msg);
	if(_terminalColor) {
		PyObject_CallMethod(_out, (char*)"write", (char*)"s", (char*)"\033[0m");
	}
	PyObject_CallMethod(_out, (char*)"write", (char*)"s", (char*)"\n");
	PyObject_CallMethod(_out, (char*)"flush", (char*)"");
}


}
