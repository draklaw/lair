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

#include <lair/meta/enum_info.h>


using namespace lair;


TEST(EnumInfoTest, TestEnum) {
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
