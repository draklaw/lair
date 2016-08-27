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


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include "lair/ec/collision_component.h"


namespace lair
{


Shape::Shape(ShapeType type, float radius)
	: _type  (type)
	, _radius(radius)
	, _points()
{
}


ShapeSP Shape::createAlignedBox(const Box2& box) {
	ShapeSP shape = std::make_shared<Shape>(SHAPE_ALIGNED_BOX);
	shape->_points.push_back(box.min());
	shape->_points.push_back(box.max());
	return shape;
}


ShapeSP Shape::createCircle(const Vector2& center, float radius) {
	ShapeSP shape = std::make_shared<Shape>(SHAPE_CIRCLE, radius);
	shape->_points.push_back(center);
	return shape;
}



CollisionComponent::CollisionComponent(Manager* manager, _Entity* entity, ShapeSP shape)
	: Component   (manager, entity)
	, _shape      (shape)
	, _penetration()
{
	for(int i = 0; i < N_DIRECTIONS; ++i)
		_penetration[i] = 0;
}



CollisionComponentManager::CollisionComponentManager(size_t componentBlockSize)
	: DenseComponentManager<CollisionComponent>("collision", componentBlockSize)
{
}


CollisionComponent* CollisionComponentManager::addComponentFromJson(
        EntityRef entity, const Json::Value& json, const Path& cd) {
	lairAssert(false);
	return nullptr;
}


CollisionComponent* CollisionComponentManager::cloneComponent(EntityRef base, EntityRef entity) {
	lairAssert(false);
	return nullptr;
}


}
