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


#ifndef LAIR_RENDER_GL3_SAMPLER_H
#define LAIR_RENDER_GL3_SAMPLER_H


#include <lair/core/lair.h>
#include <lair/core/hash.h>

#include <lair/meta/metatype.h>
#include <lair/meta/flags_info.h>
#include <lair/meta/variant.h>

#include <lair/render_gl3/context.h>


namespace lair
{


class LdlParser;
class LdlWriter;

class Context;
class Renderer;


class SamplerParams {
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
		BORDER_S           = 3 << 4,

		REPEAT_T           = 0 << 6,
		CLAMP_T            = 1 << 6,
		MIRROR_T           = 2 << 6,
		BORDER_T           = 3 << 6,

		REPEAT_R           = 0 << 8,
		CLAMP_R            = 1 << 8,
		MIRROR_R           = 2 << 8,
		BORDER_R           = 3 << 8,

		REPEAT             = REPEAT_S | REPEAT_T | REPEAT_R,
		CLAMP              =  CLAMP_S |  CLAMP_T |  CLAMP_R,
		MIRROR             = MIRROR_S | MIRROR_T | MIRROR_R,
		BORDER             = BORDER_S | BORDER_T | BORDER_R,

		COMPARE_DISABLED   = 0 << 9,
		COMPARE_ENABLED    = 1 << 9,

		COMPARE_LEQUAL      = COMPARE_ENABLED | 0 << 10,
		COMPARE_GEQUAL      = COMPARE_ENABLED | 1 << 10,
		COMPARE_LESS        = COMPARE_ENABLED | 2 << 10,
		COMPARE_GREATER     = COMPARE_ENABLED | 3 << 10,
		COMPARE_EQUAL       = COMPARE_ENABLED | 4 << 10,
		COMPARE_NOT_EQUAL   = COMPARE_ENABLED | 5 << 10,
		COMPARE_ALWAYS      = COMPARE_ENABLED | 6 << 10,
		COMPARE_NEVER       = COMPARE_ENABLED | 7 << 10,

		MAG_MASK           = (1 << 0),
		MIN_MASK           = (1 << 1),
		MIPMAP_MASK        = (1 << 2) | (1 << 3),
		FILTER_MASK        = MAG_MASK | MIN_MASK | MIPMAP_MASK,
		WRAP_S_MASK        = (1 << 4) | (1 << 5),
		WRAP_T_MASK        = (1 << 6) | (1 << 7),
		WRAP_R_MASK        = (1 << 8) | (1 << 9),
		WRAP_MASK          = WRAP_S_MASK | WRAP_T_MASK | WRAP_R_MASK,
		COMPARE_MASK       = (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12),

		DEFAULT_FLAGS = MAG_LINEAR | MIN_NEAREST | MIPMAP_LINEAR |
		                REPEAT | COMPARE_DISABLED,
	};

	static constexpr float DEFAULT_ANISOTROPY = 1.0f;
	static constexpr float DEFAULT_LOD_BIAS   = 0.0f;
	static constexpr float DEFAULT_MIN_LOD    = -1000.0f;
	static constexpr float DEFAULT_MAX_LOD    = 1000.0f;

public:
	SamplerParams(uint32 flags = DEFAULT_FLAGS, float aniso = DEFAULT_ANISOTROPY,
	              float lodBias = DEFAULT_LOD_BIAS,
	              float minLod = DEFAULT_MIN_LOD, float maxLod = DEFAULT_MAX_LOD);

#define DECL_SAMPLER_GETSET(_name, _setter, _mask) \
	inline uint32 _name() const { \
	    return flags & _mask; \
    } \
	inline void _setter(uint32 _name) { \
	    lairAssert((_name & _mask) == _name); \
	    flags = (flags & ~_mask) | _name; \
    }

	DECL_SAMPLER_GETSET(magFilter,  setMagFilter,      MAG_MASK)
	DECL_SAMPLER_GETSET(minFilter,  setMinFilter,      MIN_MASK)
	DECL_SAMPLER_GETSET(mipmapMode, setMipmapMode,  MIPMAP_MASK)
	DECL_SAMPLER_GETSET(wrapS,      setWrapS,       WRAP_S_MASK)
	DECL_SAMPLER_GETSET(wrapT,      setWrapT,       WRAP_T_MASK)
	DECL_SAMPLER_GETSET(wrapR,      setWrapR,       WRAP_R_MASK)
	DECL_SAMPLER_GETSET(wrap,       setWrap,          WRAP_MASK)
	DECL_SAMPLER_GETSET(compare,    setCompare,    COMPARE_MASK)

#undef DECL_SAMPLER_GETSET

	static const FlagsInfo* flagsInfo();

	inline bool operator==(const SamplerParams& other) const {
		return flags         == other.flags
		    && maxAnisotropy == other.maxAnisotropy
		    && lodBias       == other.lodBias
		    && minLod        == other.minLod
		    && maxLod        == other.maxLod;
	}

	inline bool operator!=(const SamplerParams& other) const {
		return !(*this == other);
	}

public:
	uint32 flags;
	float  maxAnisotropy;
	float  lodBias;
	float  minLod;
	float  maxLod;
	// Vector4 _color; // Default: 0, 0, 0, 0
	// TODO: Support border color ? The difficulty is that the type of the
	// components (float, uint, int) must match the texture internal format.
};

inline Size hash(const SamplerParams& params) {
	return hash(params.flags, params.maxAnisotropy, params.lodBias,
	            params.minLod, params.maxLod);
}


class Sampler {
public:
	Sampler(Context* context, const SamplerParams& params = SamplerParams());
	Sampler(const Sampler& ) = delete;
	Sampler(      Sampler&&) = delete;
	~Sampler();

	Sampler& operator=(const Sampler& ) = delete;
	Sampler& operator=(      Sampler&&) = delete;

	inline const SamplerParams& params() const { return _params; }

	void bind(unsigned unit);

	inline void _setParams(const SamplerParams& params) {
		_params = params;
		_dirty  = true;
	}

	void _release();

protected:
	Context*      _context;
	GLuint        _sampler;
	SamplerParams _params;
	bool          _dirty;
};


typedef std::shared_ptr<Sampler> SamplerSP;
typedef std::weak_ptr  <Sampler> SamplerWP;


bool ldlRead(LdlParser& parser, SamplerSP& sampler, Renderer* renderer);
bool ldlWrite(LdlWriter& writer, const SamplerSP& sampler);

bool varRead(SamplerSP& value, const Variant& var, Renderer* renderer, Logger& logger = noopLogger);
bool varWrite(Variant& var, const SamplerSP& value, Logger& logger = noopLogger);


}


LAIR_DECLARE_METATYPE(lair::Sampler)


#endif
