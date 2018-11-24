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


#ifndef TEST_CORE_INTRUSIVE_POINTER_TEST_BASE_H
#define TEST_CORE_INTRUSIVE_POINTER_TEST_BASE_H

#include <lair/core/intrusive_pointer.h>


namespace testIp {


class TestManager;
class TestFoo;

using TestFooPtr = lair::IntrusivePointer<TestFoo>;


class TestBase : public lair::IntrusiveBlock<TestBase> {
public:
	TestBase(TestManager* manager);
	~TestBase();

	virtual int value() const;
	virtual TestFooPtr foo() const;

public:
	virtual void _partialDestroy() = 0;
	virtual void _destroy() = 0;
	virtual void _delete() = 0;

protected:
	TestManager* _manager;
};


}

#endif
