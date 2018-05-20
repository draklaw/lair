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


#include <lair/geometry/intersection.h>

#include <lair/sys_sdl2/sys_module.h>

#include "collision_scene.h"


CollisionScene::CollisionScene(MainState* mainState)
    : Scene(mainState) {
}


void CollisionScene::load() {
	_sceneRoot = entities().createEntity(root(), "collision_scene");
	_sceneRoot.setEnabled(false);

	loadEntities("collision.ldl", _sceneRoot);

	_models   = entity("__models__", _sceneRoot);
	_dotModel = entity("dot", _models);
	_shapes   = entity("shapes", _sceneRoot);
	_player   = entity("player", _sceneRoot);
	_dotRoot  = entity("dot_root", _sceneRoot);

	updateDepth(_sceneRoot);
}


void CollisionScene::unload() {
}


void CollisionScene::start() {
	_sceneRoot.setEnabled(true);

	_shapeType = DOT;
	_rotation  = 0;
}


void CollisionScene::stop() {
	_sceneRoot.setEnabled(false);
}


void CollisionScene::updateTick() {
	float offset = 1;
	float rotAngle = M_PI / 60;
	Vector2 p = _player.position2();

	// Movement
	if(sys()->getKeyState(SDL_SCANCODE_A))
		p(0) -= offset;
	if(sys()->getKeyState(SDL_SCANCODE_D))
		p(0) += offset;
	if(sys()->getKeyState(SDL_SCANCODE_S))
		p(1) -= offset;
	if(sys()->getKeyState(SDL_SCANCODE_W))
		p(1) += offset;
	if(sys()->getKeyState(SDL_SCANCODE_E))
		_rotation -= rotAngle;
	if(sys()->getKeyState(SDL_SCANCODE_Q))
		_rotation += rotAngle;

	log().warning(fmt(p), ", ", _rotation * 180 / M_PI);
	Transform trans = Transform::Identity();
	trans.translate((Vector3() << p, _player.position3()(2)).finished());
	trans.rotate(AngleAxis(_rotation, Vector3::UnitZ()));
	_player.place(trans);

	// Shape type
	CollisionComponent* pc = collision(_player);
	if(sys()->getKeyState(SDL_SCANCODE_1)) {
		_shapeType = DOT;
		pc->setShapes(Shape2DVector{ Sphere2(Vector2(0, 0), 4) });
	}
	if(sys()->getKeyState(SDL_SCANCODE_2)) {
		_shapeType = CIRCLE;
		pc->setShapes(Shape2DVector{ Sphere2(Vector2(0, 0), 32) });
	}
	if(sys()->getKeyState(SDL_SCANCODE_3)) {
		_shapeType = ABOX;
		pc->setShapes(Shape2DVector{ AlignedBox2(Vector2(-32, -48), Vector2(32, 48)) });
	}
	if(sys()->getKeyState(SDL_SCANCODE_4)) {
		_shapeType = ABOX;
		pc->setShapes(Shape2DVector{ OrientedBox2(Vector2(0, 0), Vector2(64, 96)) });
	}
	pc->setDirty();

	// Clear dots
	EntityRef dot = _dotRoot.firstChild();
	while(dot.isValid()) {
		dot.setEnabled(false);
		dot = dot.nextSibling();
	}
	dot = _dotRoot.firstChild();

	// Compute distance to point and display dots.
	if(_shapeType == DOT) {
		EntityRef shapeEntity = _shapes.firstChild();
		while(shapeEntity.isValid()) {
			Matrix3 trans;
			trans << shapeEntity.worldTransform().matrix().topLeftCorner<2, 2>(),
			         shapeEntity.worldTransform().matrix().topRightCorner<2, 1>(),
			         0, 0, 1;

			CollisionComponent* cc = collision(shapeEntity);
			const Shape2D& shape = cc->shapes()[0].transformed(trans);

			float   dist    = shape.distance(p);
			Vector2 closest = shape.closestPoint(p);

			if(!dot.isValid()) {
				dot = entities().cloneEntity(_dotModel, _dotRoot, "dot");
			}

			dot.setEnabled(true);
			dot.placeAt(closest);
			collision(dot)->setDirty();

			log().info(shapeEntity.name(), ": ", dist, ", ", (p - closest).norm(),
			           " - ", fmt(closest));

			shapeEntity = shapeEntity.nextSibling();
			dot = dot.nextSibling();
		}
	}

	// Compute collisions
	collisions().findCollisions();

	// Clear shapes color
	EntityRef shapeEntity = _shapes.firstChild();
	while(shapeEntity.isValid()) {
		collision(shapeEntity)->setDebugColor(Vector4(0, 1, 0, .2));
		shapeEntity = shapeEntity.nextSibling();
	}

	// Process shape collisions
	if(_shapeType != DOT) {
		for(auto&& hit: collisions().hitEvents()) {
			log().info("Hit: ", hit.entities[0].name(), " - ", hit.entities[1].name());

			if(hit.entities[0] == _player) {
				collision(hit.entities[1])->setDebugColor(Vector4(1, 0, 0, .2));
			}
			else if(hit.entities[1] == _player) {
				collision(hit.entities[0])->setDebugColor(Vector4(1, 0, 0, .2));
			}
		}
	}
}


void CollisionScene::updateFrame() {

}


void CollisionScene::render() {

}
