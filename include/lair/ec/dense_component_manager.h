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
#include <lair/core/property.h>
#include <lair/core/block_array.h>

#include <lair/ec/entity.h>
#include <lair/ec/component_manager.h>


namespace lair
{


template < typename _Component >
class DenseComponentManager : public ComponentManager {
public:
	typedef _Component Component;
	typedef DenseComponentManager<Component> Self;

	typedef BlockArray<Component> ComponentArray;
	typedef std::vector<size_t> SortBuffer;

	class Iterator {
	public:
		typedef ptrdiff_t                 difference_type;
		typedef Component&                value_type;
		typedef Component*                pointer_type;
		typedef Component&                reference_type;
		typedef std::forward_iterator_tag iterator_category;

	public:
		inline Iterator(Self* self, size_t index)
			: _self(self), _index(index) {
			_skipDestroyed();
		}

		inline bool operator==(Iterator other) const {
			return _self == other._self && _index == other._index;
		}
		inline bool operator!=(Iterator other) const {
			return !(*this == other);
		}

		Iterator& operator++() {
			if(_index < _self->_components.size()) {
				++_index;
				_skipDestroyed();
			}
			return *this;
		}
		Iterator operator++(int) {
			Iterator tmp(*this);
			++(*this);
			return tmp;
		}

		const Component& operator*() const {
			return _self->_components.at(_index);
		}
		Component& operator*() {
			return _self->_components.at(_index);
		}

		const Component* operator->() const {
			return &_self->_components.at(_index);
		}
		Component* operator->() {
			return &_self->_components.at(_index);
		}

	private:
		void _skipDestroyed() {
			while(_index < _self->_components.size()
			   && !_self->_components[_index].isAlive()) {
				++_index;
			}
		}

		Self*  _self;
		size_t _index;
	};

	friend class Iterator;

public:
	DenseComponentManager(const std::string& name, size_t componentBlockSize)
	    : ComponentManager(name),
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

	Iterator begin() { return Iterator(this, 0); }
	Iterator end()   { return Iterator(this, _components.size()); }

	virtual const std::string& name() const { return _name; }

	Component* get(EntityRef entity) {
		lairAssert(_index >= 0);

		if(_index < LAIR_EC_MAX_DENSE_COMPONENTS) {
			return reinterpret_cast<Component*>(entity._get()->components[_index]);
		}

		auto it = _componentMap.find(entity._get());
		if(it == _componentMap.end()) {
			return nullptr;
		}
		return it->second;
	}
	void _setComponent(_Entity* entity, Component* comp) {
		lairAssert(_index >= 0);

		if(_index < LAIR_EC_MAX_DENSE_COMPONENTS) {
			entity->components[_index] = comp;
		}
		else {
			if(comp) {
				_componentMap[entity] = comp;
			}
			else {
				_componentMap.erase(entity);
			}
		}
	}

	virtual Component* addComponent(EntityRef entity) {
		lairAssert(_index >= 0);
		lairAssert(entity.isValid());
		lairAssert(get(entity) == nullptr);

		_components.emplace_back(static_cast<typename Component::Manager*>(this), entity._get());
		Component* comp = &_components.back();
		entity._get()->_addComponent(comp);
		_setComponent(entity._get(), comp);
		++_nComponents;

		return comp;
	}

	Component* _addComponent(EntityRef entity, Component* base) {
		lairAssert(base && base->isAlive());
		Component* comp = addComponent(entity);
		comp->_flags = base->_flags;
		return comp;
	}

	void removeComponent(EntityRef entity) {
		lairAssert(_index >= 0);
		lairAssert(entity.isValid());
		Component* comp = get(entity);
		lairAssert(comp->isAlive());
		comp->destroy();
		_setComponent(entity._get(), nullptr);
		--_nComponents;
	}

	void compactArray() {
		size_t size = _components.size();
		while(size > 0 && !_components[size - 1].isAlive()) --size;
		for(size_t i = 0; i < size; ++i) {
			Component* comp = &_components[i];
			if(!comp->isAlive()) {
				--size;
				std::swap(*comp, _components[size]);
				_setComponent(comp->_entity(), comp);
				comp->_entity()->_updateComponent(&_components[size], comp);
				while(size > 0 && !_components[size - 1].isAlive()) --size;
			}
		}
		_components.resize(size);
	}

	template < typename Cmp >
	void sortArray(const Cmp& cmp = Cmp()) {
		size_t size = _components.size();
		if(size == 0)
			return;

		_sortBuffer.resize(size);
		for(size_t i = 0; i < size; ++i)
			_sortBuffer[i] = i;

		CmpAdapter<Cmp> cmpAdapter(this, cmp);
		std::sort(_sortBuffer.begin(), _sortBuffer.end(), cmpAdapter);

		size_t lastAlive = 0;
		for(size_t i0 = 0; i0 < size-1; ++i0) {
			size_t i1 = _sortBuffer[i0];
			if(i1 < i0) {
				i1 = _sortBuffer[i1];
				_sortBuffer[i0] = i1;
			}
			i1 = (i1 < i0)? _sortBuffer[i1]: i1;

			Component* c0 = &_components[i0];
			Component* c1 = &_components[i1];

			std::swap(*c0, *c1);
			if(c0->isAlive()) {
				_setComponent(c0->_entity(), c0);
				c0->_entity()->_updateComponent(c1, c0);
				lastAlive = i0;
			}
			if(c1->isAlive()) {
				_setComponent(c1->_entity(), c1);
				c1->_entity()->_updateComponent(c0, c1);
			}
		}
		while(lastAlive < size && _components[lastAlive].isAlive()) ++lastAlive;
		_components.resize(lastAlive);
	}

	virtual const PropertyList& componentProperties() {
		return Component::properties();
	}

	virtual Component* cloneComponent(EntityRef base, EntityRef entity) {
		Component* baseComp = get(base);
		Component* comp = _addComponent(entity, baseComp);

		const PropertyList& props = baseComp->properties();
		for(unsigned pi = 0; pi < props.nProperties(); ++pi) {
			const Property& prop = props.property(pi);
			prop.setVar(comp, prop.getVar(baseComp));
		}

		return comp;
	}


protected:
	friend struct CmpAdapter;

	template < typename Cmp >
	struct CmpAdapter {
		inline CmpAdapter(Self* self, const Cmp& cmp) : self(self), cmp(cmp) {}
		inline bool operator()(size_t i0, size_t i1) const {
			Component* c0 = &self->_components[i0];
			Component* c1 = &self->_components[i1];
			if(c0->isAlive() && c1->isAlive()) {
				return cmp(&self->_components[i0], &self->_components[i1]);
			}
			return c0->isAlive();
		}

		Self*      self;
		const Cmp& cmp;
	};

	typedef std::unordered_map<_Entity const*, Component*> ComponentMap;

protected:
	size_t           _nComponents;
	ComponentArray   _components;
	SortBuffer       _sortBuffer;
	ComponentMap     _componentMap;
};


}


#endif
