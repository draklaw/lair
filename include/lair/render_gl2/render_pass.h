/*
 *  Copyright (C) 2016 Simon Boyé
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


#ifndef _LAIR_RENDER_GL2_RENDER_PASS_H
#define _LAIR_RENDER_GL2_RENDER_PASS_H


#include <lair/core/lair.h>

#include <lair/render_gl2/gl.h>
#include <lair/render_gl2/context.h>


namespace lair {


class Renderer;
class Framebuffer;
typedef int DrawCall;


enum CullMode {
	GlCullFront        = gl::FRONT,
	GlCullBack         = gl::BACK,
	GlCullBackAndFront = gl::FRONT_AND_BACK
};

enum FrontFace {
	GlFrontCCW = gl::CCW,
	GlFrontCW  = gl::CW
};

enum DepthFunc {
	GlNever     = gl::NEVER,
	GlAlways    = gl::ALWAYS,
	GlLess      = gl::LESS,
	GlLessEq    = gl::LEQUAL,
	GlEqual     = gl::EQUAL,
	GlGreater   = gl::GREATER,
	GlGreaterEq = gl::GEQUAL,
	GlDifferent = gl::NOTEQUAL
};

enum BlendEq {
	GlAdd              = gl::FUNC_ADD,
	GlSubstract        = gl::FUNC_SUBTRACT,
	GlReverseSubstract = gl::FUNC_REVERSE_SUBTRACT
};

enum BlendFunc {
	GlBlendZero                  = gl::ZERO,
	GlBlendOne                   = gl::ONE,
	GlBlendSrcColor              = gl::SRC_COLOR,
	GlBlendOneMinusSrcColor      = gl::ONE_MINUS_SRC_COLOR,
	GlBlendDstColor              = gl::DST_COLOR,
	GlBlendOneMinusDstColor      = gl::ONE_MINUS_DST_COLOR,
	GlBlendSrcAlpha              = gl::SRC_ALPHA,
	GlBlendOneMinusSrcAlpha      = gl::ONE_MINUS_SRC_ALPHA,
	GlBlendDstAlpha              = gl::DST_ALPHA,
	GlBlendOneMinusDstAlpha      = gl::ONE_MINUS_DST_ALPHA,
	GlBlendConstantColor         = gl::CONSTANT_COLOR,
	GlBlendOneMinusConstantColor = gl::ONE_MINUS_CONSTANT_COLOR,
	GlBlendConstantAlpha         = gl::CONSTANT_ALPHA,
	GlBlendOneMinusConstantAlpha = gl::ONE_MINUS_CONSTANT_ALPHA,
	GlBlendAlphaSaturate         = gl::SRC_ALPHA_SATURATE
};

enum BlendMode {
	GlAlphaBlending,
	GlAdditiveBlending
};

enum ColorMask {
	GlRed   = 0x01,
	GlGreen = 0x02,
	GlBlue  = 0x04,
	GlAlpha = 0x08,
	GlAllColors = GlRed | GlGreen | GlBlue | GlAlpha
};


class PassStates {
public:
	PassStates();
	PassStates(const PassStates&)  = default;
	PassStates(      PassStates&&) = default;
	~PassStates() = default;

	PassStates& operator=(const PassStates&)  = default;
	PassStates& operator=(      PassStates&&) = default;

	void setStates(Context* glc, PassStates* current=nullptr, bool force=false);

public:
	/// If empty, the viewport is set to match the framebuffer
	bool         viewportMatchFb;
	Box2i        viewport;
	Vector2      depthRange;

	bool         cullEnabled;
	CullMode     cullFace;
	FrontFace    frontFace;

	bool         polygonOffsetEnabled;
	Scalar       polygonOffsetFactor;
	Scalar       polygonOffsetUnit;

	bool         alphaToCoverageEnabled;
	bool         sampleCoverageEnabled;
	float        sampleCoverage;
	bool         sampleCoverageInvert;

	/// If empty, scissor test is disabled
	Box2i        scissorBox;

	// TODO: Stencil stuff

	bool         depthTestEnabled;
	DepthFunc    depthFunc;

	bool         blendEnabled;
	BlendEq      blendRgbEq;
	BlendEq      blendAlphaEq;
	BlendFunc    blendSrcRgbFunc;
	BlendFunc    blendDstRgbFunc;
	BlendFunc    blendSrcAlphaFunc;
	BlendFunc    blendDstAlphaFunc;
	Vector4      blendColor;

	bool         ditherEnabled;

	unsigned     colorWriteMask;
	bool         depthWriteMask;
	unsigned     stencilFrontWriteMask;
	unsigned     stencilBackWriteMask;

	bool         clearColorEnabled;
	Vector4      clearColor;
	bool         clearDepthEnabled;
	Scalar       clearDepth;
	bool         clearStencilEnabled;
	unsigned     clearStencilValue;

	// Note: Might need to store FB states here as we might wish to update
	// a framebuffer between 2 passes.
	Framebuffer* framebuffer;
};


class RenderPass {
public:
	typedef std::vector<DrawCall> DrawCallVector;

public:
	RenderPass(Renderer* renderer);
	RenderPass(const RenderPass&)  = delete;
	RenderPass(      RenderPass&&) = delete;
	~RenderPass();

	RenderPass& operator=(const RenderPass&)  = delete;
	RenderPass& operator=(      RenderPass&&) = delete;

	Framebuffer* framebuffer() const;
	void setFramebuffer(Framebuffer* fb);

	void enableCulling(CullMode cullMode, FrontFace frontFace=GlFrontCCW);
	void enablePolygonOffset(Scalar factor, Scalar unit);
	void enableSampleCoverage(Scalar value, bool invert=false);
	void enableDepthTest(DepthFunc depthFunc=GlLess);
	void enableBlending(BlendMode blendMode);
	void setStencilWriteMasks(unsigned front, unsigned back);
	void clearColor(const Vector4 color=Vector4(0, 0, 0, 1));
	void clearDepth(Scalar depth=1);
	void clearStencil(unsigned value=0);

	void render();

public:
	PassStates states;
	bool       enabled;

private:
	Renderer*      _renderer;
	DrawCallVector _drawCalls;
};


}


#endif
