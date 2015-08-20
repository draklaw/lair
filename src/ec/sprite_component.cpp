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
	            const_cast<Texture*>(sc->texture()));

	sc->_setEntity(entity);
	sc->setTexture(nullptr);

	entity->sprite = sc;
	++_nComponent;
}


void SpriteComponentManager::removeComponent(_Entity* entity) {
	SpriteComponent* sc = entity->sprite;
	lairAssert(sc && sc->_entity() == entity);

	entity->sprite = nullptr;
	--_nComponent;

	sc->_reset();
	sc->setTexture(reinterpret_cast<Texture*>(_firstFree));
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
			Texture* tex = (sc.texture()->_uploadNow())?
				sc.texture(): _renderer->defaultTexture();

			VertexBuffer& buff = _defaultBatch.getBuffer(
			            _renderer->defaultShader(),
			            tex, _renderer->spriteFormat());

			Scalar w = sc.texture()->width();
			Scalar h = sc.texture()->height();
			Transform& wt = sc._entity()->worldTransform;
			buff.addVertex(SpriteVertex{ wt * Vector4(0, 0, 0, 1),
			                             Vector4(1, 1, 1, 1), Vector2(0, 1) });
			buff.addVertex(SpriteVertex{ wt * Vector4(w, 0, 0, 1),
			                             Vector4(1, 1, 1, 1), Vector2(1, 1) });
			buff.addVertex(SpriteVertex{ wt * Vector4(0, h, 0, 1),
			                             Vector4(1, 1, 1, 1), Vector2(0, 0) });
			buff.addVertex(SpriteVertex{ wt * Vector4(w, h, 0, 1),
			                             Vector4(1, 1, 1, 1), Vector2(1, 0) });
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

//	_vertices.clear();
//	_indices.clear();
//	GLuint index = 0;
//	for(ComponentBlock& block: _components) {
//		for(SpriteComponent& sc: block) {
//			if(!sc._entity()) {
//				continue;
//			}
//			bool texOk = sc.texture()->_uploadNow();
//			Scalar w = sc.texture()->width();
//			Scalar h = sc.texture()->height();
//			Transform& wt = sc._entity()->worldTransform;
//			_vertices.insert(_vertices.end(), {
//			    SpriteVertex{ wt * Vector4(0, 0, 0, 1), Vector4(1, 1, 1, 1), Vector2(0, 1) },
//			    SpriteVertex{ wt * Vector4(w, 0, 0, 1), Vector4(1, 1, 1, 1), Vector2(1, 1) },
//			    SpriteVertex{ wt * Vector4(0, h, 0, 1), Vector4(1, 1, 1, 1), Vector2(0, 0) },
//			    SpriteVertex{ wt * Vector4(w, h, 0, 1), Vector4(1, 1, 1, 1), Vector2(1, 0) }
//			});
//			_indices.insert(_indices.end(), {
//			    index + 0, index + 1, index + 2,
//			    index + 2, index + 1, index + 3
//			});
//			index += 4;
//		}
//	}

//	if(!_vertexBuffer) {
//		glGenBuffers(1, &_vertexBuffer);
//	}
//	if(!_indexBuffer) {
//		glGenBuffers(1, &_indexBuffer);
//	}

//	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteVertex) * _vertices.size(),
//	             _vertices.data(), GL_STREAM_DRAW);

//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(),
//	             _indices.data(), GL_STREAM_DRAW);

//	_renderer->spriteFormat()->setup();

//	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

//	_renderer->spriteFormat()->clear();
}


void SpriteComponentManager::_addComponentBlock() {
	_components.emplace_back(_componentBlockSize);
	ComponentBlock& block = _components.back();
	std::memset(block.data(), 0, sizeof(SpriteComponent) * block.size());
	for(SpriteComponent& sc: block) {
		sc.setTexture(reinterpret_cast<Texture*>(&sc + 1));
	}
	block.back().setTexture(reinterpret_cast<Texture*>(_firstFree));
	_firstFree = &block.front();
}


}
