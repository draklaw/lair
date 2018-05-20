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

#include "lair/ec/component.h"
#include "lair/ec/entity_manager.h"

#include "lair/ec/entity.h"


namespace lair
{


_Entity* _Entity::_childBefore(int index) {
	lairAssert(index <= int(nChildren));

	if(index < 0 || index == int(nChildren)) {
		return lastChild;
	}

	_Entity* prev = nullptr;
	_Entity* e    = firstChild;
	for(int i = 0; i < index; ++i) {
		lairAssert(e);
		prev = e;
		e    = e->nextSibling;
	}
	return prev;
}


void _Entity::insertChild(_Entity* child, int index) {
	insertChild(child, _childBefore(index));
}


void _Entity::insertChild(_Entity* child, _Entity* previousSibling) {
	lairAssert(!child->parent && !child->nextSibling);
	lairAssert(!previousSibling || previousSibling->parent == this);

#ifndef NDEBUG
	{
		_Entity* e = this;
		while(e) {
			lairAssert(e != child);
			e = e->parent;
		}
	}
#endif

	child->parent = this;

	if(!firstChild) {
		lairAssert(!previousSibling);
		firstChild = child;
		lastChild  = child;
	}
	else if(!previousSibling) {
		child->nextSibling = firstChild;
		firstChild         = child;
	}
	else if(!previousSibling->nextSibling) {
		lastChild->nextSibling = child;
		lastChild              = child;
	}
	else {
		child->nextSibling           = previousSibling->nextSibling;
		previousSibling->nextSibling = child;
	}

	++nChildren;
}


void _Entity::removeChild(_Entity* child) {
	lairAssert(child->parent == this);

	--nChildren;

	_Entity* prev = nullptr;
	_Entity* e    = child->parent->firstChild;
	lairAssert(e);
	while(e != child) {
		prev = e;
		e    = e->nextSibling;
		lairAssert(e);
	}

	if(prev) {
		prev->nextSibling = child->nextSibling;
		if(!prev->nextSibling) {
			lastChild = prev;
		}
	} else {
		firstChild = child->nextSibling;
		if(!firstChild)
			lastChild = nullptr;
	}

	child->parent      = nullptr;
	child->nextSibling = nullptr;
}


void _Entity::_addComponent(Component* comp) {
	comp->_nextComponent = firstComponent;
	firstComponent = comp;
}


void _Entity::_removeComponent(Component* comp) {
	lairAssert(comp);
	if(firstComponent == comp) {
		firstComponent = comp->_nextComponent;
	} else {
		Component* prev = firstComponent;
		while(prev && prev->_nextComponent != comp) prev = prev->_nextComponent;
		lairAssert(prev);
		prev->_nextComponent = comp->_nextComponent;
		comp->_nextComponent = nullptr;
	}
}


void _Entity::_updateComponent(Component* from, Component* to) {
	lairAssert(from && to);
	if(firstComponent == from) {
		firstComponent = to;
	} else {
		Component* prev = firstComponent;
		while(prev && prev->_nextComponent != from) prev = prev->_nextComponent;
		lairAssert(prev);
		prev->_nextComponent = to;
	}
}


size_t _Entity::_countComponents() const {
	size_t count = 0;
	Component* comp = firstComponent;
	while(comp) {
		++count;
		comp = comp->_nextComponent;
	}
	return count;
}


bool _Entity::_hasComponent(Component* target) const {
	Component* comp = firstComponent;
	while(comp) {
		if(comp == target)
			return true;
		comp = comp->_nextComponent;
	}
	return false;
}



bool EntityRef::isEnabledRec() const {
	return isEnabled() && (parent().isValid()? parent().isEnabledRec(): true);
}


void EntityRef::release() {
	if(_entity) {
		lairAssert(_entity->weakRefCount != 0);
		--_entity->weakRefCount;
		if((!_entity->isAlive() || !_entity->parent)
		&& _entity->weakRefCount == 0) {
			_entity->manager->_releaseEntity(_entity);
		}
		_entity = nullptr;
	}
}


void EntityRef::destroy() {
	if(isValid()) {
		_entity->manager->destroyEntity(*this);
	}
	release();
}


void _updateWorldTransformsHelper(_Entity* entity, const Transform& parentTransform) {
	entity->worldTransform = parentTransform * entity->transform;

	_Entity* child = entity->firstChild;
	while(child) {
		_updateWorldTransformsHelper(child, entity->worldTransform);
		child = child->nextSibling;
	}
}


void EntityRef::updateWorldTransformRec() {
	updateWorldTransform();
	_updateWorldTransformsHelper(_entity, Transform::Identity());
}


void _setPrevWorldTransformsHelper(_Entity* entity, const Transform& parentTransform) {
	entity->worldTransform = parentTransform * entity->transform;
	entity->prevWorldTransform = entity->worldTransform;

	_Entity* child = entity->firstChild;
	while(child) {
		_setPrevWorldTransformsHelper(child, entity->worldTransform);
		child = child->nextSibling;
	}
}


void EntityRef::setPrevWorldTransformRec() {
	updateWorldTransform();
	_setPrevWorldTransformsHelper(_entity, Transform::Identity());
}


EntityRef EntityRef::clone(EntityRef newParent, const char* newName) const {
	lairAssert(isValid());
	EntityRef entity = _entity->manager->cloneEntity(*this, newParent, newName);
	return entity;
}


}
