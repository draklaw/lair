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


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include "lair/render_gl3/texture_set.h"


namespace lair
{


TextureSet::TextureSet(const TextureBinding* bindings)
    : _index(0xffffffff) {
	if(bindings) {
		const TextureBinding* end = bindings;
		while(end->texture) ++end;
		_bindings.assign(bindings, end);
		std::sort(_bindings.begin(), _bindings.end());
	}
}


TextureSet::TextureSet(unsigned unit, TextureAspectSP texture, SamplerSP sampler) {
	setTexture(unit, texture, sampler);
}


bool TextureSet::checkTexturesValidity(Logger& logger) const {
	bool isValid = true;
	for(const TextureBinding& binding: *this) {
		if(!binding.texture || !binding.texture->isValid()) {
			binding.texture->warnIfInvalid(logger);
			isValid = false;
		}
	}
	return isValid;
}


TextureBinding* TextureSet::_get(unsigned unit) {
	auto binding = std::find_if(_bindings.begin(), _bindings.end(),
	                            [unit](const TextureBinding& b) {
		return unit == b.unit;
	});

	return (binding != _bindings.end())? &(*binding): nullptr;
}


const TextureBinding* TextureSet::get(unsigned unit) const {
	return const_cast<TextureSet*>(this)->_get(unit);
}


TextureBinding* TextureSet::_getOrInsert(unsigned unit) {
	auto binding = _get(unit);
	if(binding)
		return binding;

	_bindings.emplace_back(TextureBinding{unit, nullptr, nullptr});
	std::sort(_bindings.begin(), _bindings.end());

	return _get(unit);
}


const Texture* TextureSet::getTexture(unsigned unit) const {
	TextureAspectSP aspect = getTextureAspect(unit);
	return (aspect && aspect->isValid())? &aspect->get(): nullptr;
}


const Texture* TextureSet::getTextureOrWarn(unsigned unit, Logger& logger) const {
	TextureAspectSP aspect = getTextureAspectOrWarn(unit, logger);
	return aspect? &aspect->get(): nullptr;
}


TextureAspectSP TextureSet::getTextureAspect(unsigned unit) const {
	const TextureBinding* binding = get(unit);
	if(!binding || !binding->texture)
		return TextureAspectSP();
	return binding->texture;
}


TextureAspectSP TextureSet::getTextureAspectOrWarn(unsigned unit, Logger& logger) const {
	const TextureBinding* binding = get(unit);
	if(!binding || !binding->texture)
		return TextureAspectSP();
	if(!binding->texture->isValid()) {
		binding->texture->warnIfInvalid(logger);
		return TextureAspectSP();
	}
	return binding->texture;
}


void TextureSet::setTexture(unsigned unit, TextureAspectSP texture) {
	if(texture) {
		TextureBinding* binding = _getOrInsert(unit);
		binding->texture = texture;
	}
	else {
		_bindings.erase(std::find_if(_bindings.begin(), _bindings.end(),
		                             [unit](const TextureBinding& b) {
			return unit == b.unit;
		}));
	}
}


void TextureSet::setTexture(unsigned unit, TextureAspectSP texture,
                SamplerSP sampler) {
	if(texture) {
		TextureBinding* binding = _getOrInsert(unit);
		binding->texture = texture;
		binding->sampler = sampler;
	}
	else {
		_bindings.erase(std::find_if(_bindings.begin(), _bindings.end(),
		                             [unit](const TextureBinding& b) {
			return unit == b.unit;
		}));
	}
}


SamplerSP TextureSet::getSampler(unsigned unit) const {
	const TextureBinding* binding = get(unit);
	return binding? binding->sampler: nullptr;
}


void TextureSet::setSampler(unsigned unit, SamplerSP sampler) {
	TextureBinding* binding = _get(unit);
	if(binding) {
		binding->sampler = sampler;
	}
}


TextureSet::BindingIterator TextureSet::begin() const {
	return _bindings.begin();
}


TextureSet::BindingIterator TextureSet::end() const {
	return _bindings.end();
}


bool TextureSet::operator==(const TextureSet& other) const {
	auto it0  = begin();
	auto it1  = other.begin();
	auto end0 = end();
	auto end1 = other.end();
	for(; it0 != end0 || it1 != end1; ++it0, ++it1) {
		if(*it0 != *it1)
			return false;
	}
	return it0 == end0 && it1 == end1;
}


Size hash(const TextureSet& texSet) {
	Size h = 0;
	for(const TextureBinding& binding: texSet) {
		h = combineHash(h, hash(binding));
	}
	return h;
}


}
