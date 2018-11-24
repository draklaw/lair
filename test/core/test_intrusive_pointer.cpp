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


#include <gtest/gtest.h>

#include <lair/core/intrusive_pointer.h>

#include "intrusive_pointer/test_base.h"
#include "intrusive_pointer/test_foo.h"
#include "intrusive_pointer/test_bar.h"
#include "intrusive_pointer/test_manager.h"


using namespace lair;

namespace testIp {

#define DECL_PTRS(_type) \
	class _type; \
	using _type##Ptr = lair::IntrusivePointer<_type>; \
	using Const##_type##Ptr = lair::IntrusivePointer<const _type>; \
	using _type##WPtr = lair::IntrusiveWeakPointer<_type>; \
	using Const##_type##WPtr = lair::IntrusiveWeakPointer<const _type>;

DECL_PTRS(TestBase)
DECL_PTRS(TestFoo)
DECL_PTRS(TestBar)

}


TEST(IntrusivePointer, DefaultConstruct) {
	testIp::TestManager::resetCounts();

	{
		testIp::TestFooPtr ptr;

		ASSERT_FALSE(ptr);
		ASSERT_EQ(nullptr, ptr.get());
	}

	ASSERT_EQ(0, testIp::TestManager::fooAllocCount());
	ASSERT_EQ(0, testIp::TestManager::fooConstructCount());
	ASSERT_EQ(0, testIp::TestManager::fooDestroyCount());
	ASSERT_EQ(0, testIp::TestManager::fooDeleteCount());
	ASSERT_EQ(0, testIp::TestManager::barAllocCount());
	ASSERT_EQ(0, testIp::TestManager::barConstructCount());
	ASSERT_EQ(0, testIp::TestManager::barDestroyCount());
	ASSERT_EQ(0, testIp::TestManager::barDeleteCount());
}

TEST(IntrusivePointer, SimpleAlloc) {
	testIp::TestManager::resetCounts();

	testIp::TestManager manager;

	{
		testIp::TestFooPtr ptr = manager.createFoo(123);

		ASSERT_TRUE(ptr);
		ASSERT_NE(nullptr, ptr.get());
		ASSERT_EQ(1, ptr.use_count());
		ASSERT_EQ(123, ptr->value());
	}

	ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
	ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
	ASSERT_EQ(1, testIp::TestManager::fooDestroyCount());
	ASSERT_EQ(1, testIp::TestManager::fooDeleteCount());
	ASSERT_EQ(0, testIp::TestManager::barAllocCount());
	ASSERT_EQ(0, testIp::TestManager::barConstructCount());
	ASSERT_EQ(0, testIp::TestManager::barDestroyCount());
	ASSERT_EQ(0, testIp::TestManager::barDeleteCount());
}

TEST(IntrusivePointer, CopyConstructor) {
	testIp::TestManager::resetCounts();

	testIp::TestManager manager;

	{
		testIp::TestFooPtr ptr1 = manager.createFoo(123);

		ASSERT_TRUE(ptr1);
		ASSERT_NE(nullptr, ptr1.get());
		ASSERT_EQ(1, ptr1.use_count());
		ASSERT_EQ(123, ptr1->value());

		testIp::TestFooPtr ptr2(ptr1);

		ASSERT_TRUE(ptr2);
		ASSERT_NE(nullptr, ptr2.get());
		ASSERT_EQ(2, ptr2.use_count());
		ASSERT_EQ(123, ptr2->value());

		ptr1 = nullptr;

		ASSERT_FALSE(ptr1);
		ASSERT_EQ(nullptr, ptr1.get());

		ASSERT_TRUE(ptr2);
		ASSERT_NE(nullptr, ptr2.get());
		ASSERT_EQ(1, ptr2.use_count());
		ASSERT_EQ(123, ptr2->value());
	}

	ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
	ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
	ASSERT_EQ(1, testIp::TestManager::fooDestroyCount());
	ASSERT_EQ(1, testIp::TestManager::fooDeleteCount());
	ASSERT_EQ(0, testIp::TestManager::barAllocCount());
	ASSERT_EQ(0, testIp::TestManager::barConstructCount());
	ASSERT_EQ(0, testIp::TestManager::barDestroyCount());
	ASSERT_EQ(0, testIp::TestManager::barDeleteCount());
}

