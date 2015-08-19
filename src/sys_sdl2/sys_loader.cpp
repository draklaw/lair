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

#include "lair/sys_sdl2/sys_loader.h"


namespace lair
{


SysLoader::SysLoader(size_t maxCacheSize, unsigned nThread, Logger& logger)
    : LoaderManager(maxCacheSize, nThread, logger) {
}


SysLoader::~SysLoader() {
}


SysLoader::ImageLoaderPtr SysLoader::loadImage(const std::string file) {
	return load<ImageLoader>(file);
}


}
