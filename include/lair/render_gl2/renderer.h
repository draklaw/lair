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

#include <lair/core/lair.h>
#include <lair/core/log.h>

#include <lair/render_gl2/glsl_source.h>
#include <lair/render_gl2/shader_object.h>
#include <lair/render_gl2/program_object.h>
#include <lair/render_gl2/texture.h>


namespace lair
{


class Image;

class RenderModule;


class Renderer {
public:
	Renderer(RenderModule* module);
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&)      = delete;
	~Renderer();

	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&)      = delete;

	Texture* loadTexture(const std::string& file,
	                     uint32 flags = Texture::BILINEAR | Texture::REPEAT);

	Texture* defaultTexture() {
		return &_defaultTexture;
	}

	const ProgramObject* defaultShader() const {
		return &_defaultShader;
	}

	Logger& log();

	ShaderObject _compileShader(const char* name, GLenum type,
	                            const GlslSource& source);
	ProgramObject _compileProgram(const char* name,
	                              const ShaderObject* vert,
	                              const ShaderObject* frag);

protected:
	struct TexId {
		inline TexId(const std::string& file, uint32 flags)
		    : file(file), flags(flags) {}
		inline bool operator==(const TexId& rhs) const {
			return file == rhs.file && flags == rhs.flags;
		}
		inline bool operator!=(const TexId& rhs) const {
			return !(*this == rhs);
		}

		std::string file;
		uint32      flags;
	};

	struct HashTexId {
		size_t operator()(const TexId& texId) const;
	};

	typedef std::unordered_map<TexId, Texture, HashTexId> TextureMap;

protected:
	void _createDefaultTexture();

protected:
	RenderModule* _module;

	Texture       _defaultTexture;
	TextureMap    _textures;

	ProgramObject _defaultShader;
};


}


#endif
