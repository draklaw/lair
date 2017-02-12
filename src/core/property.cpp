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
#include "lair/core/path.h"
#include "lair/core/property.h"

#include <lair/render_gl2/render_pass.h>



namespace lair
{


Property::Property(const MetaType* type, const std::string& name,
                   const EnumInfo* enumInfo, const FlagsInfo* flagsInfo)
    : _type     (type)
    , _enumInfo (enumInfo)
    , _flagsInfo(flagsInfo)
    , _name     (name)
{
}

const MetaType* Property::type() const {
	return _type;
}

const EnumInfo* Property::enumInfo() const {
	return _enumInfo;
}

const FlagsInfo* Property::flagsInfo() const {
	return _flagsInfo;
}

const std::string& Property::name() const {
	return _name;
}


unsigned PropertyList::nProperties() const {
	return _properties.size();
}

int PropertyList::propertyIndex(const std::string& propertyName) const {
	auto it = _propertyMap.find(propertyName);
	return (it == _propertyMap.end())? -1: it->second;
}

const Property& PropertyList::property(unsigned index) const {
	lairAssert(index < _properties.size());
	return *_properties[index];
}



//JsonPropertySerializer JsonPropertySerializer::createWithLairTypes() {
//	JsonPropertySerializer self;

//	self.registerType<bool>();

//	self.registerType<int8>();
//	self.registerType<int16>();
//	self.registerType<int32>();
//	self.registerType<int64>();

//	self.registerType<uint8>();
//	self.registerType<uint16>();
//	self.registerType<uint32>();
//	self.registerType<uint64>();

//	self.registerType<float>();
//	self.registerType<double>();

//	self.registerType<String>();

//	self.registerType<Vector2>();
//	self.registerType<Vector3>();
//	self.registerType<Vector4>();

//	self.registerType<Vector2i>();
//	self.registerType<Vector3i>();
//	self.registerType<Vector4i>();

//	self.registerType<Matrix2>();
//	self.registerType<Matrix3>();
//	self.registerType<Matrix4>();

////	self.registerType<Transform>();
////	self.registerType<Quaternion>();
////	self.registerType<AngleAxis>();
////	self.registerType<Translation>();

////	self.registerType<Box2>();
////	self.registerType<Box3>();
////	self.registerType<Box2i>();
////	self.registerType<Box3i>();

//	self.registerType<Path>();

////	self.registerType<BlendingMode>();

//	return self;
//}

//void JsonPropertySerializer::_registerType(const MetaType* type, Serialize serialize, Deserialize deserialize) {
//	lairAssert(_functions.count(type) == 0);
//	_functions.emplace(type, Functions{serialize, deserialize});
//}

//bool JsonPropertySerializer::_serialize(Json::Value& json, const void* obj, const Property& property, ErrorList* errors) const {
//	if(property.enumInfo()) {
//		int value = property.getVar(obj).asInt();
//		String label;
//		if(property.enumInfo()->label(label, value)) {
//			json = label;
//		}
//		else {
//			if(errors) {
//				errors->warning("Enum \"", property.enumInfo()->name(), "\" with unknown value: ", value);
//			}
//			json = value;
//		}
//		return true;
//	}

//	if(property.flagsInfo()) {
//		unsigned flags = property.get<unsigned>(obj);
//		json = property.flagsInfo()->format(flags);
//		return true;
//	}

//	auto it = _functions.find(property.type());
//	if(it == _functions.end() || !it->second.serialize) {
//		if(errors) {
//			errors->error("No serializer found for type \"", property.type()->identifier, "\"");
//		}
//		return false;
//	}
//	return it->second.serialize(json, obj, property, errors);
//}

//bool JsonPropertySerializer::_deserialize(void* obj, const Property& property, const Json::Value& json, ErrorList* errors) const {
//	if(property.enumInfo()) {
//		int value;
//		if(json.isInt()) {
//			value = json.asInt();
//		}
//		else if(json.isString()) {
//			if(!property.enumInfo()->value(value, json.asString())) {
//				if(errors) {
//					errors->error("Unknown label \"", json.asString(), "\" in enum \"", property.enumInfo()->name(), "\"");
//				}
//				return false;
//			}
//		}
//		else {
//			if(errors) {
//				errors->error("Invalid type while parsing enum: ", jsonTypeName(json));;
//			}
//			return false;
//		}
//		// TODO: Huug, there must be a cleaner way to do this.
//		Variant var = property.getVar(obj);
//		var.setInt(value);
//		property.setVar(obj, var);
//		return true;
//	}

//	if(property.flagsInfo()) {
//		if(!json.isString()) {
//			if(errors) {
//				errors->error("Invalid type while parsing flags: ", jsonTypeName(json));
//			}
//			return false;
//		}
//		unsigned flags = property.flagsInfo()->parse(json.asString(), errors);
//		property.set<unsigned>(obj, flags);
//		return true;
//	}

//	auto it = _functions.find(property.type());
//	if(it == _functions.end() || !it->second.deserialize) {
//		if(errors) {
//			errors->error("No deserializer found for type \"", property.type()->identifier, "\"");
//		}
//		return false;
//	}

//	return it->second.deserialize(obj, property, json, errors);
//}

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
					unsigned flags = property.flagsInfo()->parse(label, nullptr);
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
