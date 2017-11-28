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


namespace lair {


Shape2D::Shape2D(const Shape2D& other)
    : _type(other._type)
    , _shape(nullptr) {
	switch(type()) {
	case SHAPE_NONE:
		break;
	case SHAPE_SPHERE:
		_shape = new Sphere2(other.asSphere());
		break;
	case SHAPE_ALIGNED_BOX:
		_shape = new AlignedBox2(other.asAlignedBox());
		break;
	case SHAPE_ORIENTED_BOX:
		_shape = new OrientedBox2(other.asOrientedBox());
		break;
	}
}

Shape2D::Shape2D(Shape2D&& other)
    : _type(other._type)
    , _shape(other._shape){
	other._type = SHAPE_NONE;
	other._shape = nullptr;
}

Shape2D::~Shape2D() {
	switch(type()) {
	case SHAPE_NONE:
		break;
	case SHAPE_SPHERE:
		delete static_cast<Sphere2*>(_shape);
		break;
	case SHAPE_ALIGNED_BOX:
		delete static_cast<AlignedBox2*>(_shape);
		break;
	case SHAPE_ORIENTED_BOX:
		delete static_cast<OrientedBox2*>(_shape);
		break;
	}
}

Shape2D& Shape2D::operator=(Shape2D other) {
	swap(other);
	return *this;
}

Shape2D Shape2D::transformed(const Matrix3& transform) const {
	switch(type()) {
	case SHAPE_NONE:
		break;
	case SHAPE_SPHERE:
		return Shape2D(asSphere().rigidTransformed(transform));
	case SHAPE_ALIGNED_BOX:
		return Shape2D(asAlignedBox().rigidTransformed(transform));
	case SHAPE_ORIENTED_BOX:
		return Shape2D(asOrientedBox().rigidTransformed(transform));
	}
	return Shape2D();
}

AlignedBox2 Shape2D::boundingBox() const {
	switch(type()) {
	case SHAPE_NONE:
		break;
	case SHAPE_SPHERE:
		return asSphere().boundingBox();
	case SHAPE_ALIGNED_BOX:
		return asAlignedBox();
	case SHAPE_ORIENTED_BOX:
		return asOrientedBox().boundingBox();
	}
	return AlignedBox2();
}

bool Shape2D::intersect(const Shape2D& other, Vector2* position) const {
	const Shape2D* shape0 = this;
	const Shape2D* shape1 = &other;

	if(shape0->_type > shape1->_type)
		std::swap(shape0, shape1);

	bool inter = false;
	switch(shape0->_type) {
	case SHAPE_NONE:
		break;
	case SHAPE_SPHERE: {
		break;
	}
	case SHAPE_ALIGNED_BOX: {
		switch(shape1->_type) {
		case SHAPE_NONE:
		case SHAPE_SPHERE:
			lairAssert(false);
			break;
		case SHAPE_ALIGNED_BOX:
			inter = lair::intersect(shape0->asAlignedBox(), shape1->asAlignedBox(),
			                        position);
			break;
		case SHAPE_ORIENTED_BOX:
			break;
		}
	}
	case SHAPE_ORIENTED_BOX: {
		break;
	}
	}

	return inter;
}

void Shape2D::swap(Shape2D& other) {
	std::swap(_type,  other._type);
	std::swap(_shape, other._shape);
}


std::ostream& operator<<(std::ostream& out, const Shape2D& shape) {
	switch(shape.type()) {
	case SHAPE_NONE:
		out << "Shape()";
		break;
	case SHAPE_SPHERE:
		out << shape.asSphere();
		break;
	case SHAPE_ALIGNED_BOX:
		out << shape.asAlignedBox();
		break;
	case SHAPE_ORIENTED_BOX:
		out << shape.asOrientedBox();
		break;
	}
	return out;
}


bool ldlRead(LdlParser& parser, Shape2D& value) {
	if(!parser.isValueTyped()) {
		parser.error("Type annotation is required for Shape objects");
		parser.skip();
		return false;
	}

	const String& type = parser.getValueTypeName();

	bool success = true;
	if(type == "Circle") {
		if(parser.valueType() != LdlParser::TYPE_MAP) {
			parser.error("Circle shapes must be of type VarMap, got ", parser.valueTypeName());
			parser.skip();
			return false;
		}

		Vector2 center;
		float radius;
		bool hasCenter = false;
		bool hasRadius = false;

		parser.enter();
		while(parser.valueType() != LdlParser::TYPE_END) {
			const String& key = parser.getKey();
			if(key == "center") {
				if(hasCenter) {
					parser.warning("Duplicate key \"", key, "\": ignoring");
				}
				else {
					hasCenter = ldlRead(parser, center);
					success &= hasCenter;
				}
			}
			else if(key == "radius") {
				if(hasRadius) {
					parser.warning("Duplicate key \"", key, "\": ignoring");
				}
				else {
					hasRadius = ldlRead(parser, radius);
					success &= hasRadius;
				}
			}
			else {
				parser.warning("Unexpected key \"", key, "\": ignoring");
			}
		}

		if(!hasCenter) {
			parser.error("Key \"center\" is required for Circle shapes");
			success = false;
		}
		if(!hasRadius) {
			parser.error("Key \"Radius\" is required for Circle shapes");
			success = false;
		}

		if(success) {
			value = Sphere2(center, radius);
		}

		parser.leave();
	}
	else if(type == "ABox") {
		if(parser.valueType() != LdlParser::TYPE_MAP) {
			parser.error("ABox shape must be of type VarMap, got ", parser.valueTypeName());
			parser.skip();
			return false;
		}

		Vector2 min;
		Vector2 size;
		bool hasMin = false;
		bool hasSize = false;

		parser.enter();
		while(parser.valueType() != LdlParser::TYPE_END) {
			const String& key = parser.getKey();
			if(key == "min") {
				if(hasMin) {
					parser.warning("Duplicate key \"", key, "\": ignoring");
				}
				else {
					hasMin = ldlRead(parser, min);
					success &= hasMin;
				}
			}
			else if(key == "size") {
				if(hasSize) {
					parser.warning("Duplicate key \"", key, "\": ignoring");
				}
				else {
					hasSize = ldlRead(parser, size);
					success &= hasSize;
				}
			}
			else {
				parser.warning("Unexpected key \"", key, "\": ignoring");
			}
		}

		if(!hasMin) {
			parser.error("Key \"min\" is required for ABox shapes");
			success = false;
		}
		if(!hasSize) {
			parser.error("Key \"size\" is required for ABox shapes");
			success = false;
		}

		if(success) {
			value = AlignedBox2(min, min + size);
		}

		parser.leave();
	}
	else if(type == "Polygon") {
		// TODO:
		parser.error("Polygon shapes reader not implemented");
		parser.skip();
		return false;
	}
	else {
		parser.error("Unknown shape type: ", type);
		parser.skip();
		return false;
	}

	return success;
}


bool ldlWrite(LdlWriter& writer, const Shape2D& value) {
	bool success = true;

	switch(value.type()) {
	case SHAPE_SPHERE: {
		writer.openMap(LdlWriter::CF_SINGLE_LINE, "Circle");
		writer.writeKey("center");
		success &= ldlWrite(writer, value.asSphere().center());
		writer.writeKey("radius");
		success &= ldlWrite(writer, value.asSphere().radius());
		writer.close();
		break;
	}
	case SHAPE_ALIGNED_BOX: {
		writer.openMap(LdlWriter::CF_SINGLE_LINE, "ABox");
		writer.writeKey("min");
		success &= ldlWrite(writer, value.asAlignedBox().min());
		writer.writeKey("size");
		Vector2 sizes = value.asAlignedBox().sizes();
		success &= ldlWrite(writer, sizes);
		writer.close();
		break;
	}
	default:
		lairAssert(false);
	}

	return success;
}


bool ldlRead(LdlParser& parser, Shape2DVector& value) {
	bool success = true;

	if(parser.valueType() == LdlParser::TYPE_LIST) {
		parser.enter();
		while(parser.valueType() != LdlParser::TYPE_END) {
			Shape2D shape;
			success &= ldlRead(parser, shape);
			if(shape.isValid()) {
				value.push_back(shape);
			}
		}
	}
	else {
		Shape2D shape;
		success &= ldlRead(parser, shape);
		if(shape.isValid()) {
			value.push_back(shape);
		}
	}

	return success;
}


bool ldlWrite(LdlWriter& writer, const Shape2DVector& value) {
	bool success = true;

	if(value.size() == 1) {
		success &= ldlWrite(writer, value[0]);
	}
	else {
		writer.openList();
		for(const Shape2D& shape: value)
			success &= ldlWrite(writer, shape);
		writer.close();
	}

	return success;
}



CollisionComponent::CollisionComponent(Manager* manager, _Entity* entity, const Shape2DVector& shapes)
	: Component   (manager, entity)
	, _shapes     (shapes)
	, _hitMask    (1u)
	, _ignoreMask (0u)
	, _dirty      (true)
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
	compactArray();

