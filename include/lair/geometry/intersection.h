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


bool intersect(const AlignedBox2& box0, const AlignedBox2& box1,
               Vector2* position = nullptr);


}


#endif
