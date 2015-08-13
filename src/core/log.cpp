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


#include <algorithm>

#include "lair/core/log.h"


namespace lair {


LoggerBackend::~LoggerBackend() {
}


// ////////////////////////////////////////////////////////////////////////////


OStreamLogger::OStreamLogger(std::ostream& out, bool termColor)
    : _out(out),
      _terminalColor(termColor) {
}


OStreamLogger::~OStreamLogger() {
}


void OStreamLogger::write(LogLevel level, const char* moduleName,
                          std::stringbuf* msgBuf) {
	if(_terminalColor) {
		switch(level) {
		case LogLevel::Fatal:   _out << "\033[38;5;" << 16 + 5*36 + 0*6 + 0*1 << ";1m"; break;
		case LogLevel::Error:   _out << "\033[38;5;" << 16 + 4*36 + 0*6 + 0*1 << ";1m"; break;
		case LogLevel::Warning: _out << "\033[38;5;" << 16 + 4*36 + 2*6 + 0*1 << ";1m"; break;
		case LogLevel::Log:     _out << "\033[38;5;" << 16 + 0*36 + 4*6 + 0*1 << "m"; break;
		case LogLevel::Info:    _out << "\033[38;5;" << 16 + 1*36 + 2*6 + 3*1 << "m"; break;
		case LogLevel::Debug:   _out << "\033[38;5;" << 232 + 12 << "m"; break;
		}
	}
	_out << moduleName << ": " << msgBuf;
	if(_terminalColor) {
		_out << "\033[0m";
	}
	_out << std::endl;
}


void OStreamLogger::write(LogLevel level, const char* moduleName,
                          const char* msg) {
	if(_terminalColor) {
		switch(level) {
		case LogLevel::Fatal:   _out << "\033[38;5;" << 16 + 5*36 + 0*6 + 0*1 << ";1m"; break;
		case LogLevel::Error:   _out << "\033[38;5;" << 16 + 4*36 + 0*6 + 0*1 << ";1m"; break;
		case LogLevel::Warning: _out << "\033[38;5;" << 16 + 4*36 + 2*6 + 0*1 << ";1m"; break;
		case LogLevel::Log:     _out << "\033[38;5;" << 16 + 0*36 + 4*6 + 0*1 << "m"; break;
		case LogLevel::Info:    _out << "\033[38;5;" << 16 + 1*36 + 2*6 + 3*1 << "m"; break;
		case LogLevel::Debug:   _out << "\033[38;5;" << 232 + 12 << "m"; break;
		}
	}
	_out << moduleName << ": " << msg;
	if(_terminalColor) {
		_out << "\033[0m";
	}
	_out << std::endl;
}


// ////////////////////////////////////////////////////////////////////////////


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


// ////////////////////////////////////////////////////////////////////////////


MasterLogger::MasterLogger() {
}


void MasterLogger::addBackend(LoggerBackend* backend) {
	lairAssert(backend);
	_backends.push_back(backend);
}


void MasterLogger::removeBackend(LoggerBackend* backend) {
	_backends.erase(std::find(_backends.begin(), _backends.end(), backend));
}


void MasterLogger::write(LogLevel level, const char* moduleName,
                         std::stringbuf* msgBuf) {
	// TODO: make this thread safe.
	for(LoggerBackend* backend: _backends) {
		msgBuf->pubseekpos(std::ios_base::beg, std::ios_base::in);
		backend->write(level, moduleName, msgBuf);
	}
}


void MasterLogger::write(LogLevel level, const char* moduleName,
                         const char* msg) {
	// TODO: make this thread safe.
	for(LoggerBackend* backend: _backends) {
		backend->write(level, moduleName, msg);
	}
}


// ////////////////////////////////////////////////////////////////////////////


Logger::Logger(const char* defaultModuleName, MasterLogger* master,
               LogLevel level)
    : _master(master), _defaultModuleName(defaultModuleName), _level(level) {
}


Logger::Logger(Logger* other) {
	if(other) {
		setFrom(*other);
	}
}


MasterLogger* Logger::master() {
	return _master;
}


void Logger::setMaster(MasterLogger* master) {
	_master = master;
}


const char* Logger::defaultModuleName() const {
	return _defaultModuleName;
}


void Logger::setDefaultModuleName(const char* moduleName) {
	_defaultModuleName = moduleName;
}


LogLevel Logger::level() const {
	return _level;
}


void Logger::setLevel(LogLevel level) {
	_level = level;
}


void Logger::setFrom(Logger& other) {
	_master = other.master();
	_defaultModuleName = other.defaultModuleName();
	_level = other.level();
}


// ////////////////////////////////////////////////////////////////////////////


Logger noopLogger;


}