TEST(IntrusivePointer, CopyOperator) {
	testIp::TestManager::resetCounts();

	testIp::TestManager manager;

	{
		testIp::TestFooPtr ptr2(nullptr);
		testIp::TestFooPtr ptr1 = manager.createFoo(123);

		ASSERT_TRUE(ptr1);
		ASSERT_NE(nullptr, ptr1.get());
		ASSERT_EQ(1, ptr1.use_count());
		ASSERT_EQ(123, ptr1->value());

		ptr2 = ptr1;

		ASSERT_TRUE(ptr2);
		ASSERT_NE(nullptr, ptr2.get());
		ASSERT_EQ(2, ptr2.use_count());
		ASSERT_EQ(123, ptr2->value());

		ptr1 = nullptr;

		ASSERT_FALSE(ptr1);
		ASSERT_EQ(nullptr, ptr1.get());

		ASSERT_TRUE(ptr2);
		ASSERT_NE(nullptr, ptr2.get());
		ASSERT_EQ(1, ptr2.use_count());
		ASSERT_EQ(123, ptr2->value());
	}

	ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
	ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
	ASSERT_EQ(1, testIp::TestManager::fooDestroyCount());
	ASSERT_EQ(1, testIp::TestManager::fooDeleteCount());
	ASSERT_EQ(0, testIp::TestManager::barAllocCount());
	ASSERT_EQ(0, testIp::TestManager::barConstructCount());
	ASSERT_EQ(0, testIp::TestManager::barDestroyCount());
	ASSERT_EQ(0, testIp::TestManager::barDeleteCount());
}

TEST(IntrusivePointer, MoveConstructor) {
	testIp::TestManager::resetCounts();

	testIp::TestManager manager;

	{
		testIp::TestFooPtr ptr1 = manager.createFoo(123);

		ASSERT_TRUE(ptr1);
		ASSERT_NE(nullptr, ptr1.get());
		ASSERT_EQ(1, ptr1.use_count());
		ASSERT_EQ(123, ptr1->value());

		testIp::TestFooPtr ptr2(std::move(ptr1));

		ASSERT_FALSE(ptr1);
		ASSERT_EQ(nullptr, ptr1.get());
		ASSERT_TRUE(ptr2);
		ASSERT_NE(nullptr, ptr2.get());
		ASSERT_EQ(1, ptr2.use_count());
		ASSERT_EQ(123, ptr2->value());
	}

	ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
	ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
	ASSERT_EQ(1, testIp::TestManager::fooDestroyCount());
	ASSERT_EQ(1, testIp::TestManager::fooDeleteCount());
	ASSERT_EQ(0, testIp::TestManager::barAllocCount());
	ASSERT_EQ(0, testIp::TestManager::barConstructCount());
	ASSERT_EQ(0, testIp::TestManager::barDestroyCount());
	ASSERT_EQ(0, testIp::TestManager::barDeleteCount());
}

TEST(IntrusivePointer, MoveOperator) {
	testIp::TestManager::resetCounts();

	testIp::TestManager manager;

	{
		testIp::TestFooPtr ptr2 = nullptr;
		testIp::TestFooPtr ptr1 = manager.createFoo(123);

		ASSERT_TRUE(ptr1);
		ASSERT_NE(nullptr, ptr1.get());
		ASSERT_EQ(1, ptr1.use_count());
		ASSERT_EQ(123, ptr1->value());

		ptr2 = std::move(ptr1);

		ASSERT_FALSE(ptr1);
		ASSERT_EQ(nullptr, ptr1.get());
		ASSERT_TRUE(ptr2);
		ASSERT_NE(nullptr, ptr2.get());
		ASSERT_EQ(1, ptr2.use_count());
		ASSERT_EQ(123, ptr2->value());
	}

	ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
	ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
	ASSERT_EQ(1, testIp::TestManager::fooDestroyCount());
	ASSERT_EQ(1, testIp::TestManager::fooDeleteCount());
	ASSERT_EQ(0, testIp::TestManager::barAllocCount());
	ASSERT_EQ(0, testIp::TestManager::barConstructCount());
	ASSERT_EQ(0, testIp::TestManager::barDestroyCount());
	ASSERT_EQ(0, testIp::TestManager::barDeleteCount());
}

