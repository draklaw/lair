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


#ifndef _LAIR_CORE_SPHERE_H
#define _LAIR_CORE_SPHERE_H


#include <lair/core/lair.h>

#include <lair/geometry/aligned_box.h>


namespace lair
{


template<typename _Scalar, int _Dim>
class Sphere {
public:
	typedef _Scalar Scalar;

	enum {
		Dim = _Dim,
	};

	typedef Sphere<Scalar, Dim> Self;

	typedef Eigen::Matrix<Scalar, Dim, 1>           Vector;
	typedef Eigen::Matrix<Scalar, Dim, Dim>         Matrix;
	typedef Eigen::Matrix<Scalar, Dim + 1, Dim + 1> HMatrix;

public:
	inline Sphere() {
	}

	Sphere(const Vector& center, Scalar radius)
	    : _center(center)
	    , _radius(radius)
	{
	}

	Sphere(const Sphere&) = default;
	Sphere(Sphere&&)      = default;
	~Sphere() = default;

	Sphere& operator=(const Sphere&) = default;
	Sphere& operator=(Sphere&&)      = default;

	inline const Vector& center() const { return _center; }
	inline       Vector& center()       { return _center; }

	inline Scalar  radius() const { return _radius; }
	inline Scalar& radius()       { return _radius; }

	inline AlignedBox<Scalar, Dim> boundingBox() const {
		return AlignedBox<Scalar, Dim>(_center - Vector::Constant(_radius),
		                               _center + Vector::Constant(_radius));
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
		_center = rotation * _center.homogenous();
	}

	inline Self rotated(const Matrix& rotation) const {
		Self other = *this;
		other.rotate(rotation);
		return other;
	}

	inline void scale(float scale) {
		_center *= scale;
		_radius *= scale;
	}

	inline Self scaled(float scale) const {
		Self other = *this;
		other.scale(scale);
		return other;
	}

	inline void rigidTransform(const HMatrix& transform) {
		_center  = (transform * _center.homogeneous()).template head<Dim>();
	}

	inline Self rigidTransformed(const HMatrix& transform) const {
		Self other = *this;
		other.rigidTransform(transform);
		return other;
	}

protected:
	Vector _center;
	Scalar _radius;
};


typedef Sphere<float, 2> Sphere2;
typedef Sphere<float, 3> Sphere3;


template<typename Scalar, int Dim>
std::ostream& operator<<(std::ostream& out, const Sphere<Scalar, Dim>& sphere) {
	out << "Sphere(" << fmt(sphere.center()) << ", " << sphere.radius() << ")";
	return out;
}


}


#endif
