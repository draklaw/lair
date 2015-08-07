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


#ifndef _LAIR_UTILS_IMAGE_H
#define _LAIR_UTILS_IMAGE_H


#include <cstdint>
#include <vector>


namespace lair {


class Image {
public:
	enum Format {
		FormatInvalid,
		FormatRGBA8,
	};

public:
	static unsigned formatByteSize(Format format);

public:
	Image();
	Image(unsigned width, unsigned height, Format format, void* data = nullptr);

	bool isValid() const;
	unsigned width() const;
	unsigned height() const;
	Format format() const;
	const void* data() const;

private:
	typedef std::vector<std::uint8_t> Data;

private:
	unsigned _width;
	unsigned _height;
	Format _format;
	Data _data;
};


}


#endif
