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


#include "test_manager.h"
#include "test_foo.h"
#include "test_bar.h"


namespace testIp {


TestBar::TestBar(TestManager* manager, TestFooPtr foo)
    : TestBase(manager)
    , _foo(foo)
{
	_constructCount += 1;
}

TestBar::~TestBar() {
}

TestFooPtr TestBar::foo() const {
	return _foo;
}

unsigned TestBar::_constructCount = 0;
unsigned TestBar::_destroyCount   = 0;

void TestBar::_partialDestroy() {
	_destroyCount += 1;
}

void TestBar::_destroy() {
	_manager->_destroyTestBar(this);
}

void TestBar::_delete() {
	_manager->_deleteTestBar(this);
}


}
