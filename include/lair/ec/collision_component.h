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


#ifndef _LAIR_EC_COLLISION_COMPONENT_H
#define _LAIR_EC_COLLISION_COMPONENT_H


#include <lair/core/lair.h>

#include <lair/ec/component.h>
#include <lair/ec/dense_component_manager.h>


namespace lair
{


enum Direction {
	UP,
	LEFT,
	DOWN,
	RIGHT,
	N_DIRECTIONS,
};


enum ShapeType {
	SHAPE_ALIGNED_BOX,
	SHAPE_CIRCLE,
//	SHAPE_CAPSULE,
	SHAPE_POLYGON,
};


class Shape;
typedef std::shared_ptr<Shape> ShapeSP;

class Shape {
public:
	Shape(ShapeType type = SHAPE_POLYGON, float radius = 0);

	static ShapeSP newAlignedBox(const Box2& box);
	static ShapeSP newCircle(const Vector2& center, float radius);
	static ShapeSP newCircle(const Box2& box);

	Shape(const Shape&)  = delete;
	Shape(      Shape&&) = default;
	~Shape() = default;

	Shape& operator=(const Shape&)  = delete;
	Shape& operator=(      Shape&&) = default;

	ShapeType      type() const             { return _type; }
	float          radius() const           { return _radius; }
	unsigned       nPoints() const          { return _points.size(); }
	const Vector2& point(unsigned pi) const { return _points[pi]; }

private:
	typedef std::vector<Vector2> PointVector;

private:
	ShapeType   _type;
	float       _radius;
	PointVector _points;
};


class CollisionComponent;
class CollisionComponentManager;


class CollisionComponent : public Component {
public:
	typedef CollisionComponentManager Manager;

public:
	CollisionComponent(Manager* manager, _Entity* entity, ShapeSP shape = ShapeSP());
	CollisionComponent(const CollisionComponent&) = delete;
	CollisionComponent(CollisionComponent&&)      = default;
	~CollisionComponent() = default;

	CollisionComponent& operator=(const CollisionComponent&) = delete;
	CollisionComponent& operator=(CollisionComponent&&)      = default;

	const ShapeSP shape() const  { return _shape; }
	void setShape(ShapeSP shape) { _shape = shape; }

	unsigned hitMask() const          { return _hitMask; }
	void setHitMask(unsigned hitMask) { _hitMask = hitMask; }

	unsigned ignoreMask() const             { return _ignoreMask; }
	void setIgnoreMask(unsigned ignoreMask) { _ignoreMask = ignoreMask; }

	float penetration(Direction dir) const { return _penetration[dir]; }
	void setPenetration(Direction dir, float penetration) { _penetration[dir] = penetration; }

	Box2 worldAlignedBox() const;

protected:
	ShapeSP  _shape;
	unsigned _hitMask;
	unsigned _ignoreMask;
	float    _penetration[N_DIRECTIONS];
};


class HitEvent {
public:
	EntityRef entities[2];
	Box2      boxes[2];
};
typedef std::deque<HitEvent> HitEventQueue;

class CollisionComponentManager : public DenseComponentManager<CollisionComponent> {
public:

public:
	CollisionComponentManager(size_t componentBlockSize = 1024);
	CollisionComponentManager(const CollisionComponentManager&)  = delete;
	CollisionComponentManager(      CollisionComponentManager&&) = delete;
	virtual ~CollisionComponentManager() = default;

	CollisionComponentManager& operator=(const CollisionComponentManager&)  = delete;
	CollisionComponentManager& operator=(      CollisionComponentManager&&) = delete;

	virtual CollisionComponent* addComponentFromJson(EntityRef entity, const Json::Value& json,
	                                                 const Path& cd=Path());
	virtual CollisionComponent* cloneComponent(EntityRef base, EntityRef entity);

	void findCollisions(HitEventQueue& hitQueue);

	bool hitTest(std::deque<EntityRef>& hits, const Box2& box, unsigned hitMask = 0x01);
	bool hitTest(std::deque<EntityRef>& hits, const Vector2& p, unsigned hitMask = 0x01);
};



}


#endif