TEST(IntrusivePointer, ConstructorCast) {
	testIp::TestManager::resetCounts();

	testIp::TestManager manager;

	{
		testIp::TestFooPtr fooPtr = manager.createFoo(123);

		ASSERT_TRUE(fooPtr);
		ASSERT_NE(nullptr, fooPtr.get());
		ASSERT_EQ(1, fooPtr.use_count());
		ASSERT_EQ(123, fooPtr->value());

		testIp::TestBasePtr basePtr(fooPtr);

		ASSERT_TRUE(fooPtr);
		ASSERT_NE(nullptr, fooPtr.get());
		ASSERT_EQ(2, fooPtr.use_count());
		ASSERT_EQ(123, fooPtr->value());

		ASSERT_TRUE(basePtr);
		ASSERT_NE(nullptr, basePtr.get());
		ASSERT_EQ(2, basePtr.use_count());
		ASSERT_EQ(123, basePtr->value());

		// Should not compile
		// testIp::TestFooPtr fooPtr2(basePtr);

		testIp::TestFooPtr fooPtr2 = lair::dynamic_pointer_cast<testIp::TestFoo>(basePtr);

		ASSERT_TRUE(fooPtr);
		ASSERT_NE(nullptr, fooPtr.get());
		ASSERT_EQ(3, fooPtr.use_count());
		ASSERT_EQ(123, fooPtr->value());
	}

	ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
	ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
	ASSERT_EQ(1, testIp::TestManager::fooDestroyCount());
	ASSERT_EQ(1, testIp::TestManager::fooDeleteCount());
	ASSERT_EQ(0, testIp::TestManager::barAllocCount());
	ASSERT_EQ(0, testIp::TestManager::barConstructCount());
	ASSERT_EQ(0, testIp::TestManager::barDestroyCount());
	ASSERT_EQ(0, testIp::TestManager::barDeleteCount());
}

TEST(IntrusivePointer, OperatorCast) {
	testIp::TestManager::resetCounts();

	testIp::TestManager manager;

	{
		testIp::TestFooPtr fooPtr = manager.createFoo(123);
		testIp::TestBasePtr basePtr;
		testIp::TestFooPtr fooPtr2;

		ASSERT_TRUE(fooPtr);
		ASSERT_NE(nullptr, fooPtr.get());
		ASSERT_EQ(1, fooPtr.use_count());
		ASSERT_EQ(123, fooPtr->value());

		basePtr = fooPtr;

		ASSERT_TRUE(fooPtr);
		ASSERT_NE(nullptr, fooPtr.get());
		ASSERT_EQ(2, fooPtr.use_count());
		ASSERT_EQ(123, fooPtr->value());

		ASSERT_TRUE(basePtr);
		ASSERT_NE(nullptr, basePtr.get());
		ASSERT_EQ(2, basePtr.use_count());
		ASSERT_EQ(123, basePtr->value());

		// Should not compile
		// fooPtr2 = basePtr;

		fooPtr2 = lair::dynamic_pointer_cast<testIp::TestFoo>(basePtr);

		ASSERT_TRUE(fooPtr);
		ASSERT_NE(nullptr, fooPtr.get());
		ASSERT_EQ(3, fooPtr.use_count());
		ASSERT_EQ(123, fooPtr->value());
	}

	ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
	ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
	ASSERT_EQ(1, testIp::TestManager::fooDestroyCount());
	ASSERT_EQ(1, testIp::TestManager::fooDeleteCount());
	ASSERT_EQ(0, testIp::TestManager::barAllocCount());
	ASSERT_EQ(0, testIp::TestManager::barConstructCount());
	ASSERT_EQ(0, testIp::TestManager::barDestroyCount());
	ASSERT_EQ(0, testIp::TestManager::barDeleteCount());
}

