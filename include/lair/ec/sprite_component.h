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


#ifndef _LAIR_RENDER_GL2_SPRITE_COMPONENT_H
#define _LAIR_RENDER_GL2_SPRITE_COMPONENT_H


#include <cstring>
#include <vector>
#include <list>

#include <lair/core/lair.h>

#include <lair/render_gl2/gl.h>


namespace lair
{


class _Entity;
class Texture;

class SpriteComponent {
public:
	SpriteComponent();
	SpriteComponent(const SpriteComponent&) = delete;
	SpriteComponent(SpriteComponent&&)      = delete;
	~SpriteComponent();

	SpriteComponent& operator=(const SpriteComponent&) = delete;
	SpriteComponent& operator=(SpriteComponent&&)      = delete;

	inline Texture* texture() const { return _texture; }
	inline void setTexture(Texture* texture) {
		_texture = texture;
	}

	inline _Entity* _entity() const { return _entityPtr; }
	inline void _setEntity(_Entity* entity) { _entityPtr = entity; }

	inline void _reset() {
		std::memset(this, 0, sizeof(SpriteComponent));
	}

protected:
	_Entity* _entityPtr;
	Texture* _texture;
};


class SpriteComponentManager {
public:
	SpriteComponentManager();
	SpriteComponentManager(const SpriteComponentManager&) = delete;
	SpriteComponentManager(SpriteComponentManager&&)      = delete;
	~SpriteComponentManager();

	SpriteComponentManager& operator=(const SpriteComponentManager&) = delete;
	SpriteComponentManager& operator=(SpriteComponentManager&&)      = delete;

	size_t nComponent() const { return _nComponent; }

	void addComponent(_Entity* entity);
	void removeComponent(_Entity* entity);

	void render();

protected:
	typedef std::vector<SpriteComponent> ComponentBlock;
	typedef std::list<ComponentBlock>    ComponentList;

public:
	enum {
		PositionIndex,
		TexCoordIndex
	};
private:
	struct Vertex {
		Vector4 position;
		Vector2 texCoord;
	};
	typedef std::vector<Vertex> VertexVector;
	typedef std::vector<GLuint> IndexVector;

protected:
	void _addComponentBlock();

protected:
	size_t           _componentBlockSize;
	size_t           _nComponent;
	ComponentList    _components;
	SpriteComponent* _firstFree;

	VertexVector     _vertices;
	IndexVector      _indices;
	GLuint           _vertexBuffer;
	GLuint           _indexBuffer;
};


}


#endif
