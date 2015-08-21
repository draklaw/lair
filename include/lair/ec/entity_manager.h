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

#include <lair/ec/entity.h>


namespace lair
{


class OrthographicCamera;
class Renderer;

class Component;


class EntityManager {
public:
	EntityManager(Logger& logger, size_t entityBlockSize = 1024);
	EntityManager(const EntityManager&) = delete;
	EntityManager(EntityManager&&)      = delete;
	~EntityManager();

	EntityManager& operator=(const EntityManager&) = delete;
	EntityManager& operator=(EntityManager&&)      = delete;

	inline size_t    nEntities()       const { return _nEntities; }
	inline size_t    nZombieEntities() const { return _nZombieEntities; }
	inline EntityRef root()            const { return _root; }

	size_t entityCapacity() const;

	EntityRef createEntity(EntityRef parent, const char* name = nullptr);
	EntityRef createEntityFromJson(EntityRef parent, const Json::Value& json);
	EntityRef cloneEntity(EntityRef base, EntityRef newParent, const char* name = nullptr);

	// Operates in linear time wrt the number of siblings
	// O(1) if entity is the first child.
	void destroyEntity(EntityRef entity);
	void _releaseEntity(_Entity* entity);

	void moveEntity(EntityRef& entity, EntityRef& newParent);

	void updateWorldTransform();

	Logger& log() { return _logger; }

protected:
	typedef std::vector<_Entity>   EntityBlock;
	typedef std::list<EntityBlock> EntityBlockList;

protected:
	void _addEntityBlock();
	_Entity* _createDetachedEntity(const char* name);
	void _addChild(_Entity* parent, _Entity* child);
	void _detach(_Entity* child);
	void _updateWorldTransformHelper(_Entity* entity, const Transform& parentTransform);

protected:
	Logger          _logger;
	EntityRef       _root;
	_Entity*        _firstFree;
	size_t          _entityBlockSize;
	size_t          _nEntities;
	size_t          _nZombieEntities;
	EntityBlockList _entities;
};


}


#endif
