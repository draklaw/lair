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


#ifndef LAIR_FS_FILE_SYSTEM_H
#define LAIR_FS_FILE_SYSTEM_H


#include <lair/core/lair.h>
#include <lair/core/path.h>

#include <lair/fs/abstract_file_system.h>


namespace lair
{


class FileSystem : public AbstractFileSystem {
public:
	FileSystem();
	FileSystem(const FileSystem&)  = delete;
	FileSystem(      FileSystem&&) = delete;
	virtual ~FileSystem() = default;

	FileSystem& operator=(const FileSystem&)  = delete;
	FileSystem& operator=(      FileSystem&&) = delete;

	void addFileSystem(const Path& root, AbstractFileSystemSP fileSystem);

	virtual VirtualFile file(const Path& logicPath) const override;

	virtual const MemFile* _fileBuffer(const Path& logicPath) const override;
	virtual Path           _realPath  (const Path& logicPath) const override;

protected:
	struct ChildFS {
		Path                 root;
		AbstractFileSystemSP fs;
	};

	typedef std::vector<ChildFS> FSVector;

protected:
	FSVector _fileSystems;
};

typedef std::shared_ptr<FileSystem> FileSystemSP;


}


#endif
