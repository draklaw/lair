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

#include <lair/core/path.h>
#include <lair/core/property.h>


//using namespace lair;

// BlendingMode and texture flags are copied from render_gl2

enum BlendingMode {
	BLEND_NONE,
	BLEND_ALPHA,
	BLEND_ADD,
	BLEND_MULTIPLY
};
const lair::EnumInfo* blendingModeInfo() {
	static lair::EnumInfo info("BlendingMode");
	if(!info.nFields()) {
		info.add(BLEND_NONE,     "none");
		info.add(BLEND_ALPHA,    "alpha");
		info.add(BLEND_ADD,      "add");
		info.add(BLEND_MULTIPLY, "multiply");
	}
	return &info;
}

LAIR_REGISTER_METATYPE(BlendingMode, "BlendingMode");

enum {
	MAG_NEAREST        = 0 << 0,
	MAG_LINEAR         = 1 << 0,

	MIN_NEAREST        = 0 << 1,
	MIN_LINEAR         = 1 << 1,

	MIPMAP_NONE        = 0 << 2,
	MIPMAP_NEAREST     = 1 << 2,
	MIPMAP_LINEAR      = 2 << 2,

	NEAREST            = MAG_NEAREST | MIN_NEAREST | MIPMAP_NONE,
	BILINEAR_NO_MIPMAP = MAG_LINEAR  | MIN_LINEAR  | MIPMAP_NONE,
	BILINEAR_MIPMAP    = MAG_LINEAR  | MIN_LINEAR  | MIPMAP_NEAREST,
	TRILINEAR          = MAG_LINEAR  | MIN_LINEAR  | MIPMAP_LINEAR,

	REPEAT_S           = 0 << 4,
	CLAMP_S            = 1 << 4,
	MIRROR_S           = 2 << 4,

	REPEAT_T           = 0 << 6,
	CLAMP_T            = 1 << 6,
	MIRROR_T           = 2 << 6,

	REPEAT             = REPEAT_S | REPEAT_T,
	CLAMP              =  CLAMP_S |  CLAMP_T,
	MIRROR             = MIRROR_S | MIRROR_T,

	MAG_MASK           = (1 << 0),
	MIN_MASK           = (1 << 1),
	MIPMAP_MASK        = (1 << 2) | (1 << 3),
	FILTER_MASK        = MAG_MASK | MIN_MASK | MIPMAP_MASK,
	WRAP_S_MASK        = (1 << 4) | (1 << 5),
	WRAP_T_MASK        = (1 << 6) | (1 << 7),
	WRAP_MASK          = WRAP_S_MASK | WRAP_T_MASK
};
const lair::FlagsInfo* flagsInfo() {
	static lair::FlagsInfo info("TextureFlags");
	if(!info.nFlagSet()) {
		info.add(NEAREST,            FILTER_MASK, "nearest");
		info.add(BILINEAR_NO_MIPMAP, FILTER_MASK, "bilinear_no_mipmap");
		info.add(BILINEAR_MIPMAP,    FILTER_MASK, "bilinear_mipmap");
		info.add(TRILINEAR,          FILTER_MASK, "trilinear");

		info.add(REPEAT, WRAP_MASK, "repeat");
		info.add(CLAMP,  WRAP_MASK, "clamp");
		info.add(MIRROR, WRAP_MASK, "mirror");

		info.add(MAG_NEAREST, MAG_MASK, "mag_nearest");
		info.add(MAG_LINEAR,  MAG_MASK, "mag_linear");

		info.add(MIN_NEAREST, MIN_MASK, "min_nearest");
		info.add(MIN_LINEAR,  MIN_MASK, "min_linear");

		// MIMAP_NONE do not need to be explicit.
		info.add(MIPMAP_NEAREST, MIPMAP_MASK, "mipmap_nearest");
		info.add(MIPMAP_LINEAR,  MIPMAP_MASK, "mipmap_linear");

		info.add(REPEAT_S, WRAP_S_MASK, "repeat_s");
		info.add(CLAMP_S,  WRAP_S_MASK, "clamp_s");
		info.add(MIRROR_S, WRAP_S_MASK, "mirror_s");

		info.add(REPEAT_T, WRAP_T_MASK, "repeat_t");
		info.add(CLAMP_T,  WRAP_T_MASK, "clamp_t");
		info.add(MIRROR_T, WRAP_T_MASK, "mirror_t");
	}
	return &info;
}


