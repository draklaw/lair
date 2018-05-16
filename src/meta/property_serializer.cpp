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
#include <lair/meta/property.h>
#include <lair/meta/property_list.h>

#include "lair/meta/property_serializer.h"


namespace lair
{


PropertySerializer::PropertySerializer() {
	registerType<bool>();

	registerType<int8>();
	registerType<int16>();
	registerType<int32>();
	registerType<int64>();

	registerType<uint8>();
	registerType<uint16>();
	registerType<uint32>();
	registerType<uint64>();

	registerType<float>();
	registerType<double>();

	registerType<String>();
	registerType<Path>();

	registerType<Vector2>();
	registerType<Vector3>();
	registerType<Vector4>();

	registerType<Vector2i>();
	registerType<Vector3i>();
	registerType<Vector4i>();

	registerType<Matrix2>();
	registerType<Matrix3>();
	registerType<Matrix4>();

//	registerType<Transform>();
//	registerType<Quaternion>();
//	registerType<AngleAxis>();
//	registerType<Translation>();

	registerType<Box2>();
	registerType<Box3>();
	registerType<Box2i>();
	registerType<Box3i>();

//	registerType<Path>();

//	registerType<BlendingMode>();
}

bool PropertySerializer::_read(void* obj, const PropertyList& properties, const Variant& var, Logger& logger) {
	if(!var.isVarMap()) {
		logger.error("Expected VarMap, got ", var.typeName());
		return false;
	}

	bool success = true;
	for(auto&& pair: var.asVarMap()) {
		const String&  key   = pair.first;
		const Variant& value = pair.second;

		int pi = properties.propertyIndex(key);
		if(pi < 0) {
			logger.warning("Unknown field \"", key, "\": ignoring");
			continue;
		}

		const Property& property = properties.property(pi);
		if(property.enumInfo()) {
			int enumValue;
			if(value.isInt()) {
				success &= varRead(enumValue, value, logger);
			}
			else if(value.isString()) {
				String label;
				success &= varRead(label, value, logger);
				if(!property.enumInfo()->value(enumValue, label)) {
					logger.error("Unknown label \"", label, "\" in enum \"", property.enumInfo()->name(), "\"");
				}
			}
			else {
				logger.error("Expected enum, got ", value.typeName());
				success = false;
			}

			if(success) {
				// This is a convoluted way to make sure that v has the type
				// of the enum. We need a way to implicitly convert objects of
				// one type to another in order to get rid of this.
				Variant v = property.getVar(obj);
				v.setInt(enumValue);
				property.setVar(obj, v);
			}
		}
		else if(property.flagsInfo()) {
			if(value.isString()) {
				String label;
				success &= varRead(label, value, logger);
				unsigned flags = property.flagsInfo()->parse(label, logger);
				property.set<unsigned>(obj, flags);
			}
			else {
				logger.error("Expected flags, got ", value.typeName());
				success = false;
			}
		}
		else {
			const ReadWrite* rw = _getReadWrite(property.type());
			if(rw && rw->read) {
				success &= rw->read(obj, property, value, logger);
			}
			else {
				logger.error("No reader for type ", property.type()->identifier, ": ignoring");
				success = false;
			}
		}
	}

	return success;
}

bool PropertySerializer::_write(Variant& var, const void* obj, const PropertyList& properties, Logger& logger) {
	VarMap map;
	bool success = true;
	for(unsigned pi = 0; pi < properties.nProperties(); ++pi) {
		const Property& property = properties.property(pi);

		if(property.enumInfo()) {
			int value = property.getVar(obj).asInt();
			String label;
			if(property.enumInfo()->label(label, value)) {
				map.emplace(property.name(), label);
			}
			else {
				logger.warning("Enum \"", property.enumInfo()->name(), "\" with unknown value: ", value);
				map.emplace(property.name(), value);
			}
		}
		else if(property.flagsInfo()) {
			unsigned flags = property.get<unsigned>(obj);
			map.emplace(property.name(), property.flagsInfo()->format(flags));
		}
		else {
			const ReadWrite* rw = _getReadWrite(property.type());
			if(rw && rw->write) {
				Variant v;
				success &= rw->write(v, obj, property, logger);
				if(success) {
					map.emplace(property.name(), std::move(v));
				}
			}
			else {
				logger.warning("No writer for type ", property.type()->identifier, ": ignoring");
				success = false;
			}
		}
	}

	var = map;

	return success;
}

void PropertySerializer::_registerType(const MetaType* metatype, ReadFn read, WriteFn write) {
	_map.emplace(metatype, ReadWrite{ read, write });
}

const PropertySerializer::ReadWrite*
PropertySerializer::_getReadWrite(const MetaType* metatype) const {
	auto it = _map.find(metatype);
	return (it != _map.end())? &it->second: nullptr;
}


}
