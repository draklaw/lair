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
#include <lair/core/loader.h>

#include <lair/meta/variant_reader.h>
#include <lair/meta/variant_writer.h>

#include <lair/sys_sdl2/image_loader.h>

#include <lair/render_gl3/renderer.h>

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


TextureSet::TextureSet(const TextureUnit* unit, TextureAspectSP texture, SamplerSP sampler) {
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


TextureBinding* TextureSet::_get(const TextureUnit* unit) {
	auto binding = std::find_if(_bindings.begin(), _bindings.end(),
	                            [unit](const TextureBinding& b) {
		return unit == b.unit;
	});

	return (binding != _bindings.end())? &(*binding): nullptr;
}


const TextureBinding* TextureSet::get(const TextureUnit* unit) const {
	return const_cast<TextureSet*>(this)->_get(unit);
}


TextureBinding* TextureSet::_getOrInsert(const TextureUnit* unit) {
	auto binding = _get(unit);
	if(binding)
		return binding;

	_bindings.emplace_back(TextureBinding{unit, nullptr, nullptr});
	std::sort(_bindings.begin(), _bindings.end());

	return _get(unit);
}


const Texture* TextureSet::getTexture(const TextureUnit* unit) const {
	TextureAspectSP aspect = getTextureAspect(unit);
	return (aspect && aspect->isValid())? &aspect->get(): nullptr;
}


const Texture* TextureSet::getTextureOrWarn(const TextureUnit* unit, Logger& logger) const {
	TextureAspectSP aspect = getTextureAspectOrWarn(unit, logger);
	return aspect? &aspect->get(): nullptr;
}


TextureAspectSP TextureSet::getTextureAspect(const TextureUnit* unit) const {
	const TextureBinding* binding = get(unit);
	if(!binding || !binding->texture)
		return TextureAspectSP();
	return binding->texture;
}


TextureAspectSP TextureSet::getTextureAspectOrWarn(const TextureUnit* unit, Logger& logger) const {
	const TextureBinding* binding = get(unit);
	if(!binding || !binding->texture)
		return TextureAspectSP();
	if(!binding->texture->isValid()) {
		binding->texture->warnIfInvalid(logger);
		return TextureAspectSP();
	}
	return binding->texture;
}


void TextureSet::setTexture(const TextureUnit* unit, TextureAspectSP texture) {
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


void TextureSet::setTexture(const TextureUnit* unit, TextureAspectSP texture,
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


SamplerSP TextureSet::getSampler(const TextureUnit* unit) const {
	const TextureBinding* binding = get(unit);
	return binding? binding->sampler: nullptr;
}


void TextureSet::setSampler(const TextureUnit* unit, SamplerSP sampler) {
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
	for(; it0 != end0 && it1 != end1; ++it0, ++it1) {
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


bool ldlRead(LdlParser& parser, TextureBinding& binding, Renderer* renderer,
             LoaderManager* loader) {
	if(parser.isValueTyped() && (parser.getValueTypeName() != "Texture")) {
		parser.warning("Unexpected type annotation: expected Texture, got ",
		               parser.getValueTypeName());
	}

	bool success = true;
	if(parser.valueType() == LdlParser::TYPE_LIST) {
		parser.enter();

		if(parser.valueType() == LdlParser::TYPE_STRING) {
			String unitName = parser.getString();
			binding.unit = renderer->getTextureUnit(unitName);
			parser.next();
			if(!binding.unit) {
				parser.error("Unknown texture unit \"", unitName, "\"");
				success = false;
			}
		}
		else {
			parser.error("Invalid type: expected Int or texture unit name (String), got ", parser.valueTypeName());
			parser.skip();
			success = false;
		}

		if(success && parser.valueType() == LdlParser::TYPE_STRING) {
			AssetSP asset = loader->loadAsset<ImageLoader>(parser.getString());
			binding.texture = renderer->createTexture(asset);
			parser.next();
		}
		else if(success) {
			parser.error("Invalid type: expected Path (String), got ", parser.valueTypeName());
			parser.skip();
			success = false;
		}

		binding.sampler = nullptr;
		if(success && parser.valueType() != LdlParser::TYPE_END) {
			success = ldlRead(parser, binding.sampler, renderer);
		}

		if(success && parser.valueType() != LdlParser::TYPE_END) {
			parser.warning("Too many parameters in texture declaration. Ignoring.");
		}
		while(parser.valueType() != LdlParser::TYPE_END)
			parser.skip();

		parser.leave();
	}
	else {
		parser.error("Invalid type: expected VarList, got ", parser.valueTypeName());
		parser.skip();
		success = false;
	}

	return success;
}


bool ldlRead(LdlParser& parser, TextureSetCSP& textureSet, Renderer* renderer,
             LoaderManager* loader) {
	bool success  = true;
	bool typed    = parser.isValueTyped();

	if(typed && (parser.getValueTypeName() == "Texture")) {
		TextureBinding bindings[] = {
		    { nullptr, nullptr, nullptr },
		    LAIR_TEXTURE_BINDING_END
		};

		success = ldlRead(parser, bindings[0], renderer, loader);
		if(success) {
			textureSet = renderer->getTextureSet(TextureSet(bindings));
		}
	}
	else if(typed && (parser.getValueTypeName() == "TextureSet")) {
		if(parser.valueType() == LdlParser::TYPE_STRING) {
			textureSet = renderer->getTextureSet(parser.getString());
			parser.next();
			success = bool(textureSet);
		}
		else if(parser.valueType() == LdlParser::TYPE_LIST) {
			std::vector<TextureBinding> bindings;
			bindings.emplace_back(TextureBinding{nullptr, nullptr, nullptr});

			parser.enter();
			while(parser.valueType() != LdlParser::TYPE_END) {
				bool ok = ldlRead(parser, bindings.back(), renderer, loader);
				if(ok) {
					bindings.emplace_back(TextureBinding{nullptr, nullptr, nullptr});
				}
			}
			parser.leave();

			textureSet = renderer->getTextureSet(TextureSet(bindings.data()));

			success = bool(textureSet);
		}
		else {
			parser.error("Invalid type: expected identifier (String) or a list "
			             "of textures (VarList), got ", parser.valueTypeName());
			parser.skip();
			success = false;
		}
	}
	else {
		parser.error("Unexpected type annotation: expected TextureSet or Texture, got ",
		             typed? parser.getValueTypeName(): "none");
		parser.skip();
		success = false;
	}

	return success;
}


bool ldlWrite(LdlWriter& writer, const TextureSetCSP& textureSet) {
	bool success = true;

	writer.openList(LdlWriter::CF_MULTI_LINE, "TextureSet");
	for(const TextureBinding& binding: *textureSet) {
		if(!binding.texture)
			continue;

		writer.openList(LdlWriter::CF_SINGLE_LINE, "Texture");

		writer.writeString(binding.unit->name);

		writer.writeString(binding.texture->asset()->logicPath().utf8String());

		if(binding.sampler)
			success &= ldlWrite(writer, binding.sampler);

		writer.close();
	}
	writer.close();

	return success;
}


bool varRead(TextureBinding& value, const Variant& var, Renderer* renderer,
             LoaderManager* loader, Logger& logger) {
	bool success = true;

	if(var.isVarList()) {
		const VarList& varList = var.asVarList();

		if(varList.type() != "Texture") {
			logger.warning(var.parseInfoDesc(), "Unexpected type annotation: expected Texture, got ",
			               varList.type());
		}

		if(varList.size() < 2) {
			logger.error(var.parseInfoDesc(), "Texture binding takes at least 2 parameters, got ", varList.size(), ".");
			success = false;
		}

		if(varList.size() > 0) {
			String unitName;
			success &= varRead(unitName, varList[0], logger);
			value.unit = renderer->getTextureUnit(unitName);
			if(!value.unit) {
				logger.error(varList[0].parseInfoDesc(), "Unknown texture unit \"", unitName, "\".");
				success = false;
			}
		}

		if(varList.size() > 1) {
			String path;
			success &= varRead(path, varList[1], logger);
			if(success) {
				AssetSP asset = loader->loadAsset<ImageLoader>(path);
				value.texture = renderer->createTexture(asset);
			}
		}

		if(varList.size() > 2) {
			success &= varRead(value.sampler, varList[2], renderer, logger);
		}

		if(varList.size() > 3) {
			logger.warning(varList[3].parseInfoDesc(), "Too many parameters in texture binding, ignoring.");
		}
	}
	else {
		logger.error(var.parseInfoDesc(), "Invalid type: expected TextureBinding (VarList), got ", var.typeName());
		success = false;
	}

	return success;
}


bool varWrite(Variant& var, const TextureBinding& value, Logger& logger) {
	bool success = true;
	Variant v;

	VarList varList("Texture");

	success &= varWrite(v, String(value.unit->name), logger);
	varList.emplace_back(std::move(v));

	success &= varWrite(v, value.texture->asset()->logicPath().utf8String(), logger);
	varList.emplace_back(std::move(v));

	if(value.sampler) {
		success &= varWrite(v, value.sampler, logger);
		varList.emplace_back(std::move(v));
	}

	if(success)
		var = std::move(varList);

	return success;
}


bool varRead(TextureSetCSP& value, const Variant& var, Renderer* renderer,
             LoaderManager* loader, Logger& logger) {
	bool success = true;

	if(var.isString()) {
		value = renderer->getTextureSet(var.asString());
		success = bool(value);
	}
	else if(var.isVarList()) {
		const VarList& varList = var.asVarList();
		if(varList.type() == "Texture") {
			TextureBinding bindings[] = {
			    { nullptr, nullptr, nullptr },
			    LAIR_TEXTURE_BINDING_END
			};

			success = varRead(bindings[0], var, renderer, loader, logger);
			if(success) {
				value = renderer->getTextureSet(TextureSet(bindings));
			}
		}
		else if(varList.type() == "TextureSet") {
			std::vector<TextureBinding> bindings;
			bindings.emplace_back(TextureBinding{nullptr, nullptr, nullptr});

			for(auto&& v: varList) {
				bool ok = varRead(bindings.back(), v, renderer, loader, logger);
				if(ok) {
					bindings.emplace_back(TextureBinding{nullptr, nullptr, nullptr});
				}
			}

			value = renderer->getTextureSet(TextureSet(bindings.data()));

			success = bool(value);
		}
		else {
			logger.error(var.parseInfoDesc(), "Unexpected type annotation: expected TextureSet or Texture, got \"",
			             varList.type(), "\".");
			success = false;
		}
	}
	else {
		logger.error(var.parseInfoDesc(), "Invalid type: expected identifier (String) or a list "
		             "of textures (VarList), got ", var.typeName());
		success = false;
	}

	return success;
}


bool varWrite(Variant& var, const TextureSetCSP& value, Logger& logger) {
	bool success = true;

	if(value->size() == 1) {
		const TextureBinding& binding = *value->begin();
		success = varWrite(var, binding, logger);
	}
	else {
		VarList varList("TextureSet");
		for(auto&& binding: *value) {
			if(binding.texture)
				 success &= varWrite(var, binding, logger);
		}
		if(success)
			var = std::move(varList);
	}

	return success;
}


}


LAIR_IMPLEMENT_METATYPE(lair::TextureSetCSP, "TextureSet");
