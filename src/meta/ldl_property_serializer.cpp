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

#include <lair/meta/property.h>
#include <lair/meta/property_list.h>

#include "lair/meta/ldl_property_serializer.h"


namespace lair
{


LdlPropertySerializer::LdlPropertySerializer() {
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

bool LdlPropertySerializer::_read(LdlParser& parser, const PropertyList& properties, void* obj) {
	if(parser.valueType() != LdlParser::TYPE_MAP) {
		parser.error("Expected VarMap, got ", parser.valueTypeName());
		parser.skip();
		return false;
	}

	parser.enter();
	bool success = true;
	while(parser.valueType() != LdlParser::TYPE_END) {
		const String& key = parser.getKey();
		int pi = properties.propertyIndex(key);
		if(pi >= 0) {
			const Property& property = properties.property(pi);

			if(property.enumInfo()) {
				int value;
				if(parser.valueType() == LdlParser::TYPE_INT) {
					success &= ldlRead(parser, value);
				}
				else if(parser.valueType() == LdlParser::TYPE_STRING) {
					String label;
					success &= ldlRead(parser, label);
					if(!property.enumInfo()->value(value, label)) {
						parser.error("Unknown label \"", label, "\" in enum \"", property.enumInfo()->name(), "\"");
					}
				}
				else {
					parser.error("Invalid type while parsing enum: ", parser.valueTypeName());
					parser.skip();
				}
				// TODO: Huug, there must be a cleaner way to do this.
				Variant var = property.getVar(obj);
				var.setInt(value);
				property.setVar(obj, var);
			}
			else if(property.flagsInfo()) {
				if(parser.valueType() == LdlParser::TYPE_STRING) {
					String label;
					success &= ldlRead(parser, label);
					unsigned flags = property.flagsInfo()->parse(label, &parser);
					property.set<unsigned>(obj, flags);
				}
				else {
					parser.error("Invalid type while parsing flags: ", parser.valueTypeName());
					parser.skip();
				}
			}
			else {
				const ReadWrite* rw = _getReadWrite(property.type());
				if(rw && rw->read) {
					success &= rw->read(parser, obj, property);
				}
				else {
					parser.error("No reader for type ", property.type()->identifier, ": ignoring");
					parser.skip();
					success = false;
				}
			}
		}
		else {
			parser.warning("Unknown field \"", key, "\": ignoring");
			parser.skip();
		}
	}
	parser.leave();

	return success;
}

bool LdlPropertySerializer::_write(LdlWriter& writer, const PropertyList& properties, const void* obj) {
	writer.openMap();
	bool success = true;
	for(unsigned pi = 0; pi < properties.nProperties(); ++pi) {
		const Property& property = properties.property(pi);

		if(property.enumInfo()) {
			int value = property.getVar(obj).asInt();
			String label;
			if(property.enumInfo()->label(label, value)) {
				writer.writeKey(property.name());
				writer.writeString(label);
			}
			else {
				writer.warning("Enum \"", property.enumInfo()->name(), "\" with unknown value: ", value);
				writer.writeKey(property.name());
				writer.writeInt(value);
			}
		}
		else if(property.flagsInfo()) {
			unsigned flags = property.get<unsigned>(obj);
			writer.writeKey(property.name());
			writer.writeString(property.flagsInfo()->format(flags));
		}
		else {
			const ReadWrite* rw = _getReadWrite(property.type());
			if(rw && rw->write) {
				writer.writeKey(property.name());
				success &= rw->write(writer, obj, property);
			}
			else {
				writer.warning("No writer for type ", property.type()->identifier, ": ignoring");
				success = false;
			}
		}
	}
	writer.close();

	return success;
}

void LdlPropertySerializer::_registerType(const MetaType* metatype, ReadFn read, WriteFn write) {
	_map.emplace(metatype, ReadWrite{ read, write });
}

const LdlPropertySerializer::ReadWrite*
LdlPropertySerializer::_getReadWrite(const MetaType* metatype) const {
	auto it = _map.find(metatype);
	return (it != _map.end())? &it->second: nullptr;
}


}
