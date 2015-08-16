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


#ifndef _LAIR_CORE_PYTHON_LOGGER_H
#define _LAIR_CORE_PYTHON_LOGGER_H


#include <Python.h>

#include <lair/core/lair.h>


namespace lair
{


/**
 * \brief Allow to log stuff to a Python stream.
 */
class PythonLogger : public LoggerBackend {
public:
	PythonLogger(PyObject* out);
	virtual ~PythonLogger();

	virtual void write(LogLevel level, const char* moduleName,
	                   std::stringbuf* msgBuf);
	virtual void write(LogLevel level, const char* moduleName,
	                   const char* msg);

private:
	PyObject* _out;
	bool      _terminalColor;
};


}


#endif
