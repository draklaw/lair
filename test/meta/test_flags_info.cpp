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

#include <lair/ldl/parse.h>

#include <lair/meta/flags_info.h>


using namespace lair;


struct FlagTest {
	unsigned flags;
	String   fmt;
};

TEST(FlagsInfoTest, TestFlags) {
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
