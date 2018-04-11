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


#ifndef _LAIR_CORE_ORIENTED_BOX_H
#define _LAIR_CORE_ORIENTED_BOX_H


#include <lair/core/lair.h>


namespace lair
{


template<typename Scalar, int Dim>
inline Eigen::Matrix<Scalar, Dim, 1> signVector(unsigned sign) {
	Eigen::Matrix<Scalar, Dim, 1> v;
	for(unsigned i = 0; i < Dim; ++i)
		v(i) = (sign & (1 << i))? Scalar(1): Scalar(-1);
	return v;
}


template<typename _Scalar, int _Dim>
class OrientedBox {
public:
	typedef _Scalar Scalar;

	enum {
		Dim = _Dim,
	};

	typedef OrientedBox<Scalar, Dim> Self;

	typedef Eigen::Matrix<Scalar, Dim, 1>           Vector;
	typedef Eigen::Matrix<Scalar, Dim, Dim>         Matrix;
	typedef Eigen::Matrix<Scalar, Dim + 1, Dim + 1> HMatrix;

public:
	inline OrientedBox() {
	}

	OrientedBox(const Vector& center, const Vector& sizes,
	            const Matrix& basis = Matrix::Identity())
	    : _center   (center)
	    , _basis    (basis)
	    , _halfSizes(sizes / 2)
	{
	}

	OrientedBox(const OrientedBox&) = default;
	OrientedBox(OrientedBox&&)      = default;
	~OrientedBox() = default;

	OrientedBox& operator=(const OrientedBox&) = default;
	OrientedBox& operator=(OrientedBox&&)      = default;

	inline const Vector& center() const { return _center; }
	inline       Vector& center()       { return _center; }

	inline const Matrix& basis() const { return _basis; }
	inline       Matrix& basis()       { return _basis; }

	inline const Vector& halfSize() const { return _halfSizes; }
	inline       Vector& halfSize()       { return _halfSizes; }

	inline Vector sizes() const { return Scalar(2) * _halfSizes; }

	inline Vector corner(unsigned cornerIndex) const {
		return _center + _basis * _halfSizes.cwiseProduct(signVector<Scalar, Dim>(cornerIndex));
	}

	inline AlignedBox<Scalar, Dim> boundingBox() const {
		AlignedBox<Scalar, Dim> box;
		for(unsigned i = 0; i < (1 << Dim); ++i)
			box.extend(corner(i));
		return box;
	}

	inline void translate(const Vector& translation) {
		_center += translation;
	}

	inline Self translated(const Vector& translation) const {
		Self other = *this;
		other.translate(translation);
		return other;
	}

	inline void rotate(const Matrix& rotation) {
		_center = rotation * _center;
		_basis  = rotation * _basis;
	}

	inline Self rotated(const Matrix& rotation) const {
		Self other = *this;
		other.rotate(rotation);
		return other;
	}

	inline void scale(const Vector& scale) {
		_center    = _center.cwiseProduct(scale);
		_halfSizes = _halfSizes.cwiseProduct(scale);
	}

	inline Self scaled(const Vector& scale) const {
		Self other = *this;
		other.scale(scale);
		return other;
	}

	inline void rigidTransform(const HMatrix& transform) {
		_center = (transform * _center.homogeneous()).template head<Dim>();
		_basis  = transform.template topLeftCorner<Dim, Dim>() * _basis;
	}

	inline Self rigidTransformed(const HMatrix& transform) const {
		Self other = *this;
		other.rigidTransform(transform);
		return other;
	}

protected:
	Vector _center;
	Matrix _basis;
	Vector _halfSizes;
};


typedef OrientedBox<float, 2> OrientedBox2;
typedef OrientedBox<float, 3> OrientedBox3;


template<typename Scalar, int Dim>
std::ostream& operator<<(std::ostream& out, const OrientedBox<Scalar, Dim>& oBox) {
	out << "OrientedBox(" << fmt(oBox.center()) << ", " << fmt(oBox.sizes())
	    << ", " << fmt(oBox.basis().col(0)) << ", " << fmt(oBox.basis().col(1))<< ")";
	return out;
}


}


#endif
