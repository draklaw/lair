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


#include "lair/render_gl2/renderer.h"

#include "lair/render_gl2/render_pass.h"


#define LAIR_GL2_LOG_CALL(...)
#define LAIR_GL2_CHECK_GL_CALL()

namespace lair
{


PassStates::PassStates()
	: viewportMatchFb       (true)
	, viewport              ()
	, depthRange            (0, 1)

	, cullEnabled           (false)
	, cullFace              (GlCullBack)
	, frontFace             (GlFrontCCW)

	, polygonOffsetEnabled  (false)
	, polygonOffsetFactor   (0)
	, polygonOffsetUnit     (0)

	, alphaToCoverageEnabled(false)
	, sampleCoverageEnabled (false)
	, sampleCoverage        (1)
	, sampleCoverageInvert  (false)

	, scissorBox            ()

// TODO: Stencil stuff

	, depthTestEnabled      (false)
	, depthFunc             (GlLess)

	, blendEnabled          (false)
	, blendRgbEq            (GlAdd)
	, blendAlphaEq          (GlAdd)
	, blendSrcRgbFunc       (GlBlendOne)
	, blendDstRgbFunc       (GlBlendZero)
	, blendSrcAlphaFunc     (GlBlendOne)
	, blendDstAlphaFunc     (GlBlendZero)
	, blendColor            (Vector4::Zero())

	, ditherEnabled         (true)

	, colorWriteMask        (GlAllColors)
	, depthWriteMask        (true)
	, stencilFrontWriteMask (~0u)
	, stencilBackWriteMask  (~0u)

