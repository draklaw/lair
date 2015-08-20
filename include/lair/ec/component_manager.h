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


#ifndef _LAIR_EC_COMPONENT_MANAGER_H
#define _LAIR_EC_COMPONENT_MANAGER_H


#include <memory>
#include <vector>

#include <lair/core/lair.h>


namespace lair
{


class _Entity;
class EntityManager;


template < typename _Component >
class ComponentManager {
public:
	typedef _Component Component;

public:
	ComponentManager(EntityManager* manager, size_t componentBlockSize)
	    : _manager(manager),
	      _componentBlockSize(componentBlockSize),
	      _nComponents(0),
	      _components() {
	}

	ComponentManager(const ComponentManager&) = delete;
	ComponentManager(ComponentManager&&)      = delete;
	~ComponentManager() {
		while(_nComponents) {
			removeComponent(_get(_nComponents - 1)->_entity());
		}
		for(Component* block: _components) {
			free(block);
		}
	}

	ComponentManager& operator=(const ComponentManager&) = delete;
	ComponentManager& operator=(ComponentManager&&)      = delete;

	size_t nComponents() const { return _nComponents; }

	void addComponent(_Entity* entity) {
		Component* comp = _get(_nComponents);
		new (comp) Component(entity);
		++_nComponents;

		Component::_getEntityComponent(entity) = comp;
	}

	void removeComponent(_Entity* entity) {
		Component* comp = Component::_getEntityComponent(entity);
		lairAssert(comp && comp->_entity() == entity);

		--_nComponents;
		comp->~Component();

		*comp = std::move(*_get(_nComponents));
		Component::_getEntityComponent(comp->_entity()) = comp;
	}

protected:
	typedef std::vector<Component*>   ComponentList;

protected:
	Component* _get(size_t index) {
		unsigned block = index / _componentBlockSize;
		unsigned ci    = index % _componentBlockSize;
		if(block == _components.size()) {
			_components.emplace_back(reinterpret_cast<Component*>(
			        std::malloc(sizeof(Component) * _componentBlockSize)));
		}
		return &_components[block][ci];
	}

protected:
	EntityManager*   _manager;

	size_t           _componentBlockSize;
	size_t           _nComponents;
	ComponentList    _components;
};


}


#endif
