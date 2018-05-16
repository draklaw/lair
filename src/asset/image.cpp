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


#include "lair/asset/image.h"


namespace lair {


unsigned Image::formatByteSize(Format format) {
	switch(format) {
	case FormatInvalid: return 0;
	case FormatRGB8 :   return 3;
	case FormatRGBA8:   return 4;
	}
	return 0;
}


Image::Image()
    : _width(0),
      _height(0),
      _format(FormatInvalid),
      _data() {
}


Image::Image(unsigned width, unsigned height, Format format,
             void* data)
    : _width(width),
      _height(height),
      _format(format),
      _data(reinterpret_cast<Byte*>(data),
            reinterpret_cast<Byte*>(data) + (width * height * formatByteSize(format))) {
}


bool Image::isValid() const {
	return _format != FormatInvalid;
}


unsigned Image::width() const {
	return _width;
}


unsigned Image::height() const {
	return _height;
}


Image::Format Image::format() const {
	return _format;
}


const void* Image::data() const {
	return _data.data();
}


size_t Image::sizeInBytes() const {
	return sizeof(Image) + _data.size();
}


}
