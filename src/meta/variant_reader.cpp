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
#include <lair/core/path.h>

#include "lair/meta/variant_reader.h"


namespace lair
{


VariantReader::VariantReader(Logger* logger, bool skipOnError)
    : _logger(logger)
    , _skipOnError(skipOnError)
    , _success(true)
{
}


VariantReader::~VariantReader() {
}


#define IMPL_VAR_READ_INT(_type, _typename) \
	bool varRead(_type& value, const Variant& var, Logger& logger) { \
	    if(var.isInt()) { \
	        value = var.asInt(); \
        } \
	    else { \
	        logger.error(var.parseInfoDesc(), "Expected ", _typename, \
	        ", got ", var.typeName(), "."); \
	        return false; \
        } \
	    return true; \
    }

IMPL_VAR_READ_INT(bool, "Bool")

IMPL_VAR_READ_INT(int8,  "Int8")
IMPL_VAR_READ_INT(int16, "Int16")
IMPL_VAR_READ_INT(int32, "Int32")
IMPL_VAR_READ_INT(int64, "Int64")

IMPL_VAR_READ_INT(uint8,  "UInt8")
IMPL_VAR_READ_INT(uint16, "UInt16")
IMPL_VAR_READ_INT(uint32, "UInt32")
IMPL_VAR_READ_INT(uint64, "UInt64")


bool varRead(float& value, const Variant& var, Logger& logger) {
	if(var.isFloat()) {
		value = var.asFloat();
	}
	else {
		logger.error(var.parseInfoDesc(), "Expected Float, got ", var.typeName(), ".");
		return false;
	}
	return true;
}


bool varRead(double& value, const Variant& var, Logger& logger) {
	if(var.isFloat()) {
		value = var.asFloat();
	}
	else {
		logger.error(var.parseInfoDesc(), "Expected Double, got ", var.typeName(), ".");
		return false;
	}
	return true;
}


bool varRead(String& value, const Variant& var, Logger& logger) {
	if(var.isString()) {
		value = var.asString();
	}
	else {
		logger.error(var.parseInfoDesc(), "Expected String, got ", var.typeName(), ".");
		return false;
	}
	return true;
}


bool varRead(Path& value, const Variant& var, Logger& logger) {
	if(var.isString()) {
		value = var.asString();
	}
	else {
		logger.error(var.parseInfoDesc(), "Expected Path, got ", var.typeName(), ".");
		return false;
	}
	return true;
}


bool varRead(Transform& value, const Variant& var, Logger& logger) {
	if(!var.isVarList()) {
		logger.error(var.parseInfoDesc(), "Expected Transform, got ", var.typeName(), ".");
		return false;
	}

	if(!var.parseInfo() || var.parseInfo()->type.empty()
	        || var.parseInfo()->type == "Transform") {
		value = Transform::Identity();
		for(const Variant& v: var.asVarList()) {
			Transform t;
			if(!varRead(t, v, logger))
				return false;
			value = value * t;
		}
	}
	else if(var.parseInfo()->type == "translate") {
		Vector3 vec;
		if(!varRead(vec, var, logger, false))
			return false;

		value.setIdentity();
		value.translate(vec);
	}
	else if(var.parseInfo()->type == "rotate") {
		Vector4 vec(0, 0, 0, 1);

		unsigned size = var.asVarList().size();
		if(size == 1) {
			if(!varRead(vec.head<1>(), var, logger, false))
				return false;
		}
		else if(size == 4) {
			if(!varRead(vec, var, logger, false))
				return false;
		}
		else {
			logger.error(var.parseInfoDesc(), "Rotations take 1 or 4 parameters, got ", size, ".");
			return false;
		}

		value.setIdentity();
		value.rotate(AngleAxis(vec(0), vec.tail<3>()));
	}
	else if(var.parseInfo()->type == "scale") {
		Vector3 vec(1, 1, 1);

		unsigned size = var.asVarList().size();
		if(size == 1) {
			if(!varRead(vec.head<1>(), var, logger, false))
				return false;
			vec = Vector3::Constant(vec(0));
		}
		else if(size == 2) {
			if(!varRead(vec.head<2>(), var, logger, false))
				return false;
		}
		else if(size == 2) {
			if(!varRead(vec, var, logger, false))
				return false;
		}
		else {
			logger.error(var.parseInfoDesc(), "Rotations take 1 or 4 parameters, got ", size, ".");
			return false;
		}

		value.setIdentity();
		value.scale(vec);
	}
	else {
		logger.error(var.parseInfoDesc(), "Unknown Transform type ", var.parseInfo()->type, ".");
		return false;
	}

	return true;
}


}
