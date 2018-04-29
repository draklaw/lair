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


#ifndef LAIR_GEOMETRY_SHAPE_2D_H
#define LAIR_GEOMETRY_SHAPE_2D_H


#include <lair/core/lair.h>
#include <lair/core/ldl.h>

#include <lair/meta/metatype.h>

#include <lair/geometry/sphere.h>
#include <lair/geometry/aligned_box.h>
#include <lair/geometry/oriented_box.h>


namespace lair
{


enum Shape2DType {
	SHAPE_NONE,
	SHAPE_SPHERE,
	SHAPE_ALIGNED_BOX,
	SHAPE_ORIENTED_BOX,
};


class Shape2D {
public:
	inline Shape2D()
	    : _type(SHAPE_NONE)
	    , _shape(nullptr)
	{}

	inline Shape2D(const Sphere2& sphere)
	    : _type(SHAPE_SPHERE)
	    , _shape(new Sphere2(sphere))
	{}

	inline Shape2D(const AlignedBox2& box)
	    : _type(SHAPE_ALIGNED_BOX)
	    , _shape(new AlignedBox2(box))
	{}

	inline Shape2D(const OrientedBox2& oBox)
	    : _type(SHAPE_ORIENTED_BOX)
	    , _shape(new OrientedBox2(oBox))
	{}

	Shape2D(const Shape2D& other);
	Shape2D(Shape2D&& other);

	~Shape2D();

	Shape2D& operator=(Shape2D other);

	inline Shape2DType type()   const { return _type; }
	inline bool isValid()       const { return _type != SHAPE_NONE; }
	inline bool isSphere()      const { return _type == SHAPE_SPHERE; }
	inline bool isAlignedBox()  const { return _type == SHAPE_ALIGNED_BOX; }
	inline bool isOrientedBox() const { return _type == SHAPE_ORIENTED_BOX; }

	inline const Sphere2& asSphere() const {
		lairAssert(isSphere());
		return *static_cast<Sphere2*>(_shape);
	}

	inline const AlignedBox2& asAlignedBox() const {
		lairAssert(isAlignedBox());
		return *static_cast<AlignedBox2*>(_shape);
	}

	inline const OrientedBox2& asOrientedBox() const {
		lairAssert(isOrientedBox());
		return *static_cast<OrientedBox2*>(_shape);
	}

	Shape2D transformed(const Matrix3& transform) const;

	inline Shape2D transformed(const Matrix4& transform) const {
		Matrix3 m;
		m << transform.topLeftCorner<2, 2>(), transform.topRightCorner<2, 1>(),
		     0, 0, 1;
		return transformed(m);
	}

	inline Shape2D transformed(const Transform& transform) const {
		return transformed(transform.matrix());
	}

	AlignedBox2 boundingBox() const;

	bool intersect(const Shape2D& other, Vector2* position = 0) const;

	void swap(Shape2D& other);

protected:
	Shape2DType _type;
	void*       _shape;
};


std::ostream& operator<<(std::ostream& out, const Shape2D& shape);


typedef std::vector<Shape2D> Shape2DVector;


bool ldlRead(LdlParser& parser, Shape2D& value);
bool ldlWrite(LdlWriter& writer, const Shape2D& value);

bool ldlRead(LdlParser& parser, Shape2DVector& value);
bool ldlWrite(LdlWriter& writer, const Shape2DVector& value);


}


LAIR_DECLARE_METATYPE(lair::Shape2D)
LAIR_DECLARE_METATYPE(lair::Shape2DVector)


#endif
