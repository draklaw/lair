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

#include <lair/sys_sdl2/sys_module.h>

#include <lair/utils/input.h>


namespace lair {


InputManager::InputManager(SysModule* sys, Logger* log)
    : _sys(sys),
      _log(log),
      _inputMap(),
      _inputNameMap(),
      _scanCodeMap() {
}


InputManager::~InputManager() {
}


Input* InputManager::addInput(const std::string& name) {
	_inputMap.emplace_back(new Input(name));
	Input* ptr = _inputMap.back().get();
	_inputNameMap.emplace(name, ptr);
	return ptr;
}


Input* InputManager::getByName(const std::string& name) {
	auto it = _inputNameMap.find(name);
	if(it == _inputNameMap.end()) {
		log().warning("Request for non-existing input \"", name, "\"");
		return nullptr;
	}
	return it->second;
}


void InputManager::mapScanCode(Input* input, ScanCode scanCode) {
	_scanCodeMap[scanCode].push_back(input);
}


void InputManager::sync() {
	// FIXME: If an input is pressed between two updates, it may be missed.
	for(auto& input: _inputMap) {
		input->prevCount = input->count;
		input->count     = 0;
	}

	for(auto& keyInputList: _scanCodeMap) {
		for(Input* input: keyInputList.second) {
			input->count += _sys->getKeyState(keyInputList.first);
		}
	}
}


Logger& InputManager::log() {
	return _log? *_log: noopLogger;
}


}
