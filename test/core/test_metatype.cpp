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
#include <lair/core/metatype.h>


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
typedef Test<128> Test128;

LAIR_REGISTER_METATYPE(Test32,  "Test32");
LAIR_REGISTER_METATYPE(Test128, "Test128");

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

LAIR_REGISTER_METATYPE(NoMethods, "NoMethods");

enum {
	NO_REPR = 0x01,
};

template<typename T>
void testType(const T& value0, const T& value1, unsigned flags = 0) {
	const MetaType* mt = metaTypes.get<T>();

	ASSERT_TRUE(mt);
	ASSERT_TRUE(mt->copyConstruct);
	ASSERT_TRUE(mt->destroy);
	ASSERT_TRUE(mt->assign);
	ASSERT_TRUE((flags & NO_REPR) || mt->writeRepr);

	Variant var0;
	var0 = Variant(value0);

	ASSERT_FALSE(var0.isEmpty());
	ASSERT_TRUE(var0.is<T>());
	ASSERT_EQ(value0, var0.as<T>());

	Variant var1(value1);
	var0 = std::move(var1);
	ASSERT_TRUE(var1.isEmpty());
	ASSERT_FALSE(var0.isEmpty());
	ASSERT_TRUE(var0.is<T>());
	ASSERT_EQ(value1, var0.as<T>());

	var0 = value0;
	ASSERT_FALSE(var0.isEmpty());
	ASSERT_TRUE(var0.is<T>());
	ASSERT_EQ(value0, var0.as<T>());

	T value0_2 = value0;
	VarRef ref0 = value0_2;
	ASSERT_FALSE(ref0.isEmpty());
	ASSERT_TRUE(ref0.is<T>());
	ASSERT_EQ(value0, ref0.as<T>());

	ref0 = value1;
	ASSERT_EQ(value1, value0_2);

	var1 = ref0;
	ASSERT_EQ(value1, var1.as<T>());
}

TEST(MetaTypeTest, TestEmpty) {
	Variant var;
	ASSERT_TRUE(var.isEmpty());
	ASSERT_FALSE(var.is<int>());
}

TEST(MetaTypeTest, TestBasicTypes) {
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
	testType(Test128("Hello World !"), Test128("Goodbye !"), NO_REPR);
}

TEST(MetaTypeTest, TestNoMethods) {
	const MetaType* type = metaTypes.get<NoMethods>();
	ASSERT_EQ(nullptr, type->copyConstruct);
	ASSERT_EQ(nullptr, type->destroy);
	ASSERT_EQ(nullptr, type->assign);
	ASSERT_EQ(nullptr, type->writeRepr);
}

TEST(MetaTypeTest, TestEnum) {
	enum Test {
		TEST_0,
		TEST_1,
		TEST_NEG = -25,
		TEST_X = 99999,
	};
	EnumInfo info("Test");
	info.add(TEST_0,   "test0");
	info.add(TEST_1,   "test1");
	info.add(TEST_NEG, "test_neg");
	info.add(TEST_X,   "testX");

	ASSERT_EQ(TEST_0,   info.value("test0"));
	ASSERT_EQ(TEST_1,   info.value("test1"));
	ASSERT_EQ(TEST_NEG, info.value("test_neg"));
	ASSERT_EQ(TEST_X,   info.value("testX"));

	ASSERT_EQ("test0",    info.label(TEST_0));
	ASSERT_EQ("test1",    info.label(TEST_1));
	ASSERT_EQ("test_neg", info.label(TEST_NEG));
	ASSERT_EQ("testX",    info.label(TEST_X));
}

struct FlagTest {
	unsigned flags;
	String   fmt;
};

TEST(MetaTypeTest, TestFlags) {
	enum Test {
		TEST_0 = 0x0001,
		TEST_1 = 0x0002,
		TEST_2 = 0x0004,
		TEST_A = 0x0000,
		TEST_B = 0x0008,
		TEST_C = 0x0010,
		TEST_5 = 0x0020,
		TEST_6 = 0x0040,

		TEST_X_MASK = 0x18,
		TEST_BITS_MASK = TEST_0 | TEST_1 | TEST_2 | TEST_5 | TEST_6,
		TEST_ALL_MASK = TEST_BITS_MASK | TEST_X_MASK,

		TEST_BASE  = TEST_0 | TEST_1 | TEST_2,
		TEST_STUFF = TEST_1 | TEST_B | TEST_6,
		TEST_ALL_A = TEST_BITS_MASK | TEST_A,
		TEST_ALL_B = TEST_BITS_MASK | TEST_B,
		TEST_ALL_C = TEST_BITS_MASK | TEST_C,
	};
	FlagsInfo info("Test");
	info.add(TEST_ALL_A, TEST_ALL_MASK, "test_all_a");
	info.add(TEST_ALL_B, TEST_ALL_MASK, "test_all_b");
	info.add(TEST_ALL_C, TEST_ALL_MASK, "test_all_c");
	info.add(TEST_STUFF, TEST_ALL_MASK, "test-stuff");
	info.add(TEST_BASE, "test_base");
	info.add(TEST_0, "test0");
	info.add(TEST_1, "test1");
	info.add(TEST_2, "test2");
	info.add(TEST_A, TEST_X_MASK, "test_a");
	info.add(TEST_B, TEST_X_MASK, "test_b");
	info.add(TEST_C, TEST_X_MASK, "test_c");
	info.add(TEST_5, "test5");
	info.add(TEST_6, "test6");

	FlagTest tests[] = {
	    FlagTest{ 0,
	              "test_a" },
	    FlagTest{ TEST_0,
	              "test0 | test_a" },
	    FlagTest{ TEST_B,
	              "test_b" },
	    FlagTest{ TEST_1 | TEST_5,
	              "test1 | test_a | test5" },
	    FlagTest{ TEST_BASE | TEST_6,
	              "test_base | test_a | test6" },
	    FlagTest{ TEST_0 | TEST_C,
	              "test0 | test_c" },
	    FlagTest{ TEST_STUFF,
	              "test-stuff" },
	    FlagTest{ TEST_ALL_C,
	              "test_all_c" },
	    FlagTest{ TEST_1 | 0x18,
	              "0x0000001a" },
	    FlagTest{ TEST_STUFF | TEST_0,
	              "test0 | test1 | test_b | test6" },
	};
	unsigned nTests = sizeof(tests) / sizeof(*tests);

	for(unsigned i = 0; i < nTests; ++i) {
		unsigned      flags = tests[i].flags;
		const String& fmt   = tests[i].fmt;

		ASSERT_EQ(fmt, info.format(flags));

		ErrorList errors;
		unsigned tmp = info.parse(fmt, &errors);
		ASSERT_EQ(flags, tmp);
		ASSERT_EQ(0, errors.nErrors());
	}

	ErrorList errors;
	unsigned tmp;

	errors.clear();
	tmp = info.parse("  123  test_a", &errors);
	ASSERT_EQ(123, tmp);
	ASSERT_EQ(1, errors.nErrors());

	errors.clear();
	tmp = info.parse("test1 | foo | test_a bar", &errors);
	ASSERT_EQ(TEST_1 | TEST_A, tmp);
	ASSERT_EQ(2, errors.nErrors());

	errors.clear();
	tmp = info.parse("test1 | test_b | test_c", &errors);
	ASSERT_EQ(TEST_1 | TEST_B | TEST_C, tmp);
	ASSERT_EQ(1, errors.nErrors());
}
