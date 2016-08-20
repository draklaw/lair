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

#include <lair/ec/dense_array.h>


#define BLOCK_SIZE 4

using namespace lair;

struct TestObj {
	static int count;

	TestObj(int i) : i(i) {
		++count;
	}
	TestObj(const TestObj& other) : i(other.i) {
		++count;
	}

	~TestObj() {
		--count;
	}

	bool operator==(const TestObj& other) const {
		return i == other.i;
	}

	int i;
};
int TestObj::count = 0;

typedef DenseArray<TestObj> TestDenseArray;


class DenseArrayTest : public ::testing::Test {
public:
	TestDenseArray* _array;

	static constexpr int count = 10;
	static constexpr int values[count] = { 123, 234, 999, 98, 838, 57575, 333998, 663, 88332, 123123 };

	DenseArrayTest()
		: _array(nullptr) {
	}

	virtual void SetUp() {
		_array = new TestDenseArray(BLOCK_SIZE);
		TestObj::count = 0;
	}

	virtual void TearDown() {
		delete _array;
		ASSERT_EQ(0, TestObj::count);
	}

	TestDenseArray& array() {
		return *_array;
	}

	void fillArray() {
		for(int i = 0; i < count; ++i)
			array().emplace_back(values[i]);
	}
};
constexpr int DenseArrayTest::count;
constexpr int DenseArrayTest::values[DenseArrayTest::count];

TEST_F(DenseArrayTest, Constructor) {
	ASSERT_EQ(0, array().size());
	ASSERT_EQ(0, array().capacity());
	ASSERT_TRUE(array().begin() == array().end());

	ASSERT_THROW(array().at(0), AssertionFailedError);
	ASSERT_THROW(array().back(), AssertionFailedError);
}

TEST_F(DenseArrayTest, Insertion) {
	array().push_back(123);
	ASSERT_EQ(1, TestObj::count);
	ASSERT_EQ(1, array().size());
	ASSERT_EQ(BLOCK_SIZE, array().capacity());
	ASSERT_EQ(TestObj(123), array().at(0));
	ASSERT_EQ(TestObj(123), array()[0]);
	ASSERT_EQ(TestObj(123), array().back());
	ASSERT_EQ(TestObj(123), array().back());

	ASSERT_THROW(array().at(1), AssertionFailedError);
}

TEST_F(DenseArrayTest, MultiInsertion) {
	fillArray();

	ASSERT_EQ(count, TestObj::count);
	ASSERT_EQ(count, array().size());
	ASSERT_EQ((count+BLOCK_SIZE-1) / BLOCK_SIZE * BLOCK_SIZE, array().capacity());
	for(int i = 0; i < count; ++i) {
		ASSERT_EQ(TestObj(values[i]), array().at(i));
		ASSERT_EQ(TestObj(values[i]), array()[i]);
	}
	ASSERT_EQ(TestObj(values[count-1]), array().back());

	ASSERT_THROW(array().at(count), AssertionFailedError);
}

TEST_F(DenseArrayTest, Resize) {
	fillArray();

	int count2 = 5;
	array().resize(count2);

	ASSERT_EQ(count2, TestObj::count);
	ASSERT_EQ(count2, array().size());
	ASSERT_EQ((count+BLOCK_SIZE-1) / BLOCK_SIZE * BLOCK_SIZE, array().capacity());
	for(int i = 0; i < count2; ++i) {
		ASSERT_EQ(TestObj(values[i]), array().at(i));
		ASSERT_EQ(TestObj(values[i]), array()[i]);
	}
	ASSERT_EQ(TestObj(values[count2-1]), array().back());

	ASSERT_THROW(array().at(count2), AssertionFailedError);
}

TEST_F(DenseArrayTest, Iterator) {
	fillArray();

	TestDenseArray::Iterator it  = array().begin();
	TestDenseArray::Iterator end = array().end();
	for(int i = 0; i < count; ++i) {
		ASSERT_EQ(TestObj(values[i]), *it);
		ASSERT_EQ(TestObj(values[i]), array().begin()[i]);
		++it;
	}
	ASSERT_THROW(*it, AssertionFailedError);
	ASSERT_THROW(array().begin()[count], AssertionFailedError);
}

TEST_F(DenseArrayTest, IndexOf) {
	fillArray();

	for(int i = 0; i < count; ++i) {
		ASSERT_EQ(i, array()._indexOf(&array()[i]));
	}
}
