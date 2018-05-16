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


#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/path.h>

#include "lair/meta/property.h"


namespace lair
{


Property::Property(const MetaType* type, const std::string& name,
                   const EnumInfo* enumInfo, const FlagsInfo* flagsInfo)
    : _type     (type)
    , _enumInfo (enumInfo)
    , _flagsInfo(flagsInfo)
    , _name     (name)
{
}

const MetaType* Property::type() const {
	return _type;
}

const EnumInfo* Property::enumInfo() const {
	return _enumInfo;
}

const FlagsInfo* Property::flagsInfo() const {
	return _flagsInfo;
}

const std::string& Property::name() const {
	return _name;
}


}
