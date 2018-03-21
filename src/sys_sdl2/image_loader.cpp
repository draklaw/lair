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


#include <SDL_surface.h>
#include <SDL_image.h>

#include <lair/core/lair.h>
#include <lair/core/log.h>

#include <lair/sys_sdl2/sys_module.h>

#include "lair/sys_sdl2/image_loader.h"


namespace lair
{


ImageLoader::ImageLoader(LoaderManager* manager, AspectSP aspect)
    : Loader(manager, aspect) {
}


void ImageLoader::commit() {
	ImageAspectSP aspect = std::static_pointer_cast<ImageAspect>(_aspect);
	aspect->_get() = std::move(_image);
	Loader::commit();
}


void ImageLoader::loadSyncImpl(Logger& log) {
	SDL_RWops* rw = sdlRwFromFile(file());
	if(!rw) {
		log.error("Failed to load image \"", asset()->logicPath(), "\": Invalid file");
		return;
	}

	auto surf = make_unique(IMG_Load_RW(rw, true), SDL_FreeSurface);
	if(surf) {
		Image::Format format;
		switch(surf->format->format) {
		case SDL_PIXELFORMAT_RGB24:
			format = Image::Format::FormatRGB8;
			break;
		case SDL_PIXELFORMAT_ABGR8888:
			format = Image::Format::FormatRGBA8;
			break;
		default:
			surf = make_unique(SDL_ConvertSurfaceFormat(surf.get(), SDL_PIXELFORMAT_ABGR8888, 0), SDL_FreeSurface);
			format = Image::Format::FormatRGBA8;
			break;
		}

		_image = std::move(Image(surf->w, surf->h, format, surf->pixels));
	} else {
		log.error("Failed to load image \"", asset()->logicPath(), "\": ", IMG_GetError());
	}
}


}
