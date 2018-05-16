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


namespace lair {


CollisionComponent::CollisionComponent(Manager* manager, _Entity* entity, const Shape2DVector& shapes)
	: Component   (manager, entity)
	, _shapes     (shapes)
	, _hitMask    (1u)
	, _ignoreMask (0u)
	, _dirty      (true)
    , _firstElem  (nullptr)
{
}


const PropertyList& CollisionComponent::properties() {
	static PropertyList props;
	if(props.nProperties() == 0) {
		props.addProperty("shape",
		                  &CollisionComponent::shapes,
		                  &CollisionComponent::setShapes);
		props.addProperty("hit_mask",
		                  &CollisionComponent::hitMask,
		                  &CollisionComponent::setHitMask);
		props.addProperty("ignore_mask",
		                  &CollisionComponent::ignoreMask,
		                  &CollisionComponent::setIgnoreMask);
	}
	return props;
}



CollisionComponentManager::CollisionComponentManager(size_t componentBlockSize)
	: DenseComponentManager<CollisionComponent>("collision", componentBlockSize)
    , _quadTree(AlignedBox2(Vector2(0, 0), Vector2(4096, 4096)))
{
}


void CollisionComponentManager::setBounds(const AlignedBox2& bounds) {
	for(unsigned ci0 = 0; ci0 < nComponents(); ++ci0) {
		// QuadTree::reset clears the tree, so mark nodes dirty so they are re-inserted.
		_components[ci0].setDirty();
	}

	_quadTree.reset(bounds);
}


void CollisionComponentManager::findCollisions() {
	// Set all unusable shapes dirty: this will remove their elements from the list.
	for(unsigned ci0 = 0; ci0 < nComponents(); ++ci0) {
		CollisionComponent& c0 = _components[ci0];

		if(!c0.isAlive() || !c0.isEnabled() || !c0.entity().isEnabledRec())
			c0.setDirty();

		if(c0.isDirty())
			c0._firstElem = nullptr;
	}

	// Filter out dirty elements.
	_quadTree.filterIf(_FilterDirtyElement(this));

	// Filter out hitEvents with a dirty Entity.
	auto dirtyEventsBegin = std::remove_if(_hitEvents.begin(), _hitEvents.end(), _FilterDirty(this));
	_hitEvents.erase(dirtyEventsBegin, _hitEvents.end());

	compactArray();

	// Update dirty entities
	HitEvent hit;
	for(unsigned ci0 = 0; ci0 < nComponents(); ++ci0) {
		CollisionComponent& c0 = _components[ci0];

		// Don't append invalid elements.
		if(!c0.isAlive() || !c0.isDirty() || !c0.isEnabled() || !c0.entity().isEnabledRec())
			continue;

		c0.setDirty(false);

		_Element* next = nullptr;
		for(const Shape2D& shape: c0.shapes()) {
			_Element e0;
			e0.entity = c0.entity();
			e0.shape  = shape.transformed(c0.entity().worldTransform());
			e0.box    = e0.shape.boundingBox();

			_quadTree.hitTest(e0.box, [&hit, &e0, this](_Element& e1) {
				CollisionComponent* comp0 = get(e0.entity);
				CollisionComponent* comp1 = get(e1.entity);
				if(comp0 != comp1
				&& (comp0->hitMask() & comp1->hitMask())    != 0
				&& (comp0->hitMask() & comp1->ignoreMask()) == 0
				&& (comp0->hitMask() & comp1->ignoreMask()) == 0
				&& e0.shape.intersect(e1.shape, &hit.position)) {
					hit.entities[0] = e0.entity;
					hit.entities[1] = e1.entity;
					_hitEvents.push_back(hit);
				}

				return false;
			});

			_Element* elem = _quadTree.insert(e0);
			elem->next = next;
			next = elem;
		}

		c0._firstElem = next;
	}
}


bool CollisionComponentManager::hitTest(std::deque<EntityRef>& hits, const AlignedBox2& box,
                                        unsigned hitMask, EntityRef dontPick) {
	bool found = false;

	_quadTree.hitTest(box, [this, &hits, hitMask, &dontPick, &found](_Element& e) {
		CollisionComponent* comp = get(e.entity);
		if( comp
		&& e.entity != dontPick
		&& (hitMask & comp->hitMask())    != 0
		&& (hitMask & comp->ignoreMask()) == 0) {
			hits.push_back(e.entity);
			found = true;
		}

		return false;
	});

	return found;
}


bool CollisionComponentManager::hitTest(std::deque<EntityRef>& hits, const Vector2& p,
                                        unsigned hitMask, EntityRef dontPick) {
	return hitTest(hits, AlignedBox2(p, p), hitMask, dontPick);
}


void CollisionComponentManager::update(EntityRef entity) {
	CollisionComponent* comp = get(entity);

	if(comp) {
		_Element* elem = comp->_firstElem;
		while(elem) {
			_Element* next = elem->next;
			_quadTree.remove(elem);
			elem = next;
		}

		_Element* next = nullptr;
		for(const Shape2D& shape: comp->shapes()) {
			_Element e;
			e.entity = entity;
			e.shape  = shape.transformed(comp->entity().worldTransform());
			e.box    = e.shape.boundingBox();

			_Element* elem = _quadTree.insert(e);
			elem->next = next;
			next = elem;
		}
	}
}

}
