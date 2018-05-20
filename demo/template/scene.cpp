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


#include "game.h"
#include "main_state.h"

#include "scene.h"


Scene::Scene(MainState *mainState)
    : state(mainState) {
}


lair::SysModule* Scene::sys() {
	return game()->sys();
}


lair::Window* Scene::window() {
	return game()->window();
}


lair::AssetManager* Scene::assets() {
	return game()->assets();
}


lair::LoaderManager* Scene::loader() {
	return game()->loader();
}


lair::RenderModule* Scene::renderModule() {
	return game()->renderModule();
}


lair::Renderer* Scene::renderer() {
	return game()->renderer();
}


lair::AudioModule* Scene::audio() {
	return game()->audio();
}


lair::AssetSP Scene::asset(const lair::String& logicPath) {
	return assets()->getAsset(logicPath);
}


Game* Scene::game() {
	return state->game();
}


lair::EntityManager& Scene::entities() {
	return state->entities();
}


lair::CollisionComponentManager& Scene::collisions() {
	return state->collisions();
}


lair::SpriteComponentManager& Scene::sprites() {
	return state->sprites();
}


lair::BitmapTextComponentManager& Scene::texts() {
	return state->texts();
}


lair::TileLayerComponentManager& Scene::tileLayers() {
	return state->tileLayers();
}


lair::CollisionComponent* Scene::collision(lair::EntityRef entity) {
	return collisions().get(entity);
}


lair::SpriteComponent* Scene::sprite(lair::EntityRef entity) {
	return sprites().get(entity);
}


lair::BitmapTextComponent* Scene::text(lair::EntityRef entity) {
	return texts().get(entity);
}


lair::TileLayerComponent* Scene::tileLayer(lair::EntityRef entity) {
	return tileLayers().get(entity);
}


lair::EntityRef Scene::root() {
	return entities().root();
}


lair::EntityRef Scene::entity(const lair::String& name, lair::EntityRef from) {
	return entities().findByName(name, from);
}


bool Scene::loadEntities(const lair::Path& path, lair::EntityRef parent,
                         const lair::Path& cd) {
	return state->loadEntities(path, parent, cd);
}


float Scene::updateDepth(lair::EntityRef entity, float inc, float depth) {
	Vector3 p = entity.position3() + Vector3(0, 0, depth);
	entity.moveTo(p);

	EntityRef child = entity.firstChild();
	float thickness = inc;
	while(child.isValid()) {
		thickness += updateDepth(child, inc, thickness);
		child = child.nextSibling();
	}

	return thickness;
}


lair::Logger& Scene::log() {
	return state->log();
}


void Scene::load() {
}


void Scene::unload() {
}


void Scene::start() {
}


void Scene::stop() {
}


void Scene::updateTick() {
}


void Scene::updateFrame() {
}


void Scene::render() {
}
