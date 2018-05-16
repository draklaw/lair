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


#ifndef LAIR_LDL_WRITE_H
#define LAIR_LDL_WRITE_H


#include <lair/core/lair.h>

#include <lair/meta/variant.h>

#include <lair/ldl/ldl_writer.h>


namespace lair
{


template<typename T>
String toLdlString(const T& value) {
	std::ostringstream out;
	LdlWriter writer(&out, "<string>", nullptr);
	writer.openList();
	ldlWrite(writer, value);
	writer.close();
	return out.str();
}


bool ldlWrite(LdlWriter& writer, const bool&   value);
bool ldlWrite(LdlWriter& writer, const int8&   value);
bool ldlWrite(LdlWriter& writer, const int16&  value);
bool ldlWrite(LdlWriter& writer, const int32&  value);
bool ldlWrite(LdlWriter& writer, const int64&  value);
bool ldlWrite(LdlWriter& writer, const uint8&  value);
bool ldlWrite(LdlWriter& writer, const uint16& value);
bool ldlWrite(LdlWriter& writer, const uint32& value);
bool ldlWrite(LdlWriter& writer, const uint64& value);
bool ldlWrite(LdlWriter& writer, const float&  value);
bool ldlWrite(LdlWriter& writer, const double& value);
bool ldlWrite(LdlWriter& writer, const String& value);
bool ldlWrite(LdlWriter& writer, const Path&   value);

template<typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
bool ldlWrite(LdlWriter& writer, const Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& value) {
	bool success = true;

	if(value.rows() == 1 || value.cols() == 1) {
		unsigned size = std::max(value.rows(), value.cols());

		writer.openList(LdlWriter::CF_SINGLE_LINE, "Vector");
		for(unsigned i = 0; i < size; ++i)
			success &= ldlWrite(writer, value(i));
		writer.close();
	}
	else {
		unsigned rows = value.rows();
		unsigned cols = value.cols();

		writer.openList(LdlWriter::CF_MULTI_LINE, "Matrix");
		for(unsigned r = 0; r < rows; ++r) {
			writer.openList(LdlWriter::CF_SINGLE_LINE);
			for(unsigned c = 0; c < cols; ++c)
				success &= ldlWrite(writer, value(r, c));
			writer.close();
		}
		writer.close();
	}

	return success;
}

bool ldlWrite(LdlWriter& writer, const Transform& value);

template<typename Scalar, int Dim>
bool ldlWrite(LdlWriter& writer, const Eigen::AlignedBox<Scalar, Dim>& value) {
	typedef Eigen::AlignedBox<Scalar, Dim> Box;
	typedef typename Box::VectorType Vector;

	writer.openMap(LdlWriter::CF_SINGLE_LINE, "Box");

	writer.writeKey("min");
	ldlWrite(writer, value.min());

	Vector size = value.max() - value.min();
	writer.writeKey("size");
	ldlWrite(writer, size);

	writer.close();

	return true;
}

bool ldlWrite(LdlWriter& writer, const Variant& value);
bool ldlWrite(LdlWriter& writer, const VarList& value);
bool ldlWrite(LdlWriter& writer, const VarMap&  value);


}


#endif
