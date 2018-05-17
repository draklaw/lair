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


#ifndef LAIR_FS_MEMORY_FILE_SYSTEM_H
#define LAIR_FS_MEMORY_FILE_SYSTEM_H


#include <unordered_map>

#include <lair/core/lair.h>

#include <lair/fs/abstract_file_system.h>


namespace lair
{


class MemoryFileSystem : public AbstractFileSystem {
protected:
	typedef std::unordered_map<Path, MemFile, Hash<Path>> FileMap;

public:
	typedef FileMap::const_iterator Iterator;

public:
	MemoryFileSystem();
	MemoryFileSystem(const MemoryFileSystem&)  = delete;
	MemoryFileSystem(      MemoryFileSystem&&) = delete;
	~MemoryFileSystem() = default;

	MemoryFileSystem& operator=(const MemoryFileSystem&)  = delete;
	MemoryFileSystem& operator=(      MemoryFileSystem&&) = delete;

	void addFile(const Path& logicPath, Size size, const Byte* data);

	Iterator begin() const;
	Iterator end() const;

	virtual VirtualFile file(const Path& logicPath) const override;

	virtual const MemFile* _fileBuffer(const Path& logicPath) const override;

protected:
	FileMap _files;
};


typedef std::shared_ptr<MemoryFileSystem> MemoryFileSystemSP;


}


#endif
