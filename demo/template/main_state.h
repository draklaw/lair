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


#ifndef _LAIR_DEMO_TEMPLATE_MAIN_STATE_H
#define _LAIR_DEMO_TEMPLATE_MAIN_STATE_H


#include <lair/core/signal.h>

#include <lair/utils/game_state.h>
#include <lair/utils/interp_loop.h>
#include <lair/utils/input.h>
#include <lair/utils/tile_map.h>

#include <lair/render_gl3/orthographic_camera.h>
#include <lair/render_gl3/render_pass.h>

#include <lair/ec/entity.h>
#include <lair/ec/entity_manager.h>
#include <lair/ec/sprite_component.h>
#include <lair/ec/bitmap_text_component.h>
#include <lair/ec/tile_layer_component.h>

#include "scene.h"


using namespace lair;


class Game;


class MainState : public GameState {
public:
	MainState(Game* game);
	virtual ~MainState();

	Game* game();

	EntityManager& entities();
	SpriteComponentManager& sprites();
	BitmapTextComponentManager& texts();
	TileLayerComponentManager& tileLayers();

	SpriteComponent* sprite(EntityRef entity);
	BitmapTextComponent* text(EntityRef entity);
	TileLayerComponent* tileLayer(EntityRef entity);

	virtual void initialize();
	virtual void shutdown();

	virtual void run();
	virtual void quit();

	void startGame();
	void updateTick();
	void updateFrame();

	void resizeEvent();

	bool loadEntities(const Path& path, EntityRef parent = EntityRef(),
	                  const Path& cd = Path());

protected:
	// More or less system stuff

	RenderPass                 _mainPass;
	SpriteRenderer             _spriteRenderer;

	EntityManager              _entities;
	SpriteComponentManager     _sprites;
	BitmapTextComponentManager _texts;
	TileLayerComponentManager  _tileLayers;

	InputManager               _inputs;

	SlotTracker _slotTracker;

	OrthographicCamera _camera;

	bool        _initialized;
	bool        _running;
	InterpLoop  _loop;
	int64       _fpsTime;
	unsigned    _fpsCount;

	Input*      _quitInput;

	SceneSP _scene;
};


#endif
