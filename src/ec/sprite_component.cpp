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

#include <lair/core/lair.h>
#include <lair/core/log.h>

#include <lair/render_gl2/texture.h>
#include <lair/render_gl2/sprite.h>
#include <lair/render_gl2/renderer.h>

#include "lair/ec/entity.h"
#include "lair/ec/entity_manager.h"

#include "lair/ec/sprite_component.h"


namespace lair
{


SpriteComponent::SpriteComponent() {
}


SpriteComponent::~SpriteComponent() {
}


//---------------------------------------------------------------------------//


SpriteComponentManager::SpriteComponentManager(EntityManager* manager)
    : _manager(manager),
      _renderer(_manager->renderer()),
      _componentBlockSize(128),
      _nComponent(0),
      _components(),
      _firstFree(nullptr),
      _vertices(),
      _indices(),
      _vertexBuffer(0),
      _indexBuffer(0) {
}


SpriteComponentManager::~SpriteComponentManager() {
}


void SpriteComponentManager::addComponent(_Entity* entity) {
	lairAssert(!entity->sprite);

	if(!_firstFree) {
		_addComponentBlock();
	}

	SpriteComponent* sc = _firstFree;
	lairAssert(sc && !sc->_entity());
	_firstFree = reinterpret_cast<SpriteComponent*>(
	            const_cast<Sprite*>(sc->sprite()));

	sc->_setEntity(entity);
	sc->setSprite(nullptr);
	sc->setIndex(0);

	entity->sprite = sc;
	++_nComponent;
}


void SpriteComponentManager::removeComponent(_Entity* entity) {
	SpriteComponent* sc = entity->sprite;
	lairAssert(sc && sc->_entity() == entity);

	entity->sprite = nullptr;
	--_nComponent;

	sc->_reset();
	sc->setSprite(reinterpret_cast<Sprite*>(_firstFree));
	_firstFree = sc;
}


void SpriteComponentManager::render() {
	_defaultBatch.clearBuffers();

	GLuint index = 0;
	for(ComponentBlock& block: _components) {
		for(SpriteComponent& sc: block) {
			if(!sc._entity()) {
				continue;
			}
			const Sprite* sprite = sc.sprite();
			Texture* tex = (sprite->texture()->_uploadNow())?
				sprite->texture(): _renderer->defaultTexture();
			Box2 region = sprite->tileBox(sc.index());

			VertexBuffer& buff = _defaultBatch.getBuffer(
			            _renderer->defaultShader(),
			            tex, _renderer->spriteFormat());

			Scalar w = tex->width();
			Scalar h = tex->height();
			Transform& wt = sc._entity()->worldTransform;
			buff.addVertex(SpriteVertex{ wt * Vector4(0, h, 0, 1),
			                             Vector4(1, 1, 1, 1),
			                             region.corner(Box2::BottomLeft) });
			buff.addVertex(SpriteVertex{ wt * Vector4(0, 0, 0, 1),
			                             Vector4(1, 1, 1, 1),
			                             region.corner(Box2::TopLeft) });
			buff.addVertex(SpriteVertex{ wt * Vector4(w, h, 0, 1),
			                             Vector4(1, 1, 1, 1),
			                             region.corner(Box2::BottomRight) });
			buff.addVertex(SpriteVertex{ wt * Vector4(w, 0, 0, 1),
			                             Vector4(1, 1, 1, 1),
			                             region.corner(Box2::TopRight) });
			buff.addIndex(index + 0);
			buff.addIndex(index + 1);
			buff.addIndex(index + 2);
			buff.addIndex(index + 2);
			buff.addIndex(index + 1);
			buff.addIndex(index + 3);
			index += 4;
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_defaultBatch.render();
}


void SpriteComponentManager::_addComponentBlock() {
	_components.emplace_back(_componentBlockSize);
	ComponentBlock& block = _components.back();
	std::memset(block.data(), 0, sizeof(SpriteComponent) * block.size());
	for(SpriteComponent& sc: block) {
		sc.setSprite(reinterpret_cast<Sprite*>(&sc + 1));
	}
	block.back().setSprite(reinterpret_cast<Sprite*>(_firstFree));
	_firstFree = &block.front();
}


}
