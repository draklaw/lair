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
#include <lair/core/json.h>
#include <lair/core/log.h>

#include <lair/render_gl2/orthographic_camera.h>
#include <lair/render_gl2/texture.h>
#include <lair/render_gl2/renderer.h>

#include "lair/ec/entity.h"
#include "lair/ec/entity_manager.h"

#include "lair/ec/sprite_component.h"


namespace lair
{


static const VertexAttrib _spriteVertexFormat[] = {
    { "vx_position", VxPosition, 4, gl::FLOAT, false,
      offsetof(SpriteVertex, position) },
    { "vx_color",    VxColor,    4, gl::FLOAT, false,
      offsetof(SpriteVertex, color) },
    { "vx_texCoord", VxTexCoord, 2, gl::FLOAT, false,
      offsetof(SpriteVertex, texCoord) },
    { nullptr, 0, 0, 0, false, 0 }
};


const char* _spriteVertGlsl =
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"

	"uniform highp mat4 viewMatrix;\n"

	"attribute highp   vec4 vx_position;\n"
	"attribute lowp    vec4 vx_color;\n"
	"attribute mediump vec2 vx_texCoord;\n"

	"varying highp   vec4 position;\n"
	"varying lowp    vec4 color;\n"
	"varying mediump vec2 texCoord;\n"

	"void main() {\n"
	"	gl_Position = viewMatrix * vx_position;\n"
	"	position    = vx_position;\n"
	"	color       = vx_color;\n"
	"	texCoord    = vx_texCoord;\n"
	"}\n";


const char* _spriteFragGlsl =
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"

	"uniform sampler2D texture;\n"

	"varying highp   vec4 position;\n"
	"varying lowp    vec4 color;\n"
	"varying mediump vec2 texCoord;\n"

