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

#include "lair/fs/file_system.h"


namespace lair
{


FileSystem::FileSystem() {
}


void FileSystem::addFileSystem(const Path& root, AbstractFileSystemSP fileSystem) {
	_fileSystems.emplace_back(ChildFS{
	                              makeAbsolute("/", root),
	                              std::move(fileSystem)
	                          });
}


VirtualFile FileSystem::file(const Path& logicPath) const {
	Path abs = makeAbsolute("/", logicPath);
	for(const ChildFS& fs: _fileSystems) {
		auto pathIt  = abs.begin();
		auto pathEnd = abs.end();
		auto baseIt  = fs.root.begin();
		auto baseEnd = fs.root.end();
		for(; pathIt != pathEnd && baseIt != baseEnd; ++pathIt, ++baseIt) {
			if(*pathIt != *baseIt)
				continue;
		}

		if(baseIt == baseEnd) {
			Path relative;

			while(pathIt != pathEnd && Path::isDirectorySeparator(*pathIt))
				++pathIt;

			relative.set(pathIt, pathEnd);

			VirtualFile file = fs.fs->file(relative);
			if(file) {
				return file;
			}
		}
	}

	return VirtualFile();
}


const MemFile* FileSystem::_fileBuffer(const Path& logicPath) const {
	VirtualFile file = this->file(logicPath);
	if(file)
		return file.fileBuffer();
	return nullptr;
}


Path FileSystem::_realPath(const Path& logicPath) const {
	VirtualFile file = this->file(logicPath);
	if(file)
		return file.realPath();
	return Path();
}


}
