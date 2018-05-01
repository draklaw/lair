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

#include <lair/core/parse.h>

#include <lair/meta/metatype.h>
#include <lair/meta/variant.h>


using namespace lair;

template<size_t Size>
class Test {
public:
	Test(const std::string& msg) {
		size_t size = std::min(msg.size(), Size-1);
		memcpy(data, msg.data(), size);

		size_t left = Size-1 - size;
		if(left)
			memset(data+size, '.', left);

		data[Size-1] = '\0';
	}

	Test(const Test<Size>& other) {
		std::memcpy(data, other.data, Size);
	}

	Test(Test<Size>&&) = delete;

	Test& operator=(const Test<Size>& other) {
		std::memcpy(data, other.data, Size);
		return *this;
	}

	Test& operator=(Test<Size>&&) = delete;

	bool operator==(const Test<Size>& rhs) const {
		return strcmp(data, rhs.data) == 0;
	}

	char data[Size];
};

typedef Test<32>  Test32;
typedef Test<512> Test512;

LAIR_DECLARE_METATYPE(Test32);
LAIR_DECLARE_METATYPE(Test512);
LAIR_IMPLEMENT_METATYPE(Test32,  "Test32");
LAIR_IMPLEMENT_METATYPE(Test512, "Test512");

struct NoMethods {
	NoMethods(int i)
		: i(i)
	{}

	NoMethods(const NoMethods& ) = delete;
	NoMethods(      NoMethods&&) = delete;

	~NoMethods() = delete;

	NoMethods& operator=(const NoMethods& ) = delete;
	NoMethods& operator=(      NoMethods&&) = delete;

	int i;
};

LAIR_DECLARE_METATYPE(NoMethods);
LAIR_IMPLEMENT_METATYPE(NoMethods, "NoMethods");

enum {
	NO_REPR = 0x01,
};

template<typename T>
void testType(const T& value0, const T& value1, unsigned flags = 0) {
	const MetaType* mt = metaTypes.get<T>();

	ASSERT_TRUE(mt);
	ASSERT_EQ(mt->size, sizeof(T));
	ASSERT_TRUE(mt->copyConstruct);
	ASSERT_TRUE(mt->destroy);
	ASSERT_TRUE(mt->assign);
	ASSERT_TRUE((flags & NO_REPR) || mt->writeRepr);

	Variant var0;
	var0 = Variant(value0);

	ASSERT_TRUE(var0.isValid());
	ASSERT_TRUE(var0.is<T>());
	ASSERT_EQ(value0, var0.as<T>());

	Variant var1(value1);
	var0 = std::move(var1);
	ASSERT_FALSE(var1.isValid());
	ASSERT_TRUE(var0.isValid());
	ASSERT_TRUE(var0.is<T>());
	ASSERT_EQ(value1, var0.as<T>());

	var0 = value0;
	ASSERT_TRUE(var0.isValid());
	ASSERT_TRUE(var0.is<T>());
	ASSERT_EQ(value0, var0.as<T>());
}

TEST(VariantTest, TestEmpty) {
	Variant var;
	ASSERT_FALSE(var.isValid());
	ASSERT_FALSE(var.is<int>());
}

TEST(VariantTest, TestBasicTypes) {
	testType(int8 ('+'),    int8 ('-'));
	testType(int16(-123),   int16(-456));
	testType(int32(-42),    int32(-1337));
	testType(int64(-99999), int64(-1234567));

	Variant varInt(int16(42));
	ASSERT_EQ(42, varInt.asInt());
	varInt.setInt(-444);
	ASSERT_EQ(-444, varInt.as<int16>());

	testType(uint8 ('+'),   uint8 ('-'));
	testType(uint16(123),   uint16(456));
	testType(uint32(42),    uint32(1337));
	testType(uint64(99999), uint64(1234567));

	testType(float (42.42),   float (-42.42));
	testType(double(42.4242), double(-42.4242));

	testType(String("foo"), String("bar"));

	testType(Test32 ("Hello World !"), Test32 ("Goodbye !"), NO_REPR);
	testType(Test512("Hello World !"), Test512("Goodbye !"), NO_REPR);
}

TEST(VariantTest, TestNoMethods) {
	const MetaType* type = metaTypes.get<NoMethods>();
	ASSERT_EQ(nullptr, type->copyConstruct);
	ASSERT_EQ(nullptr, type->destroy);
	ASSERT_EQ(nullptr, type->assign);
	ASSERT_EQ(nullptr, type->writeRepr);
}

TEST(VariantTest, TestCopy) {
	String str  = "Hello world !";
	String str2 = "foobar";

	Variant var0 = str;
	var0.setParseInfo("test", 10, 20, "foo");

	ASSERT_TRUE(var0.isString());
	ASSERT_EQ(var0.asString(), str);
	ASSERT_TRUE(var0.parseInfo());
	ASSERT_EQ(var0.parseInfo()->filename, "test");

	Variant var1 = var0;

	ASSERT_TRUE(var0.isValid());
	ASSERT_TRUE(var1.isString());
	ASSERT_EQ(var1.asString(), str);
	ASSERT_TRUE(var1.parseInfo());
	ASSERT_EQ(var1.parseInfo()->filename, "test");

	var1 = str2;

	ASSERT_TRUE(var1.isString());
	ASSERT_EQ(var1.asString(), str2);
	ASSERT_FALSE(var1.parseInfo());
}

TEST(VariantTest, TestMove) {
	String str0 = "Hello world !";
	String str1 = str0;
	String str2 = "foobar";
	String str3 = str2;

	Variant var0 = std::move(str0);
	var0.setParseInfo("test", 10, 20, "foo");

	ASSERT_TRUE(var0.isString());
	ASSERT_TRUE(str0.empty());
	ASSERT_EQ(var0.asString(), str1);
	ASSERT_TRUE(var0.parseInfo());
	ASSERT_EQ(var0.parseInfo()->filename, "test");

	Variant var1 = std::move(var0);

	ASSERT_FALSE(var0.isValid());
	ASSERT_TRUE(var1.isString());
	ASSERT_EQ(var1.asString(), str1);
	ASSERT_TRUE(var1.parseInfo());
	ASSERT_EQ(var1.parseInfo()->filename, "test");

	var1 = std::move(str2);

	ASSERT_TRUE(var1.isString());
	ASSERT_TRUE(str2.empty());
	ASSERT_EQ(var1.asString(), str3);
	ASSERT_FALSE(var1.parseInfo());
}
