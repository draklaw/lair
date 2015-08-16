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


#ifndef _LAIR_UTILS_LOG_H
#define _LAIR_UTILS_LOG_H


#include <vector>
#include <string>
#include <ostream>
#include <sstream>

#include <iostream>

#include <lair/core/lair.h>


namespace lair {

namespace internal {
inline void printArgsHelper(std::stringstream&) {}

template < typename T, typename... Args >
inline void printArgsHelper(std::stringstream& out, T first, Args... args) {
	out << first;
	printArgsHelper(out, args...);
}
}

/// \defgroup utils utils
/// \{


enum class LogLevel {
	Fatal,
	Error,
	Warning,
	Log,
	Info,
	Debug
};


/**
 * \brief Base class for logger backend implementation.
 */
class LoggerBackend {
public:
	virtual ~LoggerBackend();

	virtual void write(LogLevel level, const char* moduleName,
	                   std::stringbuf* msgBuf) = 0;
	virtual void write(LogLevel level, const char* moduleName,
	                   const char* msg) = 0;
};


/**
 * \brief Allow to log stuff in anything that offer an iostream interface.
 */
class OStreamLogger : public LoggerBackend {
public:
	OStreamLogger(std::ostream& out, bool termColor = false);
	virtual ~OStreamLogger();

	virtual void write(LogLevel level, const char* moduleName,
	                   std::stringbuf* msgBuf);
	virtual void write(LogLevel level, const char* moduleName,
	                   const char* msg);

private:
	std::ostream& _out;
	bool          _terminalColor;
};


/**
 * \brief The MasterLogger class dispatch log entries to the backends.
 */
class MasterLogger {
public:
	MasterLogger();
	MasterLogger(const MasterLogger&) = delete;

	MasterLogger& operator=(const MasterLogger&) = delete;

	/**
	 * \brief Add a backend. The MasterLogger object does not take ownership
	 * of the backend, so this is user responsability to ensure the object
	 * stay alive as long as it is in use.
	 *
	 * \todo Make it such that MasterLogger takes ownership of backends ?
	 */
	void addBackend(LoggerBackend* backend);
	void removeBackend(LoggerBackend* backend);

	/**
	 * \brief Add a log entry.
	 *
	 * This function is not indended to be used directly, but rather through a
	 * Logger object.
	 *
	 * \todo Make this function thread-safe.
	 *
	 * \param level
	 * \param moduleName
	 * \param msgBuf
	 */
	void write(LogLevel level, const char* moduleName, std::stringbuf* msgBuf);
	void write(LogLevel level, const char* moduleName, const char* msg);

private:
	typedef std::vector<LoggerBackend*> BackendList;

	BackendList _backends;
};


/**
 * \brief The frontend logger class.
 *
 * Offers a variety of function to ease logging, the ability to set a default
 * module name and to filter enties by level. This class does not do the
 * logging itself, but rather rely on the associated MasterLogger object.
 *
 * Typical application have a single MasterLogger object and several Logger.
 * It is practical to have one Logger per module, configured to log the module
 * name by default. An other reason to use several Loggers is to enable logging
 * in multi-threaded applications: Loggers are not thread-safe, but
 * MasterLogger is (will be, at time of writing), so one should use a
 * different logger on each thread.
 */
class Logger {
public:
	Logger(const char* defaultModuleName = "", MasterLogger* master = 0,
	       LogLevel level = LogLevel::Log);
	explicit Logger(Logger *other);

	MasterLogger* master();
	void setMaster(MasterLogger* master);

	const char* defaultModuleName() const;
	void setDefaultModuleName(const char* moduleName);

	LogLevel level() const;
	void setLevel(LogLevel level);

	void setFrom(Logger& other);

	template < typename... Args >
	inline void write(LogLevel level, Args... args) {
		writeM(level, _defaultModuleName, args...);
	}

	template < typename... Args >
	void writeM(LogLevel level, const char* moduleName, Args... args);

	template < typename... Args >
	inline void fatal(Args... args) {
		write(LogLevel::Fatal, args...);
	}

	template < typename... Args >
	inline void fatalM(const char* moduleName, Args... args) {
		writeM(LogLevel::Fatal, moduleName, args...);
	}

	template < typename... Args >
	inline void error(Args... args) {
		write(LogLevel::Error, args...);
	}

	template < typename... Args >
	inline void errorM(const char* moduleName, Args... args) {
		writeM(LogLevel::Error, moduleName, args...);
	}

	template < typename... Args >
	inline void warning(Args... args) {
		write(LogLevel::Warning, args...);
	}

	template < typename... Args >
	inline void warningM(const char* moduleName, Args... args) {
		writeM(LogLevel::Warning, moduleName, args...);
	}

	template < typename... Args >
	inline void log(Args... args) {
		write(LogLevel::Log, args...);
	}

	template < typename... Args >
	inline void logM(const char* moduleName, Args... args) {
		writeM(LogLevel::Log, moduleName, args...);
	}

	template < typename... Args >
	inline void info(Args... args) {
		write(LogLevel::Info, args...);
	}

	template < typename... Args >
	inline void infoM(const char* moduleName, Args... args) {
		writeM(LogLevel::Info, moduleName, args...);
	}

	template < typename... Args >
	inline void debug(Args... args) {
		write(LogLevel::Debug, args...);
	}

	template < typename... Args >
	inline void debugM(const char* moduleName, Args... args) {
		writeM(LogLevel::Debug, moduleName, args...);
	}


private:
	MasterLogger* _master;
	const char* _defaultModuleName;
	LogLevel _level;
	std::stringstream _formatter;
};


/// \}


// ////////////////////////////////////////////////////////////////////////////


template < typename... Args >
void Logger::writeM(LogLevel level,
                  const char* moduleName, Args... args) {
	if(!_master || level > _level)
		return;

	_formatter.str(std::string());
	internal::printArgsHelper(_formatter, args...);
	_master->write(level, moduleName, _formatter.rdbuf());
}


// ////////////////////////////////////////////////////////////////////////////


/**
 * \brief A logger with no backend that does nothing.
 *
 * Can be used when you need to return a reference to a logger but don't want
 * or can't get one.
 *
 * You should not assing a MasterLogger to this object, as it is a global
 * object and it is not thead-safe.
 */
extern Logger noopLogger;

extern Logger dbgLogger;


}


#endif
