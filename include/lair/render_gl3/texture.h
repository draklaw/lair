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
#include <lair/core/metatype.h>
#include <lair/core/asset_manager.h>

#include <lair/render_gl3/context.h>


namespace lair
{


class Context;
class Renderer;
class Image;


class Texture {
public:
	enum {
		MAG_NEAREST        = 0 << 0,
		MAG_LINEAR         = 1 << 0,

		MIN_NEAREST        = 0 << 1,
		MIN_LINEAR         = 1 << 1,

		MIPMAP_NONE        = 0 << 2,
		MIPMAP_NEAREST     = 1 << 2,
		MIPMAP_LINEAR      = 2 << 2,

		NEAREST            = MAG_NEAREST | MIN_NEAREST | MIPMAP_NONE,
		BILINEAR_NO_MIPMAP = MAG_LINEAR  | MIN_LINEAR  | MIPMAP_NONE,
		BILINEAR_MIPMAP    = MAG_LINEAR  | MIN_LINEAR  | MIPMAP_NEAREST,
		TRILINEAR          = MAG_LINEAR  | MIN_LINEAR  | MIPMAP_LINEAR,

		REPEAT_S           = 0 << 4,
		CLAMP_S            = 1 << 4,
		MIRROR_S           = 2 << 4,

		REPEAT_T           = 0 << 6,
		CLAMP_T            = 1 << 6,
		MIRROR_T           = 2 << 6,

		REPEAT             = REPEAT_S | REPEAT_T,
		CLAMP              =  CLAMP_S |  CLAMP_T,
		MIRROR             = MIRROR_S | MIRROR_T,

		MAG_MASK           = (1 << 0),
		MIN_MASK           = (1 << 1),
		MIPMAP_MASK        = (1 << 2) | (1 << 3),
		FILTER_MASK        = MAG_MASK | MIN_MASK | MIPMAP_MASK,
		WRAP_S_MASK        = (1 << 4) | (1 << 5),
		WRAP_T_MASK        = (1 << 6) | (1 << 7),
		WRAP_MASK          = WRAP_S_MASK | WRAP_T_MASK
	};

public:
	Texture(Renderer* renderer = nullptr);
	Texture(const Texture&) = delete;
	Texture(Texture&& other);
	~Texture();

	Texture& operator=(Texture other);

	inline bool   isValid()    const { return _id; }

	inline uint32 magFilter()  const { return _flags &    MAG_MASK; }
	inline uint32 minFilter()  const { return _flags &    MIN_MASK; }
	inline uint32 mipmapMode() const { return _flags & MIPMAP_MASK; }
	inline uint32 wrapS()      const { return _flags & WRAP_S_MASK; }
	inline uint32 wrapT()      const { return _flags & WRAP_T_MASK; }
	inline uint32 wrap()       const { return _flags &   WRAP_MASK; }

	inline uint16 width()      const { return _width; }
	inline uint16 height()     const { return _height; }

	void bind() const;

	bool _upload(const Image& image);
	void _setFlags(uint32 flags);

	friend void swap(Texture& t0, Texture& t1);

	void _release();
	inline GLuint _glId() { return _id; }

	static const FlagsInfo* flagsInfo();

protected:
	Context*       _context;
	Renderer*      _renderer;
	GLuint         _id;
	uint32         _flags;
	uint16         _width;
	uint16         _height;
};

typedef std::shared_ptr<Texture> TextureSP;
typedef std::weak_ptr  <Texture> TextureWP;


typedef GenericAspect  <Texture>       TextureAspect;
typedef std::shared_ptr<TextureAspect> TextureAspectSP;
typedef std::weak_ptr  <TextureAspect> TextureAspectWP;


}


#endif
