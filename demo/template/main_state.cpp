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


#include <functional>

#include <lair/ldl/ldl_variant_loader.h>

#include "game.h"
#include "simple_scene.h"

#include "main_state.h"


#define ONE_SEC (1000000000)


MainState::MainState(Game* game)
	: GameState(game),

      _mainPass(renderer()),
      _spriteRenderer(loader(), renderer()),

      _entities(log(), _game->serializer()),
      _collisions(),
      _sprites(assets(), loader(), &_mainPass, &_spriteRenderer),
      _texts(loader(), &_mainPass, &_spriteRenderer),
      _tileLayers(loader(), &_mainPass, &_spriteRenderer),

      _inputs(sys(), &log()),

      _camera(),

      _initialized(false),
      _running(false),
      _loop(sys()),
      _fpsTime(0),
      _fpsCount(0),

      _quitInput(nullptr),

      _scene(std::make_shared<SimpleScene>(this)) {

	_entities.registerComponentManager(&_collisions);
	_entities.registerComponentManager(&_sprites);
	_entities.registerComponentManager(&_texts);
	_entities.registerComponentManager(&_tileLayers);
}


MainState::~MainState() {
}


Game* MainState::game() {
	return static_cast<Game*>(_game);
}


EntityManager& MainState::entities() {
	return _entities;
}


CollisionComponentManager& MainState::collisions() {
	return _collisions;
}


SpriteComponentManager& MainState::sprites() {
	return _sprites;
}


BitmapTextComponentManager& MainState::texts() {
	return _texts;
}


TileLayerComponentManager& MainState::tileLayers() {
	return _tileLayers;
}


CollisionComponent* MainState::collision(EntityRef entity) {
	return _collisions.get(entity);
}


SpriteComponent* MainState::sprite(EntityRef entity) {
	return _sprites.get(entity);
}


BitmapTextComponent* MainState::text(EntityRef entity) {
	return _texts.get(entity);
}


TileLayerComponent* MainState::tileLayer(EntityRef entity) {
	return _tileLayers.get(entity);
}


void MainState::initialize() {
	_loop.reset();
	_loop.setTickDuration(    ONE_SEC /  60);
	_loop.setFrameDuration(   ONE_SEC /  60);
	_loop.setMaxFrameDuration(_loop.frameDuration() * 3);
	_loop.setFrameMargin(     _loop.frameDuration() / 2);

	window()->onResize.connect(std::bind(&MainState::resizeEvent, this))
	        .track(_slotTracker);

	_quitInput = _inputs.addInput("quit");
	_inputs.mapScanCode(_quitInput, SDL_SCANCODE_ESCAPE);

	AssetSP whiteAsset = loader()->load<ImageLoader>("white.png")->asset();

	_scene->load();

	loader()->waitAll();

	// Set to true to debug OpenGL calls
	renderer()->context()->setLogCalls(false);

	SamplerSP sampler = renderer()->getSampler(SamplerParams(
	            SamplerParams::NEAREST | SamplerParams::CLAMP));
	_whiteTexture = renderer()->getTextureSet(
	            renderer()->getTextureUnit("sprite_color"), whiteAsset, sampler);

	_initialized = true;
}


void MainState::shutdown() {
	_scene->stop();

	_slotTracker.disconnectAll();

	_initialized = false;
}


void MainState::run() {
	lairAssert(_initialized);

	log().log("Starting main state...");
	_running = true;
	_loop.start();
	_fpsTime  = sys()->getTimeNs();
	_fpsCount = 0;

	startGame();

	do {
		switch(_loop.nextEvent()) {
		case InterpLoop::Tick:
			updateTick();
			break;
		case InterpLoop::Frame:
			updateFrame();
			break;
		}
	} while (_running);
	_loop.stop();
}


void MainState::quit() {
	_running = false;
}


void MainState::startGame() {
	_scene->start();
}


void MainState::updateTick() {
	loader()->finalizePending();

	_inputs.sync();
	_entities.setPrevWorldTransforms();

	if(_quitInput->justPressed()) {
		quit();
	}

	_scene->updateTick();

	_entities.updateWorldTransforms();
}


void MainState::updateFrame() {
	_scene->updateFrame();

	// Rendering
	Context* glc = renderer()->context();

	_texts.createTextures();
	_tileLayers.createTextures();
	renderer()->uploadPendingTextures();
	_spriteRenderer.finalizeShaders();

	glc->clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

	_mainPass.clear();

	bool buffersFilled = false;
	while(!buffersFilled) {
		_mainPass.clear();

		_spriteRenderer.beginRender();

		_sprites.render(_entities.root(), _loop.frameInterp(), _camera);
		_texts.render(_entities.root(), _loop.frameInterp(), _camera);
		_tileLayers.render(_entities.root(), _loop.frameInterp(), _camera);
		_collisions.render(&_spriteRenderer, &_mainPass, _whiteTexture, _camera);

		buffersFilled = _spriteRenderer.endRender();
	}

	_mainPass.render();

	window()->swapBuffers();
	glc->setLogCalls(false);

	uint64 now = sys()->getTimeNs();
	++_fpsCount;
	if(_fpsCount == 60) {
		log().info("Fps: ", _fpsCount * float(ONE_SEC) / (now - _fpsTime));
		_fpsTime  = now;
		_fpsCount = 0;
	}
}


void MainState::resizeEvent() {
	Box3 viewBox(Vector3::Zero(),
	             Vector3(window()->width()  / 2., // Big pixels
	                     window()->height() / 2., 1));
	_camera.setViewBox(viewBox);
}


bool MainState::loadEntities(const Path& path, EntityRef parent, const Path& cd) {
	Path localPath = makeAbsolute(cd, path);
	log().info("Load entity \"", localPath, "\"");

	Path realPath = game()->dataPath() / localPath;

	LoaderSP varLoader = loader()->load<LdlVariantLoader>(localPath);
	varLoader->wait();
	AssetSP asset = varLoader->asset();
	VariantAspectSP aspect = asset->aspect<VariantAspect>();
	return _entities.loadEntities(aspect->get(), parent);
}
