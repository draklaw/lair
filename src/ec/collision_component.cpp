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


ShapeSP Shape::newOrientedLine(const Vector2& p0, const Vector2& p1) {
	ShapeSP shape = std::make_shared<Shape>(SHAPE_ORIENTED_LINE);
	shape->_points.push_back(p0);
	shape->_points.push_back(p1);
	return shape;
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


Box2 Shape::boundingBox() const {
	Box2 box;
	switch(_type) {
	case SHAPE_ORIENTED_LINE:
	case SHAPE_ALIGNED_BOX: {
		for(unsigned i = 0; i < 2; ++i) {
			box.extend(point(i));
		}
		break;
	}
	case SHAPE_CIRCLE:
	case SHAPE_POLYGON:
		assert(false);
	}

	// FIXME: Remove the magic number and make it a parameter.
	Vector2 epsilon = Vector2::Constant(0.01f);
	return Box2(box.min() - epsilon, box.max() + epsilon);
}


void Shape::setTransformed(Shape& dst, const Matrix4& transform) const {
	// TODO: Warn the user if transform includes shear / rotation for shapes that
	// do not support it ?
	dst._type = _type;
	dst._points.resize(_points.size());
	for(unsigned i = 0; i < _points.size(); ++i)
		dst._points[i] = (transform * (Vector4() << _points[i], 0, 1).finished()).head<2>();
}


// TODO: Move this to lair.h
#define LAIR_UNUSED(_var) ((void)_var)

// FIXME: These functions deserve tests.
bool intersectOLineOLine(const Shape& shape0, const Shape& shape1, Vector2* penetration) {
	assert(shape0.type() == SHAPE_ORIENTED_LINE);
	assert(shape1.type() == SHAPE_ORIENTED_LINE);

	LAIR_UNUSED(penetration);
	assert(false);
}


bool intersectOLineABox(const Shape& shape0, const Shape& shape1, Vector2* penetration) {
	assert(shape0.type() == SHAPE_ORIENTED_LINE);
	assert(shape1.type() == SHAPE_ALIGNED_BOX);

	Vector2 p0 = shape0.point(0);
	Vector2 p1 = shape0.point(1);
	Vector2 v  = p1 - p0;
	Vector2 n  = Vector2(-v(1), v(0)).normalized();

	Scalar minD = n.dot(shape1.aboxCorner(0) - p0);
	Scalar maxD = minD;
	for(unsigned i = 1; i < 4; ++i) {
		Scalar d = n.dot(shape1.aboxCorner(i) - p0);
		minD = std::min(minD, d);
		maxD = std::max(maxD, d);
	}

	if(minD > 0 || maxD < 0) {
		return false;
	}

	Vector2 bmin = shape1.point(0);
	Vector2 bmax = shape1.point(1);

//	Eigen::IOFormat fmt(3, Eigen::DontAlignCols, ", ", "\n", "[", "]", "", "");
//	dbgLogger.info("line/box: OLine(", (shape0.point(0) / 24).transpose().format(fmt),
//	               " - ", (shape0.point(1) / 24).transpose().format(fmt),
//	               "), ABox(", (shape1.point(0) / 24).transpose().format(fmt),
//	               " - ", (shape1.point(1) / 24).transpose().format(fmt),
//	               " - ", minD, ", ", maxD);

	if((p0(0) < bmin(0)) != (p1(0) < bmin(0))
	|| (p0(0) < bmax(0)) != (p1(0) < bmax(0))
	|| (p0(1) < bmin(1)) != (p1(1) < bmin(1))
	|| (p0(1) < bmax(1)) != (p1(1) < bmax(1))) {
		if(penetration)
			*penetration = n * minD;
		return true;
	}

	return false;
}


bool intersectOLineCircle(const Shape& shape0, const Shape& shape1, Vector2* penetration) {
	assert(shape0.type() == SHAPE_ORIENTED_LINE);
	assert(shape1.type() == SHAPE_CIRCLE);

	LAIR_UNUSED(penetration);
	assert(false);
}


bool intersectOLinePolygon(const Shape& shape0, const Shape& shape1, Vector2* penetration) {
	assert(shape0.type() == SHAPE_ORIENTED_LINE);
	assert(shape1.type() == SHAPE_POLYGON);

	LAIR_UNUSED(penetration);
	assert(false);
}


bool intersectABoxABox(const Shape& shape0, const Shape& shape1, Vector2* penetration) {
	assert(shape0.type() == SHAPE_ALIGNED_BOX);
	assert(shape1.type() == SHAPE_ALIGNED_BOX);

	Box2 box0 = shape0.abox();
	Box2 box1 = shape1.abox();
	Box2 i = box0.intersection(box1);
	if(!i.isEmpty()) {
		if(penetration) {
			// FIXME: Comput penetration.
			*penetration = Vector2::Zero();
		}
		return true;
	}

	return false;
}


bool intersectABoxCircle(const Shape& shape0, const Shape& shape1, Vector2* penetration) {
	assert(shape0.type() == SHAPE_ALIGNED_BOX);
	assert(shape1.type() == SHAPE_CIRCLE);

	LAIR_UNUSED(penetration);
	assert(false);
}


bool intersectABoxPolygon(const Shape& shape0, const Shape& shape1, Vector2* penetration) {
	assert(shape0.type() == SHAPE_ALIGNED_BOX);
	assert(shape1.type() == SHAPE_POLYGON);

	LAIR_UNUSED(penetration);
	assert(false);
}


bool intersectCircleCircle(const Shape& shape0, const Shape& shape1, Vector2* penetration) {
	assert(shape0.type() == SHAPE_CIRCLE);
	assert(shape1.type() == SHAPE_CIRCLE);

	LAIR_UNUSED(penetration);
	assert(false);
}


bool intersectCirclePolygon(const Shape& shape0, const Shape& shape1, Vector2* penetration) {
	assert(shape0.type() == SHAPE_CIRCLE);
	assert(shape1.type() == SHAPE_POLYGON);

	LAIR_UNUSED(penetration);
	assert(false);
}


bool intersectPolygonPolygon(const Shape& shape0, const Shape& shape1, Vector2* penetration) {
	assert(shape0.type() == SHAPE_POLYGON);
	assert(shape1.type() == SHAPE_POLYGON);

	LAIR_UNUSED(penetration);
	assert(false);
}


bool intersect(const Shape& shape0, const Shape& shape1, Vector2* penetration) {
	if(shape1.type() < shape0.type()) {
		bool hit = intersect(shape1, shape0, penetration);
		if(hit && penetration)
			*penetration = -*penetration;
		return hit;
	}

	switch(shape0.type()) {
	case SHAPE_ORIENTED_LINE:
		switch(shape1.type()) {
		case SHAPE_ORIENTED_LINE:
			return false; // OLines cannot intersect.
		case SHAPE_ALIGNED_BOX:
			return intersectOLineABox(shape0, shape1, penetration);
		case SHAPE_CIRCLE:
			return intersectOLineCircle(shape0, shape1, penetration);
		case SHAPE_POLYGON:
			return intersectOLinePolygon(shape0, shape1, penetration);
		}
	case SHAPE_ALIGNED_BOX:
		switch(shape1.type()) {
		case SHAPE_ORIENTED_LINE:
			assert(false);
		case SHAPE_ALIGNED_BOX:
			return intersectABoxABox(shape0, shape1, penetration);
		case SHAPE_CIRCLE:
			return intersectABoxCircle(shape0, shape1, penetration);
		case SHAPE_POLYGON:
			return intersectABoxPolygon(shape0, shape1, penetration);
		}
	case SHAPE_CIRCLE:
		switch(shape1.type()) {
		case SHAPE_ORIENTED_LINE:
		case SHAPE_ALIGNED_BOX:
			assert(false);
		case SHAPE_CIRCLE:
			return intersectCircleCircle(shape0, shape1, penetration);
		case SHAPE_POLYGON:
			return intersectCirclePolygon(shape0, shape1, penetration);
		}
	case SHAPE_POLYGON:
		switch(shape1.type()) {
		case SHAPE_ORIENTED_LINE:
		case SHAPE_ALIGNED_BOX:
		case SHAPE_CIRCLE:
			assert(false);
		case SHAPE_POLYGON:
			return intersectPolygonPolygon(shape0, shape1, penetration);
		}
	}

	return false;
}


bool ldlRead(LdlParser& parser, ShapeSP& value) {
	if(!parser.isValueTyped()) {
		parser.error("Type annotation is required for Shape objects");
		parser.skip();
		return false;
	}

	const String& type = parser.getValueTypeName();

	bool success = true;
	if(type == "OLine") {
		if(parser.valueType() != LdlParser::TYPE_LIST) {
			parser.error("OLine shape must be of type VarList, got ", parser.valueTypeName());
			parser.skip();
			return false;
		}

		Vector2 pts[2];

		parser.enter();
		int count = 0;
		while(count < 2 && parser.valueType() != LdlParser::TYPE_END) {
			success &= ldlRead(parser, pts[count]);
			++count;
		}
		while(parser.valueType() != LdlParser::TYPE_END) {
			parser.skip();
			++count;
		}

		if(count < 2) {
			parser.error("OLine takes 2 Vector2 parameters, ", count, " provided");
			success = false;
		}
		if(parser.valueType() != LdlParser::TYPE_END) {
			parser.warning("OLine takes 2 Vector2 parameters, ", count, " provided: extra parameters ignored");
		}

		if(success) {
			value = Shape::newOrientedLine(pts[0], pts[1]);
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
			Box2 box(min, min + size);
			value = Shape::newAlignedBox(box);
		}

		parser.leave();
	}
	else if(type == "Circle") {
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
			value = Shape::newCircle(center, radius);
		}

		parser.leave();
	}
	else if(type == "Polygon") {
		// TODO:
		parser.error("Polygon shapes reader not implemented");
		return false;
	}

	return success;
}


