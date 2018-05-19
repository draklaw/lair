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


#include <deque>

#include <lair/core/lair.h>

#include <lair/meta/property.h>
#include <lair/meta/property_list.h>
#include <lair/meta/with_properties.h>

#include <lair/geometry/shape_2d.h>
#include <lair/geometry/octree.h>

#include <lair/ec/component.h>
#include <lair/ec/dense_component_manager.h>


namespace lair
{


class OrthographicCamera;
class RenderPass;
class TextureSet;
typedef std::shared_ptr<const TextureSet> TextureSetCSP;
class SpriteRenderer;


enum Direction {
	UP,
	LEFT,
	DOWN,
	RIGHT,
	N_DIRECTIONS,
};


class CollisionComponent;
class CollisionComponentManager;


struct _CollisionComponentElement {
	typedef float Scalar;
	enum {
		Dim = 2,
	};

	inline const AlignedBox2& boundingBox() { return box; }

	EntityRef           entity;
	Shape2D             shape;
	AlignedBox2         box;
	_CollisionComponentElement* next;
};


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

public:
	_CollisionComponentElement* _firstElem;
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

	inline AlignedBox2 bounds() const { return _quadTree.bounds(); }
	void setBounds(const AlignedBox2& bounds);

	inline const HitEventVector& hitEvents() const { return _hitEvents; }
	void findCollisions();

	bool hitTest(std::deque<EntityRef>& hits, const AlignedBox2& box,
	             unsigned hitMask = 0x01, EntityRef dontPick = EntityRef());
	bool hitTest(std::deque<EntityRef>& hits, const Vector2& p,
	             unsigned hitMask = 0x01, EntityRef dontPick = EntityRef());

	void update(EntityRef entity);

	void render(SpriteRenderer* spriteRenderer, RenderPass* renderPass,
	            TextureSetCSP texture, const OrthographicCamera& camera);


protected:
	typedef _CollisionComponentElement _Element;
	typedef Octree<_Element> QuadTree;

	struct _FilterDirtyElement {
		inline _FilterDirtyElement(CollisionComponentManager* self)
		    : _self(self) {}

		inline bool operator()(const _Element& element) const {
			CollisionComponent* comp = _self->get(element.entity);
			return !comp || comp->isDirty();
		}

		CollisionComponentManager* _self;
	};

	struct _FilterDirty {
		inline _FilterDirty(CollisionComponentManager* self)
			: _self(self) {}

		inline bool operator()(const HitEvent& hit) const {
			CollisionComponent* comp0 = _self->get(hit.entities[0]);
			CollisionComponent* comp1 = _self->get(hit.entities[1]);
			return !comp0
			    || !comp1
			    || !hit.entities[0].isValid()
			    || !hit.entities[1].isValid()
			    || comp0->isDirty()
			    || comp1->isDirty();
		}

		CollisionComponentManager* _self;
	};

protected:
	QuadTree       _quadTree;
	HitEventVector _hitEvents;
};


}


#endif
