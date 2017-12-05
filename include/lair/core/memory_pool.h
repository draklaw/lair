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


#ifndef _LAIR_CORE_MEMORY_POOL_H
#define _LAIR_CORE_MEMORY_POOL_H


#include <vector>

#include <lair/core/lair.h>


namespace lair
{


template<typename T>
class MemoryPool {
public:
	typedef T Value;

public:
	MemoryPool(size_t blockSize = 1024)
	    : _blockSize(blockSize)
	    , _blocks()
	    , _firstFree(nullptr)
	{}

	MemoryPool(const MemoryPool& ) = delete;
	MemoryPool(      MemoryPool&&) = delete;

	~MemoryPool() {
		for(Byte* block: _blocks)
			delete block;
	}

	MemoryPool& operator=(const MemoryPool& ) = delete;
	MemoryPool& operator=(      MemoryPool&&) = delete;

	inline size_t valueSize() const {
		return std::max(sizeof(Value), sizeof(Value*));
	}

	template<typename... Args>
	inline Value* construct(Args... args) {
		Value* value = allocate();
		value = new(value) Value(std::forward<Args>(args)...);
		return value;
	}

	inline void destroy(Value* value) {
		value->~Value();
		deallocate(value);
	}

	inline Value* allocate() {
		if(!_firstFree) {
			_blocks.emplace_back(new Byte[_blockSize * valueSize()]);
			Byte* block = _blocks.back();
			_firstFree = reinterpret_cast<Value*>(_blocks.back());

			Byte* v   = block;
			Byte* end = block + (_blockSize - 1) * valueSize();
			for(; v < end; v += valueSize()) {
				*reinterpret_cast<Value**>(v) =
				        reinterpret_cast<Value*>(v + valueSize());
			}
			*reinterpret_cast<Value**>(v) = nullptr;
		}

		Value* value = _firstFree;
		_firstFree   = *reinterpret_cast<Value**>(_firstFree);

#ifndef NDEBUG
		memset(value, 0xfeeefeee, valueSize());
#endif

		return value;
	}

	inline void deallocate(Value* value) {
#ifndef NDEBUG
		memset(value, 0xdeaddead, valueSize());
#endif

		*reinterpret_cast<Value**>(value) = _firstFree;
		_firstFree = value;
	}

protected:
	// FIXME: Check that this protduce the right alignment.
	typedef std::vector<Byte*, Eigen::aligned_allocator<Byte*>> BlockList;

protected:
	size_t    _blockSize;
	BlockList _blocks;
	Value*    _firstFree;
};


}


#endif
