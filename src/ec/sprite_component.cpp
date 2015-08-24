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
#include <lair/render_gl2/sprite.h>
#include <lair/render_gl2/renderer.h>

#include "lair/ec/entity.h"
#include "lair/ec/entity_manager.h"

#include "lair/ec/sprite_component.h"


namespace lair
{


SpriteComponent::SpriteComponent(_Entity* entity,
                                 ComponentManager<SpriteComponent>* manager)
    : Component(entity),
      _manager(static_cast<SpriteComponentManager*>(manager)),
      _sprite(nullptr),
      _spriteIndex(0),
      _anchor(0, 0),
      _color(1, 1, 1, 1),
      _view(Vector2(0, 0), Vector2(1, 1)) {
}


SpriteComponent::~SpriteComponent() {
}


void SpriteComponent::destroy() {
	_manager->_removeComponent(_entity());
}

void SpriteComponent::clone(EntityRef& target) {
	_manager->cloneComponent(EntityRef(_entity()), target);
}

//---------------------------------------------------------------------------//


SpriteComponentManager::SpriteComponentManager(Renderer* renderer,
                                               size_t componentBlockSize)
    : ComponentManager(componentBlockSize),
      _renderer(renderer) {
}


SpriteComponentManager::~SpriteComponentManager() {
}


void SpriteComponentManager::addComponentFromJson(EntityRef entity, const Json::Value& json) {
	addComponent(entity);
	SpriteComponent* comp = entity.sprite();
	if(json.isMember("sprite")) {
		comp->setSprite(_renderer->getSprite(json["sprite"].asString()));
	}
	comp->setIndex(json.get("index", 0).asInt());
	if(json.isMember("anchor")) {
		Json::Value anchor = json["anchor"];
		if(anchor.isArray() || anchor.size() == 2) {
			comp->setAnchor(Vector2(anchor[0].asFloat(), anchor[1].asFloat()));
		} /*else {
			log().warning("Invalid anchor field while loading entity \"", entity.name(), "\".");
		}*/
	}
	if(json.isMember("view")) {
		Json::Value view = json["view"];
		if(view.isArray() || view.size() == 4) {
			comp->setView(Box2(Vector2(view[0].asFloat(), view[1].asFloat()),
			        Vector2(view[2].asFloat(), view[3].asFloat())));
		} /*else {
			log().warning("Invalid anchor field while loading entity \"", entity.name(), "\".");
		}*/
	}
}


void SpriteComponentManager::cloneComponent(EntityRef base, EntityRef entity) {
	addComponent(entity);
	SpriteComponent* baseComp = base.sprite();
	SpriteComponent* comp = entity.sprite();
	comp->setSprite(baseComp->sprite());
	comp->setIndex(baseComp->index());
	comp->setAnchor(baseComp->anchor());
	comp->setView(baseComp->view());
}


void SpriteComponentManager::render(float interp, const OrthographicCamera& camera) {
	Batch& batch = _renderer->mainBatch();

	for(SpriteComponent& sc: *this) {
		if(!sc._entity() || !sc.sprite()) {
			continue;
		}
		const Sprite* sprite = sc.sprite();
		Box2 region = sprite->tileBox(sc.index());

		VertexBuffer& buff = batch.getBuffer(
					_renderer->spriteShader()->program(),
					sprite->texture(), _renderer->spriteFormat());
		GLuint index = buff.vertexCount();

		Scalar w = sprite->width();
		Scalar h = sprite->height();
		Matrix4 wt = lerp(interp,
		                  sc._entity()->prevWorldTransform.matrix(),
		                  sc._entity()->worldTransform.matrix());
		const Box2& view = sc.view();
		const Box2 view2(Vector2(view.min().x(), 1 - view.min().y()),
		                 Vector2(view.max().x(), 1 - view.max().y()));

		Vector4 offset(-w * sc.anchor().x(),
		               -h * sc.anchor().y(), 0, 0);
		Vector4 v0(view.min().x() * w, view.max().y() * h, 0, 1);
		Vector4 v1(view.min().x() * w, view.min().y() * h, 0, 1);
		Vector4 v2(view.max().x() * w, view.max().y() * h, 0, 1);
		Vector4 v3(view.max().x() * w, view.min().y() * h, 0, 1);
		Vector2 tc0 = region.min() + (region.sizes().array() * view2.corner(Box2::TopLeft).array()).matrix();
		Vector2 tc1 = region.min() + (region.sizes().array() * view2.corner(Box2::BottomLeft).array()).matrix();
		Vector2 tc2 = region.min() + (region.sizes().array() * view2.corner(Box2::TopRight).array()).matrix();
		Vector2 tc3 = region.min() + (region.sizes().array() * view2.corner(Box2::BottomRight).array()).matrix();
		buff.addVertex(SpriteVertex{ wt * (v0 + offset), sc.color(), tc0 });
		buff.addVertex(SpriteVertex{ wt * (v1 + offset), sc.color(), tc1 });
		buff.addVertex(SpriteVertex{ wt * (v2 + offset), sc.color(), tc2 });
		buff.addVertex(SpriteVertex{ wt * (v3 + offset), sc.color(), tc3 });
		buff.addIndex(index + 0);
		buff.addIndex(index + 1);
		buff.addIndex(index + 2);
		buff.addIndex(index + 2);
		buff.addIndex(index + 1);
		buff.addIndex(index + 3);
		index += 4;
	}
}


}
