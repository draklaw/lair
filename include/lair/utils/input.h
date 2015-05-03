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


#ifndef _UW_INPUT_H_
#define _UW_INPUT_H_


#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <lair/core/lair.h>


namespace lair {


class Logger;
class SysModule;


class Input {
public:
	inline Input(const std::string& name)
	    : name(name), count(0), prevCount(0) {}

	inline bool isPressed()    const { return count; }
	inline bool justPressed()  const { return count && !prevCount; }
	inline bool justReleased() const { return prevCount && !count; }

	const std::string name;
	unsigned          count;
	unsigned          prevCount;
};


class InputManager {
public:
	InputManager(SysModule* sys, Logger* log);
	~InputManager();

	Input* addInput (const std::string& name);
	Input* getByName(const std::string& name);

	void   mapScanCode(Input* input, ScanCode scanCode);

	void sync();

	Logger& log();

private:
	typedef std::unique_ptr<Input>                   InputPtr;
	typedef std::vector<InputPtr>                    InputMap;
	typedef std::unordered_map<std::string, Input*>  InputNameMap;

	typedef std::vector<Input*>                      InputList;
	typedef std::unordered_map<ScanCode, InputList>  ScanCodeMap;

private:
	SysModule*    _sys;
	Logger*       _log;

	InputMap      _inputMap;
	InputNameMap  _inputNameMap;
	ScanCodeMap   _scanCodeMap;
};


}

#endif
