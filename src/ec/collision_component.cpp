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
#include <lair/core/json.h>

#include "lair/ec/collision_component.h"


namespace lair
{


Shape::Shape(ShapeType type, float radius)
	: _type  (type)
	, _radius(radius)
	, _points()
{
}


ShapeSP Shape::newAlignedBox(const Box2& box) {
	ShapeSP shape = std::make_shared<Shape>(SHAPE_ALIGNED_BOX);
	shape->_points.push_back(box.min());
	shape->_points.push_back(box.max());
	return shape;
}


ShapeSP Shape::newCircle(const Vector2& center, float radius) {
	ShapeSP shape = std::make_shared<Shape>(SHAPE_CIRCLE, radius);
	shape->_points.push_back(center);
	return shape;
}



CollisionComponent::CollisionComponent(Manager* manager, _Entity* entity, ShapeSP shape)
	: Component   (manager, entity)
	, _shape      (shape)
	, _hitMask    (1u)
	, _ignoreMask (0u)
	, _penetration()
{
	for(int i = 0; i < N_DIRECTIONS; ++i)
		_penetration[i] = 0;
}


Box2 CollisionComponent::worldAlignedBox() const {
	lairAssert(_shape && _shape->type() == SHAPE_ALIGNED_BOX);
	Vector2 pos = EntityRef(_entityPtr).computeWorldTransform().translation().head<2>();
	return Box2(pos + _shape->point(0), pos + _shape->point(1));
}



CollisionComponentManager::CollisionComponentManager(size_t componentBlockSize)
	: DenseComponentManager<CollisionComponent>("collision", componentBlockSize)
{
}


CollisionComponent* CollisionComponentManager::addComponentFromJson(
        EntityRef entity, const Json::Value& json, const Path& /*cd*/) {
	CollisionComponent* comp = addComponent(entity);

	if(json.isMember("abox")) {
		Json::Value box = json["abox"];
		if(box.isArray() || box.size() == 4) {
			comp->setShape(Shape::newAlignedBox(Box2(
			        Vector2(box[0].asFloat(), box[1].asFloat()),
			        Vector2(box[2].asFloat(), box[3].asFloat()))));
		} /*else {
			log().warning("Invalid anchor field while loading entity \"", entity.name(), "\".");
		}*/
	}

	if(json.isMember("hit_mask")) {
		comp->setHitMask(json["hit_mask"].asUInt());
	}

	if(json.isMember("ignore_mask")) {
		comp->setIgnoreMask(json["ignore_mask"].asUInt());
	}

	return comp;
}


CollisionComponent* CollisionComponentManager::cloneComponent(EntityRef base, EntityRef entity) {
	CollisionComponent* baseComp = get(base);
	CollisionComponent* comp = _addComponent(entity, baseComp);
	comp->setShape     (baseComp->shape());
	comp->setHitMask   (baseComp->hitMask());
	comp->setIgnoreMask(baseComp->ignoreMask());
	return comp;
}


void CollisionComponentManager::findCollisions(HitEventQueue& hitQueue) {
	compactArray();

	// TODO: Less brute-force approach.
	// TODO: Support shapes other than aligned boxes
	HitEvent hit;
	for(int ci0 = 0; ci0 < int(nComponents()); ++ci0) {
		CollisionComponent& c0 = _components[ci0];
		if(!c0.entity().isEnabledRec() || !c0.isEnabled()
		|| !c0.shape() || c0.shape()->type() != SHAPE_ALIGNED_BOX)
			continue;
		hit.entities[0] = c0.entity();

		for(int ci1 = ci0 + 1; ci1 < int(nComponents()); ++ci1) {
			CollisionComponent& c1 = _components[ci1];

			if(!c1.entity().isEnabledRec() || !c1.isEnabled()
			|| !c1.shape() || c1.shape()->type() != SHAPE_ALIGNED_BOX
			|| (c0.hitMask() & c1.hitMask())    == 0
			|| (c0.hitMask() & c1.ignoreMask()) != 0
			|| (c1.hitMask() & c0.ignoreMask()) != 0)
				continue;
			hit.entities[1] = c1.entity();

			hit.boxes[0] = c0.worldAlignedBox();
			hit.boxes[1] = c1.worldAlignedBox();

			if(!hit.boxes[0].intersection(hit.boxes[1]).isEmpty())
				hitQueue.push_back(hit);
		}
	}
}


bool CollisionComponentManager::hitTest(std::deque<EntityRef>& hits, const Box2& box, unsigned hitMask) {
	bool found = false;
	for(int ci = 0; ci < int(nComponents()); ++ci) {
		CollisionComponent& c = _components[ci];
		if(!c.entity().isEnabledRec() || !c.isEnabled()
		|| !c.shape() || c.shape()->type() != SHAPE_ALIGNED_BOX
		|| (hitMask & c.hitMask())    == 0
		|| (hitMask & c.ignoreMask()) != 0)
			continue;

		Box2    cbox = c.worldAlignedBox();
		if(!cbox.intersection(box).isEmpty()) {
			hits.push_back(c.entity());
			found = true;
		}
	}
	return found;
}


bool CollisionComponentManager::hitTest(std::deque<EntityRef>& hits, const Vector2& p, unsigned hitMask) {
	return hitTest(hits, Box2(p, p), hitMask);
}


}
