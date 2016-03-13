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


#ifndef _LAIR_RENDER_GL2_RENDERER_H
#define _LAIR_RENDER_GL2_RENDERER_H


#include <unordered_map>
#include <string>
#include <mutex>

#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/path.h>

#include <lair/render_gl2/context.h>
#include <lair/render_gl2/vertex_format.h>
#include <lair/render_gl2/glsl_source.h>
#include <lair/render_gl2/shader_object.h>
#include <lair/render_gl2/program_object.h>
#include <lair/render_gl2/texture.h>
//#include <lair/render_gl2/sprite.h>
#include <lair/render_gl2/batch.h>
#include <lair/render_gl2/render_pass.h>


namespace lair
{


class Image;

class RenderModule;


//struct SpriteVertex {
//	Vector4 position;
//	Vector4 color;
//	Vector2 texCoord;
//};


//class SpriteShader {
//public:
//	SpriteShader();
//	SpriteShader(const ProgramObject* shader);

//	inline const ProgramObject* program() const { return _shader; }
//	inline void use() const { if(_shader) _shader->use(); }

//	inline void setViewMatrix(const Matrix4& transform) const {
//		if(_viewMatrixLoc >= 0) {
//			glUniformMatrix4fv(_viewMatrixLoc, 1, false, transform.data());
//		}
//	}

//	void setTextureUnit(unsigned unit) const {
//		if(_textureLoc >= 0) {
//			glUniform1i(_textureLoc, unit);
//		}
//	}

//private:
//	const ProgramObject* _shader;
//	GLint _viewMatrixLoc;
//	GLint _textureLoc;
//};


class Renderer {
public:
	enum {
		VxPosition,
		VxColor,
		VxTexCoord
	};

public:
	Renderer(RenderModule* module);
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&)      = delete;
	~Renderer();

	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&)      = delete;

	Context* context();

	PassStates* currentPassStates();

	inline bool passStatesDirty() const {
		return _passStatesDirty;
	}
	inline void setPassStatesDirty(bool dirty = true) {
		_passStatesDirty = dirty;
	}

//	const VertexFormat* spriteFormat() const { return &_spriteFormat; }

	ShaderObject* compileShader(GLenum type, const GlslSource& source);

	void preloadTexture(const Path& file,
	                    uint32 flags = Texture::BILINEAR | Texture::REPEAT);
//	void preloadSprite(const Path& file);

	Texture* getTexture(const Path& file,
	                    uint32 flags = Texture::BILINEAR | Texture::REPEAT);
//	Sprite* getSprite(const Path& file);

	Texture* defaultTexture() {
		return &_defaultTexture;
	}

//	const SpriteShader* spriteShader() const {
//		return &_spriteShader;
//	}

	Batch& mainBatch() { return _mainBatch; }

	Logger& log();

	ShaderObject _compileShader(const char* name, GLenum type,
	                            const GlslSource& source);
	ProgramObject _compileProgram(const char* name,
	                              const VertexFormat* format,
	                              const ShaderObject* vert,
	                              const ShaderObject* frag);

protected:
	struct TexId {
		inline TexId(const Path& file, uint32 flags)
		    : file(file), flags(flags) {}
		inline bool operator==(const TexId& rhs) const {
			return file == rhs.file && flags == rhs.flags;
		}
		inline bool operator!=(const TexId& rhs) const {
			return !(*this == rhs);
		}

		Path        file;
		uint32      flags;
	};

	struct HashTexId {
		size_t operator()(const TexId& texId) const;
	};

	typedef std::unordered_map<TexId, Texture, HashTexId> TextureMap;

//	typedef std::shared_ptr<SpriteLoader> SpriteLoaderPtr;
//	typedef std::unordered_map<Path, SpriteLoaderPtr> SpriteMap;

protected:
	void _createDefaultTexture();

protected:
	RenderModule* _module;

	Context*      _context;

	PassStates    _currentPassStates;
	bool          _passStatesDirty;

//	VertexFormat  _spriteFormat;

	Texture       _defaultTexture;
	std::mutex    _textureLock;
	TextureMap    _textures;

//	std::mutex    _spriteLock;
//	SpriteMap     _sprites;

//	ProgramObject _spriteShaderProg;
//	SpriteShader  _spriteShader;

	Batch         _mainBatch;
};


}


#endif
