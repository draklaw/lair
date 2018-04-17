/*
 *  Copyright (C) 2015-2018 Simon Boyé
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


#ifndef _LAIR_DEMO_TEMPLATE_SIMPLE_SCENE_H
#define _LAIR_DEMO_TEMPLATE_SIMPLE_SCENE_H


#include "main_state.h"


class SimpleScene : public Scene {
public:
	SimpleScene(MainState* mainState);

	virtual void load() override;
	virtual void unload() override;

	virtual void start() override;
	virtual void stop() override;

	virtual void updateTick() override;
	virtual void updateFrame() override;

protected:
	TileMapAspectSP _tileMap;

	EntityRef   _modelRoot;
	EntityRef   _tileLayer;
};


#endif
