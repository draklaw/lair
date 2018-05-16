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

#include <lair/ldl/ldl.h>

#include <lair/meta/variant_reader.h>
#include <lair/meta/variant_writer.h>
#include <lair/meta/var_list.h>

#include <lair/render_gl3/renderer.h>

#include "lair/render_gl3/sampler.h"


namespace lair
{


SamplerParams::SamplerParams(uint32 flags, float aniso, float lodBias,
                             float minLod, float maxLod)
    : flags(flags)
    , maxAnisotropy(aniso)
    , lodBias(lodBias)
    , minLod(minLod)
    , maxLod(maxLod) {
}


const FlagsInfo* SamplerParams::flagsInfo() {
	static FlagsInfo info("TextureFlags");
	if(!info.nFlagSet()) {
		info.add(DEFAULT_FLAGS,      ~uint32(0),  "default");

		info.add(NEAREST,            FILTER_MASK, "nearest");
		info.add(BILINEAR_NO_MIPMAP, FILTER_MASK, "bilinear_no_mipmap");
		info.add(BILINEAR_MIPMAP,    FILTER_MASK, "bilinear_mipmap");
		info.add(TRILINEAR,          FILTER_MASK, "trilinear");

		info.add(REPEAT, WRAP_MASK, "repeat");
		info.add(CLAMP,  WRAP_MASK, "clamp");
		info.add(MIRROR, WRAP_MASK, "mirror");

		info.add(MAG_NEAREST, MAG_MASK, "mag_nearest");
		info.add(MAG_LINEAR,  MAG_MASK, "mag_linear");

		info.add(MIN_NEAREST, MIN_MASK, "min_nearest");
		info.add(MIN_LINEAR,  MIN_MASK, "min_linear");

		// MIMAP_NONE do not need to be explicit.
		info.add(MIPMAP_NEAREST, MIPMAP_MASK, "mipmap_nearest");
		info.add(MIPMAP_LINEAR,  MIPMAP_MASK, "mipmap_linear");

		info.add(REPEAT_S, WRAP_S_MASK, "repeat_s");
		info.add(CLAMP_S,  WRAP_S_MASK, "clamp_s");
		info.add(MIRROR_S, WRAP_S_MASK, "mirror_s");

		info.add(REPEAT_T, WRAP_T_MASK, "repeat_t");
		info.add(CLAMP_T,  WRAP_T_MASK, "clamp_t");
		info.add(MIRROR_T, WRAP_T_MASK, "mirror_t");

		// COMPARE_DISABLED do not need to be explicit.
		info.add(COMPARE_LEQUAL,    COMPARE_MASK, "cmp_lequal");
		info.add(COMPARE_GEQUAL,    COMPARE_MASK, "cmp_gequal");
		info.add(COMPARE_LESS,      COMPARE_MASK, "cmp_less");
		info.add(COMPARE_GREATER,   COMPARE_MASK, "cmp_greater");
		info.add(COMPARE_EQUAL,     COMPARE_MASK, "cmp_equal");
		info.add(COMPARE_NOT_EQUAL, COMPARE_MASK, "cmp_not_equal");
		info.add(COMPARE_ALWAYS,    COMPARE_MASK, "cmp_always");
		info.add(COMPARE_NEVER,     COMPARE_MASK, "cmp_never");
	}
	return &info;
}


Sampler::Sampler(Context* context, const SamplerParams& params)
    : _context(context)
    , _sampler(0)
    , _params(params)
    , _dirty(true) {
}


Sampler::~Sampler() {
	_release();
}


void Sampler::bind(unsigned unit) {
	typedef SamplerParams SP;

	if(!_sampler) {
		_context->genSamplers(1, &_sampler);
	}

	if(_dirty) {
		_dirty = false;

		GLenum mag = gl::NEAREST;
		switch(_params.magFilter()) {
		case SP::MAG_NEAREST: mag = gl::NEAREST; break;
		case SP::MAG_LINEAR:  mag = gl::LINEAR; break;
		}

		GLenum min = gl::NEAREST;
		switch(_params.minFilter() | _params.mipmapMode()) {
		case SP::MIN_NEAREST | SP::MIPMAP_NONE:    min = gl::NEAREST; break;
		case SP::MIN_LINEAR  | SP::MIPMAP_NONE:    min = gl::LINEAR; break;
		case SP::MIN_NEAREST | SP::MIPMAP_NEAREST: min = gl::NEAREST_MIPMAP_NEAREST; break;
		case SP::MIN_LINEAR  | SP::MIPMAP_NEAREST: min = gl::LINEAR_MIPMAP_NEAREST; break;
		case SP::MIN_NEAREST | SP::MIPMAP_LINEAR:  min = gl::NEAREST_MIPMAP_LINEAR; break;
		case SP::MIN_LINEAR  | SP::MIPMAP_LINEAR:  min = gl::LINEAR_MIPMAP_LINEAR; break;
		}

		GLenum wraps = gl::REPEAT;
		switch(_params.wrapS()) {
		case SP::REPEAT_S: wraps = gl::REPEAT; break;
		case SP::CLAMP_S:  wraps = gl::CLAMP_TO_EDGE; break;
		case SP::MIRROR_S: wraps = gl::MIRRORED_REPEAT; break;
		}

		GLenum wrapt = gl::REPEAT;
		switch(_params.wrapT()) {
		case SP::REPEAT_T: wrapt = gl::REPEAT; break;
		case SP::CLAMP_T:  wrapt = gl::CLAMP_TO_EDGE; break;
		case SP::MIRROR_T: wrapt = gl::MIRRORED_REPEAT; break;
		}

		GLenum wrapr = gl::REPEAT;
		switch(_params.wrapR()) {
		case SP::REPEAT_R: wrapt = gl::REPEAT; break;
		case SP::CLAMP_R:  wrapt = gl::CLAMP_TO_EDGE; break;
		case SP::MIRROR_R: wrapt = gl::MIRRORED_REPEAT; break;
		}

		GLenum compareMode = gl::NONE;
		GLenum compareFunc = gl::LEQUAL;
		switch(_params.compare()) {
		case SP::COMPARE_DISABLED:
			compareMode = gl::NONE;
			compareFunc = gl::LEQUAL;
			break;
		case SP::COMPARE_LEQUAL:
			compareMode = gl::COMPARE_REF_TO_TEXTURE;
			compareFunc = gl::LEQUAL;
			break;
		case SP::COMPARE_GEQUAL:
			compareMode = gl::COMPARE_REF_TO_TEXTURE;
			compareFunc = gl::GEQUAL;
			break;
		case SP::COMPARE_LESS:
			compareMode = gl::COMPARE_REF_TO_TEXTURE;
			compareFunc = gl::LESS;
			break;
		case SP::COMPARE_GREATER:
			compareMode = gl::COMPARE_REF_TO_TEXTURE;
			compareFunc = gl::GREATER;
			break;
		case SP::COMPARE_EQUAL:
			compareMode = gl::COMPARE_REF_TO_TEXTURE;
			compareFunc = gl::EQUAL;
			break;
		case SP::COMPARE_NOT_EQUAL:
			compareMode = gl::COMPARE_REF_TO_TEXTURE;
			compareFunc = gl::NOTEQUAL;
			break;
		case SP::COMPARE_ALWAYS:
			compareMode = gl::COMPARE_REF_TO_TEXTURE;
			compareFunc = gl::ALWAYS;
			break;
		case SP::COMPARE_NEVER:
			compareMode = gl::COMPARE_REF_TO_TEXTURE;
			compareFunc = gl::NEVER;
			break;
		}

		_context->samplerParameteri(_sampler, gl::TEXTURE_MAG_FILTER,   mag);
		_context->samplerParameteri(_sampler, gl::TEXTURE_MIN_FILTER,   min);
		_context->samplerParameteri(_sampler, gl::TEXTURE_WRAP_S,       wraps);
		_context->samplerParameteri(_sampler, gl::TEXTURE_WRAP_T,       wrapt);
		_context->samplerParameteri(_sampler, gl::TEXTURE_WRAP_R,       wrapr);
		_context->samplerParameteri(_sampler, gl::TEXTURE_COMPARE_MODE, compareMode);
		_context->samplerParameteri(_sampler, gl::TEXTURE_COMPARE_FUNC, compareFunc);

		_context->samplerParameterf(_sampler, gl::TEXTURE_LOD_BIAS,_params.lodBias);
		_context->samplerParameterf(_sampler, gl::TEXTURE_MIN_LOD, _params.minLod);
		_context->samplerParameterf(_sampler, gl::TEXTURE_MAX_LOD, _params.maxLod);

		if(_context->_gl_ext_texture_filter_anisotropic) {
			_context->samplerParameterf(_sampler, gl::TEXTURE_MAX_ANISOTROPY_EXT,
			                                      _params.maxAnisotropy);
		}
	}

	_context->bindSampler(unit, _sampler);
}


void Sampler::_release() {
	if(_sampler) {
		_context->deleteSamplers(1, &_sampler);
		_sampler = 0;
	}
}


bool ldlRead(LdlParser& parser, SamplerSP& sampler, Renderer* renderer) {
	if(parser.isValueTyped() && parser.getValueTypeName() != "Sampler") {
		parser.warning("Unexpected type annotation: expected Sampler, got ", parser.getValueTypeName());
	}

	bool success = true;
	SamplerParams params;
	if(parser.valueType() == LdlParser::TYPE_LIST) {
		parser.enter();

		if(success && parser.valueType() == LdlParser::TYPE_STRING) {
			params.flags = SamplerParams::flagsInfo()->parse(parser.getString(), &parser);
			parser.next();
		}
		else {
			parser.error("Type error: expected texture flags (String), got ", parser.valueTypeName());
			parser.skip();
			success = false;
		}

		if(success && parser.valueType() == LdlParser::TYPE_FLOAT) {
			params.maxAnisotropy = parser.getFloat();
			parser.next();
		}
		else if(parser.valueType() != LdlParser::TYPE_END) {
			parser.error("Type error: expected max anisotropy (Float), got ", parser.valueTypeName());
			parser.skip();
			success  = false;
		}

		if(success && parser.valueType() == LdlParser::TYPE_FLOAT) {
			params.lodBias = parser.getFloat();
			parser.next();
		}
		else if(parser.valueType() != LdlParser::TYPE_END) {
			parser.error("Type error: expected lod bias (Float), got ", parser.valueTypeName());
			parser.skip();
			success  = false;
		}

		if(success && parser.valueType() == LdlParser::TYPE_FLOAT) {
			params.minLod = parser.getFloat();
			parser.next();
		}
		else if(parser.valueType() != LdlParser::TYPE_END) {
			parser.error("Type error: expected min lod (Float), got ", parser.valueTypeName());
			parser.skip();
			success  = false;
		}

		if(success && parser.valueType() == LdlParser::TYPE_FLOAT) {
			params.maxLod = parser.getFloat();
			parser.next();
		}
		else if(parser.valueType() != LdlParser::TYPE_END) {
			parser.error("Type error: expected max lod (Float), got ", parser.valueTypeName());
			parser.skip();
			success  = false;
		}

		if(success && parser.valueType() != LdlParser::TYPE_END) {
			parser.warning("Too many parameters in texture declaration. Ignoring.");
		}
		while(parser.valueType() != LdlParser::TYPE_END)
			parser.skip();

		parser.leave();
	}
	else {
		parser.error("Invalid type: expected Sampler (VarList), got ", parser.valueTypeName());
		parser.skip();
		success = false;
	}

	if(success) {
		sampler = renderer->getSampler(params);
	}

	return success;
}


bool ldlWrite(LdlWriter& writer, const SamplerSP& sampler) {
	const SamplerParams& params = sampler->params();

	writer.openList(LdlWriter::CF_SINGLE_LINE, "Sampler");

	{
		std::ostringstream flags;
		SamplerParams::flagsInfo()->write(flags, params.flags);
		writer.writeString(flags.str());
	}

	writer.writeFloat(params.maxAnisotropy);
	writer.writeFloat(params.lodBias);
	writer.writeFloat(params.minLod);
	writer.writeFloat(params.maxLod);

	writer.close();

	return true;
}


bool varRead(SamplerSP& value, const Variant& var, Renderer* renderer, Logger& logger) {
	if(!var.isVarList()) {
		logger.error(var.parseInfoDesc(), "Invalid type: expected Sampler (VarList), got ", var.typeName());
		return false;
	}

	bool success = true;
	const VarList& varList = var.asVarList();
	SamplerParams params;

	if(varList.type() != "Sampler") {
		logger.warning(var.parseInfoDesc(), "Expected type annotation \"Sampler\", got \"", varList.type(), "\".");
	}

	if(varList.size() < 1) {
		logger.error(var.parseInfoDesc(), "Sampler take at least one parameter, got ", varList.size(), ".");
		success = false;
	}

	unsigned p = 0;
	if(varList.size() > p) {
		String flagsString;
		success &= varRead(flagsString, varList[p], logger);
		params.flags = SamplerParams::flagsInfo()->parse(flagsString, logger);
	}

	p += 1;
	if(varList.size() > p) {
		success &= varRead(params.maxAnisotropy, varList[p], logger);
	}

	p += 1;
	if(varList.size() > p) {
		success &= varRead(params.lodBias, varList[p], logger);
	}

	p += 1;
	if(varList.size() > p) {
		success &= varRead(params.minLod, varList[p], logger);
	}

	p += 1;
	if(varList.size() > p) {
		success &= varRead(params.maxLod, varList[p], logger);
	}

	p += 1;
	if(varList.size() > p) {
		logger.warning(varList[p], "Too much paramater in Sampler, ignoring.");
	}

	if(success) {
		value = renderer->getSampler(params);
	}

	return success;
}


bool varWrite(Variant& var, const SamplerSP& value, Logger& logger) {
	bool success = true;

	Variant v;
	VarList varList("Sampler");
	const SamplerParams& params = value->params();

	{
		std::ostringstream flags;
		SamplerParams::flagsInfo()->write(flags, params.flags);
		success &= varWrite(v, flags.str(), logger);
		varList.emplace_back(std::move(v));
	}

	success &= varWrite(v, params.maxAnisotropy, logger);
	varList.emplace_back(std::move(v));

	success &= varWrite(v, params.lodBias, logger);
	varList.emplace_back(std::move(v));

	success &= varWrite(v, params.minLod, logger);
	varList.emplace_back(std::move(v));

	success &= varWrite(v, params.maxLod, logger);
	varList.emplace_back(std::move(v));

	if(success) {
		var = std::move(varList);
	}

	return success;
}


}


LAIR_IMPLEMENT_METATYPE(lair::Sampler, "Sampler");
