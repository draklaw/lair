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


#include <cstring>
#include <cmath>

#include <lair/core/lair.h>
#include <lair/core/json.h>
#include <lair/core/log.h>

#include <lair/ec/component.h>
#include <lair/ec/component_manager.h>

#include "lair/ec/entity_manager.h"


namespace lair
{


EntityManager::EntityManager(Logger& logger, size_t entityBlockSize)
    : _logger             (&logger),
      _compManagers       (),
      _nEntities          (0),
      _nZombieEntities    (0),
      _entities           (entityBlockSize),
      _firstFree          (nullptr),
      _root               (nullptr) {
	_root = createEntity(EntityRef(), "__root__");
}


EntityManager::~EntityManager() {
	_root.release();
//	for(auto nameComp: _compManagers) {
//		nameComp->second->clear();
//	}
}


void EntityManager::registerComponentManager(ComponentManager* cmi) {
	_compManagers[cmi->name()] = cmi;
}


EntityRef EntityManager::createEntity(EntityRef parent, const char* name) {
	_Entity* entity = _createDetachedEntity(name);
	if(parent.isValid()) {
		_addChild(parent._get(), entity);
	}
	return EntityRef(entity);
}


EntityRef EntityManager::createEntityFromJson(EntityRef parent,
                                              const Json::Value& json,
                                              const Path& cd) {
	EntityRef entity = createEntity(parent, json.get("name", "").asCString());
	if(json.isMember("transform")) {
		entity.place(Transform(parseMatrix4(json["transform"])));
	}
	for(const std::string& key: json.getMemberNames()) {
		auto it = _compManagers.find(key);
		if(it != _compManagers.end()) {
			it->second->addComponentFromJson(entity, json[key], cd);
		}
	}
	return entity;
}


EntityRef EntityManager::cloneEntity(EntityRef base, EntityRef newParent, const char* name) {
	EntityRef entity = createEntity(newParent, name? name: base.name());
	entity.place(base.transform());

	Component* comp = base._get()->firstComponent;
	while(comp) {
		comp->manager()->cloneComponent(base, entity);
		comp = comp->_nextComponent;
	}

	return entity;
}


void EntityManager::destroyEntity(EntityRef entity) {
	lairAssert(entity.isValid() && entity != _root);

	while(entity.firstChild().isValid()) {
		destroyEntity(entity.firstChild());
	}

	while(entity._get()->firstComponent) {
		lairAssert(entity._get()->firstComponent->manager());
		entity._get()->firstComponent->manager()->removeComponent(entity);
	}

	_detach(entity._get());
	delete[] entity._get()->name;
	entity._get()->reset();
	--_nEntities;
	++_nZombieEntities;

	if(entity._get()->weakRefCount == 0) {
		_releaseEntity(entity._get());
	}
}


void EntityManager::_releaseEntity(_Entity* entity) {
	lairAssert(entity->weakRefCount == 0);
	entity->nextSibling = _firstFree;
	_firstFree = entity;
	--_nZombieEntities;
}


void EntityManager::moveEntity(EntityRef& entity, EntityRef& newParent) {
	lairAssert(entity.isValid() && newParent.isValid());

	_detach(entity._get());
	_addChild(newParent._get(), entity._get());
}


void EntityManager::updateWorldTransform() {
	// TODO: Update this algorithm when using homogenous arrays to make it
	// more cache-firendly.

	_updateWorldTransformHelper(_root._get(), Transform::Identity());
}


_Entity* EntityManager::_createDetachedEntity(const char* name) {
	// Do this first so there is no side effect in case of bad_alloc.
	std::unique_ptr<char> ownedName;
	if(!name) {
		name = "";
	}
	size_t nameLen = std::strlen(name) + 1;
	ownedName.reset(new char[nameLen]);
	std::memcpy(ownedName.get(), name, nameLen);

	if(!_firstFree) {
		_entities.emplace_back();
		_firstFree = &_entities.back();
		_firstFree->reset();
		_firstFree->manager = this;
		_firstFree->weakRefCount = 0;
	}

	_Entity* entity = _firstFree;
	lairAssert(entity && entity->flags == 0 && entity->weakRefCount == 0);
	_firstFree = entity->nextSibling;
	entity->nextSibling = nullptr;

	entity->setAlive(true);
	++_nEntities;
	entity->transform.setIdentity();

	// It is safe to release the unique_pointer now, as the object is alive it
	// will be correctly deleted.
	entity->name = ownedName.release();

	return entity;
}

void EntityManager::_addChild(_Entity* parent, _Entity* child) {
	lairAssert(!child->parent && !child->nextSibling);
	child->parent = parent;
	if(parent->firstChild) {
		parent->lastChild->nextSibling = child;
	}
	else {
		parent->firstChild = child;
	}
	parent->lastChild = child;
}

void EntityManager::_detach(_Entity* child) {
	lairAssert(child->parent);

	_Entity* prevSibling = nullptr;
	_Entity* e = child->parent->firstChild;
	lairAssert(e);
	while(e != child) {
		prevSibling = e;
		e = e->nextSibling;
		lairAssert(e);
	}

	if(prevSibling) {
		prevSibling->nextSibling = child->nextSibling;
		if(!prevSibling->nextSibling) {
			child->parent->lastChild = prevSibling;
		}
	} else {
		child->parent->firstChild = child->nextSibling;
	}

	child->parent = nullptr;
	child->nextSibling = nullptr;
}

void EntityManager::_updateWorldTransformHelper(
        _Entity* entity, const Transform& parentTransform) {
	entity->prevWorldTransform = entity->worldTransform;
	entity->worldTransform = parentTransform * entity->transform;

	_Entity* child = entity->firstChild;
	while(child) {
		_updateWorldTransformHelper(child, entity->worldTransform);
		child = child->nextSibling;
	}
}


}