class TestClass : public lair::WithProperties<TestClass> {
public:
	TestClass()
		: _count   (42)
		, _radius  (.5)
		, _pos     (1, 2, 3, 4)
		, _name    ("test")
		, _blend   (BLEND_NONE)
		, _texFlags(BILINEAR_MIPMAP | REPEAT)
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

	const lair::Vector4& getPos() const {
		return _pos;
	}

	void setPos(const lair::Vector4& pos) {
		_pos = pos;
	}

	const std::string& getName() const {
		return _name;
	}

	void setName(const std::string& name) {
		_name = name;
	}

	const BlendingMode& getBlendingMode() const {
		return _blend;
	}

	void setBlendingMode(const BlendingMode& bm) {
		_blend = bm;
	}

	const unsigned& getTexFlags() const {
		return _texFlags;
	}

	void setTexFlags(const unsigned& tf) {
		_texFlags = tf;
	}

	enum {
		P_COUNT,
		P_RADIUS,
		P_POS,
		P_NAME,
		P_BLEND,
		P_TEXFLAGS,

		N_PROPERTIES,
	};
	const lair::PropertyList& properties() const {
		static lair::PropertyList props;
		if(props.nProperties() == 0) {
			props.addProperty("count",  &TestClass::getCount,  &TestClass::setCount);
			props.addProperty("radius", &TestClass::getRadius, &TestClass::setRadius);
			props.addProperty("pos",    &TestClass::getPos,    &TestClass::setPos);
			props.addProperty("name",   &TestClass::getName,   &TestClass::setName);
			props.addProperty("blendingMode", blendingModeInfo(),
			                  &TestClass::getBlendingMode, &TestClass::setBlendingMode);
			props.addProperty("texFlags", flagsInfo(),
			                  &TestClass::getTexFlags, &TestClass::setTexFlags);
		}
		return props;
	}

private:
	int           _count;
	float         _radius;
	lair::Vector4 _pos;
	std::string   _name;
	BlendingMode  _blend;
	unsigned      _texFlags;
};


