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

#include "lair/render_gl2/context.h"
#include "lair/render_gl2/program_object.h"
#include "lair/render_gl2/vertex_format.h"
#include "lair/render_gl2/vertex_buffer.h"
#include "lair/render_gl2/texture.h"
#include "lair/render_gl2/renderer.h"

#include "lair/render_gl2/render_pass.h"


namespace lair
{


RenderPass::DrawCall::DrawCall(const DrawStates& states, const ShaderParameter* params,
                               unsigned depth, unsigned index, unsigned count)
    : states(states),
      params(params),
      depth(depth),
      index(index),
      count(count) {
}


RenderPass::RenderPass(Renderer* renderer)
    : _renderer(renderer) {
}


RenderPass::~RenderPass() {
}


void RenderPass::clear() {
	_drawCalls.clear();
}


void RenderPass::addDrawCall(const DrawStates& states, const ShaderParameter* param,
                             float depth, unsigned index, unsigned count) {
	unsigned maxDepth = 0x00ffffffu;
	unsigned idepth = clamp(unsigned(depth * maxDepth), 0u, maxDepth);
	_drawCalls.emplace_back(states, param, idepth, index, count);
	transparentIndex(_drawCalls.back());
}


void RenderPass::render() {
	_sortBuffer.clear();
	_sortBuffer.reserve(_drawCalls.size());
	for(DrawCall& call: _drawCalls) {
		Index index = (call.states.blendingMode == BLEND_NONE)? solidIndex(call):
		                                                        transparentIndex(call);
		_sortBuffer.emplace_back(index, &call);
	}
	std::sort(_sortBuffer.begin(), _sortBuffer.end());

	Context* glc = _renderer->context();
	// TODO: Support multitexturing
	glc->activeTexture(gl::TEXTURE0);

	DrawCall* prev = 0;
	for(IndexedCall icall: _sortBuffer) {
		DrawCall&   call   = *icall.call;
		DrawStates& states = call.states;

		if(!prev || prev->states.shader != states.shader) {
			states.shader->use();
		}

		bool updateBuffer = !prev || prev->states.buffer != states.buffer;
		if(updateBuffer) {
			states.buffer->bindAndUpload(glc);
		}

		if(updateBuffer || !prev || prev->states.format != states.format) {
			states.format->setup(glc);
		}

		const ShaderParameter* param = call.params;
		while(param->index >= 0) {
			#define LAIR_UNIF_CASE(_glType, _suffix, _type) case _glType:\
				glc->uniform##_suffix(param->index, 1, reinterpret_cast<const _type*>(param->value));\
				break
			#define LAIR_UNIF_CASE_MAT(_glType, _suffix, _type) case _glType:\
				glc->uniformMatrix##_suffix(param->index, 1, false, reinterpret_cast<const _type*>(param->value));\
				break
			switch(param->type) {
			LAIR_UNIF_CASE    (gl::INT,        1iv, int);
			LAIR_UNIF_CASE    (gl::INT_VEC2,   2iv, int);
			LAIR_UNIF_CASE    (gl::INT_VEC3,   3iv, int);
			LAIR_UNIF_CASE    (gl::INT_VEC4,   4iv, int);
			LAIR_UNIF_CASE    (gl::BOOL,       1iv, int);
			LAIR_UNIF_CASE    (gl::BOOL_VEC2,  2iv, int);
			LAIR_UNIF_CASE    (gl::BOOL_VEC3,  3iv, int);
			LAIR_UNIF_CASE    (gl::BOOL_VEC4,  4iv, int);
			LAIR_UNIF_CASE    (gl::FLOAT,      1fv, float);
			LAIR_UNIF_CASE    (gl::FLOAT_VEC2, 2fv, float);
			LAIR_UNIF_CASE    (gl::FLOAT_VEC3, 3fv, float);
			LAIR_UNIF_CASE    (gl::FLOAT_VEC4, 4fv, float);
			LAIR_UNIF_CASE_MAT(gl::FLOAT_MAT2, 2fv, float);
			LAIR_UNIF_CASE_MAT(gl::FLOAT_MAT3, 3fv, float);
			LAIR_UNIF_CASE_MAT(gl::FLOAT_MAT4, 4fv, float);
			}
			#undef LAIR_UNIF_CASE
			#undef LAIR_UNIF_CASE_MAT
			++param;
		}
//		SpriteShaderParams params(viewTransform);
//		_defaultShader.setParams(glc, params);

		bool updateTexture = !prev || prev->states.texture != states.texture;
		if(updateTexture) {
			states.texture->bind();
		}

		if(updateTexture || !prev || prev->states.textureFlags != states.textureFlags) {
			states.texture->_setFlags(states.textureFlags);
		}

		if(!prev || prev->states.blendingMode != states.blendingMode) {
			switch(states.blendingMode) {
			case BLEND_NONE:
				glc->disable(gl::BLEND);
				break;
			case BLEND_ALPHA:
				glc->enable(gl::BLEND);
				glc->blendEquation(gl::FUNC_ADD);
				glc->blendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
				break;
			case BLEND_ADD:
				glc->enable(gl::BLEND);
				glc->blendEquation(gl::FUNC_ADD);
				glc->blendFunc(gl::ONE, gl::ONE);
				break;
			case BLEND_MULTIPLY:
				glc->enable(gl::BLEND);
				glc->blendEquation(gl::FUNC_ADD);
				glc->blendFunc(gl::DST_COLOR, gl::ZERO);
				break;
			}
		}

		glc->drawElements(gl::TRIANGLES, call.count, gl::UNSIGNED_INT,
						  reinterpret_cast<void*>(call.index*sizeof(unsigned)));
	}
}


inline void RenderPass::setBits(Index& index, Index value, unsigned loBit, unsigned bitCount) {
	lairAssert(loBit + bitCount <= (8 * sizeof(Index)));
	lairAssert(value < (1 << bitCount));
	(void)bitCount;
	index |= value << loBit;
}


RenderPass::Index RenderPass::solidIndex(const DrawCall& call) {
	using namespace std;

	Index index = 0;

	constexpr unsigned shaderBits   =  6;
	constexpr unsigned formatBits   =  6;
	constexpr unsigned bufferBits   =  8;
	constexpr unsigned textureBits  = 10;
	constexpr unsigned texFlagsBits =  9;
	constexpr unsigned depthBits    = 24;

	Index bufferHash = hash<const VertexBuffer*>()(call.states.buffer) % (1 << bufferBits);
	Index formatHash = hash<const VertexFormat*>()(call.states.format) % (1 << formatBits);

	int i = 0;
	#define SET_BITS(_value, _size) setBits(index, _value, i, _size); i += _size
	SET_BITS(call.depth,                   depthBits);
	SET_BITS(call.states.textureFlags,     texFlagsBits);
	SET_BITS(call.states.texture->_glId(), textureBits);
	SET_BITS(formatHash,                   formatBits);
	SET_BITS(bufferHash,                   bufferBits);
	SET_BITS(call.states.shader->id(),     shaderBits);
	SET_BITS(1,                            1);
	lairAssert(i == (8 * sizeof(Index)));
	#undef SET_BITS

	return index;
}


RenderPass::Index RenderPass::transparentIndex(const DrawCall& call) {
	using namespace std;

	Index index = 0;

	constexpr unsigned shaderBits   =  6;
	constexpr unsigned formatBits   =  6;
	constexpr unsigned bufferBits   =  8;
	constexpr unsigned textureBits  = 10;
	constexpr unsigned texFlagsBits =  9;
	constexpr unsigned depthBits    = 24;

	Index bufferHash = hash<const VertexBuffer*>()(call.states.buffer) % (1 << bufferBits);
	Index formatHash = hash<const VertexFormat*>()(call.states.format) % (1 << formatBits);

	int i = 0;
	#define SET_BITS(_value, _size) setBits(index, _value, i, _size); i += _size
	SET_BITS(call.states.textureFlags,     texFlagsBits);
	SET_BITS(call.states.texture->_glId(), textureBits);
	SET_BITS(formatHash,                   formatBits);
	SET_BITS(bufferHash,                   bufferBits);
	SET_BITS(call.states.shader->id(),     shaderBits);
	SET_BITS(0x00ffffffu - call.depth,     depthBits);
	SET_BITS(1,                            1);
	lairAssert(i == (8 * sizeof(Index)));
	#undef SET_BITS

	return index;
}


RenderPass::IndexedCall::IndexedCall(Index index, DrawCall* call)
    : index(index),
      call(call) {
}


}
