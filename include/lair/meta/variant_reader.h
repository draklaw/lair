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


#ifndef LAIR_META_VARIANT_READER_H
#define LAIR_META_VARIANT_READER_H


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include <lair/meta/variant.h>


namespace lair
{


class VariantReader {
public:
	VariantReader(Logger* logger = &dbgLogger, bool skipOnError = false);
	VariantReader(const VariantReader&) = delete;
	VariantReader(VariantReader&&)      = default;
	~VariantReader();

	VariantReader& operator=(const VariantReader&) = delete;
	VariantReader& operator=(VariantReader&&)      = default;


	inline explicit operator bool() const {
		return _success;
	}


	inline void reset() {
		_success = true;
	}


	template<typename T>
	inline T read(const Variant& var, const T& defaultValue) {
		T value = defaultValue;
		if(var.isValid() && (!_skipOnError || _success)) {
			_success = _success && varRead(value, var, *_logger);
		}
		return value;
	}


	template<typename T>
	inline bool read(T& value, const Variant& var) {
		if(!_skipOnError || _success) {
			_success = varRead(value, var, *_logger);
		}
		return _success;
	}


protected:
	Logger* _logger;
	bool    _skipOnError;
	bool    _success;
};


bool varRead(bool& value, const Variant& var, Logger& logger = noopLogger);

bool varRead(int8& value, const Variant& var, Logger& logger = noopLogger);
bool varRead(int16& value, const Variant& var, Logger& logger = noopLogger);
bool varRead(int32& value, const Variant& var, Logger& logger = noopLogger);
bool varRead(int64& value, const Variant& var, Logger& logger = noopLogger);

bool varRead(uint8& value, const Variant& var, Logger& logger = noopLogger);
bool varRead(uint16& value, const Variant& var, Logger& logger = noopLogger);
bool varRead(uint32& value, const Variant& var, Logger& logger = noopLogger);
bool varRead(uint64& value, const Variant& var, Logger& logger = noopLogger);

bool varRead(float& value, const Variant& var, Logger& logger = noopLogger);
bool varRead(double& value, const Variant& var, Logger& logger = noopLogger);

bool varRead(String& value, const Variant& var, Logger& logger = noopLogger);
bool varRead(Path& value, const Variant& var, Logger& logger = noopLogger);

template<typename Derived>
bool varRead(Eigen::DenseBase<Derived> const & value, const Variant& var, Logger& logger = noopLogger, bool annotationWarning = false);

bool varRead(Transform& value, const Variant& var, Logger& logger = noopLogger);

template<typename Scalar, int Dim>
bool varRead(Eigen::AlignedBox<Scalar, Dim>& value, const Variant& var, Logger& logger = noopLogger);



template<typename Derived>
bool varRead(Eigen::DenseBase<Derived> const & value_, const Variant& var, Logger& logger, bool annotationWarning) {
	if(!var.isVarList()) {
		logger.error(var.parseInfoDesc(), "Expected Vector, got ", var.typeName(), ".");
		return false;
	}

	if(annotationWarning && var.parseInfo() && var.parseInfo()->type != "Vector") {
		logger.warning("Invalid type annotation while reading Vector: ", var.parseInfo()->type, ".");
	}

	Eigen::DenseBase<Derived>& value = const_cast<Eigen::DenseBase<Derived>&>(value_);

	unsigned rows = value.rows();
	unsigned cols = value.cols();

	const VarList& varList = var.asVarList();
	if(varList.size() == rows * cols) {
		unsigned i = 0;
		for(const Variant& v: varList) {
			if(!varRead(value(i++), v, logger)) {
				return false;
			}
		}
	}
	else {
		logger.error(var.parseInfoDesc(), "Incorrect number of coefficient in Vector/Matrix, expected ",
		             rows * cols, ", got ", varList.size(), ".");
		return false;
	}

	return true;
}


template<typename Scalar, int Dim>
bool varRead(Eigen::AlignedBox<Scalar, Dim>& value, const Variant& var, Logger& logger) {
	if(!var.isVarMap()) {
		logger.error(var.parseInfoDesc(), "Expected Box, got ", var.typeName(), ".");
		return false;
	}

	typedef Eigen::Matrix<Scalar, Dim, 1> Vector;

	const Variant& minVar  = var.get("min");
	const Variant& maxVar  = var.get("max");
	const Variant& sizeVar = var.get("size");

	if(!minVar.isValid()) {
		logger.error(var.parseInfoDesc(), "Error while reading Box: missing field \"min\".");
		return false;
	}
	if(!maxVar.isValid() && !sizeVar.isValid()) {
		logger.error(var.parseInfoDesc(), "Error while reading Box: missing field \"max\" or \"size\".");
		return false;
	}
	if(maxVar.isValid() && sizeVar.isValid()) {
		logger.error(var.parseInfoDesc(), "Error while reading Box: fields \"max\" and \"size\" can not be present at the same time.");
		return false;
	}

	Vector min;
	if(!varRead(min, minVar, logger)) {
		return false;
	}

	if(maxVar.isValid()) {
		Vector max;
		if(!varRead(max, maxVar, logger)) {
			return false;
		}

		value.min() = min;
		value.max() = max;
	}
	else if(sizeVar.isValid()) {
		Vector size;
		if(!varRead(size, maxVar, logger)) {
			return false;
		}

		value.min() = min;
		value.max() = min + size;
	}
	else {
		lairAssert(false);
	}

	return true;
}


}


#endif