TEST(PropertyTest, TestPropertyTypes) {
	ASSERT_EQ("Int32", lair::metaTypes.get<int32_t>()->identifier);
	ASSERT_EQ("Int32", lair::metaTypes.get<int32_t&>()->identifier);
	ASSERT_EQ("Int32", lair::metaTypes.get<const int32_t>()->identifier);
	ASSERT_EQ("Int32", lair::metaTypes.get<const int32_t&>()->identifier);

	ASSERT_EQ( "Int8",  lair::metaTypes.get< int8_t >()->identifier);
	ASSERT_EQ("UInt8",  lair::metaTypes.get<uint8_t >()->identifier);
	ASSERT_EQ( "Int16", lair::metaTypes.get< int16_t>()->identifier);
	ASSERT_EQ("UInt16", lair::metaTypes.get<uint16_t>()->identifier);
	ASSERT_EQ( "Int32", lair::metaTypes.get< int32_t>()->identifier);
	ASSERT_EQ("UInt32", lair::metaTypes.get<uint32_t>()->identifier);
	ASSERT_EQ( "Int64", lair::metaTypes.get< int64_t>()->identifier);
	ASSERT_EQ("UInt64", lair::metaTypes.get<uint64_t>()->identifier);

	ASSERT_EQ("Float",  lair::metaTypes.get<float >()->identifier);
	ASSERT_EQ("Double", lair::metaTypes.get<double>()->identifier);

	ASSERT_EQ("String", lair::metaTypes.get<std::string>()->identifier);

	ASSERT_EQ("Vector2f", lair::metaTypes.get<lair::Vector2 >()->identifier);
	ASSERT_EQ("Vector3f", lair::metaTypes.get<lair::Vector3 >()->identifier);
	ASSERT_EQ("Vector4f", lair::metaTypes.get<lair::Vector4 >()->identifier);
	ASSERT_EQ("Vector2i", lair::metaTypes.get<lair::Vector2i>()->identifier);
	ASSERT_EQ("Vector3i", lair::metaTypes.get<lair::Vector3i>()->identifier);
	ASSERT_EQ("Vector4i", lair::metaTypes.get<lair::Vector4i>()->identifier);
	ASSERT_EQ("Matrix2f", lair::metaTypes.get<lair::Matrix2 >()->identifier);
	ASSERT_EQ("Matrix3f", lair::metaTypes.get<lair::Matrix3 >()->identifier);
	ASSERT_EQ("Matrix4f", lair::metaTypes.get<lair::Matrix4 >()->identifier);

	ASSERT_EQ("Transform",  lair::metaTypes.get<lair::Transform>()->identifier);
	ASSERT_EQ("Quaternion", lair::metaTypes.get<lair::Quaternion>()->identifier);

	ASSERT_EQ("Box2f", lair::metaTypes.get<lair::Box2 >()->identifier);
	ASSERT_EQ("Box3f", lair::metaTypes.get<lair::Box3 >()->identifier);
	ASSERT_EQ("Box2i", lair::metaTypes.get<lair::Box2i>()->identifier);
	ASSERT_EQ("Box3i", lair::metaTypes.get<lair::Box3i>()->identifier);

	ASSERT_EQ("Path", lair::metaTypes.get<lair::Path>()->identifier);

	ASSERT_EQ("BlendingMode", lair::metaTypes.get<BlendingMode>()->identifier);

//	const lair::PropertyType* blendModeType = lair::metaTypes.get<BlendingMode>();
//	ASSERT_EQ("none", *blendModeType->findEnum(lair::BLEND_NONE));
}

