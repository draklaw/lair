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


#ifndef _LAIR_EC_DENSE_COMPONENT_MANAGER_H
#define _LAIR_EC_DENSE_COMPONENT_MANAGER_H


#include <lair/core/lair.h>

#include <lair/ec/entity.h>
#include <lair/ec/dense_array.h>


namespace lair
{


template < typename _Component >
class DenseComponentManager : public ComponentManager {
public:
	typedef _Component Component;

	typedef DenseArray<Component> ComponentArray;
	typedef typename ComponentArray::Iterator Iterator;

public:
	DenseComponentManager(const std::string& name, size_t componentBlockSize)
	    : _name(name),
	      _nComponents(0),
	      _components(componentBlockSize) {
	}

	DenseComponentManager(const DenseComponentManager&) = delete;
	DenseComponentManager(DenseComponentManager&&)      = delete;

	~DenseComponentManager() {
	}

	DenseComponentManager& operator=(const DenseComponentManager&) = delete;
	DenseComponentManager& operator=(DenseComponentManager&&)      = delete;

	size_t nComponents() const { return _nComponents; }
	size_t nZombies()    const { return _components.size() - _nComponents; }
	size_t capacity()    const { return _components.capacity(); }

	Iterator begin() { return _components.begin(); }
	Iterator end()   { return _components.end(); }

	virtual const std::string& name() const { return _name; }

	Component* getComponent(EntityRef entity) {
		return reinterpret_cast<Component*>(entity._get()->components[Component::INDEX]);
	}
	void _setComponent(_Entity* entity, Component* comp) {
		entity->components[Component::INDEX] = comp;
	}

	Component* addComponent(EntityRef entity) {
		lairAssert(entity.isValid());
		lairAssert(getComponent(entity) == nullptr);

		_components.emplace_back(static_cast<typename Component::Manager*>(this), entity._get());
		Component* comp = &_components.back();
		entity._get()->_addComponent(comp);
		_setComponent(entity._get(), comp);
		++_nComponents;

		return comp;
	}

	void removeComponent(EntityRef entity) {
		lairAssert(entity.isValid());
		Component* comp = getComponent(entity);
		comp->destroy();
		_setComponent(entity._get(), nullptr);
		--_nComponents;
	}

	void compactArray() {
		size_t size = _components.size();
		while(size > 0 && !_components[size - 1]._alive) --size;
		for(size_t i = 0; i < size; ++i) {
			Component* comp = &_components[i];
			if(!comp->_alive) {
				--size;
				std::swap(*comp, _components[size]);
				_setComponent(comp->_entity(), comp);
				comp->_entity()->_updateComponent(&_components[size], comp);
				while(size > 0 && !_components[size - 1]._alive) --size;
			}
		}
		_components.resize(size);
	}

protected:
	std::string      _name;
	size_t           _nComponents;
	ComponentArray   _components;
};


}


#endif
