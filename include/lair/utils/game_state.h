/*
 *  Copyright (C) 2015, 2016 Simon Boyé
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


#ifndef _LAIR_UTILS_GAME_STATE_H
#define _LAIR_UTILS_GAME_STATE_H


#include <memory>

#include <lair/core/lair.h>
#include <lair/core/log.h>


namespace lair {


class GameState {
public:
	GameState() = default;
	virtual ~GameState() = default;

	virtual void initialize() = 0;
	virtual void shutdown() = 0;

	virtual void run() = 0;
	virtual void quit() = 0;

protected:
};

typedef std::unique_ptr<GameState> GameStateUP;


}

#endif
