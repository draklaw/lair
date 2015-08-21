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


#ifndef _LAIR_EC_SPRITE_COMPONENT_H
#define _LAIR_EC_SPRITE_COMPONENT_H


#include <cstring>
#include <vector>
#include <list>

#include <lair/core/lair.h>

#include <lair/render_gl2/gl.h>
#include <lair/render_gl2/batch.h>
#include <lair/render_gl2/renderer.h>

#include <lair/ec/entity.h>
#include <lair/ec/component.h>
#include <lair/ec/component_manager.h>


namespace lair
{


class OrthographicCamera;
class Sprite;

class _Entity;
class EntityManager;

class SpriteComponentManager;


class SpriteComponent : public Component {
public:
	SpriteComponent(_Entity* entity, ComponentManager<SpriteComponent>* manager);
	SpriteComponent(const SpriteComponent&) = delete;
	SpriteComponent(SpriteComponent&&)      = default;
	~SpriteComponent();

	SpriteComponent& operator=(const SpriteComponent&) = delete;
	SpriteComponent& operator=(SpriteComponent&&)      = default;

	inline Sprite* sprite() const { return _sprite; }
	inline void setSprite(Sprite* sprite) {
		_sprite = sprite;
	}

	inline unsigned index() const { return _spriteIndex; }
	inline void setIndex(unsigned index) { _spriteIndex = index; }

	virtual void destroy();

	static inline SpriteComponent*& _getEntityComponent(_Entity* entity) {
		return entity->sprite;
	}

protected:
	SpriteComponentManager* _manager;
	Sprite*   _sprite;
	unsigned  _spriteIndex;
};


class SpriteComponentManager : public ComponentManager<SpriteComponent> {
public:
	SpriteComponentManager(Renderer* renderer,
	                       size_t componentBlockSize = 1024);
	SpriteComponentManager(const SpriteComponentManager&) = delete;
	SpriteComponentManager(SpriteComponentManager&&)      = delete;
	~SpriteComponentManager();

	SpriteComponentManager& operator=(const SpriteComponentManager&) = delete;
	SpriteComponentManager& operator=(SpriteComponentManager&&)      = delete;

	void render(float interp, const OrthographicCamera& camera);

protected:
	Renderer*        _renderer;

	Batch            _defaultBatch;
};


}


#endif
