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
	AssetSP tileMapAsset = loader()->load<TileMapLoader>("test_map.ldl")->asset();
	_tileMap = tileMapAsset->aspect<TileMapAspect>();

	_tileLayer = entities().createEntity(entities().root(), "tile_layer");
	TileLayerComponent* tileLayerComp = tileLayers().addComponent(_tileLayer);
	tileLayerComp->setTileMap(_tileMap);
//	_tileLayer.place(Vector3(120, 90, .5));

	loadEntities("entities.ldl", root());

	loader()->load<lair::SoundLoader>("sound.ogg");
	//loader()->load<MusicLoader>("music.ogg");
}


void SimpleScene::unload() {
}


void SimpleScene::start() {
	_modelRoot = entities().createEntity(entities().root(), "modelRoot");

	EntityRef sprite = entities().findByName("sprite");
	EntityRef text   = entities().findByName("text");

	if(sprite.isValid())
		log().info("Entity \"", sprite.name(), "\" found.");
	if(text.isValid())
		log().info("Entity \"", text.name(), "\" found.");

	//audio()->playMusic(assets()->getAsset("music.ogg"));
	audio()->playSound(asset("sound.ogg"), 2);
}


void SimpleScene::stop() {

}


void SimpleScene::updateTick() {

}


void SimpleScene::updateFrame() {

}
