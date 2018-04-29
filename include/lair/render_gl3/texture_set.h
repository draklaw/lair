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


#ifndef LAIR_RENDER_GL3_TEXTURE_SET_H
#define LAIR_RENDER_GL3_TEXTURE_SET_H


#include <lair/core/lair.h>
#include <lair/core/ldl.h>

#include <lair/meta/metatype.h>

#include <lair/render_gl3/sampler.h>
#include <lair/render_gl3/texture.h>


namespace lair
{


class LoaderManager;

struct TextureUnit {
	unsigned    index;
	const char* name;
};

struct TextureBinding {
	const TextureUnit* unit;
	SamplerSP          sampler;
	TextureAspectSP    texture;

	inline bool operator==(const TextureBinding& other) const {
		return unit    == other.unit
		    && sampler == other.sampler
		    && texture == other.texture;
	}

	inline bool operator!=(const TextureBinding& other) const {
		return !(*this == other);
	}

	inline bool operator<(const TextureBinding& other) const {
		return unit < other.unit;
	}
};

#define LAIR_TEXTURE_BINDING_END { nullptr, SamplerSP(), TextureAspectSP() }

inline Size hash(const TextureBinding& binding) {
	return hash(binding.unit->index, binding.sampler, binding.texture);
}


class TextureSet {
protected:
	typedef std::vector<TextureBinding> BindingList;

public:
	typedef BindingList::const_iterator BindingIterator;

public:
	TextureSet(const TextureBinding* bindings = nullptr);
	TextureSet(const TextureUnit* unit, TextureAspectSP texture, SamplerSP sampler);
	TextureSet(const TextureSet&) = default;
	TextureSet(TextureSet&&)      = default;
	~TextureSet() = default;

	TextureSet& operator=(const TextureSet&) = default;
	TextureSet& operator=(TextureSet&&)      = default;

	/**
	 * \brief Check if contained textures aspect are valid, warn and return
	 * false if not.
	 */
	bool checkTexturesValidity(Logger& logger) const;

	inline unsigned index() const { return _index; }
	inline void _setIndex(unsigned index) { _index = index; }

	TextureBinding* _get(const TextureUnit* unit);
	const TextureBinding* get(const TextureUnit* unit) const;
	TextureBinding* _getOrInsert(const TextureUnit* unit);

	const Texture* getTexture(const TextureUnit* unit) const;
	const Texture* getTextureOrWarn(const TextureUnit* unit, Logger& logger) const;
	TextureAspectSP getTextureAspect(const TextureUnit* unit) const;
	TextureAspectSP getTextureAspectOrWarn(const TextureUnit* unit, Logger& logger) const;

	void setTexture(const TextureUnit* unit, TextureAspectSP texture);
	void setTexture(const TextureUnit* unit, TextureAspectSP texture, SamplerSP sampler);

	SamplerSP getSampler(const TextureUnit* unit) const;
	void setSampler(const TextureUnit* unit, SamplerSP sampler);

	BindingIterator begin() const;
	BindingIterator end() const;

	bool operator==(const TextureSet& other) const;

	inline bool operator!=(const TextureSet& other) const {
		return !(*this == other);
	}

protected:
	BindingList _bindings;
	unsigned    _index;
};

Size hash(const TextureSet& texSet);


typedef std::shared_ptr<TextureSet>       TextureSetSP;
typedef std::shared_ptr<const TextureSet> TextureSetCSP;
typedef std::weak_ptr<TextureSet>         TextureSetWP;


bool ldlRead(LdlParser& parser, TextureSetCSP& textureSet, Renderer* renderer,
             LoaderManager* loader);
bool ldlWrite(LdlWriter& writer, const TextureSetCSP& textureSet);


}


LAIR_REGISTER_METATYPE(lair::TextureSetCSP, "TextureSet");


#endif
