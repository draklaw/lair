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
#include <lair/core/path.h>

#include <lair/ec/entity.h>


namespace lair
{


class ComponentManagerInterface {
public:
	virtual const std::string& name() const = 0;

	virtual void addComponentFromJson(EntityRef entity, const Json::Value& json,
	                                  const Path& cd=Path()) = 0;
	virtual void cloneComponent(EntityRef base, EntityRef entity) = 0;
};


template < typename _Component >
class ComponentManager : public ComponentManagerInterface {
public:
	typedef _Component Component;

public:

	class Iterator {
	public:
		typedef ptrdiff_t  difference_type;
		typedef Component  value_type;
		typedef Component* pointer;
		typedef Component& reference;
		typedef std::random_access_iterator_tag iterator_category;

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
	ComponentManager(const std::string& name, size_t componentBlockSize)
	    : _name(name),
	      _componentBlockSize(componentBlockSize),
	      _nComponents(0),
	      _components() {
	}

	ComponentManager(const ComponentManager&) = delete;
	ComponentManager(ComponentManager&&)      = delete;

	~ComponentManager() {
		while(_nComponents) {
			_removeComponent(_get(_nComponents - 1)->_entity());
		}
		for(Component* block: _components) {
			Eigen::internal::aligned_free(block);
		}
	}

	ComponentManager& operator=(const ComponentManager&) = delete;
	ComponentManager& operator=(ComponentManager&&)      = delete;

	size_t nComponents() const { return _nComponents; }

	Iterator begin() { return Iterator(this, 0); }
	Iterator end()   { return Iterator(this, _nComponents); }

	virtual const std::string& name() const { return _name; }

	void addComponent(EntityRef entity) {
		lairAssert(entity.isValid());
		lairAssert(Component::_getEntityComponent(entity._get()) == nullptr);

		Component* comp = _get(_nComponents);
		new (comp) Component(entity._get(), this);
		entity._get()->_addComponent(comp);
		++_nComponents;

		Component::_getEntityComponent(entity._get()) = comp;
	}

	void removeComponent(EntityRef entity) {
		lairAssert(entity.isValid());
		_removeComponent(entity._get());
	}

	void _removeComponent(_Entity* entity) {
		Component* comp = Component::_getEntityComponent(entity);
		lairAssert(comp && comp->_entity() == entity);

		--_nComponents;
		Component::_getEntityComponent(comp->_entity()) = nullptr;
		entity->_removeComponent(comp);
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
			        Eigen::internal::aligned_malloc(sizeof(Component) * _componentBlockSize)));
		}
		return &_components[block][ci];
	}

protected:
	typedef std::vector<Component*, Eigen::aligned_allocator<Component*>>   ComponentList;

protected:
	std::string      _name;
	size_t           _componentBlockSize;
	size_t           _nComponents;
	ComponentList    _components;
};


template < typename _Component >
class SparseComponentManager : public ComponentManagerInterface {
public:
	typedef _Component Component;

protected:
	typedef std::unordered_map<_Entity const* const, Component,
	        boost::hash<_Entity const* const>, std::equal_to<_Entity const* const>,
	        Eigen::aligned_allocator<std::pair<_Entity const* const, Component>>> ComponentMap;

public:
	typedef typename ComponentMap::iterator Iterator;

public:
	SparseComponentManager(const std::string& name)
	    : _name(name),
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

	virtual const std::string& name() const { return _name; }

	void addComponent(EntityRef& entity) {
		lairAssert(entity.isValid());
		auto it = _components.find(entity._get());

		lairAssert(it == _components.end() || !it->second._alive);

		if(it == _components.end()) {
			it = _components.emplace_hint(it, entity._get(), Component(entity._get(), this));
		} else {
			it->second = Component(entity._get(), this);
		}

		entity._get()->_addComponent(&it->second);
	}

	void removeComponent(EntityRef entity) {
		lairAssert(entity.isValid());
		auto it = _components.find(entity._get());
		lairAssert(it != _components.end());

		entity._get()->_removeComponent(&it->second);
		it->second._alive = false;
//		_components.erase(it);
	}

	Component* get(EntityRef entity) {
		auto it = _components.find(entity._get());
		if(it == _components.end()) {
			return nullptr;
		}
		return &it->second;
	}

	void _collectGarbages() {
		std::vector<typename ComponentMap::iterator> toRemove;
		for(auto it = _components.begin(); it != _components.end(); ++it) {
			if(!it->second._alive) {
				toRemove.push_back(it);
			}
		}
		for(auto it: toRemove) {
			_components.erase(it);
		}
	}

protected:
	std::string    _name;
	ComponentMap   _components;
};


}


#endif
