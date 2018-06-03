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


#ifndef _LAIR_ASSET_IMAGE_H
#define _LAIR_ASSET_IMAGE_H


#include <cstdint>
#include <vector>

#include <lair/core/lair.h>

#include <lair/asset/asset_manager.h>


namespace lair {


class Image {
public:
	enum Format {
		FormatInvalid,
		FormatRGB8,
		FormatRGBA8,
	};

public:
	static unsigned formatByteSize(Format format);

public:
	Image();
	Image(unsigned width, unsigned height, Format format, void* data = nullptr);
	Image(const Image&) = delete;
	Image(Image&&)      = default;

	Image& operator=(const Image& rhs) = delete;
	Image& operator=(Image&& rhs)      = default;

	bool isValid() const;
	unsigned width() const;
	unsigned height() const;
	Format format() const;
	const void* data() const;
	size_t sizeInBytes() const;

private:
	typedef std::vector<Byte> Data;

private:
	unsigned _width;
	unsigned _height;
	Format _format;
	Data _data;
};

typedef std::shared_ptr<Image> ImageSP;
typedef std::weak_ptr  <Image> ImageWP;


typedef GenericAspect       <Image>       ImageAspect;
typedef IntrusivePointer    <ImageAspect> ImageAspectSP;
typedef IntrusiveWeakPointer<ImageAspect> ImageAspectWP;


}


#endif
