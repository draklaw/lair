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


namespace testIp {


TestBase::TestBase(TestManager* manager)
    : _manager(manager)
{
}

TestBase::~TestBase()
{
}

int TestBase::value() const
{
	return 0;
}

TestFooPtr TestBase::foo() const
{
	return nullptr;
}


}
