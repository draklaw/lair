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


#include <functional>

#include <lair/core/json.h>

#include "game.h"

#include "main_state.h"


#define ONE_SEC (1000000000)


MainState::MainState(Game* game)
	: _game(game),

      _entities(_game->log()),
      _sprites(_game->renderer(), _game->assets(), _game->loader()),

      _inputs(_game->sys(), &log()),

      _camera(),

      _initialized(false),
      _running(false),
      _loop(_game->sys()),
      _fpsTime(0),
      _fpsCount(0),

      _quitInput(nullptr) {

	_entities.registerComponentManager(&_sprites);
}


MainState::~MainState() {
}


void MainState::initialize() {
	_loop.reset();
	_loop.setTickDuration(    ONE_SEC /  60);
	_loop.setFrameDuration(   ONE_SEC /  60);
	_loop.setMaxFrameDuration(_loop.frameDuration() * 3);
	_loop.setFrameMargin(     _loop.frameDuration() / 2);

	_game->window()->onResize.connect(std::bind(&MainState::resizeEvent, this))
	        .track(_slotTracker);

	_quitInput = _inputs.addInput("quit");
	_inputs.mapScanCode(_quitInput, SDL_SCANCODE_ESCAPE);

	_modelRoot = _entities.createEntity(_entities.root(), "modelRoot");

	// TODO: load stuff.
	EntityRef sprite = loadEntity("sprite.json", _entities.root());
	sprite.place(Vector3(160, 90, .5));

	_game->loader()->waitAll();

	_initialized = true;
}


void MainState::shutdown() {
//	_game->audio()->releaseMusic(_music1);

	_slotTracker.disconnectAll();

	_initialized = false;
}


void MainState::run() {
	lairAssert(_initialized);

	log().log("Starting main state...");
	_running = true;
	_loop.start();
	_fpsTime  = _game->sys()->getTimeNs();
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
	// TODO: Setup game
}


void MainState::updateTick() {
	_inputs.sync();

	if(_quitInput->justPressed()) {
		quit();
	}

	// TODO: Game update.

	_entities.updateWorldTransform();
}


void MainState::updateFrame() {
	// Rendering
	Context* glc = _game->renderer()->context();
	_game->renderer()->uploadPendingTextures();

	glc->clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

	_sprites.render(_loop.frameInterp(), _camera);

	_game->window()->swapBuffers();

	uint64 now = _game->sys()->getTimeNs();
	++_fpsCount;
	if(_fpsCount == 60) {
		log().info("Fps: ", _fpsCount * float(ONE_SEC) / (now - _fpsTime));
		_fpsTime  = now;
		_fpsCount = 0;
	}
}


void MainState::resizeEvent() {
	Box3 viewBox(Vector3::Zero(),
	             Vector3(_game->window()->width()  / 4., // Big pixels
	                     _game->window()->height() / 4., 1));
	_camera.setViewBox(viewBox);
}


EntityRef MainState::loadEntity(const Path& path, EntityRef parent, const Path& cd) {
	Path localPath = make_absolute(cd, path);
	log().info("Load entity \"", localPath, "\"");

	std::ifstream in((_game->dataPath() / localPath).native().c_str());
	if(!in.good()) {
		log().error("Unable to read \"", localPath, "\".");
		return EntityRef();
	}

	Json::Value json;
	Json::Reader reader;
	if(!reader.parse(in, json)) {
		log().error("Error while parsing json \"", localPath, "\": ",
		            reader.getFormattedErrorMessages());
		return EntityRef();
	}

	if(!parent.isValid()) {
		parent = _modelRoot;
	}

	return _entities.createEntityFromJson(parent, json, localPath.dir());
}


Logger& MainState::log() {
	return _game->log();
}
