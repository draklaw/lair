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

#include <lair/utils/path.h>
#include <lair/utils/input.h>

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

#ifdef SIMPLE_WINDOW_DATA_DIR
	lair::Path dataPath = boost::filesystem::canonical(SIMPLE_WINDOW_DATA_DIR);
#else
	lair::Path dataPath = lair::exePath(argv[0]);
#endif
	glog.log("Data directory: ", dataPath.native());

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

	lair::Texture* texture = renderer->loadTexture("lair.png");
	lair::Sprite   sprite(texture);


	Eigen::AlignedBox3f viewBox(Eigen::Vector3f(-400, -300, -1), Eigen::Vector3f(400, 300, 1));
	lair::OrthographicCamera camera;
	camera.setViewBox(viewBox);


	lair::EntityManager entityManager(renderer);

	// Need to know the size to create entity.
	if(!texture->_uploadNow()) {
		texture = renderer->defaultTexture();
	}

	lair::EntityRef testSprite = entityManager.createEntity(entityManager.root(), "test");
	entityManager.addSpriteComponent(testSprite);
	testSprite.sprite()->setSprite(&sprite);
	testSprite.setTransform(lair::Transform(
	        lair::Translation(-lair::Vector3(texture->width(),
	                                         texture->height(), 0) / 2)));


	while(running) {
//		sys.waitAndDispatchSystemEvents();
		sys.dispatchPendingSystemEvents();
		inputs.sync();
		if(space->justPressed()) glog.log("Space pressed");
		if(space->justReleased()) glog.log("Space released");

		float speed = 1;
		lair::Transform trans = testSprite.transform();
		if( left->isPressed()) trans.translate(lair::Vector3(-speed, 0, 0));
		if(right->isPressed()) trans.translate(lair::Vector3( speed, 0, 0));
		if(   up->isPressed()) trans.translate(lair::Vector3(0,  speed, 0));
		if( down->isPressed()) trans.translate(lair::Vector3(0, -speed, 0));
		testSprite.setTransform(trans);

		entityManager.updateWorldTransform();
		entityManager.render(camera);
		w->swapBuffers();

		LAIR_LOG_OPENGL_ERRORS_TO(glog);
	}

	// ////////////////////////////////////////////////////////////////////////

	renderModule.shutdown();
	sys.shutdown();
	return EXIT_SUCCESS;
}