	// Set all unusable shapes dirty: this will remove their elements from the list.
	for(unsigned ci0 = 0; ci0 < nComponents(); ++ci0) {
		CollisionComponent& c0 = _components[ci0];

		if(!c0.isAlive() || !c0.isEnabled() || !c0.entity().isEnabledRec())
			c0.setDirty();
	}

	// Filter out dirty elements.
	_quadTree.filterIf(_FilterDirtyElement());

	// Filter out hitEvents with a dirty Entity.
	auto dirtyEventsBegin = std::remove_if(_hitEvents.begin(), _hitEvents.end(), _FilterDirty(this));
	_hitEvents.erase(dirtyEventsBegin, _hitEvents.end());

	// Update dirty entities
	HitEvent hit;
	for(unsigned ci0 = 0; ci0 < nComponents(); ++ci0) {
		CollisionComponent& c0 = _components[ci0];

		// Don't append invalid elements.
		if(!c0.isAlive() || !c0.isDirty() || !c0.isEnabled() || !c0.entity().isEnabledRec())
			continue;

		c0.setDirty(false);

		for(const Shape2D& shape: c0.shapes()) {
			_Element e0;
			e0.comp  = &c0;
			e0.shape = shape.transformed(c0.entity().worldTransform());
			e0.box   = e0.shape.boundingBox();

			_quadTree.hitTest(e0.box, [&hit, &e0, this](_Element& e1) {
				if(e0.comp != e1.comp
				&& (e0.comp->hitMask() & e1.comp->hitMask())    != 0
				&& (e0.comp->hitMask() & e1.comp->ignoreMask()) == 0
				&& (e1.comp->hitMask() & e0.comp->ignoreMask()) == 0
				&& e0.shape.intersect(e1.shape, &hit.position)) {
					hit.entities[0] = e0.comp->entity();
					hit.entities[1] = e1.comp->entity();
					_hitEvents.push_back(hit);
				}

				return false;
			});

			_quadTree.insert(e0);
		}
	}
}


bool CollisionComponentManager::hitTest(std::deque<EntityRef>& hits, const AlignedBox2& box, unsigned hitMask) {
	bool found = false;

	_quadTree.hitTest(box, [&hits, hitMask, &found](_Element& e) {
		if((hitMask & e.comp->hitMask())    != 0
		&& (hitMask & e.comp->ignoreMask()) == 0) {
			hits.push_back(e.comp->entity());
			found = true;
		}

		return false;
	});

	return found;
}


bool CollisionComponentManager::hitTest(std::deque<EntityRef>& hits, const Vector2& p, unsigned hitMask) {
	return hitTest(hits, AlignedBox2(p, p), hitMask);
}


}
