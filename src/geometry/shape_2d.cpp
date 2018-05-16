/*
 *  Copyright (C) 2018 Simon Boyé
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

#include <lair/meta/variant.h>
#include <lair/meta/var_list.h>
#include <lair/meta/var_map.h>
#include <lair/meta/variant_reader.h>
#include <lair/meta/variant_writer.h>

#include <lair/geometry/intersection.h>

#include "lair/geometry/shape_2d.h"


namespace lair
{


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


bool varRead(Shape2D& value, const Variant& var, Logger& logger) {
	if(!var.isVarMap()) {
		logger.error(var.parseInfoDesc(), "Expected shape (VarMap), got \"", var.typeName(), "\".");
		return false;
	}

	bool success = true;
	const VarMap& varMap = var.asVarMap();

	if(varMap.type().empty()) {
		logger.error(var.parseInfoDesc(), "Expected type annotation for Shape object.");
		return false;
	}

	if(varMap.type() == "Circle") {
		Vector2 center;
		float radius;

		success &= varRead(center, varMap.get("center"), logger);
		success &= varRead(radius, varMap.get("radius"), logger);

		if(success) {
			value = Sphere2(center, radius);
		}
	}
	else if(varMap.type() == "ABox") {
		Vector2 min;
		Vector2 max;

		success &= varRead(min, varMap.get("min"), logger);

		const Variant& sizeVar = varMap.get("size");
		const Variant& maxVar  = varMap.get("max");
		if(sizeVar.isValid() && maxVar.isValid()) {
			logger.warning("Both \"size\" and \"max\" defined for ABox. Ignoring \"size\".");
		}

		if(maxVar.isValid()) {
			success &= varRead(max, maxVar, logger);
		}
		if(sizeVar.isValid()) {
			success &= varRead(max, sizeVar, logger);
			max += min;
		}

		if(success) {
			value = AlignedBox2(min, max);
		}
	}
	else {
		logger.error("Unknown shape type: ", varMap.type());
		success = false;
	}

	return success;
}


bool varWrite(Variant& var, const Shape2D& value, Logger& logger) {
	bool success = true;

	switch(value.type()) {
	case SHAPE_SPHERE: {
		VarMap varMap("Circle");
		Variant v;

		success &= varWrite(v, value.asSphere().center(), logger);
		varMap.emplace("center", std::move(v));

		success &= varWrite(v, value.asSphere().radius(), logger);
		varMap.emplace("radius", std::move(v));

		if(success) {
			var = std::move(varMap);
		}

		break;
	}
	case SHAPE_ALIGNED_BOX: {
		VarMap varMap("ABox");
		Variant v;

		success &= varWrite(v, value.asAlignedBox().min(), logger);
		varMap.emplace("min", std::move(v));

		success &= varWrite(v, Vector2(value.asAlignedBox().sizes()), logger);
		varMap.emplace("size", std::move(v));

		if(success) {
			var = std::move(varMap);
		}

		break;
	}
	default:
		lairAssert(false);
	}

	return success;
}


bool varRead(Shape2DVector& value, const Variant& var, Logger& logger) {
	bool success = true;

	if(var.isVarList()) {
		for(auto&& v: var.asVarList()) {
			Shape2D shape;
			success &= varRead(shape, v, logger);
			if(shape.isValid()) {
				value.push_back(shape);
			}
		}
	}
	else {
		Shape2D shape;
		success &= varRead(shape, var, logger);
		if(shape.isValid()) {
			value.push_back(shape);
		}
	}

	return success;
}


bool varWrite(Variant& var, const Shape2DVector& value, Logger& logger) {
	bool success = true;

	if(value.size() == 1) {
		success &= varWrite(var, value[0], logger);
	}
	else {
		VarList varList;
		for(const Shape2D& shape: value) {
			Variant v;
			success &= varWrite(v, shape, logger);
			varList.emplace_back(std::move(v));
		}
		var = std::move(varList);
	}

	return success;
}


}


LAIR_IMPLEMENT_METATYPE(lair::Shape2D, "Shape2D");
LAIR_IMPLEMENT_METATYPE(lair::Shape2DVector, "Shape2DList");
