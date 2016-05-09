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


#ifndef _LAIR_EC_SPARSE_COMPONENT_MANAGER_H
#define _LAIR_EC_SPARSE_COMPONENT_MANAGER_H


#include <lair/core/lair.h>


namespace lair
{


template < typename _Component >
class SparseComponentManager : public ComponentManager {
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

	Component* addComponent(EntityRef& entity) {
		lairAssert(entity.isValid());
		dbgLogger.debug(name(), ": add component: ", entity._get(), " (", entity.name(), ")");

		auto it = _components.find(entity._get());

		lairAssert(it == _components.end() || !it->second._alive);

		if(it == _components.end()) {
			it = _components.emplace_hint(it, entity._get(),
			            Component(static_cast<typename Component::Manager*>(this), entity._get()));
		} else {
			it->second = Component(static_cast<typename Component::Manager*>(this), entity._get());
		}

		entity._get()->_addComponent(&it->second);
		return &it->second;
	}

	void removeComponent(EntityRef entity) {
		lairAssert(entity.isValid());
		dbgLogger.debug(name(), ": remove component: ", entity._get(), " (", entity.name(), ")");
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
