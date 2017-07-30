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
#include <lair/core/ldl.h>

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
	SHAPE_ORIENTED_LINE,
	SHAPE_ALIGNED_BOX,
	SHAPE_CIRCLE,
//	SHAPE_CAPSULE,
	SHAPE_POLYGON,
};


class Shape;
typedef std::shared_ptr<Shape> ShapeSP;
typedef std::vector<ShapeSP> ShapeSPVector;

class Shape {
public:
	Shape(ShapeType type = SHAPE_POLYGON, float radius = 0);

	static ShapeSP newOrientedLine(const Vector2& p0, const Vector2& p1);
	static ShapeSP newAlignedBox(const Box2& box);
	static ShapeSP newCircle(const Vector2& center, float radius);
	static ShapeSP newCircle(const Box2& box);

	Shape(const Shape&)  = delete;
	Shape(      Shape&&) = default;
	~Shape() = default;

	Shape& operator=(const Shape&)  = delete;
	Shape& operator=(      Shape&&) = default;

	ShapeType      type() const             { return _type; }
	Scalar         radius() const           { return _radius; }
	unsigned       nPoints() const          { return _points.size(); }
	const Vector2& point(unsigned pi) const { return _points[pi]; }

	Box2 boundingBox() const;

	Box2 abox() const {
		assert(_type == SHAPE_ALIGNED_BOX);
		return Box2(point(0), point(1));
	}

	Vector2 aboxCorner(unsigned i) const {
		assert(_type == SHAPE_ALIGNED_BOX);
		assert(i < 4);
		return Vector2(point(i & 0x01)(0), point((i >> 1) & 0x01)(1));
	}

	void setTransformed(Shape& dst, const Matrix4& transform) const;

private:
	typedef std::vector<Vector2> PointVector;

private:
	ShapeType   _type;
	Scalar      _radius;
	PointVector _points;
};

bool intersectOLineOLine    (const Shape& shape0, const Shape& shape1, Vector2* penetration=0);
bool intersectOLineABox     (const Shape& shape0, const Shape& shape1, Vector2* penetration=0);
bool intersectOLineCircle   (const Shape& shape0, const Shape& shape1, Vector2* penetration=0);
bool intersectOLinePolygon  (const Shape& shape0, const Shape& shape1, Vector2* penetration=0);
bool intersectABoxABox      (const Shape& shape0, const Shape& shape1, Vector2* penetration=0);
bool intersectABoxCircle    (const Shape& shape0, const Shape& shape1, Vector2* penetration=0);
bool intersectABoxPolygon   (const Shape& shape0, const Shape& shape1, Vector2* penetration=0);
bool intersectCircleCircle  (const Shape& shape0, const Shape& shape1, Vector2* penetration=0);
bool intersectCirclePolygon (const Shape& shape0, const Shape& shape1, Vector2* penetration=0);
bool intersectPolygonPolygon(const Shape& shape0, const Shape& shape1, Vector2* penetration=0);

bool intersect(const Shape& shape0, const Shape& shape1, Vector2* penetration=0);

bool ldlRead(LdlParser& parser, ShapeSP& value);
bool ldlWrite(LdlWriter& writer, const ShapeSP& value);

bool ldlRead(LdlParser& parser, ShapeSPVector& value);
bool ldlWrite(LdlWriter& writer, const ShapeSPVector& value);


class CollisionComponent;
class CollisionComponentManager;


class CollisionComponent : public Component, WithProperties<CollisionComponent> {
public:
	typedef CollisionComponentManager Manager;

public:
	CollisionComponent(Manager* manager, _Entity* entity, const ShapeSPVector& shape = ShapeSPVector());
	CollisionComponent(const CollisionComponent&) = delete;
	CollisionComponent(CollisionComponent&&)      = default;
	~CollisionComponent() = default;

	CollisionComponent& operator=(const CollisionComponent&) = delete;
	CollisionComponent& operator=(CollisionComponent&&)      = default;

	inline const ShapeSPVector& shapes() const  { return _shapes; }
	inline void setShapes(const ShapeSPVector& shapes) { _shapes = shapes; }
	inline void addShape(const ShapeSP& shape) { _shapes.push_back(shape); }

	inline unsigned hitMask() const          { return _hitMask; }
	inline void setHitMask(unsigned hitMask) { _hitMask = hitMask; }

	inline unsigned ignoreMask() const             { return _ignoreMask; }
	inline void setIgnoreMask(unsigned ignoreMask) { _ignoreMask = ignoreMask; }

//	inline float penetration(Direction dir) const { return _penetration[dir]; }
//	inline void setPenetration(Direction dir, float penetration) { _penetration[dir] = penetration; }

	inline bool isDirty() const { return _dirty; }
	inline void setDirty(bool dirty = true) { _dirty= dirty; }

	static const PropertyList& properties();

protected:
	ShapeSPVector _shapes;
	unsigned      _hitMask;
	unsigned      _ignoreMask;
	bool          _dirty;
//	float         _penetration[N_DIRECTIONS];
};


class HitEvent {
public:
	EntityRef entities[2];
//	ShapeSP   shapes[2];
	Vector2   penetration; // entities[1] -> entities[2]
};
typedef std::vector<HitEvent> HitEventVector;
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

	inline const HitEventVector& hitEvents() const { return _hitEvents; }
	void findCollisions();

	bool hitTest(std::deque<EntityRef>& hits, const Box2& box, unsigned hitMask = 0x01);
	bool hitTest(std::deque<EntityRef>& hits, const Vector2& p, unsigned hitMask = 0x01);

protected:
	struct _Element {
		CollisionComponent* comp;
		Shape               shape;
		Box2                box;
	};
	typedef std::vector<_Element> _ElementVector;

	struct _FilterDirtyElement {
		inline bool operator()(const _Element& element) const {
			return element.comp->isDirty();
		}
	};

	struct _FilterDirty {
		inline _FilterDirty(CollisionComponentManager* self)
			: _self(self) {}

		inline bool operator()(const HitEvent& hit) const {
			return _self->get(hit.entities[0])->isDirty()
			    || _self->get(hit.entities[1])->isDirty();
		}

		CollisionComponentManager* _self;
	};

protected:
	_ElementVector _elements;
	HitEventVector _hitEvents;
};


}


LAIR_REGISTER_METATYPE(lair::ShapeSP, "Shape");
LAIR_REGISTER_METATYPE(lair::ShapeSPVector, "ShapeList");


#endif
