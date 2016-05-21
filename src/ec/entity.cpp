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


EntityRef EntityRef::clone(EntityRef newParent, const char* newName) const {
	lairAssert(isValid());
	EntityRef entity = _entity->manager->cloneEntity(*this, newParent, newName);
	return entity;
}


}
