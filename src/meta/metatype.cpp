/*
 *  Copyright (C) 2017-2018 Simon Boyé
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


#include <cstdlib>

#include "lair/meta/metatype.h"


namespace lair {


unsigned MetaTypeManager::_register(MetaType metaType) {
	lairAssert(_metaTypeMap.count(metaType.identifier) == 0);

	metaType.index = _metaTypes.size();
	_metaTypes.emplace_back(MetaTypePtr(new MetaType(metaType)));
	_metaTypeMap.emplace(metaType.identifier, _metaTypes[metaType.index].get());

	return metaType.index;
}

MetaTypeManager& MetaTypeManager::_getInstance() {
	static MetaTypeManager metaTypes;
	return metaTypes;
}

MetaTypeManager& metaTypes = MetaTypeManager::_getInstance();


}


LAIR_IMPLEMENT_METATYPE(bool,  "Bool");

LAIR_IMPLEMENT_METATYPE(lair::int8,  "Int8");
LAIR_IMPLEMENT_METATYPE(lair::int16, "Int16");
LAIR_IMPLEMENT_METATYPE(lair::int32, "Int32");
LAIR_IMPLEMENT_METATYPE(lair::int64, "Int64");

LAIR_IMPLEMENT_METATYPE(lair::uint8,  "UInt8");
LAIR_IMPLEMENT_METATYPE(lair::uint16, "UInt16");
LAIR_IMPLEMENT_METATYPE(lair::uint32, "UInt32");
LAIR_IMPLEMENT_METATYPE(lair::uint64, "UInt64");

LAIR_IMPLEMENT_METATYPE(float,  "Float");
LAIR_IMPLEMENT_METATYPE(double, "Double");

LAIR_IMPLEMENT_METATYPE(lair::String, "String");

LAIR_IMPLEMENT_METATYPE(lair::Vector2, "Vector2f");
LAIR_IMPLEMENT_METATYPE(lair::Vector3, "Vector3f");
LAIR_IMPLEMENT_METATYPE(lair::Vector4, "Vector4f");

LAIR_IMPLEMENT_METATYPE(lair::Vector2i, "Vector2i");
LAIR_IMPLEMENT_METATYPE(lair::Vector3i, "Vector3i");
LAIR_IMPLEMENT_METATYPE(lair::Vector4i, "Vector4i");

LAIR_IMPLEMENT_METATYPE(lair::Matrix2, "Matrix2f");
LAIR_IMPLEMENT_METATYPE(lair::Matrix3, "Matrix3f");
LAIR_IMPLEMENT_METATYPE(lair::Matrix4, "Matrix4f");

LAIR_IMPLEMENT_METATYPE(lair::Transform,   "Transform");
LAIR_IMPLEMENT_METATYPE(lair::Quaternion,  "Quaternion");
LAIR_IMPLEMENT_METATYPE(lair::AngleAxis,   "AngleAxis");
LAIR_IMPLEMENT_METATYPE(lair::Translation, "Translation");

LAIR_IMPLEMENT_METATYPE(lair::Box2,  "Box2f");
LAIR_IMPLEMENT_METATYPE(lair::Box3,  "Box3f");
LAIR_IMPLEMENT_METATYPE(lair::Box2i, "Box2i");
LAIR_IMPLEMENT_METATYPE(lair::Box3i, "Box3i");
