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


#include <lair/core/log.h>

#include "lair/geometry/intersection.h"


namespace lair {


bool intersect(const AlignedBox2& box0, const AlignedBox2& box1) {
	return box0.intersects(box1);
}


bool intersect(const AlignedBox2& box0, const OrientedBox2& box1) {
	// Technically, we can skip this if aabb have been checked previously.
	for(unsigned axis = 0; axis < 2; ++axis) {
		Vector2 row = box1.basis().row(axis).transpose().cwiseAbs();
		float r = row.cwiseProduct(box1.halfSize()).sum();
		if(box0.max()(axis) < box1.center()(axis) - r ||
		   box1.center()(axis) + r < box0.min()(axis))
			return false;
	}

	Vector2 c = box1.basis().transpose() * (box0.center() - box1.center());
	Vector2 h = box0.sizes() / 2;
	for(unsigned axis = 0; axis < 2; ++axis) {
		Vector2 row = box1.basis().row(axis).cwiseAbs();
		float r = row.cwiseProduct(h).sum();
		if(c(axis) + r < -box1.halfSize()(axis) ||
		   box1.halfSize()(axis) < c(axis) - r)
			return false;
	}

	return true;
}


bool intersect(const OrientedBox2& box0, const OrientedBox2& box1) {
	Vector2 c = box1.basis().transpose() * (box0.center() - box1.center());
	Vector2 h = (box1.basis().transpose() * box0.basis()).cwiseAbs() * box0.halfSize();
	for(unsigned axis = 0; axis < 2; ++axis) {
		dbgLogger.info("[", c(axis) - h(axis), ", ", c(axis) + h(axis), "] - [",
		               -box1.halfSize()(axis), ", ", box1.halfSize()(axis), "]");
		if(c(axis) + h(axis) < -box1.halfSize()(axis) ||
		   box1.halfSize()(axis) < c(axis) - h(axis))
			return false;
	}

	c = box0.basis().transpose() * (box1.center() - box0.center());
	h = (box0.basis().transpose() * box1.basis()).cwiseAbs() * box1.halfSize();
	for(unsigned axis = 0; axis < 2; ++axis) {
		if(box0.halfSize()(axis) < c(axis) - h(axis) ||
		   c(axis) + h(axis) < -box0.halfSize()(axis))
			return false;
	}

	return true;
}


}
