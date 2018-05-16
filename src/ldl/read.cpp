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

#include <lair/meta/var_list.h>
#include <lair/meta/var_map.h>

#include "lair/ldl/read.h"


namespace lair
{


bool ldlRead(LdlParser& parser, bool& value) {
	switch(parser.valueType()) {
	case LdlParser::TYPE_BOOL:
		value = parser.getBool();
		parser.next();
		return true;
	case LdlParser::TYPE_INT:
		parser.warning("Expected Bool, got ", parser.valueTypeName(), ": implicit conversion");
		value = (parser.getInt() != 0);
		parser.next();
		return true;
	case LdlParser::TYPE_FLOAT:
		parser.warning("Expected Bool, got ", parser.valueTypeName(), ": implicit conversion");
		value = (parser.getFloat() != 0);
		parser.next();
		return true;
	case LdlParser::TYPE_NULL:
	case LdlParser::TYPE_STRING:
	case LdlParser::TYPE_LIST:
	case LdlParser::TYPE_MAP:
		parser.error("Expected Bool, got ", parser.valueTypeName(), ": ignoring");
		parser.skip();
		return false;
	default:
		lairAssert(false);
	}
	lairAssert(false);
	return false;
}

#define DECL_LDL_READ_INT(_type, _typeName) \
bool ldlRead(LdlParser& parser, _type& value) { \
	int64  min = std::numeric_limits<_type>::min(); \
	uint64 max = std::numeric_limits<_type>::max(); \
	switch(parser.valueType()) { \
	case LdlParser::TYPE_BOOL: \
	    parser.warning("Expected " _typeName ", got ", parser.valueTypeName(), ": implicit conversion"); \
	    value = parser.getBool()? 1: 0; \
	    parser.next(); \
	    return true; \
	case LdlParser::TYPE_INT: \
	    if(parser.getInt() < min) \
	        value = min; \
	    else if(parser.getInt() > 0 && uint64(parser.getInt()) > max) \
	        value = max; \
	    else \
	        value = parser.getInt(); \
	    if(int64(value) != parser.getInt()) \
	        parser.warning("Out of bound value of type " _typeName ": ", parser.getInt(), \
	                       " clamped to ", value); \
	    parser.next(); \
	    return true; \
	case LdlParser::TYPE_FLOAT: \
	    value = parser.getFloat(); \
	    if(double(value) != parser.getFloat()) \
	        parser.warning("Expected " _typeName ", got ", parser.valueTypeName(), \
	                       ": ", parser.getFloat(), " rounded to ", value); \
	    parser.next(); \
	    return true; \
	case LdlParser::TYPE_NULL: \
	case LdlParser::TYPE_STRING: \
	case LdlParser::TYPE_LIST: \
	case LdlParser::TYPE_MAP: \
	    parser.error("Expected " _typeName ", got ", parser.valueTypeName(), ": ignoring"); \
	    parser.skip(); \
	    return false; \
	default: \
	    lairAssert(false); \
    } \
	lairAssert(false); \
	return false; \
}

DECL_LDL_READ_INT(int8,  "Int8")
DECL_LDL_READ_INT(int16, "Int16")
DECL_LDL_READ_INT(int32, "Int32")
DECL_LDL_READ_INT(int64, "Int64")

DECL_LDL_READ_INT(uint8,  "UInt8")
DECL_LDL_READ_INT(uint16, "UInt16")
DECL_LDL_READ_INT(uint32, "UInt32")
DECL_LDL_READ_INT(uint64, "UInt64")

#define DECL_LDL_READ_FLOAT(_type, _typeName) \
bool ldlRead(LdlParser& parser, _type& value) { \
	switch(parser.valueType()) { \
	case LdlParser::TYPE_BOOL: \
	    parser.warning("Expected " _typeName ", got ", parser.valueTypeName(), ": implicit conversion"); \
	    value = parser.getBool()? 1: 0; \
	    parser.next(); \
	    return true; \
	case LdlParser::TYPE_INT: \
	    value = parser.getInt(); \
	    parser.next(); \
	    return true; \
	case LdlParser::TYPE_FLOAT: \
	    value = parser.getFloat(); \
	    parser.next(); \
	    return true; \
	case LdlParser::TYPE_NULL: \
	case LdlParser::TYPE_STRING: \
	case LdlParser::TYPE_LIST: \
	case LdlParser::TYPE_MAP: \
	    parser.error("Expected " _typeName ", got ", parser.valueTypeName(), ": ignoring"); \
	    parser.skip(); \
	    return false; \
	default: \
	    lairAssert(false); \
    } \
	lairAssert(false); \
	return false; \
}

DECL_LDL_READ_FLOAT(float,  "Float")
DECL_LDL_READ_FLOAT(double, "Double")


#define DECL_LDL_READ_STRING(_type, _typeName) \
bool ldlRead(LdlParser& parser, _type& value) { \
	switch(parser.valueType()) { \
	case LdlParser::TYPE_STRING: \
	    value = parser.getString(); \
	    parser.next(); \
	    return true; \
	case LdlParser::TYPE_BOOL: \
	case LdlParser::TYPE_INT: \
	case LdlParser::TYPE_FLOAT: \
	case LdlParser::TYPE_NULL: \
	case LdlParser::TYPE_LIST: \
	case LdlParser::TYPE_MAP: \
	    parser.error("Expected " _typeName ", got ", parser.valueTypeName(), ": ignoring"); \
	    parser.skip(); \
	    return false; \
	default: \
	    lairAssert(false); \
    } \
	lairAssert(false); \
	return false; \
}

DECL_LDL_READ_STRING(String, "String")
DECL_LDL_READ_STRING(Path,   "Path")

bool ldlRead(LdlParser& parser, Transform& value) {
	if(parser.valueType() != LdlParser::TYPE_LIST) {
		parser.error("Expected Transform (VarList), got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	bool success = true;
	if(!parser.isValueTyped() || parser.getValueTypeName() == "Transform") {
		parser.enter();
		value = Transform::Identity();
		while(parser.valueType() != LdlParser::TYPE_END) {
			Transform trans = Transform::Identity();
			success &= ldlRead(parser, trans);
			value = value * trans;
		}
		parser.leave();
	}
	else if(parser.getValueTypeName() == "translate" || parser.getValueTypeName() == "Vector") {
		Vector3 vec;
		success &= ldlRead(parser, vec, NO_ANNOTATION_WARNING);

		value.setIdentity();
		value.translate(vec);
	}
	else if(parser.getValueTypeName() == "rotate") {
		if(parser.valueType() != LdlParser::TYPE_LIST) {
			parser.error("Rotate transform expected (VarList), got ", parser.valueTypeName());
			parser.skip();
			return false;
		}

		Vector4  vec;
		unsigned index    = 0;
		bool     vSuccess = true;
		parser.enter();
		for(; parser.valueType() != LdlParser::TYPE_END; ++index) {
			if(index < 4) {
				vSuccess &= ldlRead(parser, vec(index));
			}
		}

		if(vSuccess) {
			if(index == 1) {
				value.setIdentity();
				value.rotate(AngleAxis(vec(0) * M_PI / 180, Vector3(0, 0, 1)));
			}
			else if(index == 4) {
				value.setIdentity();
				value.rotate(AngleAxis(vec(0) * M_PI / 180, vec.tail<3>()));
			}
			else {
				parser.error("Invalid number of arguments in rotate transform, expected 1 or 4, got ", index);
				success = false;
			}
		}
		success &= vSuccess;

		parser.leave();
	}
	else if(parser.getValueTypeName() == "scale") {
		if(parser.valueType() != LdlParser::TYPE_LIST) {
			parser.error("Scale transform expected (VarList), got ", parser.valueTypeName());
			parser.skip();
			return false;
		}

		Vector3  vec(1, 1, 1);
		unsigned index    = 0;
		bool     vSuccess = true;
		parser.enter();
		for(; parser.valueType() != LdlParser::TYPE_END; ++index) {
			if(index < 3) {
				vSuccess &= ldlRead(parser, vec(index));
			}
		}

		if(vSuccess) {
			switch(index) {
			case 1:
				value.setIdentity();
				value.scale(vec(0));
				break;
			case 2:
			case 3:
				value.setIdentity();
				value.scale(vec);
				break;
			default:
				parser.error("Invalid number of arguments in scale transform, expected 1 to 3, got ", index);
				success = false;
				break;
			}
		}
		success &= vSuccess;

		parser.leave();
	}

	return success;
}


bool ldlRead(LdlParser& parser, Variant& value) {
	bool success = true;

	Variant::ParseInfo parseInfo = {
	    parser.streamName(),
	    parser.line(),
	    parser.col(),
	};
	String type = parser.isValueTyped()? parser.getValueTypeName(): String();

	switch(parser.valueType()) {
	case LdlParser::TYPE_ERROR:
	case LdlParser::TYPE_END:
		success = false;
		parser.error("Unexpected type: ", parser.valueTypeName());
		break;
	case LdlParser::TYPE_NULL:
		value.clear();
		parser.next();
		break;
	case LdlParser::TYPE_BOOL:
		value = Variant(parser.getBool());
		parser.next();
		break;
	case LdlParser::TYPE_INT:
		value = Variant(parser.getInt());
		parser.next();
		break;
	case LdlParser::TYPE_FLOAT:
		value = Variant(parser.getFloat());
		parser.next();
		break;
	case LdlParser::TYPE_STRING:
		value = Variant(parser.getString());
		parser.next();
		break;
	case LdlParser::TYPE_LIST: {
		VarList list;
		success = success && ldlRead(parser, list);
		value = Variant(std::move(list));
		break;
	}
	case LdlParser::TYPE_MAP: {
		VarMap map;
		success = success && ldlRead(parser, map);
		value = Variant(std::move(map));
		break;
	}
	}

	value.setParseInfo(parseInfo);
	if(value.isVarList())
		value.asVarList().type() = type;
	if(value.isVarMap())
		value.asVarMap().type() = type;

	return success;
}


bool ldlRead(LdlParser& parser, VarList& value) {
	if(parser.valueType() != LdlParser::TYPE_LIST) {
		parser.error("Expected VarList, got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	bool success = true;
	value.clear();

	parser.enter();

	while(success && parser.valueType() != LdlParser::TYPE_END) {
		Variant var;
		success = success && ldlRead(parser, var);
		if(success)
			value.emplace_back(std::move(var));
	}

	while(parser.valueType() != LdlParser::TYPE_END) {
		parser.next();
	}

	parser.leave();

	return success;
}


bool ldlRead(LdlParser& parser, VarMap&  value) {
	if(parser.valueType() != LdlParser::TYPE_MAP) {
		parser.error("Expected VarMap, got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	bool success = true;
	value.clear();

	parser.enter();

	while(success && parser.valueType() != LdlParser::TYPE_END) {
		String key = parser.getKey();
		if(value.count(key)) {
			parser.warning("Duplicate key \"", key, "\": ignoring");
		}
		else {
			Variant var;
			success = success && ldlRead(parser, var);
			if(success) {
				value.emplace(key, std::move(var));
			}
		}
	}

	while(parser.valueType() != LdlParser::TYPE_END) {
		parser.next();
	}

	parser.leave();

	return success;
}


}
