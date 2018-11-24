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


#ifndef TEST_CORE_INTRUSIVE_POINTER_TEST_BAR_H
#define TEST_CORE_INTRUSIVE_POINTER_TEST_BAR_H

#include "test_base.h"


namespace testIp {


class TestBar : public TestBase {
public:
	TestBar(TestManager* manager, TestFooPtr foo);
	~TestBar();

	TestFooPtr foo() const override;

public:
	static unsigned _constructCount;
	static unsigned _destroyCount;

public:
	void _partialDestroy() override;
	void _destroy() override;
	void _delete() override;

private:
	TestFooPtr _foo;
};


}

#endif
