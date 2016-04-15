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

#include <lair/utils/game_base.h>

#include "lair/utils/game_state.h"


namespace lair
{


GameState::GameState(GameBase* game)
	: _game(game) {
}


GameState::~GameState() {
}


SysModule* GameState::sys() {
	return _game->sys();
}


Window* GameState::window() {
	return _game->window();
}


RenderModule* GameState::renderModule() {
	return _game->renderModule();
}


Renderer* GameState::renderer() {
	return _game->renderer();
}


AudioModule* GameState::audio() {
	return _game->audio();
}


AssetManager* GameState::assets() {
	return _game->assets();
}


LoaderManager* GameState::loader() {
	return _game->loader();
}


Logger& GameState::log() {
	return _game->log();
}


}
