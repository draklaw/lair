/*
 *  Copyright (C) 2016 Simon Boyé
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


#include "main_state.h"

#include "game.h"


GameConfig::GameConfig()
	: GameConfigBase()
{
}

void GameConfig::setFromArgs(int& argc, char** argv) {
	GameConfigBase::setFromArgs(argc, argv);
}

const PropertyList& GameConfig::properties() const {
	return GameConfig::staticProperties();
}

const PropertyList& GameConfig::staticProperties() {
	return GameConfigBase::staticProperties();
}



Game::Game(int argc, char** argv)
    : GameBase(argc, argv),
      _mainState() {
}


Game::~Game() {
}


void Game::initialize() {
	GameBase::initialize(_config);

#ifdef LAIR_DATA_DIR
	_dataPath = LAIR_DATA_DIR;
	_loader->setBasePath(_dataPath);
#endif

	window()->setUtf8Title("Lair - template");

	_mainState.reset(new MainState(this));
	_mainState->initialize();
}


void Game::shutdown() {
	_mainState->shutdown();
	_mainState.reset();  // Required to ensure that everything is freed

	GameBase::shutdown();
}


GameConfig& Game::config() {
	return _config;
}


MainState* Game::mainState() {
	return _mainState.get();
}
