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
#include <lair/core/log.h>


#ifndef LAIR_EC_MAX_DENSE_COMPONENTS
#define LAIR_EC_MAX_DENSE_COMPONENTS 4
#endif


namespace lair
{


class Component;
class EntityManager;


constexpr size_t MAX_DENSE_COMPONENTS = LAIR_EC_MAX_DENSE_COMPONENTS;

class _Entity {
public:
	enum {
		Alive   = 1 << 0,
		Enabled = 1 << 1,
	};

public:
	// Constructor and destructor do nothing: EntityManager deal with it.
	inline _Entity() = default;
	_Entity(const _Entity&) = delete;
	_Entity(_Entity&&)      = delete;
	~_Entity()              = default;

	_Entity& operator=(const _Entity&) = delete;
	_Entity& operator==(_Entity&&)     = delete;

	inline bool isAlive() const {
		return bitsEnabled(flags, Alive);
	}

	inline void setAlive(bool alive) {
		flags = setBits(flags, Alive, alive);
	}

	inline bool isEnabled() const {
		return bitsEnabled(flags, Alive | Enabled);
	}

	inline void setEnabled(bool enabled) {
		flags = setBits(flags, Enabled, enabled);
	}

	inline void reset() {
		// Erase everything from the field flags
		std::memset(&flags, 0,
		            sizeof(_Entity) - ptrdiff_t(offsetof(_Entity, flags)));
	}

	_Entity* _childBefore(int index);

	void insertChild(_Entity* child, int index);
	void insertChild(_Entity* child, _Entity* previousSibling);
	void removeChild(_Entity* child);

	void _addComponent(Component* comp);
	void _removeComponent(Component* comp);
	void _updateComponent(Component* from, Component* to);

	// For testing and debugging
	size_t _countComponents() const;
	bool _hasComponent(Component* target) const;

public:
	EntityManager* manager;
	uint32         weakRefCount;
	uint32         flags;

	unsigned       nChildren;
	_Entity*       parent;
	_Entity*       firstChild;
	_Entity*       lastChild;
	_Entity*       nextSibling;

	char*          name;
//	Json::Value    extra;

	// TODO: make homogenous arrays for these (managed by EntityManager)
	Transform      transform;
	Transform      worldTransform;
	Transform      prevWorldTransform;
//	Transform*     transform;
//	Transform*     worldTransform;

	// Components form a single linked list for efficient iteration.
	Component*     firstComponent;

	/* Components here ! */
	Component*     components[MAX_DENSE_COMPONENTS];
};


class EntityRef {
public:
	inline EntityRef()
	    : _entity(nullptr) {
	}

	explicit inline EntityRef(_Entity* entity)
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

	inline bool isEnabled() const {
		lairAssert(isValid());
		return _entity->isEnabled();
	}

	bool isEnabledRec() const;

	inline void setEnabled(bool enabled) {
		lairAssert(isValid());
		_entity->setEnabled(enabled);
	}

	void release();
	void destroy();

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

	inline       EntityRef  lastChild()     const {
		lairAssert(isValid());
		return EntityRef(_entity->lastChild);
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

	inline       Transform& transform() {
		lairAssert(isValid());
//		lairAssert(_entity->transform);
		return /* * */_entity->transform;
	}

	inline const Transform& worldTransform() const {
		lairAssert(isValid());
//		lairAssert(_entity->worldTransform);
		return /* * */_entity->worldTransform;
	}

	inline Vector2 interpPosition2(float interp) const {
		lairAssert(isValid());
		return lerp(interp, _entity->prevWorldTransform.matrix().block<2, 1>(0, 3).eval(),
		                    _entity->worldTransform    .matrix().block<2, 1>(0, 3).eval());
	}

	inline Vector3 interpPosition3(float interp) const {
		lairAssert(isValid());
		return lerp(interp, _entity->prevWorldTransform.matrix().block<3, 1>(0, 3).eval(),
		                    _entity->worldTransform    .matrix().block<3, 1>(0, 3).eval());
	}

	inline Matrix4 interpMatrix(float interp) const {
		lairAssert(isValid());
		return lerp(interp, _entity->prevWorldTransform.matrix(),
		                    _entity->worldTransform    .matrix());
	}

	inline Transform interpTransform(float interp) const {
		lairAssert(isValid());
		return Transform(interpMatrix(interp));
	}

	inline Vector2 position2() const {
		return transform().matrix().block<2, 1>(0, 3);
	}

	inline Vector3 position3() const {
		return transform().matrix().block<3, 1>(0, 3);
	}

	inline Transform computeWorldTransform() const {
		if(parent().isValid()) {
			return parent().computeWorldTransform() * _entity->transform;
		} else {
			return _entity->transform;
		}
	}

	inline void updateWorldTransform() {
		_entity->worldTransform = computeWorldTransform();
	}

	void updateWorldTransformRec();

	inline void setPrevWorldTransform() {
		updateWorldTransform();
		_entity->prevWorldTransform = _entity->worldTransform;
	}

	void setPrevWorldTransformRec();

	inline void place(const Transform& transform) {
		lairAssert(isValid());
//		lairAssert(_entity->transform);
		/* * */_entity->transform = transform;
		setPrevWorldTransform();
	}

	inline void placeAt(const Vector3& pos) {
		lairAssert(isValid());
		moveTo(pos);
		setPrevWorldTransform();
	}

	inline void placeAt(const Vector2& pos) {
		lairAssert(isValid());
		moveTo(pos);
		setPrevWorldTransform();
	}

	inline void moveTo(const Transform& transform) {
		lairAssert(isValid());
//		lairAssert(_entity->transform);
		/* * */_entity->transform = transform;
	}

	inline void moveTo(const Vector3& pos) {
		lairAssert(isValid());
		_entity->transform.translation() = pos;
	}

	inline void moveTo(const Vector2& pos) {
		lairAssert(isValid());
		_entity->transform.translation().head<2>() = pos;
	}

	inline void translate(const Vector2& trans) {
		Vector3 t2;
		t2 << trans, 0;
		translate(t2);
	}

	inline void translate(const Vector3& trans) {
		transform().translate(trans);
	}

	inline void translate(float x, float y, float z = 0) {
		translate(Vector3(x, y, z));
	}

//	Json::Value& extra() {
//		lairAssert(isValid());
//		return _entity->extra;
//	}

	inline void destroyChildren() {
		while(firstChild().isValid()) {
			firstChild().destroy();
		}
	}

	EntityRef clone(EntityRef newParent, const char* newName = nullptr) const;

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