	, clearColorEnabled     (false)
	, clearColor            ()
	, clearDepthEnabled     (false)
	, clearDepth            ()
	, clearStencilEnabled   (false)
	, clearStencilValue     ()
	// TODO: Set the default framebuffer
	, framebuffer           (nullptr) {
}


void PassStates::setStates(Context* glc, PassStates* current, bool force) {
	#define LAIR_PS_DIFF(_field) _field != current->_field
	#define LAIR_PS_DIFFBOX(_field) _field.isApprox(current->_field, 0)
	#define LAIR_PS_UPDATE(_field) current->_field = _field

	if(!current) {
		force = true;
	}

	// TODO: Get viewport size from framebuffer object
//	if(viewportMatchFb) {
//		viewport = framebuffer->viewBox();
//	}

	if(force || LAIR_PS_DIFFBOX(viewport)) {
		glc->viewport(viewport.min()  (0), viewport.min()  (1),
					  viewport.sizes()(0), viewport.sizes()(1));
		LAIR_PS_UPDATE(viewport);
	}
	if(force || LAIR_PS_DIFF(depthRange)) {
		glc->depthRangef(depthRange(0), depthRange(1));
		LAIR_PS_UPDATE(depthRange);
	}
	if(force || LAIR_PS_DIFF(cullEnabled)) {
		glc->setEnabled(gl::CULL_FACE, cullEnabled);
		LAIR_PS_UPDATE(cullEnabled);
	}
	if(force || cullEnabled) {
		if(force || LAIR_PS_DIFF(cullFace)) {
			glc->cullFace(cullFace);
			LAIR_PS_UPDATE(cullFace);
		}
		// FIXME: is this use for something else ?
		if(force || LAIR_PS_DIFF(frontFace)) {
			glc->frontFace(frontFace);
			LAIR_PS_UPDATE(frontFace);
		}
	}
	if(force || LAIR_PS_DIFF(polygonOffsetEnabled)) {
		glc->setEnabled(gl::POLYGON_OFFSET_FILL, polygonOffsetEnabled);
		LAIR_PS_UPDATE(polygonOffsetEnabled);
	}
	if(force || polygonOffsetEnabled) {
		if(force || LAIR_PS_DIFF(polygonOffsetFactor)
				 || LAIR_PS_DIFF(polygonOffsetUnit)) {
			glc->polygonOffset(polygonOffsetFactor, polygonOffsetUnit);
			LAIR_PS_UPDATE(polygonOffsetFactor);
			LAIR_PS_UPDATE(polygonOffsetUnit);
		}
	}
	if(force || LAIR_PS_DIFF(alphaToCoverageEnabled)) {
		glc->setEnabled(gl::SAMPLE_ALPHA_TO_COVERAGE, alphaToCoverageEnabled);
		LAIR_PS_UPDATE(alphaToCoverageEnabled);
	}
	if(force || LAIR_PS_DIFF(sampleCoverageEnabled)) {
		glc->setEnabled(gl::SAMPLE_COVERAGE, sampleCoverage);
		LAIR_PS_UPDATE(sampleCoverage);
	}
	if(force || sampleCoverageEnabled) {
		if(force || LAIR_PS_DIFF(sampleCoverage)
				 || LAIR_PS_DIFF(sampleCoverageInvert)) {
			glc->sampleCoverage(sampleCoverage, sampleCoverageInvert);
			LAIR_PS_UPDATE(sampleCoverage);
			LAIR_PS_UPDATE(sampleCoverageInvert);
		}
	}
	if(force || LAIR_PS_DIFF(scissorBox.isEmpty())) {
		glc->setEnabled(gl::SCISSOR_TEST, !scissorBox.isEmpty());
		LAIR_PS_UPDATE(scissorBox);
	}
	if(force || !scissorBox.isEmpty()) {
		if(force || LAIR_PS_DIFFBOX(scissorBox)) {
			glc->scissor(scissorBox.min()  (0), scissorBox.min()  (1),
						 scissorBox.sizes()(0), scissorBox.sizes()(1));
			LAIR_PS_UPDATE(scissorBox);
		}
	}
	if(force || LAIR_PS_DIFF(depthTestEnabled)) {
		glc->setEnabled(gl::DEPTH_TEST, depthTestEnabled);
		LAIR_PS_UPDATE(depthTestEnabled);
	}
	if(force || depthTestEnabled) {
		if(force || LAIR_PS_DIFF(depthFunc)) {
			glc->depthFunc(depthFunc);
			LAIR_PS_UPDATE(depthFunc);
		}
	}
	if(force || LAIR_PS_DIFF(blendEnabled)) {
		glc->setEnabled(gl::BLEND, blendEnabled);
		LAIR_PS_UPDATE(blendEnabled);
	}
	if(force || blendEnabled) {
		if(force || LAIR_PS_DIFF(blendRgbEq)
				 || LAIR_PS_DIFF(blendAlphaEq)) {
			glc->blendEquationSeparate(blendRgbEq, blendAlphaEq);
			LAIR_PS_UPDATE(blendRgbEq);
			LAIR_PS_UPDATE(blendAlphaEq);
		}
		if(force || LAIR_PS_DIFF(blendSrcRgbFunc)
				 || LAIR_PS_DIFF(blendDstRgbFunc)
				 || LAIR_PS_DIFF(blendSrcAlphaFunc)
				 || LAIR_PS_DIFF(blendDstAlphaFunc)) {
			glc->blendFuncSeparate(blendSrcRgbFunc,   blendDstRgbFunc,
								   blendSrcAlphaFunc, blendDstAlphaFunc);
			LAIR_PS_UPDATE(blendSrcRgbFunc);
			LAIR_PS_UPDATE(blendDstRgbFunc);
			LAIR_PS_UPDATE(blendSrcAlphaFunc);
			LAIR_PS_UPDATE(blendDstAlphaFunc);
		}
		if(force || LAIR_PS_DIFF(blendColor)) {
			glc->blendColor(blendColor(0), blendColor(1), blendColor(2), blendColor(3));
			LAIR_PS_UPDATE(blendColor);
		}
	}
	if(force || LAIR_PS_DIFF(ditherEnabled)) {
		glc->setEnabled(gl::DITHER, ditherEnabled);
		LAIR_PS_UPDATE(ditherEnabled);
	}
	if(force || LAIR_PS_DIFF(colorWriteMask)) {
		glc->colorMask(colorWriteMask & GlRed,  colorWriteMask & GlGreen,
					   colorWriteMask & GlBlue, colorWriteMask & GlAlpha);
		LAIR_PS_UPDATE(colorWriteMask);
	}
	if(force || LAIR_PS_DIFF(depthWriteMask)) {
		glc->depthMask(depthWriteMask);
		LAIR_PS_UPDATE(depthWriteMask);
	}
	if(force || LAIR_PS_DIFF(stencilFrontWriteMask)) {
		glc->stencilMaskSeparate(gl::FRONT, stencilFrontWriteMask);
		LAIR_PS_UPDATE(stencilFrontWriteMask);
	}
	if(force || LAIR_PS_DIFF(stencilBackWriteMask)) {
		glc->stencilMaskSeparate(gl::FRONT, stencilBackWriteMask);
		LAIR_PS_UPDATE(stencilBackWriteMask);
	}
	if(force || clearColorEnabled) {
		if(force || LAIR_PS_DIFF(clearColor)) {
			glc->clearColor(clearColor(0), clearColor(1), clearColor(2), clearColor(3));
			LAIR_PS_UPDATE(clearColor);
		}
	}
	if(force || clearDepthEnabled) {
		if(force || LAIR_PS_DIFF(clearDepth)) {
			glc->clearDepthf(clearDepth);
			LAIR_PS_UPDATE(clearDepth);
		}
	}
	if(force || clearStencilEnabled) {
		if(force || LAIR_PS_DIFF(clearStencilValue)) {
			glc->clearStencil(clearStencilValue);
			LAIR_PS_UPDATE(clearStencilValue);
		}
	}
	if(force || LAIR_PS_DIFF(framebuffer)) {
		// TODO: set framebuffer
		LAIR_PS_UPDATE(framebuffer);
	}
}


RenderPass::RenderPass(Renderer* renderer)
	: enabled  (true)
	, _renderer(renderer) {
}


Framebuffer* RenderPass::framebuffer() const {
	return states.framebuffer;
}


void RenderPass::setFramebuffer(Framebuffer* fb) {
	//assert(fb);
	// TODO: check that the framebuffer belongs to _renderer
	states.framebuffer = fb;
}


void RenderPass::enableCulling(CullMode cullMode, FrontFace frontFace) {
	states.cullFace  = cullMode;
	states.frontFace = frontFace;
}


void RenderPass::enablePolygonOffset(Scalar factor, Scalar unit) {
	states.polygonOffsetEnabled = true;
	states.polygonOffsetFactor  = factor;
	states.polygonOffsetUnit    = unit;
}


void RenderPass::enableSampleCoverage(Scalar value, bool invert) {
	states.sampleCoverageEnabled = true;
	states.sampleCoverage        = value;
	states.sampleCoverageInvert  = invert;
}


void RenderPass::enableDepthTest(DepthFunc depthFunc) {
	states.depthTestEnabled = true;
	states.depthFunc        = depthFunc;
}


void RenderPass::enableBlending(BlendMode blendMode) {
	states.blendEnabled = true;
	states.blendRgbEq   = GlAdd;
	states.blendAlphaEq = GlAdd;
	switch(blendMode) {
	case GlAlphaBlending:
		states.blendSrcRgbFunc   = GlBlendSrcAlpha;
		states.blendSrcAlphaFunc = GlBlendSrcAlpha;
		states.blendDstRgbFunc   = GlBlendOneMinusSrcAlpha;
		states.blendDstAlphaFunc = GlBlendOneMinusSrcAlpha;
		break;
	case GlAdditiveBlending:
		states.blendSrcRgbFunc   = GlBlendOne;
		states.blendSrcAlphaFunc = GlBlendOne;
		states.blendDstRgbFunc   = GlBlendOne;
		states.blendDstAlphaFunc = GlBlendOne;
		break;
	}
}


void RenderPass::setStencilWriteMasks(unsigned front, unsigned back) {
	states.stencilFrontWriteMask = front;
	states.stencilBackWriteMask  = back;
}


void RenderPass::clearColor(const Vector4 color) {
	states.clearColorEnabled = true;
	states.clearColor        = color;
}


void RenderPass::clearDepth(Scalar depth) {
	states.clearDepthEnabled = true;
	states.clearDepth        = depth;
}


void RenderPass::clearStencil(unsigned value) {
	states.clearStencilEnabled = true;
	states.clearStencilValue   = value;
}


void RenderPass::render() {
	if(!enabled)
		return;

	Context*    glc     = _renderer->context();
	PassStates* current = _renderer->currentPassStates();
	bool        force   = _renderer->passStatesDirty();

	states.setStates(glc, current, force);
	_renderer->setPassStatesDirty(false);

	GLenum clearFlags = 0;
	if(states.clearColorEnabled)   { clearFlags |= gl::COLOR_BUFFER_BIT; }
	if(states.clearDepthEnabled)   { clearFlags |= gl::DEPTH_BUFFER_BIT; }
	if(states.clearStencilEnabled) { clearFlags |= gl::STENCIL_BUFFER_BIT; }
	if(clearFlags) {
		glc->clear(clearFlags);
	}

	// TODO: sort draw calls

	// TODO: render draw calls
}


}
