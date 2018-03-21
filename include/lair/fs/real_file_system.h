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


#ifndef LAIR_FS_REAL_FILE_SYSTEM_H
#define LAIR_FS_REAL_FILE_SYSTEM_H


#include <lair/core/lair.h>

#include <lair/fs/abstract_file_system.h>


namespace lair
{


class RealFileSystem : public AbstractFileSystem {
public:
	RealFileSystem(const Path& basePath);
	RealFileSystem(const RealFileSystem&) = delete;
	RealFileSystem(RealFileSystem&&)      = delete;
	~RealFileSystem() = default;

	RealFileSystem& operator=(const RealFileSystem&) = delete;
	RealFileSystem& operator=(RealFileSystem&&)      = delete;

	void setBasePath(const Path& basePath);

	virtual VirtualFile file(const Path& logicPath) const override;

	virtual Path           _realPath  (const Path& logicPath) const override;

protected:
	Path _basePath;
};


typedef std::shared_ptr<RealFileSystem> RealFileSystemSP;

}


#endif
