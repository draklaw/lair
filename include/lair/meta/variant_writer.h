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


#ifndef LAIR_META_VARIANT_WRITER_H
#define LAIR_META_VARIANT_WRITER_H


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include <lair/meta/variant.h>


namespace lair
{


//class VariantWriter {
//public:
//	VariantWriter(Logger* logger = &dbgLogger, bool skipOnError = false);
//	VariantWriter(const VariantWriter&) = delete;
//	VariantWriter(VariantWriter&&)      = default;
//	~VariantWriter();

//	VariantWriter& operator=(const VariantWriter&) = delete;
//	VariantWriter& operator=(VariantWriter&&)      = default;


//	inline explicit operator bool() const {
//		return _success;
//	}


//	inline void reset() {
//		_success = true;
//	}


//	template<typename T>
//	inline T read(const Variant& var, const T& defaultValue) {
//		T value = defaultValue;
//		if(var.isValid() && (!_skipOnError || _success)) {
//			_success = _success && varRead(value, var, *_logger);
//		}
//		return value;
//	}


//	template<typename T>
//	inline bool read(T& value, const Variant& var) {
//		if(!_skipOnError || _success) {
//			_success = varRead(value, var, *_logger);
//		}
//		return _success;
//	}


//protected:
//	Logger* _logger;
//	bool    _skipOnError;
//	bool    _success;
//};


bool varWrite(Variant& var, const bool& value, Logger& logger = noopLogger);

bool varWrite(Variant& var, const int8& value, Logger& logger = noopLogger);
bool varWrite(Variant& var, const int16& value, Logger& logger = noopLogger);
bool varWrite(Variant& var, const int32& value, Logger& logger = noopLogger);
bool varWrite(Variant& var, const int64& value, Logger& logger = noopLogger);

bool varWrite(Variant& var, const uint8& value, Logger& logger = noopLogger);
bool varWrite(Variant& var, const uint16& value, Logger& logger = noopLogger);
bool varWrite(Variant& var, const uint32& value, Logger& logger = noopLogger);
bool varWrite(Variant& var, const uint64& value, Logger& logger = noopLogger);

bool varWrite(Variant& var, const float& value, Logger& logger = noopLogger);
bool varWrite(Variant& var, const double& value, Logger& logger = noopLogger);

bool varWrite(Variant& var, const String& value, Logger& logger = noopLogger);
bool varWrite(Variant& var, const Path& value, Logger& logger = noopLogger);

template<typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
bool varWrite(Variant& var, const Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& value, Logger& logger = noopLogger);

bool varWrite(Variant& var, const Transform& value, Logger& logger = noopLogger);

template<typename Scalar, int Dim>
bool varWrite(Variant& var, const Eigen::AlignedBox<Scalar, Dim>& value, Logger& logger = noopLogger);


template<typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
bool varWrite(Variant& var, const Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& value, Logger& /*logger*/) {
	VarList varList;
	varList.type() = "Vector";
	for(unsigned i = 0; i < value.size(); ++i)
		varList.emplace_back(value(i));
	var = std::move(varList);
	return true;
}


template<typename Scalar, int Dim>
bool varWrite(Variant& var, const Eigen::AlignedBox<Scalar, Dim>& value, Logger& logger) {
	VarMap varMap;
	varMap.type() = "Box";

	bool success = true;
	Variant v;

	success &= varWrite(v, value.min(), logger);
	varMap.emplace("min", std::move(v));

	success &= varWrite(v, Eigen::Matrix<Scalar, Dim, 1>(value.sizes()), logger);
	varMap.emplace("size", std::move(v));

	var = std::move(varMap);
	return success;
}


}


#endif
