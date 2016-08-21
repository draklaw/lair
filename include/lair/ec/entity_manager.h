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


#ifndef _LAIR_EC_ENTITY_MANAGER_H
#define _LAIR_EC_ENTITY_MANAGER_H


#include <memory>
#include <list>
#include <vector>
#include <unordered_map>
#include <string>

#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/path.h>
#include <lair/core/loader.h>

#include <lair/ec/dense_array.h>
#include <lair/ec/component_manager.h>
#include <lair/ec/entity.h>
#include <lair/ec/component.h>


namespace lair
{


class OrthographicCamera;
class Renderer;

class Component;
class ComponentManager;


class EntityManager {
public:
	EntityManager(Logger& logger, size_t entityBlockSize = 1024);
	EntityManager(const EntityManager&) = delete;
	EntityManager(EntityManager&&)      = delete;
	~EntityManager();

	EntityManager& operator=(const EntityManager&) = delete;
	EntityManager& operator=(EntityManager&&)      = delete;

	inline size_t    nEntities()       const { return _nEntities; }
	inline size_t    entityCapacity()  const { return _entities.capacity(); }
	inline size_t    nZombieEntities() const { return _nZombieEntities; }
	inline EntityRef root()            const { return _root; }

	int registerComponentManager(ComponentManager* cmi);

	EntityRef createEntity(EntityRef parent, const char* name = nullptr, int index = -1);
	EntityRef createEntity(EntityRef parent, const char* name, EntityRef insertAfter);
	EntityRef cloneEntity(EntityRef base, EntityRef newParent, const char* name = nullptr, int index = -1);
	EntityRef cloneEntity(EntityRef base, EntityRef newParent, const char* name, EntityRef insertAfter);

	void initializeFromJson(EntityRef entity, const Json::Value& json, const Path& cd=Path());

	// Operates in linear time wrt the number of siblings
	// O(1) if entity is the first child.
	void destroyEntity(EntityRef entity);
	void _releaseEntity(_Entity* entity);

	void moveEntity(EntityRef& entity, EntityRef& newParent, int index = -1);
	void moveEntity(EntityRef& entity, EntityRef& newParent, EntityRef insertAfter);

	void updateWorldTransform();

	Logger& log() { return _logger; }

protected:
	typedef DenseArray<_Entity> EntityArray;
	typedef std::vector<ComponentManager*> CompManagerArray;
	typedef std::unordered_map<std::string, ComponentManager*> CompManagerMap;

protected:
	_Entity* _createDetachedEntity(const char* name);
	void _updateWorldTransformHelper(_Entity* entity, const Transform& parentTransform);

protected:
	Logger           _logger;

	CompManagerArray _compManagers;
	CompManagerMap   _compManagerMap;

	size_t           _nEntities;
	size_t           _nZombieEntities;
	EntityArray      _entities;
	_Entity*         _firstFree;

	EntityRef        _root;
};


}


#endif
