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
#include <lair/core/shapes.h>
#include <lair/core/octree.h>

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


enum Shape2DType {
	SHAPE_NONE,
	SHAPE_SPHERE,
	SHAPE_ALIGNED_BOX,
	SHAPE_ORIENTED_BOX,
};

class Shape2D {
public:
	inline Shape2D()
	    : _type(SHAPE_NONE)
	    , _shape(nullptr)
	{}

	inline Shape2D(const Sphere2& sphere)
	    : _type(SHAPE_SPHERE)
	    , _shape(new Sphere2(sphere))
	{}

	inline Shape2D(const AlignedBox2& box)
	    : _type(SHAPE_ALIGNED_BOX)
	    , _shape(new AlignedBox2(box))
	{}

	inline Shape2D(const OrientedBox2& oBox)
	    : _type(SHAPE_ORIENTED_BOX)
	    , _shape(new OrientedBox2(oBox))
	{}

	Shape2D(const Shape2D& other);
	Shape2D(Shape2D&& other);

	~Shape2D();

	Shape2D& operator=(Shape2D other);

	inline Shape2DType type()   const { return _type; }
	inline bool isValid()       const { return _type != SHAPE_NONE; }
	inline bool isSphere()      const { return _type == SHAPE_SPHERE; }
	inline bool isAlignedBox()  const { return _type == SHAPE_ALIGNED_BOX; }
	inline bool isOrientedBox() const { return _type == SHAPE_ORIENTED_BOX; }

	inline const Sphere2& asSphere() const {
		lairAssert(isSphere());
		return *static_cast<Sphere2*>(_shape);
	}

	inline const AlignedBox2& asAlignedBox() const {
		lairAssert(isAlignedBox());
		return *static_cast<AlignedBox2*>(_shape);
	}

	inline const OrientedBox2& asOrientedBox() const {
		lairAssert(isOrientedBox());
		return *static_cast<OrientedBox2*>(_shape);
	}

	Shape2D transformed(const Matrix3& transform) const;

	inline Shape2D transformed(const Matrix4& transform) const {
		Matrix3 m;
		m << transform.topLeftCorner<2, 2>(), transform.topRightCorner<2, 1>(),
		     0, 0, 1;
		return transformed(m);
	}

	inline Shape2D transformed(const Transform& transform) const {
		return transformed(transform.matrix());
	}

	AlignedBox2 boundingBox() const;

	bool intersect(const Shape2D& other, Vector2* position = 0) const;

	void swap(Shape2D& other);

protected:
	Shape2DType _type;
	void*       _shape;
};

std::ostream& operator<<(std::ostream& out, const Shape2D& shape);

typedef std::vector<Shape2D> Shape2DVector;


bool ldlRead(LdlParser& parser, Shape2D& value);
bool ldlWrite(LdlWriter& writer, const Shape2D& value);

bool ldlRead(LdlParser& parser, Shape2DVector& value);
bool ldlWrite(LdlWriter& writer, const Shape2DVector& value);


class CollisionComponent;
class CollisionComponentManager;


class CollisionComponent : public Component, WithProperties<CollisionComponent> {
public:
	typedef CollisionComponentManager Manager;

public:
	CollisionComponent(Manager* manager, _Entity* entity, const Shape2DVector& shape = Shape2DVector());
	CollisionComponent(const CollisionComponent&) = delete;
	CollisionComponent(CollisionComponent&&)      = default;
	~CollisionComponent() = default;

	CollisionComponent& operator=(const CollisionComponent&) = delete;
	CollisionComponent& operator=(CollisionComponent&&)      = default;

	inline const Shape2DVector& shapes() const  { return _shapes; }
	inline void setShapes(const Shape2DVector& shapes) { _shapes = shapes; }
	inline void addShape(const Shape2D& shape) { _shapes.push_back(shape); }

	inline unsigned hitMask() const          { return _hitMask; }
	inline void setHitMask(unsigned hitMask) { _hitMask = hitMask; }

	inline unsigned ignoreMask() const             { return _ignoreMask; }
	inline void setIgnoreMask(unsigned ignoreMask) { _ignoreMask = ignoreMask; }

	inline bool isDirty() const { return _dirty; }
	inline void setDirty(bool dirty = true) { _dirty= dirty; }

	static const PropertyList& properties();

protected:
	Shape2DVector _shapes;
	unsigned      _hitMask;
	unsigned      _ignoreMask;
	bool          _dirty;
};


class HitEvent {
public:
	EntityRef entities[2];
	Vector2   position;
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

	void setBounds(const AlignedBox2& bounds);

	inline const HitEventVector& hitEvents() const { return _hitEvents; }
	void findCollisions();

	bool hitTest(std::deque<EntityRef>& hits, const AlignedBox2& box, unsigned hitMask = 0x01);
	bool hitTest(std::deque<EntityRef>& hits, const Vector2& p, unsigned hitMask = 0x01);

protected:
	struct _Element {
		typedef float Scalar;
		enum {
			Dim = 2,
		};

		inline const AlignedBox2& boundingBox() { return box; }

		CollisionComponent* comp;
		Shape2D             shape;
		AlignedBox2         box;
	};
	typedef Octree<_Element> QuadTree;

	struct _FilterDirtyElement {
		inline bool operator()(const _Element& element) const {
			return element.comp->isDirty();
		}
	};

	struct _FilterDirty {
		inline _FilterDirty(CollisionComponentManager* self)
			: _self(self) {}

		inline bool operator()(const HitEvent& hit) const {
			return !hit.entities[0].isValid()
			    || !hit.entities[1].isValid()
			    || _self->get(hit.entities[0])->isDirty()
			    || _self->get(hit.entities[1])->isDirty();
		}

		CollisionComponentManager* _self;
	};

protected:
	QuadTree       _quadTree;
	HitEventVector _hitEvents;
};


}


LAIR_REGISTER_METATYPE(lair::Shape2D, "Shape2D");
LAIR_REGISTER_METATYPE(lair::Shape2DVector, "Shape2DList");


#endif
