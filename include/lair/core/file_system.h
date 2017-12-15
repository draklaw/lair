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


#ifndef _LAIR_CORE_FILE_SYSTEM_H
#define _LAIR_CORE_FILE_SYSTEM_H


#include <lair/core/lair.h>
#include <lair/core/path.h>


namespace lair
{


enum FileFlags {
	FILE_MEMORY     = 0x01,
	FILE_FILESYSTEM = 0x02,
	FILE_STREAM     = 0x04,
};

struct MemFile {
	Byte* data;
	Size  size;
};

class AbstractFileSystem {
public:
	AbstractFileSystem();
	AbstractFileSystem(const AbstractFileSystem&) = delete;
	AbstractFileSystem(AbstractFileSystem&&)      = delete;
	virtual ~AbstractFileSystem() = default;

	AbstractFileSystem& operator=(const AbstractFileSystem&) = delete;
	AbstractFileSystem& operator=(AbstractFileSystem&&)      = delete;

	virtual FileFlags fileFlags(const Path& logicPath) const;

	virtual MemFile       fileBuffer(const Path& logicPath) const;
	virtual Path          realPath  (const Path& logicPath) const;
	virtual Path::IStream stream    (const Path& logicPath) const;

protected:
	AbstractFileSystem* _parent;
};


class FileSystem : public AbstractFileSystem {
public:
	FileSystem() = default;
	FileSystem(const FileSystem&)  = delete;
	FileSystem(      FileSystem&&) = delete;
	virtual ~FileSystem() = default;

	FileSystem& operator=(const FileSystem&)  = delete;
	FileSystem& operator=(      FileSystem&&) = delete;

	void registerFileSystem(AbstractFileSystem* fileSystem);

	virtual FileFlags fileFlags(const Path& logicPath) const;

	virtual MemFile       fileBuffer(const Path& logicPath) const;
	virtual Path          realPath  (const Path& logicPath) const;
	virtual Path::IStream stream    (const Path& logicPath) const;

protected:
	struct ChildFS {
		Path                root;
		AbstractFileSystem* fs;
	};

	typedef std::vector<ChildFS> FSVector;

protected:
	FSVector _fileSystems;
};


class MemFileSystem {
public:
	MemFileSystem() = default;
	MemFileSystem(const MemFileSystem&)  = delete;
	MemFileSystem(      MemFileSystem&&) = delete;
	~MemFileSystem() = default;

	MemFileSystem& operator=(const MemFileSystem&)  = delete;
	MemFileSystem& operator=(      MemFileSystem&&) = delete;

	void addStaticFile(const Path& path, Byte* data, size_t size);

	virtual FileFlags fileFlags(const Path& logicPath) const;
	virtual MemFile fileBuffer(const Path& logicPath) const;

protected:
	typedef std::unordered_map<Path, MemFile> FileMap;

protected:
	FileMap _files;
};



}


#endif