	"void main() {\n"
	"	vec4 fcolor = color * texture2D(texture, texCoord);\n"
	"	if(fcolor.a < .5){\n"
	"		discard;\n"
	"	}\n"
	"	gl_FragColor = fcolor;\n"
	"//	gl_FragColor = vec4(texCoord, 0., 1.);\n"
	"//	gl_FragColor = vec4(1., 0., 0., 1.);\n"
	"}\n";


//---------------------------------------------------------------------------//


SpriteShaderParams::SpriteShaderParams(const Matrix4& viewMatrix, unsigned texUnit)
	: viewMatrix(viewMatrix),
	  texUnit(texUnit) {
}


//---------------------------------------------------------------------------//


SpriteShader::SpriteShader()
    : _shader       (nullptr),
      _viewMatrixLoc(-1),
      _textureLoc   (-1) {
}


SpriteShader::SpriteShader(const ProgramObject* shader)
    : _shader       (shader),
      _viewMatrixLoc(shader->getUniformLocation("viewMatrix")),
      _textureLoc   (shader->getUniformLocation("texture")) {
}


void SpriteShader::setParams(Context* glc, const SpriteShaderParams& params) {
	if(_viewMatrixLoc >= 0) {
		glc->uniformMatrix4fv(_viewMatrixLoc, 1, false,
							  const_cast<float*>(params.viewMatrix.data()));
	}
	if(_textureLoc >= 0) {
		glc->uniform1i(_textureLoc, params.texUnit);
	}
}


//---------------------------------------------------------------------------//


SpriteComponent::SpriteComponent(_Entity* entity,
                                 ComponentManager<SpriteComponent>* manager)
    : Component(entity),
      _manager(static_cast<SpriteComponentManager*>(manager)),
      _texture(),
      _anchor(0, 0),
      _color(1, 1, 1, 1),
      _tileGridSize(1, 1),
      _tileIndex(0),
      _view(Vector2(0, 0), Vector2(1, 1)),
      _blendingMode(BLEND_NONE),
      _textureFlags(Texture::BILINEAR | Texture::REPEAT) {
}


SpriteComponent::~SpriteComponent() {
}


void SpriteComponent::setTexture(AssetSP texture) {
	TextureAspectSP ta = texture->aspect<TextureAspect>();
	if(!ta) {
		ta = _manager->renderer()->createTexture(texture);
	}
	setTexture(ta);
}


void SpriteComponent::setTexture(const Path& logicPath) {
	AssetSP asset = _manager->assets()->getAsset(logicPath);
	if(!asset) {
		asset = _manager->assets()->createAsset(logicPath);
	}
	ImageAspectSP img = asset->aspect<ImageAspect>();
	if(!img) {
		_manager->loader()->load<ImageLoader>(asset);
	}
	setTexture(asset);
}


Box2 SpriteComponent::_texCoords() const {
	Vector2i size = _tileGridSize.cwiseMax(Vector2i(1, 1));
	unsigned i = _tileIndex % size.prod();
	std::div_t tile = std::div(i, size(0));
	unsigned x = tile.rem;
	unsigned y = tile.quot;
	Vector2 vmin = Vector2i(x  , y  ).cast<Scalar>().cwiseQuotient(size.cast<Scalar>());
	Vector2 vmax = Vector2i(x+1, y+1).cast<Scalar>().cwiseQuotient(size.cast<Scalar>());
	return Box2(vmin + _view.min().cwiseProduct(vmax - vmin),
	            vmin + _view.max().cwiseProduct(vmax - vmin));
}


void SpriteComponent::destroy() {
	_manager->_removeComponent(_entity());
}

void SpriteComponent::clone(EntityRef& target) {
	_manager->cloneComponent(EntityRef(_entity()), target);
}

//---------------------------------------------------------------------------//


SpriteComponentManager::SpriteComponentManager(Renderer* renderer,
                                               AssetManager* assetManager,
                                               LoaderManager* loaderManager,
                                               size_t componentBlockSize)
    : ComponentManager("sprite", componentBlockSize),
      _renderer(renderer),
      _assets(assetManager),
      _loader(loaderManager),
      _spriteFormat(sizeof(SpriteVertex), _spriteVertexFormat),
      _buffer(sizeof(SpriteVertex)) {
	lairAssert(_renderer);
	lairAssert(_assets);
	lairAssert(_loader);

	ShaderObject vert = _renderer->compileShader("sprite", gl::VERTEX_SHADER,
	                                   GlslSource(_spriteVertGlsl));
	ShaderObject frag = _renderer->compileShader("sprite", gl::FRAGMENT_SHADER,
	                                   GlslSource(_spriteFragGlsl));
	if(vert.isCompiled() && frag.isCompiled()) {
		_defaultShaderProg = _renderer->compileProgram(
		                         "sprite", &_spriteFormat, &vert, &frag);
	}
	lairAssert(_defaultShaderProg.isLinked());
	_defaultShader = SpriteShader(&_defaultShaderProg);
}


SpriteComponentManager::~SpriteComponentManager() {
}


void SpriteComponentManager::addComponentFromJson(EntityRef entity, const Json::Value& json,
                                                  const Path& cd) {
	addComponent(entity);
	SpriteComponent* comp = entity.sprite();
	if(json.isMember("sprite")) {
		comp->setTexture(make_absolute(cd, json["sprite"].asString()));
	}
	if(json.isMember("anchor")) {
		Json::Value anchor = json["anchor"];
		if(anchor.isArray() || anchor.size() == 2) {
			comp->setAnchor(Vector2(anchor[0].asFloat(), anchor[1].asFloat()));
		} /*else {
			log().warning("Invalid anchor field while loading entity \"", entity.name(), "\".");
		}*/
	}
	if(json.isMember("color")) {
		Json::Value color = json["color"];
		if(color.isArray() || color.size() == 4) {
			comp->setColor(Vector4(color[0].asFloat(), color[1].asFloat(),
			               color[2].asFloat(), color[3].asFloat()));
		} /*else {
			log().warning("Invalid anchor field while loading entity \"", entity.name(), "\".");
		}*/
	}
	comp->setTileGridSize(Vector2i(json.get("h_tiles", 1).asInt(),
	                               json.get("v_tiles", 1).asInt()));
	comp->setTileIndex(json.get("index", 0).asInt());
	if(json.isMember("view")) {
		Json::Value view = json["view"];
		if(view.isArray() || view.size() == 4) {
			comp->setView(Box2(Vector2(view[0].asFloat(), view[1].asFloat()),
			        Vector2(view[2].asFloat(), view[3].asFloat())));
		} /*else {
			log().warning("Invalid anchor field while loading entity \"", entity.name(), "\".");
		}*/
	}
	if(json.isMember("blend")) {
		std::string blend = json["blend"].asString();
		if(blend == "none") {
			comp->setBlendingMode(BLEND_NONE);
		}
		else if(blend == "alpha") {
			comp->setBlendingMode(BLEND_ALPHA);
		}
		else if(blend == "add") {
			comp->setBlendingMode(BLEND_ADD);
		}
		else if(blend == "multiply") {
			comp->setBlendingMode(BLEND_MULTIPLY);
		}
	}
	if(json.isMember("texture_flags")) {
		std::string flags = json["texture_flags"].asString();
		if(flags == "nearest") {
			comp->setTextureFlags(Texture::NEAREST | Texture::REPEAT);
		}
		if(flags == "bilinear") {
			comp->setTextureFlags(Texture::BILINEAR | Texture::REPEAT);
		}
		if(flags == "trilinear") {
			comp->setTextureFlags(Texture::TRILINEAR | Texture::REPEAT);
		}
	}
}


void SpriteComponentManager::cloneComponent(EntityRef base, EntityRef entity) {
	addComponent(entity);
	SpriteComponent* baseComp = base.sprite();
	SpriteComponent* comp = entity.sprite();
	comp->setTexture(     baseComp->texture());
	comp->setAnchor(      baseComp->anchor());
	comp->setColor(       baseComp->color());
	comp->setTileGridSize(baseComp->tileGridSize());
	comp->setTileIndex(   baseComp->tileIndex());
	comp->setView(        baseComp->view());
	comp->setBlendingMode(baseComp->blendingMode());
	comp->setTextureFlags(baseComp->textureFlags());
}


void SpriteComponentManager::render(float interp, const OrthographicCamera& camera) {
	Context* glc = _renderer->context();
	_buffer.clear();

	for(SpriteComponent& sc: *this) {
		if(!sc._entity() || !sc.texture()) {
			continue;
		}
		const Texture& tex = sc.texture()->texture();

		GLuint index = _buffer.vertexCount();

		const Box2& view = sc._texCoords();
		Scalar w = tex.width()  * view.sizes()(0);
		Scalar h = tex.height() * view.sizes()(1);
		Matrix4 wt = lerp(interp,
		                  sc._entity()->prevWorldTransform.matrix(),
		                  sc._entity()->worldTransform.matrix());

		Vector4 offset(-w * sc.anchor().x(),
		               -h * sc.anchor().y(), 0, 0);
		Vector4 v0(0, h, 0, 1);
		Vector4 v1(0, 0, 0, 1);
		Vector4 v2(w, h, 0, 1);
		Vector4 v3(w, 0, 0, 1);
//		_renderer->log().warning("v0: ", (wt * (v0 + offset)).transpose(),
//		                         " - ", view.corner(Box2::BottomLeft).transpose());
//		_renderer->log().warning("v1: ", (wt * (v1 + offset)).transpose(),
//		                         " - ", view.corner(Box2::TopLeft).transpose());
//		_renderer->log().warning("v2: ", (wt * (v2 + offset)).transpose(),
//		                         " - ", view.corner(Box2::BottomRight).transpose());
//		_renderer->log().warning("v3: ", (wt * (v3 + offset)).transpose(),
//		                         " - ", view.corner(Box2::TopRight).transpose());
		_buffer.addVertex(SpriteVertex{ wt * (v0 + offset), sc.color(), view.corner(Box2::BottomLeft) });
		_buffer.addVertex(SpriteVertex{ wt * (v1 + offset), sc.color(), view.corner(Box2::TopLeft) });
		_buffer.addVertex(SpriteVertex{ wt * (v2 + offset), sc.color(), view.corner(Box2::BottomRight) });
		_buffer.addVertex(SpriteVertex{ wt * (v3 + offset), sc.color(), view.corner(Box2::TopRight) });
		_buffer.addIndex(index + 0);
		_buffer.addIndex(index + 1);
		_buffer.addIndex(index + 2);
		_buffer.addIndex(index + 2);
		_buffer.addIndex(index + 1);
		_buffer.addIndex(index + 3);
	}

	_buffer.bindAndUpload(glc);
	_spriteFormat.setup(glc);

	_defaultShader.use();
	SpriteShaderParams params(camera.transform());
	_defaultShader.setParams(glc, params);
	glc->activeTexture(gl::TEXTURE0);

	uintptr index = 0;
	for(SpriteComponent& sc: *this) {
		if(!sc._entity() || !sc.texture()) {
			continue;
		}
		Texture& tex = sc.texture()->_texture();
		// TODO: replace this by a default texture
		if(!tex.isValid()) {
			continue;
		}
		tex.bind();
		tex._setFlags(sc.textureFlags());

		switch(sc.blendingMode()) {
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

		glc->drawElements(gl::TRIANGLES, 6, gl::UNSIGNED_INT,
		                  reinterpret_cast<void*>(index*sizeof(unsigned)));
		index += 6;
	}
}


Renderer* SpriteComponentManager::renderer() {
	return _renderer;
}


AssetManager* SpriteComponentManager::assets() {
	return _assets;
}


LoaderManager* SpriteComponentManager::loader() {
	return _loader;
}


}
