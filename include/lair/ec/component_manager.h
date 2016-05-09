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


class Component;


class ComponentManager {
public:
	virtual const std::string& name() const = 0;

	virtual Component* addComponentFromJson(EntityRef entity, const Json::Value& json,
	                                  const Path& cd=Path()) = 0;
	virtual Component* cloneComponent(EntityRef base, EntityRef entity) = 0;
	virtual void removeComponent(EntityRef entity) = 0;
};


}


#endif
