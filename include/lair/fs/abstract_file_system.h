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


#ifndef LAIR_FS_ABSTRACT_FILE_SYSTEM_H
#define LAIR_FS_ABSTRACT_FILE_SYSTEM_H


#include <lair/core/lair.h>

#include <lair/core/lair.h>
#include <lair/core/path.h>


namespace lair
{


class AbstractFileSystem;


struct MemFile {
	Size        size;
	const Byte* data;
};


class VirtualFile {
public:
	VirtualFile();
	VirtualFile(const AbstractFileSystem* fs, const Path& relativePath);

	bool isValid() const;
	explicit operator bool() const;

	const MemFile* fileBuffer() const;
	Path           realPath  () const;

protected:
	const AbstractFileSystem* _fs;
	Path                      _relativePath;
};


class AbstractFileSystem {
public:
	AbstractFileSystem();
	AbstractFileSystem(const AbstractFileSystem&) = delete;
	AbstractFileSystem(AbstractFileSystem&&)      = delete;
	virtual ~AbstractFileSystem() = default;

	AbstractFileSystem& operator=(const AbstractFileSystem&) = delete;
	AbstractFileSystem& operator=(AbstractFileSystem&&)      = delete;

	virtual VirtualFile file(const Path& logicPath) const = 0;

	virtual const MemFile* _fileBuffer(const Path& logicPath) const;
	virtual Path           _realPath  (const Path& logicPath) const;

protected:
	AbstractFileSystem* _parent;
};


typedef std::shared_ptr<AbstractFileSystem> AbstractFileSystemSP;

}


#endif
