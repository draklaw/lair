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


#ifndef _LAIR_RENDER_GL3_TEXTURE_H
#define _LAIR_RENDER_GL3_TEXTURE_H


#include <lair/core/lair.h>
#include <lair/core/asset_manager.h>

#include <lair/meta/metatype.h>

#include <lair/render_gl3/context.h>


namespace lair
{


class Context;
class Renderer;
class Image;


class Texture {
public:
	static constexpr unsigned DEFAULT_MAX_MIPMAP_LEVEL = 1000;

public:
	Texture(Renderer* renderer = nullptr);
	Texture(const Texture&) = delete;
	Texture(Texture&& other);
	~Texture();

	Texture& operator=(Texture other);

	inline bool   isValid()    const { return _id; }

	inline uint16   width()          const { return _width; }
	inline uint16   height()         const { return _height; }
	inline unsigned maxMipmapLevel() const { return _maxMipmapLevel; }

	void bind() const;

	bool _upload(const Image& image, unsigned maxMipmapLevel = DEFAULT_MAX_MIPMAP_LEVEL,
	             bool linear = false);

	friend void swap(Texture& t0, Texture& t1);

	void _release();
	inline GLuint _glId() { return _id; }

protected:
	Context*       _context;
	Renderer*      _renderer;
	GLenum         _target;
	GLuint         _id;
	uint16         _width;
	uint16         _height;
	GLuint         _format;
	// unsigned       _baseMipmapLevel;
	unsigned       _maxMipmapLevel;
	// unsigned       _swizzle;
	// TODO: Support base mipmap level & swizzling.
};


typedef std::shared_ptr<Texture> TextureSP;
typedef std::weak_ptr  <Texture> TextureWP;

typedef GenericAspect  <Texture>       TextureAspect;
typedef std::shared_ptr<TextureAspect> TextureAspectSP;
typedef std::weak_ptr  <TextureAspect> TextureAspectWP;


}


#endif
