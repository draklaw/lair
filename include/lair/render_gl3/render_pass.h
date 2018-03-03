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


#ifndef _LAIR_RENDER_GL3_RENDER_PASS_H
#define _LAIR_RENDER_GL3_RENDER_PASS_H


#include <vector>

#include <lair/core/lair.h>
#include <lair/core/metatype.h>

#include <lair/render_gl3/context.h>


namespace lair
{


class Renderer;
class ProgramObject;
class BufferObject;
class VertexArray;
class Texture;

typedef std::shared_ptr<Texture> TextureSP;


class ShaderParameter {
public:
	int         index;
	GLenum      type;
	const void* value;
};

template < typename T, int Rows, int Cols >
struct GLType {
};

#define LAIR_GL_TYPE(_type, _rows, _cols, _glType) template<> struct GLType<_type, _rows, _cols> { enum { Type = _glType }; };
LAIR_GL_TYPE(int,   2, 1, gl::INT_VEC2)
LAIR_GL_TYPE(int,   3, 1, gl::INT_VEC3)
LAIR_GL_TYPE(int,   4, 1, gl::INT_VEC4)
LAIR_GL_TYPE(bool,  2, 1, gl::BOOL_VEC2)
LAIR_GL_TYPE(bool,  3, 1, gl::BOOL_VEC3)
LAIR_GL_TYPE(bool,  4, 1, gl::BOOL_VEC4)
LAIR_GL_TYPE(float, 2, 1, gl::FLOAT_VEC2)
LAIR_GL_TYPE(float, 3, 1, gl::FLOAT_VEC3)
LAIR_GL_TYPE(float, 4, 1, gl::FLOAT_VEC4)
LAIR_GL_TYPE(float, 2, 2, gl::FLOAT_MAT2)
LAIR_GL_TYPE(float, 3, 3, gl::FLOAT_MAT3)
LAIR_GL_TYPE(float, 4, 4, gl::FLOAT_MAT4)
#undef LAIR_GL_TYPE

//template < typename T >
//struct _MakeShaderParameter {
//	ShaderParameter operator()(int index, const T& value) {
//	}
//};


//template<>
//struct _MakeShaderParameter<int> {
//	ShaderParameter operator()(int index, const int& value) {
//		return ShaderParameter{ index, gl::INT, &value };
//	}
//};

//template<>
//struct _MakeShaderParameter<bool> {
//	ShaderParameter operator()(int index, const bool& value) {
//		return ShaderParameter{ index, gl::BOOL, &value };
//	}
//};

//template<>
//struct _MakeShaderParameter<float> {
//	ShaderParameter operator()(int index, const float& value) {
//		return ShaderParameter{ index, gl::FLOAT, &value };
//	}
//};

//template< typename Derived >
//struct _MakeShaderParameter< Eigen::DenseBase<Derived> > {
//	ShaderParameter operator()(int index, const Eigen::DenseBase<Derived>& value) {
//		GLenum type = GLType<typename Derived::Scalar,
//		                     Derived::RowsAtCompileTime,
//		                     Derived::ColsAtCompileTime>::Type;
//		return ShaderParameter{ index, type, value.data() };
//	}
//};

//template < typename T >
//ShaderParameter makeShaderParameter(int index, const T& value) {
//	return _MakeShaderParameter<T>()(index, value);
//}


inline ShaderParameter makeShaderParameter(int index, const int* value) {
	return ShaderParameter{ index, gl::INT, value };
}

inline ShaderParameter makeShaderParameter(int index, const bool* value) {
	return ShaderParameter{ index, gl::BOOL, value };
}

inline ShaderParameter makeShaderParameter(int index, const float* value) {
	return ShaderParameter{ index, gl::FLOAT, value };
}

template< typename Derived >
ShaderParameter makeShaderParameter(int index, const Eigen::PlainObjectBase<Derived>& value) {
	GLenum type = GLType<typename Derived::Scalar,
						 Derived::RowsAtCompileTime,
						 Derived::ColsAtCompileTime>::Type;
	return ShaderParameter{ index, type, value.data() };
}


enum BlendingMode {
	BLEND_NONE,
	BLEND_ALPHA,
	BLEND_ADD,
	BLEND_MULTIPLY
};
const EnumInfo* blendingModeInfo();


class RenderPass {
public:
	typedef uint64 Index;

	// Stuff that is likely to be the same for several contiguous draw calls
	struct DrawStates {
		ProgramObject* shader;
		VertexArray*   vertices;
		Texture*       texture;
		unsigned       textureFlags;
		BlendingMode   blendingMode;
		// Box2i _viewport;
	};

	// Stuff that is unlikely to be the same even for contiguous draw calls
	struct DrawCall {
		DrawCall(const DrawStates& states, const ShaderParameter* params,
		         unsigned depth, unsigned index, unsigned count);

		// TODO: Test if using a pointer to avoid state duplication helps in any way.
		DrawStates              states;
		const ShaderParameter*  params;
		unsigned                depth; // Used for sorting
		unsigned                index;
		unsigned                count;
	};
	typedef std::vector<DrawCall> DrawCallList;


	// Stats about rendering
	struct Stats {
		void reset();
		void dump(Logger& log) const;

		unsigned shaderStateChangeCount;
		unsigned vertexArraySetupCount;
		unsigned textureBindCount;
		unsigned textureSetFlagCount;
		unsigned blendingModeChangeCount;
		unsigned drawCallCount;
	};

public:
	RenderPass(Renderer* renderer);
	RenderPass(const RenderPass&) = delete;
	RenderPass(RenderPass&&)      = delete;
	~RenderPass();

	RenderPass& operator=(const RenderPass&) = delete;
	RenderPass& operator=(RenderPass&&)      = delete;

	void clear();
	void addDrawCall(const DrawStates& states, const ShaderParameter* param,
	                 float depth, unsigned index, unsigned count);
	void render();

	static void setBits(Index& index, Index value, unsigned loBit, unsigned bitCount);
	static Index solidIndex(const DrawCall& call);
	static Index transparentIndex(const DrawCall& call);

protected:
	struct IndexedCall {
		IndexedCall(Index index, DrawCall* call);

		inline bool operator<(const IndexedCall& other) const {
			return index < other.index;
		}

		Index     index;
		DrawCall* call;
	};
	typedef std::vector<IndexedCall> SortBuffer;


protected:
	Renderer* _renderer;

	// TODO: Framebuffer* _framebuffer;
	// bool _depthTestEnable;
	// DepthTest _depthTest;

	DrawCallList _drawCalls;
	SortBuffer   _sortBuffer;

	Stats _stats;
};


}


LAIR_REGISTER_METATYPE(lair::BlendingMode, "BlendingMode");


#endif
