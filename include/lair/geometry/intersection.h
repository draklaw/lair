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


#ifndef LAIR_GEOMETRY_INTERSECTION_H
#define LAIR_GEOMETRY_INTERSECTION_H


#include <lair/core/lair.h>

#include <lair/geometry/sphere.h>
#include <lair/geometry/aligned_box.h>
#include <lair/geometry/oriented_box.h>


namespace lair
{


// Distance to point

template<typename Scalar, int Dim>
float distance(const Sphere<Scalar, Dim>& sphere,
               const Eigen::Matrix<Scalar, Dim, 1>& p);

template<typename Scalar, int Dim>
float squaredDistance(const AlignedBox<Scalar, Dim>& box,
                      const Eigen::Matrix<Scalar, Dim, 1>& p);

template<typename Scalar, int Dim>
float distance(const AlignedBox<Scalar, Dim>& box,
               const Eigen::Matrix<Scalar, Dim, 1>& p);

template<typename Scalar, int Dim>
float squaredDistance(const OrientedBox<Scalar, Dim>& box,
                      const Eigen::Matrix<Scalar, Dim, 1>& p);

template<typename Scalar, int Dim>
float distance(const OrientedBox<Scalar, Dim>& box,
               const Eigen::Matrix<Scalar, Dim, 1>& p);


// Closest point to point

template<typename Scalar, int Dim>
Eigen::Matrix<Scalar, Dim, 1> closestPoint(const Sphere<Scalar, Dim>& sphere,
                                           const Eigen::Matrix<Scalar, Dim, 1>& p);

template<typename Scalar, int Dim>
Eigen::Matrix<Scalar, Dim, 1> closestPoint(const AlignedBox<Scalar, Dim>& box,
                                           const Eigen::Matrix<Scalar, Dim, 1>& p);

template<typename Scalar, int Dim>
Eigen::Matrix<Scalar, Dim, 1> closestPoint(const OrientedBox<Scalar, Dim>& box,
                                           const Eigen::Matrix<Scalar, Dim, 1>& p);


// Intersections

template<typename Scalar, int Dim>
bool intersect(const Sphere<Scalar, Dim>& sphere0,
               const Sphere<Scalar, Dim>& sphere1);

bool intersect(const Sphere2& box0, const AlignedBox2& box1);
bool intersect(const Sphere2& box0, const OrientedBox2& box1);
bool intersect(const AlignedBox2& box0, const AlignedBox2& box1);
bool intersect(const AlignedBox2& box0, const OrientedBox2& box1);
bool intersect(const OrientedBox2& box0, const OrientedBox2& box1);


// Distance to object

template<typename Scalar, int Dim>
float distance(const Sphere<Scalar, Dim>& sphere0,
               const Sphere<Scalar, Dim>& sphere1);

float distance(const Sphere2& box0, const AlignedBox2& box1);
float distance(const Sphere2& box0, const OrientedBox2& box1);
float distance(const AlignedBox2& box0, const AlignedBox2& box1);
float distance(const AlignedBox2& box0, const OrientedBox2& box1);
float distance(const OrientedBox2& box0, const OrientedBox2& box1);


// ////////////////////////////////////////////////////////////////////////////


template<typename Scalar, int Dim>
inline Scalar distance(const Sphere<Scalar, Dim>& sphere,
                       const Eigen::Matrix<Scalar, Dim, 1>& p) {
	Scalar d = (sphere.center() - p).norm();
	return d - sphere.radius();
}


template<typename Scalar, int Dim>
inline float squaredDistance(const AlignedBox<Scalar, Dim>& box,
                             const Eigen::Matrix<Scalar, Dim, 1>& p) {
	Scalar sqDist = Scalar(0);
	for(int i = 0; i < Dim; ++i) {
		if(p(i) < box.min()(i)) {
			float d = box.min()(i) - p(i);
			sqDist += d * d;
		}
		if(box.max()(i) < p(i)) {
			float d = p(i) - box.max()(i);
			sqDist += d * d;
		}
	}
	return sqDist;
}


template<typename Scalar, int Dim>
inline float distance(const AlignedBox<Scalar, Dim>& box,
                      const Eigen::Matrix<Scalar, Dim, 1>& p) {
	return std::sqrt(squaredDistance(box, p));
}


template<typename Scalar, int Dim>
inline float squaredDistance(const OrientedBox<Scalar, Dim>& box,
                             const Eigen::Matrix<Scalar, Dim, 1>& p) {
	Eigen::Matrix<Scalar, Dim, 1> q = box.basis().transpose() * (p - box.center());
	Scalar sqDist = Scalar(0);
	for(int i = 0; i < Dim; ++i) {
		Scalar c = std::abs(q(i));
		if(box.sizes()(i) < c) {
			float d = c - box.sizes()(i);
			sqDist += d * d;
		}
	}
	return sqDist;
}


template<typename Scalar, int Dim>
inline float distance(const OrientedBox<Scalar, Dim>& box,
                      const Eigen::Matrix<Scalar, Dim, 1>& p) {
	return std::sqrt(squaredDistance(box, p));
}


template<typename Scalar, int Dim>
inline Eigen::Matrix<Scalar, Dim, 1> closestPoint(
        const Sphere<Scalar, Dim>& sphere,
        const Eigen::Matrix<Scalar, Dim, 1>& p) {
	Eigen::Matrix<Scalar, Dim, 1> v = (p - sphere.center()).normalized();
	return sphere.center() + v * sphere.radius();
}


template<typename Scalar, int Dim>
inline Eigen::Matrix<Scalar, Dim, 1> closestPoint(
        const AlignedBox<Scalar, Dim>& box,
        const Eigen::Matrix<Scalar, Dim, 1>& p) {
	Eigen::Matrix<Scalar, Dim, 1> r;
	for(int i = 0; i < Dim; ++i) {
		r(i) = clamp(p(i), box.min()(i), box.max()(i));
	}
	return r;
}


template<typename Scalar, int Dim>
inline Eigen::Matrix<Scalar, Dim, 1> closestPoint(
        const OrientedBox<Scalar, Dim>& box,
        const Eigen::Matrix<Scalar, Dim, 1>& p) {
	Eigen::Matrix<Scalar, Dim, 1> q = box.basis().transpose() * (p - box.center());
	Eigen::Matrix<Scalar, Dim, 1> r;
	for(int i = 0; i < Dim; ++i) {
		r(i) = clamp(q(i), box.min()(i), box.max()(i));
	}
	return box.basis() * r;
}


template<typename Scalar, int Dim>
inline bool intersect(const Sphere<Scalar, Dim>& sphere0,
                      const Sphere<Scalar, Dim>& sphere1) {
	return distance(sphere0, sphere1) < Scalar(0);
}


template<typename Scalar, int Dim>
Scalar distance(const Sphere<Scalar, Dim>& sphere0,
                const Sphere<Scalar, Dim>& sphere1) {
	Scalar d = (sphere0.center() - sphere1.center()).norm();
	return d - sphere0.radius() - sphere1.radius();
}


}


#endif
