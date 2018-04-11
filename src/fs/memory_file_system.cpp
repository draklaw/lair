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

#include "lair/fs/memory_file_system.h"


namespace lair
{


MemoryFileSystem::MemoryFileSystem() {
}


void MemoryFileSystem::addFile(const Path& logicPath, Size size, const Byte* data) {
	_files.emplace(logicPath, MemFile{size, data});
}


MemoryFileSystem::Iterator MemoryFileSystem::begin() const {
	return _files.begin();
}


MemoryFileSystem::Iterator MemoryFileSystem::end() const {
	return _files.end();
}


VirtualFile MemoryFileSystem::file(const Path& logicPath) const {
	if(_files.find(logicPath) != _files.end())
		return VirtualFile(this, logicPath);
	return VirtualFile();
}


const MemFile* MemoryFileSystem::_fileBuffer(const Path& logicPath) const {
	auto it = _files.find(logicPath);
	return (it != _files.end())? &it->second: nullptr;
}


}
