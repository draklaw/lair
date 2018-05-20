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


#include <lair/sys_sdl2/audio_module.h>

#include "simple_scene.h"


SimpleScene::SimpleScene(MainState* mainState)
    : Scene(mainState) {
}


void SimpleScene::load() {
	_sceneRoot = entities().createEntity(root(), "simple_scene");
	_sceneRoot.setEnabled(false);

	LoaderSP tileMapLoader = loader()->load<TileMapLoader>("test_map.ldl");

//	loader()->load<lair::SoundLoader>("sound.ogg");
	//loader()->load<MusicLoader>("music.ogg");

	loader()->waitAll();
	_tileMap = tileMapLoader->asset()->aspect<TileMapAspect>();
	entities().loadEntities(_tileMap->get().objects(), _sceneRoot);

	loader()->waitAll();

	updateDepth(_sceneRoot);
}


void SimpleScene::unload() {
}


void SimpleScene::start() {
	_modelRoot = entity("__model__");

	_sceneRoot.setEnabled(true);

	//audio()->playMusic(assets()->getAsset("music.ogg"));
//	audio()->playSound(asset("sound.ogg"), 2);
}


void SimpleScene::stop() {
	_sceneRoot.setEnabled(false);
}


void SimpleScene::updateTick() {
	collisions().findCollisions();
}


void SimpleScene::updateFrame() {

}
