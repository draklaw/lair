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

#include "lair/ldl/write.h"


namespace lair
{


bool ldlWrite(LdlWriter& writer, const bool& value) {
	writer.writeBool(value);
	return true;
}

#define DECL_LDL_WRITE_INT(_type) \
bool ldlWrite(LdlWriter& writer, const _type& value) { \
	uint64 max = std::numeric_limits<_type>::max(); \
	int64  v   = value; \
	if(value > 0 && uint64(value) > max) { \
	    writer.warning("Out-of-bound interger: clamping ", value, " to ", max); \
	    v = max; \
    } \
	writer.writeInt(v); \
	return true; \
}

DECL_LDL_WRITE_INT(int8)
DECL_LDL_WRITE_INT(int16)
DECL_LDL_WRITE_INT(int32)
DECL_LDL_WRITE_INT(int64)
DECL_LDL_WRITE_INT(uint8)
DECL_LDL_WRITE_INT(uint16)
DECL_LDL_WRITE_INT(uint32)
DECL_LDL_WRITE_INT(uint64)

#define DECL_LDL_WRITE_FLOAT(_type) \
bool ldlWrite(LdlWriter& writer, const _type& value) { \
	writer.writeFloat(value); \
	return true; \
}

DECL_LDL_WRITE_FLOAT(float)
DECL_LDL_WRITE_FLOAT(double)

bool ldlWrite(LdlWriter& writer, const String& value) {
	writer.writeString(value);
	return true;
}

bool ldlWrite(LdlWriter& writer, const Path& value) {
	writer.writeString(value.utf8String(), LdlWriter::SF_DOUBLE);
	return true;
}

bool ldlWrite(LdlWriter& writer, const Transform& value) {
	bool compound = !value.linear().isApprox(Matrix3::Identity());
	bool success = true;

	if(compound) {
		writer.openList(LdlWriter::CF_SINGLE_LINE);
	}

	writer.openList(LdlWriter::CF_SINGLE_LINE, "translate");
	for(unsigned i = 0; i < 3; ++i)
		writer.writeFloat(value(i, 3));
	writer.close();

	if(compound) {
		Matrix3 rot;
		Matrix3 scale;
		value.computeScalingRotation(&scale, &rot);

		if(!rot.isApprox(Matrix3::Identity())) {
			Eigen::AngleAxisf aa(rot);

			writer.openList(LdlWriter::CF_SINGLE_LINE, "rotate");
			if(aa.axis().isApprox(Vector3::UnitZ())) {
				writer.writeFloat(aa.angle() * 180 / M_PI);
			}
			else if(aa.axis().isApprox(-Vector3::UnitZ())) {
				writer.writeFloat(-aa.angle() * 180 / M_PI);
			}
			else {
				writer.writeFloat(aa.angle() * 180 / M_PI);
				for(unsigned i = 0; i < 3; ++i)
					writer.writeFloat(aa.axis()(i));
			}
			writer.close();
		}

		if(!scale.isApprox(Matrix3::Identity())) {
			writer.openList(LdlWriter::CF_SINGLE_LINE, "scale");
			for(unsigned i = 0; i < 3; ++i)
				writer.writeFloat(scale(i, i));
			writer.close();
		}

		writer.close();
	}

	return success;
}


bool ldlWrite(LdlWriter& writer, const Variant& value) {
	if(value.isNull()) {
		writer.writeNull();
		return true;
	}
	if(value.is<bool>()) {
		return ldlWrite(writer, value.asBool());
	}
	if(value.isInt()) {
		return ldlWrite(writer, value.asInt());
	}
	if(value.isFloat()) {
		return ldlWrite(writer, value.asFloat());
	}
	if(value.isString()) {
		return ldlWrite(writer, value.asString());
	}
	if(value.isVarList()) {
		return ldlWrite(writer, value.asVarList());
	}
	if(value.isVarMap()) {
		return ldlWrite(writer, value.asVarMap());
	}

	writer.error("Cannot write variant containing type \"", value.typeName(), "\", write null instead.");
	writer.writeNull();

	return false;
}


bool ldlWrite(LdlWriter& writer, const VarList& value) {
	bool success = true;

	LdlWriter::CompoundFormat format = value.isInline()?
	                                       LdlWriter::CF_SINGLE_LINE:
	                                       LdlWriter::CF_MULTI_LINE;
	writer.openList(format, value.type());
	for(auto&& var: value) {
		success = success && ldlWrite(writer, var);
	}
	writer.close();

	return success;
}


bool ldlWrite(LdlWriter& writer, const VarMap&  value) {
	bool success = true;

	LdlWriter::CompoundFormat format = value.isInline()?
	                                       LdlWriter::CF_SINGLE_LINE:
	                                       LdlWriter::CF_MULTI_LINE;
	writer.openMap(format, value.type());
	for(auto&& pair: value) {
		writer.writeKey(pair.first);
		success = success && ldlWrite(writer, pair.second);
	}
	writer.close();

	return success;
}


}