TEST(IntrusivePointer, MemberPointer) {
	testIp::TestManager::resetCounts();

	testIp::TestManager manager;

	{
		testIp::TestFooPtr fooPtr = manager.createFoo(123);

		ASSERT_TRUE(fooPtr);
		ASSERT_NE(nullptr, fooPtr.get());
		ASSERT_EQ(1, fooPtr.use_count());
		ASSERT_EQ(123, fooPtr->value());

		testIp::TestBarPtr barPtr = manager.createBar(fooPtr);

		ASSERT_TRUE(barPtr);
		ASSERT_NE(nullptr, barPtr.get());
		ASSERT_EQ(1, barPtr.use_count());
		ASSERT_EQ(fooPtr, barPtr->foo());

		ASSERT_EQ(2, fooPtr.use_count());

		ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
		ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
		ASSERT_EQ(0, testIp::TestManager::fooDestroyCount());
		ASSERT_EQ(0, testIp::TestManager::fooDeleteCount());
		ASSERT_EQ(1, testIp::TestManager::barAllocCount());
		ASSERT_EQ(1, testIp::TestManager::barConstructCount());
		ASSERT_EQ(0, testIp::TestManager::barDestroyCount());
		ASSERT_EQ(0, testIp::TestManager::barDeleteCount());

		fooPtr = nullptr;

		ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
		ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
		ASSERT_EQ(0, testIp::TestManager::fooDestroyCount());
		ASSERT_EQ(0, testIp::TestManager::fooDeleteCount());
		ASSERT_EQ(1, testIp::TestManager::barAllocCount());
		ASSERT_EQ(1, testIp::TestManager::barConstructCount());
		ASSERT_EQ(0, testIp::TestManager::barDestroyCount());
		ASSERT_EQ(0, testIp::TestManager::barDeleteCount());
	}

	ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
	ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
	ASSERT_EQ(1, testIp::TestManager::fooDestroyCount());
	ASSERT_EQ(1, testIp::TestManager::fooDeleteCount());
	ASSERT_EQ(1, testIp::TestManager::barAllocCount());
	ASSERT_EQ(1, testIp::TestManager::barConstructCount());
	ASSERT_EQ(1, testIp::TestManager::barDestroyCount());
	ASSERT_EQ(1, testIp::TestManager::barDeleteCount());
}

TEST(IntrusivePointer, WeakPointer) {
	testIp::TestManager::resetCounts();

	testIp::TestManager manager;

	{
		testIp::TestFooPtr fooPtr = manager.createFoo(123);
		testIp::TestFooWPtr fooWPtr = fooPtr;

		ASSERT_TRUE(fooPtr);
		ASSERT_NE(nullptr, fooPtr.get());
		ASSERT_EQ(1, fooPtr.use_count());
		ASSERT_EQ(123, fooPtr->value());

		ASSERT_EQ(1, fooWPtr.use_count());

		testIp::TestFooPtr fooPtr2 = fooWPtr.lock();
		ASSERT_TRUE(fooPtr2);
		ASSERT_EQ(fooPtr, fooPtr2);
		ASSERT_EQ(2, fooPtr2.use_count());

		ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
		ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
		ASSERT_EQ(0, testIp::TestManager::fooDestroyCount());
		ASSERT_EQ(0, testIp::TestManager::fooDeleteCount());

		fooPtr = nullptr;
		fooPtr2 = nullptr;

		ASSERT_FALSE(fooPtr);
		ASSERT_EQ(nullptr, fooPtr.get());

		ASSERT_EQ(0, fooWPtr.use_count());
		ASSERT_EQ(nullptr, fooWPtr.lock());

		ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
		ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
		ASSERT_EQ(1, testIp::TestManager::fooDestroyCount());
		ASSERT_EQ(0, testIp::TestManager::fooDeleteCount());
	}

	ASSERT_EQ(1, testIp::TestManager::fooAllocCount());
	ASSERT_EQ(1, testIp::TestManager::fooConstructCount());
	ASSERT_EQ(1, testIp::TestManager::fooDestroyCount());
	ASSERT_EQ(1, testIp::TestManager::fooDeleteCount());
	ASSERT_EQ(0, testIp::TestManager::barAllocCount());
	ASSERT_EQ(0, testIp::TestManager::barConstructCount());
	ASSERT_EQ(0, testIp::TestManager::barDestroyCount());
	ASSERT_EQ(0, testIp::TestManager::barDeleteCount());
}

