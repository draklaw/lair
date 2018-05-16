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


#ifndef LAIR_LDL_READ_H
#define LAIR_LDL_READ_H


#include <lair/core/lair.h>

#include <lair/meta/variant.h>

#include <lair/fs/abstract_file_system.h>

#include <lair/ldl/ldl_parser.h>


namespace lair
{


bool parseLdl(Variant& value, std::istream& in, const Path& localPath, Logger& log);
bool parseLdl(Variant& value, const Path& realPath, const Path& localPath, Logger& log);
bool parseLdl(Variant& value, const VirtualFile& file, const Path& localPath, Logger& log);


bool ldlRead(LdlParser& parser, bool&   value);
bool ldlRead(LdlParser& parser, uint8&  value);
bool ldlRead(LdlParser& parser,  int8&  value);
bool ldlRead(LdlParser& parser, uint16& value);
bool ldlRead(LdlParser& parser,  int16& value);
bool ldlRead(LdlParser& parser, uint32& value);
bool ldlRead(LdlParser& parser,  int32& value);
bool ldlRead(LdlParser& parser, uint64& value);
bool ldlRead(LdlParser& parser,  int64& value);
bool ldlRead(LdlParser& parser, float&  value);
bool ldlRead(LdlParser& parser, double& value);
bool ldlRead(LdlParser& parser, String& value);
bool ldlRead(LdlParser& parser, Path&   value);

template<typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
bool ldlRead(LdlParser& parser, Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& value, unsigned flags);

template<typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
bool ldlRead(LdlParser& parser, Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& value);

bool ldlRead(LdlParser& parser, Transform& value);

template<typename Scalar, int Dim>
bool ldlRead(LdlParser& parser, Eigen::AlignedBox<Scalar, Dim>& value);

bool ldlRead(LdlParser& parser, Variant& value);
bool ldlRead(LdlParser& parser, VarList& value);
bool ldlRead(LdlParser& parser, VarMap&  value);


// ////////////////////////////////////////////////////////////////////////////


template<typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
bool ldlRead(LdlParser& parser, Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& value, unsigned flags) {
	typedef Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols> Matrix;

	if(parser.valueType() != LdlParser::TYPE_LIST) {
		parser.error("Expected Vector/Matrix (VarList), got ", parser.valueTypeName(), ": ignoring");
		parser.skip();
		return false;
	}

	lairAssert(value.rows() > 0 && value.cols() > 0);
	if(value.rows() == 1 || value.cols() == 1) {
		if(parser.isValueTyped()) {
			if((flags & WITH_ANNOTATION_WARNING)
			&& parser.getValueTypeName() != "Vector") {
				parser.warning("Unexpected type annotation: expected Vector, got ", parser.getValueTypeName());
			}
		}
		else {
//			parser.warning("Expected type annotation \"Vector\"");
		}

		unsigned dim = std::max(value.cols(), value.rows());
		Matrix tmp(value.rows(), value.cols());

		parser.enter();

		bool success = true;
		for(unsigned i = 0; success && i < dim; ++i) {
			if(parser.valueType() != LdlParser::TYPE_END)
				success &= ldlRead(parser, tmp(i));
			else
				tmp(i) = (i == 3)? Scalar(1): Scalar(0);
		}

		if(success && parser.valueType() != LdlParser::TYPE_END) {
			parser.warning("To much coefficient in Vector", dim);
		}
		while(parser.valueType() != LdlParser::TYPE_END)
			parser.skip();

		parser.leave();

		if(success)
			value = tmp;
		return success;
	}
	else {
		if(parser.isValueTyped()) {
			if((flags & WITH_ANNOTATION_WARNING)
			&& parser.getValueTypeName() != "Matrix") {
				parser.warning("Unexpected type annotation: expected Matrix, got ", parser.getValueTypeName());
			}
		}
		else {
//			parser.warning("Expected type annotation \"Matrix\"");
		}

		unsigned rows = value.rows();
		unsigned cols = value.cols();
		Matrix tmp;

		parser.enter();

		bool success = true;
		for(unsigned r = 0; success && r < rows; ++r) {
			if(parser.valueType() != LdlParser::TYPE_END) {
				if(parser.valueType() == LdlParser::TYPE_LIST) {
					parser.enter();

					for(unsigned c = 0; success && c < cols; ++c) {
						if(parser.valueType() != LdlParser::TYPE_END)
							success &= ldlRead(parser, tmp(r, c));
						else
							tmp(r, c) = (r == c)? Scalar(1): Scalar(0);
					}

					if(success && parser.valueType() != LdlParser::TYPE_END) {
						parser.warning("To much coefficient in Matrix", rows, "x", cols, " row");
					}

					while(parser.valueType() != LdlParser::TYPE_END)
						parser.skip();

					parser.leave();
				}
				else {
					parser.error("Expected Matrix row (VarList), got ", parser.valueTypeName(), ": ignoring");
					parser.skip();
					success = false;
				}
			}
			else {
				for(unsigned c = 0; c < cols; ++c)
					tmp(r, c) = (r == c)? Scalar(1): Scalar(0);
			}
		}

		if(success && parser.valueType() != LdlParser::TYPE_END) {
			parser.warning("To much rows in Matrix", rows, "x", cols);
		}

		while(parser.valueType() != LdlParser::TYPE_END)
			parser.skip();

		parser.leave();

		if(success)
			value = tmp;
		return success;
	}

	return true;
}


template<typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
bool ldlRead(LdlParser& parser, Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& value) {
	return ldlRead(parser, value, WITH_ANNOTATION_WARNING);
}


template<typename Scalar, int Dim>
bool ldlRead(LdlParser& parser, Eigen::AlignedBox<Scalar, Dim>& value) {
	typedef Eigen::AlignedBox<Scalar, Dim> Box;

	if(parser.valueType() == LdlParser::TYPE_NULL) {
		value = Box();
		parser.next();
		return true;
	}

	if(parser.valueType() != LdlParser::TYPE_MAP) {
		parser.error("Expected Box (VarMap), got ", parser.valueTypeName(), ": ignoring");
		parser.skip();
		return false;
	}

	if(parser.isValueTyped()) {
		if(parser.getValueTypeName() != "Box") {
			parser.warning("Unexpected type annotation: expected Box, got ", parser.getValueTypeName());
		}
	}
	else {
//		parser.warning("Expected type annotation \"Box\"");
	}

	static unsigned minIndex  = 0;
	static unsigned maxIndex  = 1;
	static unsigned sizeIndex = 2;
	Eigen::Matrix<Scalar, Dim, 1> vec[3];
	bool success = true;
	bool has[] = { false, false, false };

	parser.enter();
	while(parser.valueType() != LdlParser::TYPE_END) {
		int index = -1;
		if(success && parser.getKey() == "min")
			index = 0;
		else if(success && parser.getKey() == "max")
			index = 1;
		else if(success && parser.getKey() == "size")
			index = 2;

		if(index >= 0) {
			if(has[index]) {
				parser.warning("Duplicate key \"", parser.getKey(), "\": ignoring");
			}
			else {
				has[index] = ldlRead(parser, vec[index]);
				success    = success && has[index];
			}
		}
		else {
			parser.warning("Unexpected key \"", parser.getKey(), "\": ignoring");
			parser.skip();
		}
	}

	if(!success) {
		parser.leave();
		return false;
	}

	if(!has[minIndex] || (!has[maxIndex] && !has[sizeIndex])) {
		parser.error("Box require attributes \"min\" and either \"max\" or \"size\"");
		parser.leave();
		return false;
	}
	if(has[maxIndex] && has[sizeIndex]) {
		parser.warning("Both \"max\" and \"size\" set for a Box: size will be ignored");
	}

	value.min() = vec[minIndex];
	if(has[maxIndex])
		value.max() = vec[maxIndex];
	else
		value.max() = vec[minIndex] + vec[sizeIndex];

	for(int i = 0; i < Dim; ++i) {
		if(value.max()(i) < value.min()(i)) {
			parser.warning("Box has a max coefficient smaller than min: the box will be empty");
			break;
		}
	}

	parser.leave();
	return true;
}


}


#endif
