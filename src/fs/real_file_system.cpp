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

#include "lair/fs/real_file_system.h"


namespace lair
{


RealFileSystem::RealFileSystem(const Path& basePath)
    : _basePath(basePath) {
}


void RealFileSystem::setBasePath(const Path& basePath) {
	_basePath = basePath;
}


VirtualFile RealFileSystem::file(const Path& logicPath) const {
	if(Path::IStream((_basePath / logicPath).native().c_str()).good())
		return VirtualFile(this, logicPath);
	return VirtualFile();
}


Path RealFileSystem::_realPath(const Path& logicPath) const {
	return makeAbsolute(_basePath, logicPath);
}


}
