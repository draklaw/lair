/*
 *  Copyright (C) 2015 Simon Boyé
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


#include <gtest/gtest.h>

#include <lair/ec/property.h>


//using namespace lair;


class TestClass : public lair::Properties<TestClass> {
public:
	TestClass()
		: _count (42)
		, _radius(.5)
		, _pos   (1, 2)
		, _name  ("test")
	{
	}

	const int& getCount() const {
		return _count;
	}

	void setCount(const int& count) {
		_count = count;
	}

	const float& getRadius() const {
		return _radius;
	}

	void setRadius(const float& radius) {
		_radius = radius;
	}

	const lair::Vector2& getPos() const {
		return _pos;
	}

	void setPos(const lair::Vector2& pos) {
		_pos = pos;
	}

	const std::string& getName() const {
		return _name;
	}

	void setName(const std::string& name) {
		_name = name;
	}

	enum {
		P_COUNT,
		P_RADIUS,
		P_POS,
		P_NAME,

		N_PROPERTIES,
	};
	const lair::PropertyList& properties() const {
		static lair::PropertyList props;
		if(props.nProperties() == 0) {
			props.addProperty("count",  &TestClass::getCount,  &TestClass::setCount);
			props.addProperty("radius", &TestClass::getRadius, &TestClass::setRadius);
			props.addProperty("pos",    &TestClass::getPos,    &TestClass::setPos);
			props.addProperty("name",   &TestClass::getName,   &TestClass::setName);
		}
		return props;
	}

private:
	int           _count;
	float         _radius;
	lair::Vector2 _pos;
	std::string   _name;
};


TEST(PropertyTest, TestPropertyTypes) {
	ASSERT_EQ("Int32", LAIR_EC_PROPERTY_TYPE(int32_t)->name);
	ASSERT_EQ("Int32", LAIR_EC_PROPERTY_TYPE(int32_t&)->name);
	ASSERT_EQ("Int32", LAIR_EC_PROPERTY_TYPE(const int32_t)->name);
	ASSERT_EQ("Int32", LAIR_EC_PROPERTY_TYPE(const int32_t&)->name);

	ASSERT_EQ( "Int8",  LAIR_EC_PROPERTY_TYPE( int8_t )->name);
	ASSERT_EQ("UInt8",  LAIR_EC_PROPERTY_TYPE(uint8_t )->name);
	ASSERT_EQ( "Int16", LAIR_EC_PROPERTY_TYPE( int16_t)->name);
	ASSERT_EQ("UInt16", LAIR_EC_PROPERTY_TYPE(uint16_t)->name);
	ASSERT_EQ( "Int32", LAIR_EC_PROPERTY_TYPE( int32_t)->name);
	ASSERT_EQ("UInt32", LAIR_EC_PROPERTY_TYPE(uint32_t)->name);
	ASSERT_EQ( "Int64", LAIR_EC_PROPERTY_TYPE( int64_t)->name);
	ASSERT_EQ("UInt64", LAIR_EC_PROPERTY_TYPE(uint64_t)->name);

	ASSERT_EQ("Scalar", LAIR_EC_PROPERTY_TYPE(lair::Scalar)->name);

	ASSERT_EQ("String", LAIR_EC_PROPERTY_TYPE(std::string)->name);

	ASSERT_EQ("Vector2",  LAIR_EC_PROPERTY_TYPE(lair::Vector2 )->name);
	ASSERT_EQ("Vector3",  LAIR_EC_PROPERTY_TYPE(lair::Vector3 )->name);
	ASSERT_EQ("Vector4",  LAIR_EC_PROPERTY_TYPE(lair::Vector4 )->name);
	ASSERT_EQ("Vector2i", LAIR_EC_PROPERTY_TYPE(lair::Vector2i)->name);
	ASSERT_EQ("Vector3i", LAIR_EC_PROPERTY_TYPE(lair::Vector3i)->name);
	ASSERT_EQ("Vector4i", LAIR_EC_PROPERTY_TYPE(lair::Vector4i)->name);
	ASSERT_EQ("Matrix2",  LAIR_EC_PROPERTY_TYPE(lair::Matrix2 )->name);
	ASSERT_EQ("Matrix3",  LAIR_EC_PROPERTY_TYPE(lair::Matrix3 )->name);
	ASSERT_EQ("Matrix4",  LAIR_EC_PROPERTY_TYPE(lair::Matrix4 )->name);

	ASSERT_EQ("Transform",  LAIR_EC_PROPERTY_TYPE(lair::Transform)->name);
	ASSERT_EQ("Quaternion", LAIR_EC_PROPERTY_TYPE(lair::Quaternion)->name);

	ASSERT_EQ("Box2",  LAIR_EC_PROPERTY_TYPE(lair::Box2 )->name);
	ASSERT_EQ("Box3",  LAIR_EC_PROPERTY_TYPE(lair::Box3 )->name);
	ASSERT_EQ("Box2i", LAIR_EC_PROPERTY_TYPE(lair::Box2i)->name);
	ASSERT_EQ("Box3i", LAIR_EC_PROPERTY_TYPE(lair::Box3i)->name);

	ASSERT_EQ("Path", LAIR_EC_PROPERTY_TYPE(lair::Path)->name);

	ASSERT_EQ("BlendingMode", LAIR_EC_PROPERTY_TYPE(lair::BlendingMode)->name);

	const lair::PropertyType* blendModeType = LAIR_EC_PROPERTY_TYPE(lair::BlendingMode);
	ASSERT_EQ("none", *blendModeType->findEnum(lair::BLEND_NONE));
}

TEST(PropertyTest, TestGetSet) {
	TestClass obj;

	ASSERT_EQ(TestClass::N_PROPERTIES, obj.nProperties());

	ASSERT_EQ(TestClass::P_COUNT,  obj.propertyIndex("count"));
	ASSERT_EQ(TestClass::P_RADIUS, obj.propertyIndex("radius"));
	ASSERT_EQ(TestClass::P_POS,    obj.propertyIndex("pos"));
	ASSERT_EQ(TestClass::P_NAME,   obj.propertyIndex("name"));

	int           count  = obj.getCount();
	float         radius = obj.getRadius();
	lair::Vector2 pos    = obj.getPos();
	std::string   name   = obj.getName();

	ASSERT_EQ(count,  obj.get<int>          (TestClass::P_COUNT));
	ASSERT_EQ(count,  obj.get<int>          ("count"));
	ASSERT_EQ(radius, obj.get<float>        (TestClass::P_RADIUS));
	ASSERT_EQ(radius, obj.get<float>        ("radius"));
	ASSERT_EQ(pos,    obj.get<lair::Vector2>(TestClass::P_POS));
	ASSERT_EQ(pos,    obj.get<lair::Vector2>("pos"));
	ASSERT_EQ(name,   obj.get<std::string>  (TestClass::P_NAME));
	ASSERT_EQ(name,   obj.get<std::string>  ("name"));

	count  = 123;
	radius = -12.34;
	pos    = lair::Vector2(-1265, 786);
	name   = "aoeu";

	obj.set<int>          (TestClass::P_COUNT, count);
	obj.set<float>        ("radius",           radius);
	obj.set<lair::Vector2>(TestClass::P_POS,   pos);
	obj.set<std::string>  ("name",             name);

	ASSERT_EQ(count,  obj.getCount());
	ASSERT_EQ(radius, obj.getRadius());
	ASSERT_EQ(pos,    obj.getPos());
	ASSERT_EQ(name,   obj.getName());

	ASSERT_EQ(count,  obj.get<int>          (TestClass::P_COUNT));
	ASSERT_EQ(count,  obj.get<int>          ("count"));
	ASSERT_EQ(radius, obj.get<float>        (TestClass::P_RADIUS));
	ASSERT_EQ(radius, obj.get<float>        ("radius"));
	ASSERT_EQ(pos,    obj.get<lair::Vector2>(TestClass::P_POS));
	ASSERT_EQ(pos,    obj.get<lair::Vector2>("pos"));
	ASSERT_EQ(name,   obj.get<std::string>  (TestClass::P_NAME));
	ASSERT_EQ(name,   obj.get<std::string>  ("name"));

	ASSERT_THROW(obj.get<float>(TestClass::P_COUNT), lair::AssertionFailedError);
	ASSERT_THROW(obj.set(TestClass::P_COUNT, 1.5f), lair::AssertionFailedError);
}

