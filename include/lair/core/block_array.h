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


#ifndef _LAIR_CORE_BLOCK_ARRAY_H
#define _LAIR_CORE_BLOCK_ARRAY_H


#include <vector>
#include <utility>

#include <lair/core/lair.h>


namespace lair
{


template < typename T >
class BlockArray {
public:
	typedef T          value_type;
	typedef ptrdiff_t  difference_type;
	typedef size_t     size_type;
	typedef T&         reference;
	typedef const T&   const_reference;
	typedef T*         pointer;
	typedef const T*   const_pointer;

	typedef T                 Value;
	typedef BlockArray<Value> Self;

public:

	class Iterator {
	public:
		typedef T          value_type;
		typedef ptrdiff_t  difference_type;
		typedef size_t     size_type;
		typedef T&         reference;
		typedef T*         pointer;
		typedef std::random_access_iterator_tag iterator_category;

		Iterator(Self* array, unsigned index)
		    : _array(array),
		      _index(index) {
		}

		bool operator==(const Iterator& other) const {
			return _array == other._array && _index == other._index;
		}
		bool operator!=(const Iterator& other) const {
			return !(*this == other);
		}

		bool operator<(const Iterator& other) const {
			return _array < other._array
			    || (_array == other._array && _index < other._index);
		}
		bool operator<=(const Iterator& other) const {
			return *this < other || *this == other;
		}
		bool operator>(const Iterator& other) const {
			return !(*this <= other);
		}
		bool operator>=(const Iterator& other) const {
			return !(*this < other);
		}

		Iterator& operator++() {
			++_index;
			return *this;
		}
		Iterator operator++(int) {
			Iterator tmp(*this);
			++_index;
			return tmp;
		}

		Iterator& operator--() {
			--_index;
			return *this;
		}
		Iterator operator--(int) {
			Iterator tmp(*this);
			--_index;
			return tmp;
		}

		Iterator& operator+=(ptrdiff_t n) {
			_index += n;
			return *this;
		}

		Iterator& operator-=(ptrdiff_t n) {
			_index -= n;
			return *this;
		}

		friend Iterator& operator+(Iterator it, ptrdiff_t n) {
			it += n;
			return it;
		}
		friend Iterator& operator+(ptrdiff_t n, Iterator it) {
			it += n;
			return it;
		}

		friend Iterator& operator-(Iterator it, ptrdiff_t n) {
			it -= n;
			return it;
		}
		friend Iterator& operator-(ptrdiff_t n, Iterator it) {
			it -= n;
			return it;
		}

		ptrdiff_t operator-(const Iterator& other) {
			return _index - other._index;
		}

		const Value& operator*() const {
			lairAssert(_index < _array->size());
			return *_array->_get(_index);
		}
		Value& operator*() {
			lairAssert(_index < _array->size());
			return *_array->_get(_index);
		}

		const Value& operator[](size_t subIndex) const {
			lairAssert(_index + subIndex < _array->size());
			return *_array->_get(_index + subIndex);
		}
		Value& operator[](size_t subIndex) {
			lairAssert(_index + subIndex < _array->size());
			return *_array->_get(_index + subIndex);
		}

	protected:
		Self*    _array;
		unsigned _index;
	};

public:
	BlockArray(size_t blockSize)
	    : _blockSize(blockSize),
	      _size(0),
	      _blocks() {
	}

	BlockArray(const BlockArray&) = delete;
	BlockArray(BlockArray&&)      = delete;

	~BlockArray() {
		clear();
		for(Value* block: _blocks) {
			Eigen::internal::aligned_free(block);
		}
	}

	BlockArray& operator=(const BlockArray&) = delete;
	BlockArray& operator=(BlockArray&&)      = delete;

	size_t size() const { return _size; }
	size_t capacity() const { return _blocks.size() * _blockSize; }

	Iterator begin() { return Iterator(this, 0); }
	Iterator end()   { return Iterator(this, _size); }

	const Value& at(size_t index) const {
		return const_cast<Self*>(this)->at(index);
	}
	Value& at(size_t index) {
		lairAssert(index < _size);
		unsigned block = index / _blockSize;
		unsigned ci    = index % _blockSize;
		lairAssert(block < _blocks.size());
		lairAssert(ci < _blockSize);
		return _blocks[block][ci];
	}

	const Value& operator[](size_t index) const {
		return (*const_cast<Self*>(this))[index];
	}
	Value& operator[](size_t index) {
		unsigned block = index / _blockSize;
		unsigned ci    = index % _blockSize;
		return _blocks[block][ci];
	}

	const Value& back() const {
		return at(_size - 1);
	}
	Value& back() {
		return at(_size - 1);
	}

	void push_back(const T& value) {
		Value* v = _get(_size);
		new (v) Value(value);
		++_size;
	}
	void push_back(T&& value) {
		Value* v = _get(_size);
		new (v) Value(std::move(value));
		++_size;
	}

	template < typename... Args >
	void emplace_back(Args... args) {
		Value* v = _get(_size);
		new (v) Value(std::forward<Args>(args)...);
		++_size;
	}

	void resize(size_t count) {
		lairAssert(count <= _size);
		while(count < _size) {
			--_size;
			_get(_size)->~Value();
		}
	}

	void clear() {
		resize(0);
	}

	Value* _get(size_t index) {
		unsigned block = index / _blockSize;
		unsigned ci    = index % _blockSize;
		while(_blocks.size() <= block) {
			_blocks.emplace_back(reinterpret_cast<Value*>(
			        Eigen::internal::aligned_malloc(sizeof(Value) * _blockSize)));
		}
		return &_blocks[block][ci];
	}

	size_t _indexOf(const Value* valuePtr) const {
		for(unsigned i = 0; i < _blocks.size(); ++i) {
			const Value* block = _blocks[i];
			if(valuePtr >= block && valuePtr < block + _blockSize) {
				return valuePtr - block + i * _blockSize;
			}
		}
		lairAssert(false);
		return _size;
	}

protected:
	typedef std::vector<Value*, Eigen::aligned_allocator<Value*>> BlockList;

protected:
	size_t       _blockSize;
	size_t       _size;
	BlockList    _blocks;
};


}


#endif
