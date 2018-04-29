/*
 *  Copyright (C) 2017 Simon Boyé
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


#include <cstdlib>

#include "lair/meta/metatype.h"


namespace lair {


unsigned MetaTypeManager::_register(MetaType metaType) {
	lairAssert(_metaTypeMap.count(metaType.identifier) == 0);

	metaType.index = _metaTypes.size();
	_metaTypes.emplace_back(MetaTypePtr(new MetaType(metaType)));
	_metaTypeMap.emplace(metaType.identifier, _metaTypes[metaType.index].get());

	return metaType.index;
}

MetaTypeManager& MetaTypeManager::_getInstance() {
	static MetaTypeManager metaTypes;
	return metaTypes;
}

MetaTypeManager& metaTypes = MetaTypeManager::_getInstance();


}
