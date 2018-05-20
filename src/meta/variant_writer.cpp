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

#include "lair/meta/variant_writer.h"


namespace lair
{


//VariantWriter::VariantWriter(Logger* logger, bool skipOnError)
//    : _logger(logger)
//    , _skipOnError(skipOnError)
//    , _success(true)
//{
//}


//VariantWriter::~VariantWriter() {
//}


#define IMPL_VAR_WRITE_INT(_type, _typename) \
	bool varWrite(Variant& var, const _type& value, Logger& /*logger*/) { \
	    var = value; \
	    return true; \
    }

IMPL_VAR_WRITE_INT(bool, "Bool")

IMPL_VAR_WRITE_INT(int8,  "Int8")
IMPL_VAR_WRITE_INT(int16, "Int16")
IMPL_VAR_WRITE_INT(int32, "Int32")
IMPL_VAR_WRITE_INT(int64, "Int64")

IMPL_VAR_WRITE_INT(uint8,  "UInt8")
IMPL_VAR_WRITE_INT(uint16, "UInt16")
IMPL_VAR_WRITE_INT(uint32, "UInt32")
IMPL_VAR_WRITE_INT(uint64, "UInt64")


bool varWrite(Variant& var, const float& value, Logger& /*logger*/) {
	var = value;
	return true;
}


bool varWrite(Variant& var, const double& value, Logger& /*logger*/) {
	var = value;
	return true;
}


bool varWrite(Variant& var, const String& value, Logger& /*logger*/) {
	var = value;
	return true;
}


bool varWrite(Variant& var, const Path& value, Logger& /*logger*/) {
	var = value.utf8String();
	return true;
}


bool varWrite(Variant& var, const Transform& value, Logger& logger) {
	bool success = true;

	Variant trans;
	success &= varWrite(trans, Vector3(value.translation()), logger);
	trans.asVarList().type() = "translate";

	if(value.linear().isApprox(Matrix3::Identity())) {
		var = std::move(trans);
		return success;
	}

	VarList varList(VarList::INLINE);
	varList.emplace_back(std::move(trans));

	Matrix3 rot;
	Matrix3 scale;
	value.computeScalingRotation(&scale, &rot);

	if(!rot.isApprox(Matrix3::Identity())) {
		Eigen::AngleAxisf aa(rot);

		VarList rot(VarList::INLINE | VarList::CALL);
		if(aa.axis().isApprox(Vector3::UnitZ())) {
			rot.emplace_back(aa.angle() * 180 / M_PI);
		}
		else if(aa.axis().isApprox(-Vector3::UnitZ())) {
			rot.emplace_back(-aa.angle() * 180 / M_PI);
		}
		else {
			rot.emplace_back(aa.angle() * 180 / M_PI);
			for(unsigned i = 0; i < 3; ++i)
				rot.emplace_back(aa.axis()(i));
		}
		rot.type() = "rotate";
		varList.emplace_back(std::move(rot));
	}

	if(!scale.isApprox(Matrix3::Identity())) {
		VarList scl(VarList::INLINE | VarList::CALL);
		for(unsigned i = 0; i < 3; ++i)
			scl.emplace_back(scale(i, i));
		scl.type() = "scale";
		varList.emplace_back(std::move(scl));
	}

	var = std::move(varList);
	return success;
}


}
