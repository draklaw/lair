/*
 *  Copyright (C) 2019 Simon Boyé
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


#ifndef TEST_CORE_INTRUSIVE_POINTER_TEST_MANAGER_H
#define TEST_CORE_INTRUSIVE_POINTER_TEST_MANAGER_H

#include <lair/core/intrusive_pointer.h>


namespace testIp {


class TestBase;
class TestFoo;
class TestBar;

using TestBasePtr = lair::IntrusivePointer<TestBase>;
using TestFooPtr = lair::IntrusivePointer<TestFoo>;
using TestBarPtr = lair::IntrusivePointer<TestBar>;


class TestManager {
public:
	TestFooPtr createFoo(int value);
	TestBarPtr createBar(TestFooPtr foo);

	void _destroyTestFoo(TestFoo* test);
	void _deleteTestFoo(TestFoo* test);
	void _destroyTestBar(TestBar* test);
	void _deleteTestBar(TestBar* test);

	static void resetCounts();
	static unsigned fooConstructCount();
	static unsigned fooDestroyCount();
	static unsigned barConstructCount();
	static unsigned barDestroyCount();
	static unsigned fooAllocCount();
	static unsigned fooDeleteCount();
	static unsigned barAllocCount();
	static unsigned barDeleteCount();

public:
	static unsigned _fooAllocCount;
	static unsigned _fooDeleteCount;
	static unsigned _barAllocCount;
	static unsigned _barDeleteCount;
};


}

#endif
