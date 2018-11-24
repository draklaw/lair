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
#include "test_base.h"
#include "test_foo.h"
#include "test_bar.h"


namespace testIp {

TestFooPtr TestManager::createFoo(int value) {
	_fooAllocCount += 1;
	TestFoo* ptr = static_cast<TestFoo*>(malloc(sizeof(TestFoo)));
	return lair::makeIntrusiveAt<TestFoo>(ptr, this, value);
}

TestBarPtr TestManager::createBar(TestFooPtr foo) {
	_barAllocCount += 1;
	TestBar* ptr = static_cast<TestBar*>(malloc(sizeof(TestBar)));
	return lair::makeIntrusiveAt<TestBar>(ptr, this, foo);
}

void TestManager::_destroyTestFoo(TestFoo* test) {
	test->_partialDestroy();
}

void TestManager::_deleteTestFoo(TestFoo* test) {
	test->~TestFoo();
	free(test);
	_fooDeleteCount += 1;
}

void TestManager::_destroyTestBar(TestBar* test) {
	test->_partialDestroy();
}

void TestManager::_deleteTestBar(TestBar* test) {
	test->~TestBar();
	free(test);
	_barDeleteCount += 1;
}

void TestManager::resetCounts() {
	_fooAllocCount  = 0;
	_fooDeleteCount = 0;
	_barAllocCount  = 0;
	_barDeleteCount = 0;
	TestFoo::_constructCount = 0;
	TestFoo::_destroyCount   = 0;
	TestBar::_constructCount = 0;
	TestBar::_destroyCount   = 0;
}

unsigned TestManager::fooConstructCount() {
	return TestFoo::_constructCount;
}

unsigned TestManager::fooDestroyCount() {
	return TestFoo::_destroyCount;
}

unsigned TestManager::barConstructCount() {
	return TestBar::_constructCount;
}

unsigned TestManager::barDestroyCount() {
	return TestBar::_destroyCount;
}

unsigned TestManager::fooAllocCount() {
	return _fooAllocCount;
}

unsigned TestManager::fooDeleteCount() {
	return _fooDeleteCount;
}

unsigned TestManager::barAllocCount() {
	return _barAllocCount;
}

unsigned TestManager::barDeleteCount() {
	return _barDeleteCount;
}

unsigned TestManager::_fooAllocCount  = 0;
unsigned TestManager::_fooDeleteCount = 0;
unsigned TestManager::_barAllocCount  = 0;
unsigned TestManager::_barDeleteCount = 0;


}