bool ldlWrite(LdlWriter& writer, const ShapeSP& value) {
	bool success = true;

	switch(value->type()) {
	case SHAPE_ORIENTED_LINE: {
		writer.openList(LdlWriter::CF_SINGLE_LINE, "OLine");
		success &= ldlWrite(writer, value->point(0));
		success &= ldlWrite(writer, value->point(1));
		writer.close();
		break;
	}
	case SHAPE_ALIGNED_BOX: {
		writer.openMap(LdlWriter::CF_SINGLE_LINE, "ABox");
		writer.writeKey("min");
		success &= ldlWrite(writer, value->point(0));
		writer.writeKey("size");
		Vector2 size = value->point(1) - value->point(0);
		success &= ldlWrite(writer, size);
		writer.close();
		break;
	}
	case SHAPE_CIRCLE: {
		writer.openMap(LdlWriter::CF_SINGLE_LINE, "Circle");
		writer.writeKey("center");
		success &= ldlWrite(writer, value->point(0));
		writer.writeKey("radius");
		success &= ldlWrite(writer, value->radius());
		writer.close();
		break;
	}
	case SHAPE_POLYGON: {
		writer.openList(LdlWriter::CF_MULTI_LINE, "Polygon");
		for(unsigned i = 0; i < value->nPoints(); ++i)
			success &= ldlWrite(writer, value->point(i));
		writer.close();
		break;
	}
	}

	return success;
}


