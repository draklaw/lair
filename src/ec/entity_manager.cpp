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
      _compManagerMap     (),
      _nEntities          (0),
      _nZombieEntities    (0),
      _entities           (entityBlockSize),
      _firstFree          (nullptr),
      _root               (nullptr) {
	_root = createEntity(EntityRef(), "__root__");
}


EntityManager::~EntityManager() {
	_root.release();
}


int EntityManager::registerComponentManager(ComponentManager* cmi) {
	cmi->_setIndex(_compManagers.size());
	_compManagers.push_back(cmi);
	_compManagerMap[cmi->name()] = cmi;
	return cmi->index();
}


EntityRef EntityManager::createEntity(EntityRef parent, const char* name, int index) {
	_Entity* entity = _createDetachedEntity(name);
	if(parent.isValid()) {
		parent._get()->insertChild(entity, index);
	}
	return EntityRef(entity);
}


EntityRef EntityManager::createEntityFromJson(EntityRef parent,
                                              const Json::Value& json,
                                              const Path& cd) {
	return createEntityFromJson(parent, json.get("name", "").asCString(), -1, json, cd);
}


EntityRef EntityManager::createEntityFromJson(EntityRef parent,
                                              const char* name,
                                              const Json::Value& json,
                                              const Path& cd) {
	return createEntityFromJson(parent, json.get("name", "").asCString(), -1, json, cd);
}


EntityRef EntityManager::createEntityFromJson(EntityRef parent,
                                              int index,
                                              const Json::Value& json,
                                              const Path& cd) {
	return createEntityFromJson(parent, json.get("name", "").asCString(), index, json, cd);
}


EntityRef EntityManager::createEntityFromJson(EntityRef parent,
                                              const char* name,
                                              int index,
                                              const Json::Value& json,
                                              const Path& cd) {
	EntityRef entity = createEntity(parent, name, index);
	if(json.isMember("transform")) {
		entity.place(Transform(parseMatrix4(json["transform"])));
	}
	for(const std::string& key: json.getMemberNames()) {
		auto it = _compManagerMap.find(key);
		if(it != _compManagerMap.end()) {
			it->second->addComponentFromJson(entity, json[key], cd);
		}
	}
	return entity;
}


EntityRef EntityManager::cloneEntity(EntityRef base, EntityRef newParent, const char* name, int index) {
	EntityRef entity = createEntity(newParent, name? name: base.name(), index);
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

	entity._get()->parent->removeChild(entity._get());
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


void EntityManager::moveEntity(EntityRef& entity, EntityRef& newParent, int index) {
	lairAssert(entity.isValid() && newParent.isValid());

	entity._get()->parent->removeChild(entity._get());
	newParent._get()->insertChild(entity._get(), index);
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

