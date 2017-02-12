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
#include <lair/core/ldl.h>
#include <lair/core/json.h>
#include <lair/core/log.h>

#include <lair/ec/component.h>
#include <lair/ec/component_manager.h>

#include "lair/ec/entity_manager.h"


namespace lair
{


EntityManager::EntityManager(Logger& logger, LdlPropertySerializer& serializer, size_t entityBlockSize)
    : _logger             (&logger),
      _serializer         (serializer),
      _compManagerMap     (),
      _nEntities          (0),
      _nZombieEntities    (0),
      _entities           (entityBlockSize),
      _firstFree          (nullptr),
      _root               (nullptr) {
	_root = createEntity(EntityRef(), "__root__", EntityRef());
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


ComponentManager* EntityManager::componentManager(const String& name) const {
	auto it = _compManagerMap.find(name);
	return (it != _compManagerMap.end())? it->second: nullptr;
}


EntityRef EntityManager::findByName(const String& name, EntityRef from) const {
	if(!from.isValid())
		from = _root;

	if(from.name() == name)
		return from;

	EntityRef child = from.firstChild();
	while(child.isValid()) {
		EntityRef entity = findByName(name, child);
		if(entity.isValid())
			return entity;
		child = child.nextSibling();
	}

	return EntityRef();
}


EntityRef EntityManager::createEntity(EntityRef parent, const char* name, int index) {
	return createEntity(parent, name, EntityRef(parent._get()->_childBefore(index)));
}


EntityRef EntityManager::createEntity(EntityRef parent, const char* name, EntityRef insertAfter) {
	_Entity* entity = _createDetachedEntity(name);
	if(parent.isValid()) {
		parent._get()->insertChild(entity, insertAfter._get());
	}
	return EntityRef(entity);
}


EntityRef EntityManager::cloneEntity(EntityRef base, EntityRef newParent, const char* name, int index) {
	return cloneEntity(base, newParent, name, EntityRef(newParent._get()->_childBefore(index)));
}


EntityRef EntityManager::cloneEntity(EntityRef base, EntityRef newParent, const char* name, EntityRef insertAfter) {
	lairAssert(base.isValid());

	EntityRef entity = createEntity(newParent, name? name: base.name(), insertAfter);
	entity._get()->flags = base._get()->flags;
	entity.place(base.transform());

	Component* comp = base._get()->firstComponent;
	while(comp) {
		comp->manager()->cloneComponent(base, entity);
		comp = comp->_nextComponent;
	}

	return entity;
}


bool EntityManager::initializeFromLdl(EntityRef entity, LdlParser& parser) {
	if(parser.valueType() != LdlParser::TYPE_MAP) {
		parser.error("Expected entity (VarMap), got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	bool success = true;
	parser.enter();
	while(parser.valueType() != LdlParser::TYPE_END) {
		String key = parser.getKey();

		if(key == "name" && entity.name()[0] == '\0') {
			if(parser.valueType() == LdlParser::TYPE_STRING) {
				setEntityName(entity, parser.getString());
				parser.next();
			}
			else {
				parser.error("Entity name must be a String, got ", parser.valueTypeName());
				parser.skip();
				success = false;
			}
		}
		else if(key == "transform") {
			Transform transform;
			if(ldlRead(parser, transform)) {
				entity.place(transform);
			}
			else {
				success = false;
			}
		}
		else if(key == "children") {
			success &= loadEntitiesFromLdl(parser, entity);
		}
		else {
			ComponentManager* cm = componentManager(key);
			if(cm) {
				Component* cmp = cm->addComponent(entity);
				_serializer._read(parser, cm->componentProperties(), cmp);
			}
			else {
				parser.warning("Unknown component type \"", key, "\"");
				parser.skip();
			}
		}
	}
	parser.leave();

	return success;
}


bool EntityManager::loadEntitiesFromLdl(LdlParser& parser, EntityRef parent) {
	if(parser.valueType() != LdlParser::TYPE_LIST && parser.valueType() != LdlParser::TYPE_MAP) {
		parser.error("Expected entity list (VarList or VarMap), got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	bool isList = parser.valueType() == LdlParser::TYPE_LIST;
	parser.enter();
	while(parser.valueType() != LdlParser::TYPE_END) {
		if(parser.valueType() == LdlParser::TYPE_LIST
		|| parser.valueType() == LdlParser::TYPE_MAP) {
			if(parser.isValueTyped() && parser.getValueTypeName() != "Entity") {
				parser.warning("Unexpected type annotation \"", parser.valueType(), "\" when reading an Entity");
			}
			EntityRef entity = createEntity(parent, isList? nullptr: parser.getKey().c_str());
			initializeFromLdl(entity, parser);
		}
		else {
			parser.error("Expected Entity (VarList or VarMap), got ", parser.valueTypeName());
			parser.skip();
		}
	}
	parser.leave();
	return true;
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


void EntityManager::setEntityName(EntityRef entity, const String& name) {
	size_t nameLen = name.size() + 1;
	std::unique_ptr<char> ownedName(new char[nameLen]);
	std::memcpy(ownedName.get(), name.data(), nameLen);

	delete[] entity._get()->name;
	entity._get()->name = ownedName.release();
}


void EntityManager::moveEntity(EntityRef& entity, EntityRef& newParent, int index) {
	lairAssert(entity.isValid() && newParent.isValid());

	entity._get()->parent->removeChild(entity._get());
	newParent._get()->insertChild(entity._get(), index);
}


void EntityManager::setPrevWorldTransforms() {
	for(_Entity& entity: _entities)
		entity.prevWorldTransform = entity.worldTransform;
}


void EntityManager::updateWorldTransforms() {
	// TODO: Update this algorithm when using homogenous arrays to make it
	// more cache-firendly.

	_updateWorldTransformsHelper(_root._get(), Transform::Identity());
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
	entity->setEnabled(true);
	++_nEntities;
	entity->transform.setIdentity();

	// It is safe to release the unique_pointer now, as the object is alive it
	// will be correctly deleted.
	entity->name = ownedName.release();

	return entity;
}


void EntityManager::_updateWorldTransformsHelper(
        _Entity* entity, const Transform& parentTransform) {
	entity->worldTransform = parentTransform * entity->transform;

	_Entity* child = entity->firstChild;
	while(child) {
		_updateWorldTransformsHelper(child, entity->worldTransform);
		child = child->nextSibling;
	}
}


}

