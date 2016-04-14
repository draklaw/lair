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


#ifndef _LAIR_SYS_SDL2_IMAGE_LOADER_H
#define _LAIR_SYS_SDL2_IMAGE_LOADER_H


#include <lair/core/lair.h>
#include <lair/core/image.h>
#include <lair/core/path.h>
#include <lair/core/asset_manager.h>
#include <lair/core/loader.h>


namespace lair
{


class ImageLoader : public Loader {
public:
	typedef ImageAspect Aspect;

public:
	ImageLoader(LoaderManager* manager, AspectSP aspect);
	ImageLoader(const ImageLoader&) = delete;
	ImageLoader(ImageLoader&&)      = delete;
	~ImageLoader();

	ImageLoader& operator=(const ImageLoader&) = delete;
	ImageLoader& operator=(ImageLoader&&)      = delete;

protected:
	virtual void loadSyncImpl(Logger& log);
};


}


#endif
