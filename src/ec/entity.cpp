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

#include "lair/ec/entity.h"
#include "lair/ec/entity_manager.h"


namespace lair
{


void _Entity::_addComponent(Component* comp) {
	comp->_nextComponent = firstComponent;
	firstComponent = comp;
}


void _Entity::_removeComponent(Component* comp) {
	if(firstComponent == comp) {
		firstComponent = comp->_nextComponent;
	} else {
		Component* prev = firstComponent;
		while(prev && prev->_nextComponent != comp) prev = prev->_nextComponent;
		lairAssert(prev);
		prev->_nextComponent = prev->_nextComponent->_nextComponent;
	}
}


void EntityRef::release() {
	if(_entity) {
		--_entity->weakRefCount;
		if(!_entity->isAlive() && _entity->weakRefCount == 0) {
			_entity->manager->_releaseEntity(_entity);
		}
		_entity = nullptr;
	}
}


}
