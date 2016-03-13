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


#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>

#include <Eigen/Dense>

#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/image.h>
#include <lair/core/path.h>

#include <lair/utils/input.h>
#include <lair/utils/interp_loop.h>

#include <lair/sys_sdl2/sys_module.h>
#include <lair/sys_sdl2/window.h>
#include <lair/sys_sdl2/sys_loader.h>

#include <lair/render_gl2/orthographic_camera.h>
#include <lair/render_gl2/shader_object.h>
#include <lair/render_gl2/program_object.h>
#include <lair/render_gl2/texture.h>
#include <lair/render_gl2/sprite.h>
#include <lair/render_gl2/renderer.h>
#include <lair/render_gl2/render_module.h>

#include <lair/ec/entity_manager.h>
#include <lair/ec/sprite_component.h>

#include <lair/utils/tiled_map.h>


#ifndef SIMPLE_WINDOW_DATA_DIR
#define SIMPLE_WINDOW_DATA_DIR "."
#endif


typedef Eigen::Vector4f Vec4;
typedef Eigen::Vector2f Vec2;

bool running = true;
std::string dataDir;
lair::Logger glog("main");


bool quit() {
	running = false;
	return true;
}


int main(int /*argc*/, char** argv) {
	bool ok = true;

	lair::MasterLogger logger;

	lair::OStreamLogger clogBackend(std::clog, true);
	logger.addBackend(&clogBackend);

	glog.setLevel(lair::LogLevel::Debug);
	glog.setMaster(&logger);

	// ////////////////////////////////////////////////////////////////////////

//	dataDir = SIMPLE_WINDOW_DATA_DIR;
//	glog.log("Data directory: \"", dataDir, "\"");

//	lair::Loader loader;
//	lair::NativeFileSystem nativeFs(dataDir.data());
//	loader.mountFilesystem(&nativeFs);

	// ////////////////////////////////////////////////////////////////////////

	lair::SysModule sys(&logger, lair::LogLevel::Info);
	ok = sys.initialize();
	if(!ok) abort();
	sys.onQuit = quit;

#ifdef SIMPLE_WINDOW_DATA_DIR
	lair::Path dataPath = lair::Path(SIMPLE_WINDOW_DATA_DIR);
#else
	lair::Path dataPath = sys.basePath();
#endif

	glog.log("Data directory: ", dataPath.native());

	sys.loader().setBasePath(dataPath);

	lair::Window* w = sys.createWindow("simple_window", 800, 600);
//	w->setFullscreen(true);

	// VSync works only with OpenGL.
	sys.setVSyncEnabled(true);
	glog.info("VSync: ", sys.isVSyncEnabled());

	lair::InputManager inputs(&sys, &glog);
	lair::Input* space = inputs.addInput("space");
	inputs.mapScanCode(space, SDL_SCANCODE_SPACE);
	lair::Input* left = inputs.addInput("left");
	inputs.mapScanCode(left, SDL_SCANCODE_LEFT);
	lair::Input* right = inputs.addInput("right");
	inputs.mapScanCode(right, SDL_SCANCODE_RIGHT);
	lair::Input* up = inputs.addInput("up");
	inputs.mapScanCode(up, SDL_SCANCODE_UP);
	lair::Input* down = inputs.addInput("down");
	inputs.mapScanCode(down, SDL_SCANCODE_DOWN);

	// ////////////////////////////////////////////////////////////////////////

	lair::RenderModule renderModule(&sys, &logger, lair::LogLevel::Info);
	renderModule.initialize();

	lair::Renderer* renderer = renderModule.createRenderer();

	renderer->preloadTexture("lair.png");
	lair::Texture* tex = renderer->getTexture("lair.png");
//	renderer->preloadSprite("lair.spr");


	Eigen::AlignedBox3f viewBox(Eigen::Vector3f(-400, -300, -1), Eigen::Vector3f(400, 300, 1));
	lair::OrthographicCamera camera;
	camera.setViewBox(viewBox);


//	lair::EntityManager entityManager(glog);
//	lair::SpriteComponentManager spriteManager(renderer);

//	lair::Sprite* sprite = renderer->getSprite("lair.spr");

//	const Json::Value& testJson = sys.loader().getJson("lair.obj");
//	lair::EntityRef baseSprite = entityManager.createEntityFromJson(
//	            lair::EntityRef(), testJson);
//	spriteManager.addComponentFromJson(baseSprite, testJson["sprite"]);

//	glog.warning("Anchor: ", baseSprite.sprite()->anchor().transpose());

//	lair::EntityRef testSprite = baseSprite.clone(entityManager.root(), "mainLair");

//	for(unsigned i = 1; i <= 8; ++i) {
//		lair::Transform t = lair::Transform::Identity();
//		t.translate(lair::Vector3(i * 32, 0, 0));
//		t.rotate(Eigen::AngleAxisf(i*.5, lair::Vector3::UnitZ()));

//		glog.info("Clone ", i, "...");
//		lair::EntityRef e = baseSprite.clone(testSprite, "Clone");
//		e.place(t);
//	}

//	testSprite.sprite()->setView(lair::Box2(lair::Vector2(.25, .25),
//	                                        lair::Vector2(.75, .75)));

//	const Json::Value& mapJson = sys.loader().getJson("map.json");
//	lair::TiledMap map;
//	map.setFromJson(glog, "map.json", mapJson);
//	map.setTileset(sprite);

	lair::InterpLoop loop(&sys);
	loop.setTickDuration(    1000000000/120);
	loop.setFrameDuration(   1000000000/60);
	loop.setMaxFrameDuration(1000000000/20);
	loop.setFrameMargin(     1000000000/120);

	glog.info("tps: ", 1000000000. / loop.tickDuration());

	loop.start();
	while(running) {
		switch(loop.nextEvent()) {
		case lair::InterpLoop::Tick: {
			inputs.sync();
			if(space->justPressed()) glog.log("Space pressed");
			if(space->justReleased()) glog.log("Space released");

//			float speed = 100. * loop.tickDuration() / 1000000000;
//			lair::Transform trans = testSprite.transform();
//			if( left->isPressed()) trans.translate(lair::Vector3(-speed, 0, 0));
//			if(right->isPressed()) trans.translate(lair::Vector3( speed, 0, 0));
//			if(   up->isPressed()) trans.translate(lair::Vector3(0,  speed, 0));
//			if( down->isPressed()) trans.translate(lair::Vector3(0, -speed, 0));
//			testSprite.move(trans);

//			entityManager.updateWorldTransform();
			break;
		}
		case lair::InterpLoop::Frame: {
//			testSprite.sprite()->setIndex(loop.frameTime() / 200000000);

			renderer->context()->clear(lair::gl::COLOR_BUFFER_BIT | lair::gl::DEPTH_BUFFER_BIT);
			renderer->mainBatch().clearBuffers();

//			map.render(renderer);
//			spriteManager.render(loop.frameInterp(), camera);

//			lair::Transform t = lair::Transform::Identity();
//			t.translate(-testSprite.transform().translation());

//			renderer->spriteShader()->use();
//			renderer->spriteShader()->setTextureUnit(0);
//			renderer->spriteShader()->setViewMatrix(camera.transform() * t.matrix());
//			renderer->mainBatch().render(renderer->context());

			w->swapBuffers();

			LAIR_LOG_OPENGL_ERRORS_TO(renderer->context(), glog);
			break;
		}}
	}
	loop.stop();

	// ////////////////////////////////////////////////////////////////////////

	renderModule.shutdown();
	sys.shutdown();
	return EXIT_SUCCESS;
}
