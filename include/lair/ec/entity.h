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


#ifndef _LAIR_EC_ENTITY_H
#define _LAIR_EC_ENTITY_H


#include <utility>
#include <memory>

#include <lair/core/lair.h>

#include <lair/ec/component.h>


namespace lair
{


class EntityManager;

class SpriteComponent;


struct _Entity {
public:
	enum {
		Alive = 0x01
	};

public:
	inline _Entity() {}
	_Entity(const _Entity&) = delete;
	_Entity(_Entity&&)      = delete;
	~_Entity() {
		delete[] name;
	}

	_Entity& operator=(const _Entity&) = delete;
	_Entity& operator==(_Entity&&)     = delete;

	inline bool isAlive() const {
		return flags & Alive;
	}

	inline void setAlive() {
		flags |= Alive;
	}

	inline void reset() {
		delete[] name;
		// Erase everything from the field flag
		std::memset(&flags, 0,
		            sizeof(_Entity) - ptrdiff_t(offsetof(_Entity, flags)));
	}

	void _addComponent(Component* comp);
	void _removeComponent(Component* comp);

public:
	uint32         weakRefCount;
	EntityManager* manager;
	uint32         flags;

	_Entity*       parent;
	_Entity*       firstChild;
	_Entity*       nextSibling;

	char*          name;

	// TODO: make homogenous arrays for these (managed by EntityManager)
	Transform      transform;
	Transform      worldTransform;
	Transform      prevWorldTransform;
//	Transform*     transform;
//	Transform*     worldTransform;

	// Components form a single linked list for efficient iteration.
	Component*     firstComponent;

	/* Components here ! */
	SpriteComponent* sprite;
};


class EntityRef {
public:
	explicit inline EntityRef(_Entity* entity = nullptr)
	    : _entity(entity) {
		if(_entity) {
			++_entity->weakRefCount;
		}
	}

	inline EntityRef(const EntityRef& rhs)
	    : _entity(rhs._entity) {
		if(_entity) {
			++_entity->weakRefCount;
		}
	}

	inline EntityRef(EntityRef&& rhs)
	    : _entity(rhs._entity) {
		rhs._entity = nullptr;
	}

	~EntityRef() {
		release();
	}

	EntityRef& operator=(EntityRef rhs) {
		swap(*this, rhs);
		return *this;
	}

	inline bool operator==(const EntityRef& other) const {
		return _entity == other._entity;
	}

	inline bool operator!=(const EntityRef& other) const {
		return !(*this == other);
	}

	inline bool isValid() const {
		return _entity && _entity->isAlive();
	}

	void release();

	inline friend void swap(EntityRef& er0, EntityRef& er1) {
		std::swap(er0._entity, er1._entity);
	}

	inline const char*      name()           const {
		lairAssert(isValid());
		return _entity->name;
	}

	inline       EntityRef  parent()         const {
		lairAssert(isValid());
		return EntityRef(_entity->parent);
	}

	inline       EntityRef  firstChild()     const {
		lairAssert(isValid());
		return EntityRef(_entity->firstChild);
	}

	inline       EntityRef  nextSibling()    const {
		lairAssert(isValid());
		return EntityRef(_entity->nextSibling);
	}

	inline const Transform& transform()      const {
		lairAssert(isValid());
//		lairAssert(_entity->transform);
		return /* * */_entity->transform;
	}

	inline const Transform& worldTransform() const {
		lairAssert(isValid());
//		lairAssert(_entity->worldTransform);
		return /* * */_entity->worldTransform;
	}

	inline void setTransform(const Transform& transform) {
		lairAssert(isValid());
//		lairAssert(_entity->transform);
		/* * */_entity->transform = transform;
	}

	EntityRef clone(EntityRef newParent, const char* newName = nullptr);

	inline SpriteComponent* sprite() {
		return _entity->sprite;
	}

	inline _Entity* _get() {
		return _entity;
	}

private:
	// These fields are "public", but really should not be accessed by anything
	// which is outside this module.
	_Entity* _entity;
};


}


#endif