bool ldlRead(LdlParser& parser, ShapeSPVector& value) {
	bool success = true;

	if(parser.valueType() == LdlParser::TYPE_LIST) {
		parser.enter();
		while(parser.valueType() != LdlParser::TYPE_END) {
			ShapeSP shape;
			success &= ldlRead(parser, shape);
			if(shape) {
				value.push_back(shape);
			}
		}
	}
	else {
		ShapeSP shape;
		success &= ldlRead(parser, shape);
		if(shape) {
			value.push_back(shape);
		}
	}

	return success;
}


bool ldlWrite(LdlWriter& writer, const ShapeSPVector& value) {
	bool success = true;

	if(value.size() == 1) {
		success &= ldlWrite(writer, value[0]);
	}
	else {
		writer.openList();
		for(ShapeSP shape: value)
			success &= ldlWrite(writer, shape);
		writer.close();
	}

	return success;
}



CollisionComponent::CollisionComponent(Manager* manager, _Entity* entity, const ShapeSPVector& shapes)
	: Component   (manager, entity)
	, _shapes     (shapes)
	, _hitMask    (1u)
	, _ignoreMask (0u)
	, _dirty      (true)
//	, _penetration()
{
//	for(int i = 0; i < N_DIRECTIONS; ++i)
//		_penetration[i] = 0;
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
{
}


CollisionComponent* CollisionComponentManager::addComponentFromJson(
        EntityRef entity, const Json::Value& json, const Path& /*cd*/) {
	CollisionComponent* comp = addComponent(entity);

	if(json.isMember("abox")) {
		Json::Value box = json["abox"];
		if(box.isArray() || box.size() == 4) {
			comp->addShape(Shape::newAlignedBox(Box2(
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
	comp->setShapes    (baseComp->shapes());
	comp->setHitMask   (baseComp->hitMask());
	comp->setIgnoreMask(baseComp->ignoreMask());
	return comp;
}


void CollisionComponentManager::findCollisions() {
//	compactArray();

	// Set all unusable shapes dirty: this will remove their elements from the list.
	for(unsigned ci0 = 0; ci0 < nComponents(); ++ci0) {
		CollisionComponent& c0 = _components[ci0];

		if(!c0.isAlive() || !c0.isEnabled() || !c0.entity().isEnabledRec())
			c0.setDirty();
	}

	// Filter out dirty elements.
	auto dirtyElementsBegin = std::remove_if(_elements.begin(), _elements.end(), _FilterDirtyElement());
	unsigned firstDirtyElement = dirtyElementsBegin - _elements.begin();
//	dbgLogger.info("remove ", _elements.end() - dirtyElementsBegin, " dirty elements.");
	_elements.erase(dirtyElementsBegin, _elements.end());

	// Filter out hitEvents with a dirty Entity.
	auto dirtyEventsBegin = std::remove_if(_hitEvents.begin(), _hitEvents.end(), _FilterDirty(this));
//	unsigned firstDirtyEvent = dirtyEventBegin - _hitEvents.begin();
//	dbgLogger.info("remove ", _hitEvents.end() - dirtyEventsBegin, " dirty hitEvents.");
	_hitEvents.erase(dirtyEventsBegin, _hitEvents.end());

	// Update dirty entities
	for(unsigned ci0 = 0; ci0 < nComponents(); ++ci0) {
		CollisionComponent& c0 = _components[ci0];

		// Don't append invalid elements.
		if(!c0.isAlive() || !c0.isDirty() || !c0.isEnabled() || !c0.entity().isEnabledRec())
			continue;

		for(ShapeSP shape: c0.shapes()) {
			_Element element;
			element.comp  = &c0;
			shape->setTransformed(element.shape, c0.entity().worldTransform().matrix());
			element.box   = element.shape.boundingBox();

			_elements.push_back(std::move(element));

//			dbgLogger.info("add shape \"", c0.entity().name(), "\".");
		}
	}
//	dbgLogger.info("add ", _elements.size() - firstDirtyElement, " elements.");

	// Broad-phase: append candidates
	// TODO: Broad-phase.

	// Test collisions
	HitEvent hit;
//	dbgLogger.info("hit test: [", firstDirtyElement, ", ", _elements.size(), "[.");
	for(unsigned ei0 = 0; ei0 < _elements.size(); ++ei0) {
		_Element& e0 = _elements[ei0];

		e0.comp->setDirty(false);

//		if(ei0 == 0 && e0.shape.type() == SHAPE_ORIENTED_LINE) {
//			dbgLogger.info("\"", e0.comp->entity().name(), "\" OLine: ",
//			               e0.shape.point(0).transpose(), ", ",
//			               e0.shape.point(1).transpose(), " - ",
//			               e0.box.min().transpose(), ", ", e0.box.sizes().transpose());
//		}
//		if(e0.shape.type() == SHAPE_ALIGNED_BOX) {
//			dbgLogger.info("\"", e0.comp->entity().name(), "\" ABox: ",
//			               e0.shape.point(0).transpose(), ", ",
//			               (e0.shape.point(1) - e0.shape.point(0)).transpose(), " - ",
//			               e0.box.min().transpose(), ", ", e0.box.sizes().transpose());
//		}

		for(unsigned ei1 = std::max(ei0 + 1, firstDirtyElement); ei1 < _elements.size(); ++ei1) {
			_Element& e1 = _elements[ei1];

			if(e0.comp == e1.comp
			|| (e0.comp->hitMask() & e1.comp->hitMask())    == 0
			|| (e0.comp->hitMask() & e1.comp->ignoreMask()) != 0
			|| (e1.comp->hitMask() & e0.comp->ignoreMask()) != 0)
				continue;


//			dbgLogger.info("  test intersection \"", e0.comp->entity().name(), "\",  \"", e1.comp->entity().name(), "\".");
			if(!e0.box.intersection(e1.box).isEmpty()
			&& intersect(e0.shape, e1.shape, &hit.penetration)) {
				hit.entities[0] = e0.comp->entity();
				hit.entities[1] = e1.comp->entity();
//				hit.shapes[0] = std::make_shared<Shape>(e0.shape);
//				hit.shapes[1] = std::make_shared<Shape>(e1.shape);
				_hitEvents.push_back(hit);
//				dbgLogger.info("  intersection \"", e0.comp->entity().name(), "\",  \"", e1.comp->entity().name(), "\"",
//				               " - ", hit.entities[0].name(), ", ", hit.entities[1].name());
			}
		}
	}

//	std::ofstream out("collision.svg");

//	out << "<?xml version=\"1.0\" standalone=\"yes\"?>\n";
//	out << "<svg width=\"1920px\" height=\"1080px\" version=\"1.1\"\n";
//	out << "     xmlns=\"http://www.w3.org/2000/svg\">\n";

//	out << "  <style>\n";
//	out << "    .layer_base {\n";
//	out << "      stroke: black;\n";
//	out << "      fill:   none;\n";
//	out << "    }\n";
//	out << "    .player {\n";
//	out << "      stroke: green;\n";
//	out << "      fill:   none;\n";
//	out << "    }\n";
//	out << "  </style>\n";

//	out << "  <g transform=\"translate(0, 1080) scale(1, -1)\">\n";
//	for(unsigned ei = 0; ei < _elements.size(); ++ei) {
//		const _Element& e = _elements[ei];
//		if(e.shape.type() == SHAPE_ORIENTED_LINE) {
//			out << "    <path class=\"" << e.comp->entity().name() << "\" d=\"M "
//				<< e.shape.point(0)(0) << "," << e.shape.point(0)(1) << " L "
//				<< e.shape.point(1)(0) << "," << e.shape.point(1)(1) << "\" />\n";
//		}
//		if(e.shape.type() == SHAPE_ALIGNED_BOX) {
//			out << "    <path class=\"" << e.comp->entity().name() << "\" d=\"M "
//			    << e.shape.aboxCorner(0)(0) << "," << e.shape.aboxCorner(0)(1) << " L "
//			    << e.shape.aboxCorner(1)(0) << "," << e.shape.aboxCorner(1)(1) << " L "
//			    << e.shape.aboxCorner(3)(0) << "," << e.shape.aboxCorner(3)(1) << " L "
//			    << e.shape.aboxCorner(2)(0) << "," << e.shape.aboxCorner(2)(1) << " Z\" />\n";
//		}
//	}
//	out << "  </g>\n";

//	out << "</svg>\n";
}


bool CollisionComponentManager::hitTest(std::deque<EntityRef>& hits, const Box2& box, unsigned hitMask) {
	bool found = false;
	for(unsigned ei = 0; ei < _elements.size(); ++ei) {
		_Element& e = _elements[ei];

		if((hitMask & e.comp->hitMask())    == 0
		|| (hitMask & e.comp->ignoreMask()) != 0)
			continue;

		ShapeSP boxShape = Shape::newAlignedBox(box);
		if(intersect(e.shape, *boxShape)) {
			hits.push_back(e.comp->entity());
			found = true;
		}
	}
	return found;
}


bool CollisionComponentManager::hitTest(std::deque<EntityRef>& hits, const Vector2& p, unsigned hitMask) {
	return hitTest(hits, Box2(p, p), hitMask);
}


}
