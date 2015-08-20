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
#include <unordered_map>

#include <lair/core/lair.h>


namespace lair
{


class _Entity;
class EntityManager;


template < typename _Component >
class ComponentManager {
public:
	typedef _Component Component;

	typedef ptrdiff_t  difference_type;
	typedef Component  value_type;
	typedef Component* pointer;
	typedef Component& reference;
	typedef std::random_access_iterator_tag iterator_category;

public:

	class Iterator {
	public:
		Iterator(ComponentManager<Component>* manager, unsigned index)
		    : _manager(manager),
		      _index(index) {
		}

		bool operator==(const Iterator& other) const {
			return _manager == other._manager && _index == other._index;
		}
		bool operator!=(const Iterator& other) const {
			return !(*this == other);
		}

		bool operator<(const Iterator& other) const {
			return _manager < other._manager
			    || (_manager == other._manager && _index < other._index);
		}
		bool operator<=(const Iterator& other) const {
			return *this < other || *this == other;
		}
		bool operator>(const Iterator& other) const {
			return !(*this <= other);
		}
		bool operator>=(const Iterator& other) const {
			return !(*this < other);
		}

		Iterator& operator++() {
			++_index;
			return *this;
		}
		Iterator operator++(int) {
			Iterator tmp(*this);
			++_index;
			return tmp;
		}

		Iterator& operator--() {
			--_index;
			return *this;
		}
		Iterator operator--(int) {
			Iterator tmp(*this);
			--_index;
			return tmp;
		}

		Iterator& operator+=(ptrdiff_t n) {
			_index += n;
			return *this;
		}

		Iterator& operator-=(ptrdiff_t n) {
			_index -= n;
			return *this;
		}

		friend Iterator& operator+(Iterator it, ptrdiff_t n) {
			it += n;
			return it;
		}
		friend Iterator& operator+(ptrdiff_t n, Iterator it) {
			it += n;
			return it;
		}

		friend Iterator& operator-(Iterator it, ptrdiff_t n) {
			it -= n;
			return it;
		}
		friend Iterator& operator-(ptrdiff_t n, Iterator it) {
			it -= n;
			return it;
		}

		ptrdiff_t operator-(const Iterator& other) {
			return _index - other._index;
		}

		const Component& operator*() const {
			lairAssert(_index < _manager->nComponents());
			return *_manager->_get(_index);
		}
		Component& operator*() {
			lairAssert(_index < _manager->nComponents());
			return *_manager->_get(_index);
		}

		const Component& operator[](size_t subIndex) const {
			lairAssert(_index + subIndex < _manager->nComponents());
			return *_manager->_get(_index + subIndex);
		}
		Component& operator[](size_t subIndex) {
			lairAssert(_index + subIndex < _manager->nComponents());
			return *_manager->_get(_index + subIndex);
		}

	protected:
		ComponentManager<Component>* _manager;
		unsigned                     _index;
	};

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

	Iterator begin() { return Iterator(this, 0); }
	Iterator end()   { return Iterator(this, _nComponents); }

	void addComponent(_Entity* entity) {
		lairAssert(Component::_getEntityComponent(entity) == nullptr);

		Component* comp = _get(_nComponents);
		new (comp) Component(entity);
		++_nComponents;

		Component::_getEntityComponent(entity) = comp;
	}

	void removeComponent(_Entity* entity) {
		Component* comp = Component::_getEntityComponent(entity);
		lairAssert(comp && comp->_entity() == entity);

		--_nComponents;
		Component::_getEntityComponent(comp->_entity()) = nullptr;
		comp->~Component();

		Component* last = _get(_nComponents);
		if(comp != last) {
			*comp = std::move(*last);
		}
		Component::_getEntityComponent(comp->_entity()) = comp;
	}

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
	typedef std::vector<Component*>   ComponentList;

protected:
	EntityManager*   _manager;

	size_t           _componentBlockSize;
	size_t           _nComponents;
	ComponentList    _components;
};


template < typename _Component >
class SparseComponentManager {
public:
	typedef _Component Component;

protected:
	typedef std::unordered_map<_Entity*, Component> ComponentMap;

public:
	typedef typename ComponentMap::iterator Iterator;

public:
	SparseComponentManager(EntityManager* manager)
	    : _manager(manager),
	      _components() {
	}

	SparseComponentManager(const SparseComponentManager&) = delete;
	SparseComponentManager(SparseComponentManager&&)      = delete;

	~SparseComponentManager() {
	}

	SparseComponentManager& operator=(const SparseComponentManager&) = delete;
	SparseComponentManager& operator=(SparseComponentManager&&)      = delete;

	size_t nComponent() const { return _components.size(); }

	Iterator begin() { return _components.begin(); }
	Iterator end()   { return _components.end(); }

	void addComponent(_Entity* entity) {
		auto it = _components.find(entity);
		lairAssert(it == _components.end());

		it = _components.emplace_hint(it, entity, Component(entity));
	}

	void removeComponent(_Entity* entity) {
		auto it = _components.find(entity);
		lairAssert(it != _components.end());

		_components.erase(it);
	}

	Component* get(_Entity* entity) {
		auto it = _components.find(entity);
		if(it == _components.end()) {
			return nullptr;
		}
		return &it->second;
	}


protected:
	EntityManager* _manager;
	ComponentMap   _components;
};


}


#endif
