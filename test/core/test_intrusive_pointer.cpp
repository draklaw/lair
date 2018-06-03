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

#include <lair/core/intrusive_pointer.h>


using namespace lair;


class FooManager;

class Foo : public IntrusiveBlock<Foo> {
public:
	Foo(FooManager* manager)
	    : _manager(manager)
	{
//		std::cerr << "construct Foo (" << constructCount - destroyCount + 1 << ")\n";
		constructCount += 1;
	}

	~Foo() {
		destroyCount += 1;
//		std::cerr << "destroy Foo (" << constructCount - destroyCount << ")\n";
	}

public:
	static unsigned constructCount;
	static unsigned destroyCount;

public:
	void _destroy();
	void _delete();

protected:
	FooManager* _manager;
};

using FooPtr       = Foo::Pointer;
using ConstFooPtr  = Foo::ConstPointer;
using FooWPtr      = Foo::WeakPointer;
using ConstFooWPtr = Foo::ConstWeakPointer;

class FooManager {
public:
	FooPtr createFoo() {
//		std::cerr << "allocate Foo (" << allocCount - deleteCount + 1 << ")\n";
		allocCount += 1;
		Foo* fooPtr = static_cast<Foo*>(malloc(sizeof(Foo)));

		return makeIntrusiveAt<Foo>(fooPtr, this);
	}

	void _destroyFoo(Foo* foo) {
		foo->~Foo();
	}

	void _deleteFoo(Foo* foo) {
		free(foo);
		deleteCount += 1;
//		std::cerr << "delete Foo (" << allocCount - deleteCount << ")\n";
	}

public:
	static unsigned allocCount;
	static unsigned deleteCount;
};

unsigned Foo::constructCount     = 0;
unsigned Foo::destroyCount       = 0;
unsigned FooManager::allocCount  = 0;
unsigned FooManager::deleteCount = 0;

void Foo::_destroy() {
	_manager->_destroyFoo(this);
}

void Foo::_delete() {
	_manager->_deleteFoo(this);
}



TEST(IntrusivePointer, DefaultConstruct) {
	FooManager::allocCount  = 0;
	Foo::constructCount     = 0;
	Foo::destroyCount       = 0;
	FooManager::deleteCount = 0;

	{
		FooPtr ptr;

		ASSERT_FALSE(ptr);
		ASSERT_EQ(nullptr, ptr.get());
	}

	ASSERT_EQ(0, FooManager::allocCount);
	ASSERT_EQ(0, Foo::constructCount);
	ASSERT_EQ(0, Foo::destroyCount);
	ASSERT_EQ(0, FooManager::deleteCount);
}


TEST(IntrusivePointer, SimpleAlloc) {
	FooManager::allocCount  = 0;
	Foo::constructCount     = 0;
	Foo::destroyCount       = 0;
	FooManager::deleteCount = 0;

	FooManager fm;

	{
		FooPtr ptr = fm.createFoo();

		ASSERT_TRUE(ptr);
		ASSERT_NE(nullptr, ptr.get());
		ASSERT_EQ(1, ptr.use_count());
	}

	ASSERT_EQ(1, FooManager::allocCount);
	ASSERT_EQ(1, Foo::constructCount);
	ASSERT_EQ(1, Foo::destroyCount);
	ASSERT_EQ(1, FooManager::deleteCount);
}


TEST(IntrusivePointer, Manip) {
	FooManager::allocCount  = 0;
	Foo::constructCount     = 0;
	Foo::destroyCount       = 0;
	FooManager::deleteCount = 0;

	FooManager fm;

	FooPtr ptr0 = nullptr;
	{
		FooPtr ptr1 = fm.createFoo();
		ptr0 = ptr1;
		ASSERT_EQ(ptr1, ptr0);
	}

	ASSERT_EQ(1, FooManager::allocCount);
	ASSERT_EQ(1, Foo::constructCount);
	ASSERT_EQ(0, Foo::destroyCount);
	ASSERT_EQ(0, FooManager::deleteCount);

	ASSERT_EQ(ptr0->pointer(), ptr0);

	ptr0.reset();

	ASSERT_EQ(1, FooManager::allocCount);
	ASSERT_EQ(1, Foo::constructCount);
	ASSERT_EQ(1, Foo::destroyCount);
	ASSERT_EQ(1, FooManager::deleteCount);
}


TEST(IntrusivePointer, WeakSimpleAlloc) {
	FooManager::allocCount  = 0;
	Foo::constructCount     = 0;
	Foo::destroyCount       = 0;
	FooManager::deleteCount = 0;

	FooManager fm;

	{
		FooWPtr wptr;
		{
			FooPtr ptr = fm.createFoo();
			wptr = ptr;

			ASSERT_TRUE(ptr);
			ASSERT_NE(nullptr, ptr.get());
			ASSERT_EQ(1, ptr.use_count());

			ASSERT_EQ(1, FooManager::allocCount);
			ASSERT_EQ(1, Foo::constructCount);
			ASSERT_EQ(0, Foo::destroyCount);
			ASSERT_EQ(0, FooManager::deleteCount);
		}

		ASSERT_EQ(1, FooManager::allocCount);
		ASSERT_EQ(1, Foo::constructCount);
		ASSERT_EQ(1, Foo::destroyCount);
		ASSERT_EQ(0, FooManager::deleteCount);
	}

	ASSERT_EQ(1, FooManager::allocCount);
	ASSERT_EQ(1, Foo::constructCount);
	ASSERT_EQ(1, Foo::destroyCount);
	ASSERT_EQ(1, FooManager::deleteCount);
}


TEST(IntrusivePointer, WeakManip) {
	FooManager::allocCount  = 0;
	Foo::constructCount     = 0;
	Foo::destroyCount       = 0;
	FooManager::deleteCount = 0;

	FooManager fm;

	FooWPtr wptr;
	{
		FooPtr ptr = fm.createFoo();
		wptr = ptr;

		ASSERT_EQ(1, FooManager::allocCount);
		ASSERT_EQ(1, Foo::constructCount);
		ASSERT_EQ(0, Foo::destroyCount);
		ASSERT_EQ(0, FooManager::deleteCount);

		FooPtr ptr2 = wptr.lock();

		ASSERT_EQ(ptr, ptr2);
	}

	ASSERT_EQ(1, FooManager::allocCount);
	ASSERT_EQ(1, Foo::constructCount);
	ASSERT_EQ(1, Foo::destroyCount);
	ASSERT_EQ(0, FooManager::deleteCount);

	FooPtr ptr2 = wptr.lock();

	ASSERT_EQ(nullptr, ptr2);

	wptr.reset();

	ASSERT_EQ(1, FooManager::allocCount);
	ASSERT_EQ(1, Foo::constructCount);
	ASSERT_EQ(1, Foo::destroyCount);
	ASSERT_EQ(1, FooManager::deleteCount);
}
