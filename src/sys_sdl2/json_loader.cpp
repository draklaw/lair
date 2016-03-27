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


#include <fstream>

#include <lair/core/lair.h>
#include <lair/core/log.h>

#include "lair/sys_sdl2/json_loader.h"


namespace lair
{


JsonLoader::JsonLoader(LoaderManager* manager, const Path& path)
    : Loader(manager, path),
      _value() {
}


JsonLoader::~JsonLoader() {
}


void JsonLoader::loadSyncImpl(Logger& log) {
	std::ifstream in(realPath().native().c_str());
	if(!in.good()) {
		log.error("Unable to read \"", _file, "\".");
		return;
	}

	Json::Reader reader;
	if(!reader.parse(in, _value)) {
		log.error("Error while parsing json \"", _file, "\": ",
		          reader.getFormattedErrorMessages());
		return;
	}

	// TODO: Return the real size ?
	_success(1024);
}


}
