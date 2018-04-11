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


#ifndef _LAIR_CORE_ALIGNED_BOX_H
#define _LAIR_CORE_ALIGNED_BOX_H


#include <lair/core/lair.h>


namespace lair
{


template<typename _Scalar, int _Dim>
class AlignedBox : public Eigen::AlignedBox<_Scalar, _Dim> {
public:
	typedef _Scalar Scalar;

	enum {
		Dim = _Dim,
	};

	typedef Eigen::AlignedBox<Scalar, Dim> Super;
	typedef AlignedBox<Scalar, Dim>        Self;

	typedef Eigen::Matrix<Scalar, Dim, 1>           Vector;
	typedef Eigen::Matrix<Scalar, Dim, Dim>         Matrix;
	typedef Eigen::Matrix<Scalar, Dim + 1, Dim + 1> HMatrix;

public:
	inline AlignedBox() {
	}

	AlignedBox(const Vector& min, const Vector& max)
	    : Super(min, max) {
	}

	AlignedBox(const AlignedBox&) = default;
	AlignedBox(AlignedBox&&)      = default;
	~AlignedBox() = default;

	AlignedBox& operator=(const AlignedBox&) = default;
	AlignedBox& operator=(AlignedBox&&)      = default;

	inline Vector corner(unsigned cornerIndex) const {
		Vector c;
		for(unsigned i = 0; i < Dim; ++i)
			c(i) = (cornerIndex & (1 << i))? this->max()(i): this->min()(i);
		return c;
	}

	inline void translate(const Vector& translation) {
		this->min() += translation;
		this->max() += translation;
	}

	inline Self translated(const Vector& translation) const {
		Self other = *this;
		other.translate(translation);
		return other;
	}

	inline Self rotated(const Matrix& rotation) const {
		Self other;
		for(unsigned i = 0; i < (1 << Dim); ++i)
			other.extend(rotation * corner(i));
		return other;
	}

	inline void scale(const Vector& scale) {
		this->min() = this->min().cwiseProduct(scale);
		this->max() = this->max().cwiseProduct(scale);
	}

	inline Self scaled(const Vector& scale) const {
		Self other = *this;
		other.scale(scale);
		return other;
	}

	inline Self rigidTransformed(const HMatrix& transform) const {
		Self other;
		for(unsigned i = 0; i < (1 << Dim); ++i)
			other.extend((transform * corner(i).homogeneous()).template head<Dim>());
		return other;
	}
};


typedef AlignedBox<float, 2> AlignedBox2;
typedef AlignedBox<float, 3> AlignedBox3;


template<typename Scalar, int Dim>
std::ostream& operator<<(std::ostream& out, const AlignedBox<Scalar, Dim>& aBox) {
	out << "AlignedBox(" << fmt(aBox.min()) << ", " << fmt(aBox.sizes()) << ")";
	return out;
}


}


#endif
