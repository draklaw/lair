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

#include <lair/core/text.h>


using namespace lair;

TEST(TextTest, SimpleDecode) {
	char msg[] = { char(0x24),
	               char(0xc2), char(0xa2),
	               char(0xe2), char(0x82), char(0xac),
	               char(0xf0), char(0x90), char(0x8d), char(0x88) };

	Utf8CodepointIterator it(msg, msg + sizeof(msg));
	ASSERT_TRUE(it.hasNext());
	ASSERT_EQ(0x24, it.next());
	ASSERT_TRUE(it.hasNext());
	ASSERT_EQ(0xa2, it.next());
	ASSERT_TRUE(it.hasNext());
	ASSERT_EQ(0x20ac, it.next());
	ASSERT_TRUE(it.hasNext());
	ASSERT_EQ(0x010348, it.next());
	ASSERT_FALSE(it.hasNext());
}

TEST(TextTest, InvalidUtf8) {
	char msg[] = { char(0x82), char(0xac),
	               char(0xe2), char(0x82), char(0xac),
	               char(0xe2), char(0x82),
	               char(0xe2), char(0x82), char(0xac),
	               char(0x90), char(0x8d), char(0x88) };

	Utf8CodepointIterator it(msg, msg + sizeof(msg));
	ASSERT_TRUE(it.hasNext());
	ASSERT_EQ(-1, it.next());
	ASSERT_TRUE(it.hasNext());
	ASSERT_EQ(0x20ac, it.next());
	ASSERT_TRUE(it.hasNext());
	ASSERT_EQ(-1, it.next());
	ASSERT_TRUE(it.hasNext());
	ASSERT_EQ(0x20ac, it.next());
	ASSERT_TRUE(it.hasNext());
	ASSERT_EQ(-1, it.next());
	ASSERT_FALSE(it.hasNext());
}

TEST(TextTest, SimpleEncode) {
	char char1[] = { char(0x24) };
	char char2[] = { char(0xc2), char(0xa2) };
	char char3[] = { char(0xe2), char(0x82), char(0xac) };
	char char4[] = { char(0xf0), char(0x90), char(0x8d), char(0x88) };

	char  tmp[4];
	char* end;

	end = utf8FromCodepoint(tmp, 0x24);
	ASSERT_EQ(1, end - tmp);
	ASSERT_EQ(0, strncmp(tmp, char1, 1));

	end = utf8FromCodepoint(tmp, 0xa2);
	ASSERT_EQ(2, end - tmp);
	ASSERT_EQ(0, strncmp(tmp, char2, 2));

	end = utf8FromCodepoint(tmp, 0x20ac);
	ASSERT_EQ(3, end - tmp);
	ASSERT_EQ(0, strncmp(tmp, char3, 3));

	end = utf8FromCodepoint(tmp, 0x010348);
	ASSERT_EQ(4, end - tmp);
	ASSERT_EQ(0, strncmp(tmp, char4, 4));
}
