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

#include "lair/fs/abstract_file_system.h"


namespace lair
{


VirtualFile::VirtualFile()
    : _fs(nullptr)
    , _relativePath() {
}


VirtualFile::VirtualFile(const AbstractFileSystem* fs, const Path& relativePath)
    : _fs(fs)
    , _relativePath(relativePath) {
}


bool VirtualFile::isValid() const {
	return _fs;
}


VirtualFile::operator bool() const {
	return _fs;
}


const MemFile* VirtualFile::fileBuffer() const {
	lairAssert(_fs);
	return _fs->_fileBuffer(_relativePath);
}


Path VirtualFile::realPath() const {
	lairAssert(_fs);
	return _fs->_realPath(_relativePath);
}



AbstractFileSystem::AbstractFileSystem() {
}


const MemFile* AbstractFileSystem::_fileBuffer(const Path& /*logicPath*/) const {
	return nullptr;
}


Path AbstractFileSystem::_realPath(const Path& /*logicPath*/) const {
	return Path();
}


}