TEST(PropertyTest, TestGetSet) {
	TestClass obj;

	ASSERT_EQ(TestClass::N_PROPERTIES, obj.nProperties());

	ASSERT_EQ(TestClass::P_COUNT,    obj.propertyIndex("count"));
	ASSERT_EQ(TestClass::P_RADIUS,   obj.propertyIndex("radius"));
	ASSERT_EQ(TestClass::P_POS,      obj.propertyIndex("pos"));
	ASSERT_EQ(TestClass::P_NAME,     obj.propertyIndex("name"));
	ASSERT_EQ(TestClass::P_BLEND,    obj.propertyIndex("blendingMode"));
	ASSERT_EQ(TestClass::P_TEXFLAGS, obj.propertyIndex("texFlags"));

	int           count    = obj.getCount();
	float         radius   = obj.getRadius();
	lair::Vector4 pos      = obj.getPos();
	std::string   name     = obj.getName();
	BlendingMode  blend    = obj.getBlendingMode();
	unsigned      texFlags = obj.getTexFlags();

	ASSERT_EQ(count,    obj.get<int>          (TestClass::P_COUNT));
	ASSERT_EQ(count,    obj.get<int>          ("count"));
	ASSERT_EQ(radius,   obj.get<float>        (TestClass::P_RADIUS));
	ASSERT_EQ(radius,   obj.get<float>        ("radius"));
	ASSERT_EQ(pos,      obj.get<lair::Vector4>(TestClass::P_POS));
	ASSERT_EQ(pos,      obj.get<lair::Vector4>("pos"));
	ASSERT_EQ(name,     obj.get<std::string>  (TestClass::P_NAME));
	ASSERT_EQ(name,     obj.get<std::string>  ("name"));
	ASSERT_EQ(blend,    obj.get<BlendingMode> (TestClass::P_BLEND));
	ASSERT_EQ(blend,    obj.get<BlendingMode> ("blendingMode"));
	ASSERT_EQ(texFlags, obj.get<unsigned>     (TestClass::P_TEXFLAGS));
	ASSERT_EQ(texFlags, obj.get<unsigned>     ("texFlags"));

	count  = 123;
	radius = -12.34;
	pos    = lair::Vector4(-1265, 786, 888, 111);
	name   = "aoeu";
	blend  = BLEND_MULTIPLY;
	texFlags = MIN_LINEAR | MAG_NEAREST | MIRROR;

	obj.set<int>          (TestClass::P_COUNT, count);
	obj.set<float>        ("radius",           radius);
	obj.set<lair::Vector4>(TestClass::P_POS,   pos);
	obj.set<std::string>  ("name",             name);
	obj.set<BlendingMode> (TestClass::P_BLEND, blend);
	obj.set<unsigned>     ("texFlags",         texFlags);

	ASSERT_EQ(count,    obj.getCount());
	ASSERT_EQ(radius,   obj.getRadius());
	ASSERT_EQ(pos,      obj.getPos());
	ASSERT_EQ(name,     obj.getName());
	ASSERT_EQ(blend,    obj.getBlendingMode());
	ASSERT_EQ(texFlags, obj.getTexFlags());

	ASSERT_EQ(count,    obj.get<int>          (TestClass::P_COUNT));
	ASSERT_EQ(count,    obj.get<int>          ("count"));
	ASSERT_EQ(radius,   obj.get<float>        (TestClass::P_RADIUS));
	ASSERT_EQ(radius,   obj.get<float>        ("radius"));
	ASSERT_EQ(pos,      obj.get<lair::Vector4>(TestClass::P_POS));
	ASSERT_EQ(pos,      obj.get<lair::Vector4>("pos"));
	ASSERT_EQ(name,     obj.get<std::string>  (TestClass::P_NAME));
	ASSERT_EQ(name,     obj.get<std::string>  ("name"));
	ASSERT_EQ(blend,    obj.get<BlendingMode> (TestClass::P_BLEND));
	ASSERT_EQ(blend,    obj.get<BlendingMode> ("blendingMode"));
	ASSERT_EQ(texFlags, obj.get<unsigned>     (TestClass::P_TEXFLAGS));
	ASSERT_EQ(texFlags, obj.get<unsigned>     ("texFlags"));

	ASSERT_THROW(obj.get<float>(TestClass::P_COUNT), lair::AssertionFailedError);
	ASSERT_THROW(obj.set(TestClass::P_COUNT, 1.5f), lair::AssertionFailedError);
}

TEST(PropertyTest, TestIO) {
//	typedef lair::BlendingMode Blend;

	TestClass obj0;
	obj0.setCount(777);
	obj0.setRadius(77.55);
	obj0.setPos(lair::Vector4(8, 8.5, 4, -5));
	obj0.setName("Plop");
	obj0.setBlendingMode(BLEND_ALPHA);
	obj0.setTexFlags(BILINEAR_MIPMAP | MIRROR_S | CLAMP_T);

	std::stringstream stream;
	lair::ErrorList errors;

	lair::LdlPropertySerializer serializer;
	serializer.registerEnum<BlendingMode>();

	lair::LdlWriter writer(&stream, "<test>", &errors);
	serializer.write(writer, obj0);
	ASSERT_EQ(0, errors.nErrors());

	TestClass obj1;
	lair::LdlParser parser(&stream, "<test>", &errors, lair::LdlParser::CTX_MAP);
	serializer.read(parser, obj1);
	ASSERT_EQ(0, errors.nErrors());

	ASSERT_EQ(obj0.getCount(),        obj1.getCount());
	ASSERT_EQ(obj0.getRadius(),       obj1.getRadius());
	ASSERT_EQ(obj0.getPos(),          obj1.getPos());
	ASSERT_EQ(obj0.getName(),         obj1.getName());
	ASSERT_EQ(obj0.getBlendingMode(), obj1.getBlendingMode());
	ASSERT_EQ(obj0.getTexFlags(),     obj1.getTexFlags());

	std::cerr << stream.str() << "\n";
}
