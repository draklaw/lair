/*
 *  Copyright (C) 2018 Simon Boyé
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


#ifndef _LAIR_DEMO_TEMPLATE_SCENE_H
#define _LAIR_DEMO_TEMPLATE_SCENE_H


#include <memory>

#include <lair/core/lair.h>

#include <lair/ec/entity_manager.h>


namespace lair {
class SysModule;
class Window;

class AssetManager;
class LoaderManager;

class RenderModule;
class Renderer;

class AudioModule;

class EntityManager;

class SpriteComponentManager;
class BitmapTextComponentManager;
class TileLayerComponentManager;

class SpriteComponent;
class BitmapTextComponent;
class TileLayerComponent;
}


class Game;

class MainState;


class Scene {
public:
	Scene(MainState* mainState);

	lair::SysModule*     sys();
	lair::Window*        window();

	lair::AssetManager*  assets();
	lair::LoaderManager* loader();

	lair::RenderModule*  renderModule();
	lair::Renderer*      renderer();

	lair::AudioModule*   audio();

	Game* game();

	lair::AssetSP asset(const lair::String& logicPath);

	lair::EntityManager&              entities();
	lair::SpriteComponentManager&     sprites();
	lair::BitmapTextComponentManager& texts();
	lair::TileLayerComponentManager&  tileLayers();

	lair::SpriteComponent*     sprite   (lair::EntityRef entity);
	lair::BitmapTextComponent* text     (lair::EntityRef entity);
	lair::TileLayerComponent*  tileLayer(lair::EntityRef entity);

	lair::EntityRef root();
	lair::EntityRef entity(const lair::String& name,
	                       lair::EntityRef from = lair::EntityRef());

	bool loadEntities(const lair::Path& path,
	                  lair::EntityRef parent = lair::EntityRef(),
	                  const lair::Path& cd = lair::Path());

	lair::Logger& log();

	virtual void load();
	virtual void unload();

	virtual void start();
	virtual void stop();

	virtual void updateTick();
	virtual void updateFrame();

protected:
	MainState* state;
};


typedef std::shared_ptr<Scene> SceneSP;


#endif
