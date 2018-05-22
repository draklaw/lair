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


#ifndef _LAIR_CORE_OCTREE_H
#define _LAIR_CORE_OCTREE_H


#include <vector>

#include <lair/core/lair.h>
#include <lair/core/memory_pool.h>

#include <lair/geometry/aligned_box.h>
#include <lair/geometry/intersection.h>


namespace lair
{


template<typename T>
class Placeholder {
public:
	inline const T* operator&() const {
		return reinterpret_cast<const T*>(_data);
	}

	inline T* operator&() {
		return reinterpret_cast<T*>(_data);
	}

	inline const T& operator*() const {
		return *reinterpret_cast<const T*>(_data);
	}

	inline T& operator*() {
		return *reinterpret_cast<T*>(_data);
	}

	inline const T* operator->() const {
		return reinterpret_cast<const T*>(_data);
	}

	inline T* operator->() {
		return reinterpret_cast<T*>(_data);
	}

protected:
	Byte _data[sizeof(T)];
};


template<typename _Object>
class Octree {
public:
	typedef _Object        Object;
	typedef Octree<Object> Self;

	typedef typename Object::Scalar Scalar;

	enum {
		Dim = Object::Dim,
		NCells = (1 << Dim),
	};

	typedef Eigen::Matrix<Scalar, Dim, 1> Vector;
	typedef AlignedBox   <Scalar, Dim>    Box;

public:
	inline Octree(const Box& box, unsigned maxDepth = 8,
	       size_t blockSize = 1024)
	    : _items(blockSize)
	    , _cells(blockSize)
	    , _root(_cells.construct(box.min(), box.max()))
	    , _maxDepth(maxDepth)
	{}

	inline Octree(const Vector& min, const Vector& max, unsigned maxDepth = 8,
	       size_t blockSize = 1024)
	    : Octree(Box(min, max), maxDepth, blockSize)
	{}

	Octree(const Octree& ) = delete;
	Octree(      Octree&&) = delete;

	inline ~Octree() {
		clear();
		_cells.destroy(_root);
	}

	Octree& operator=(const Octree& ) = delete;
	Octree& operator=(      Octree&&) = delete;

	inline Box bounds() const {
		return Box(*_root);
	}

	template<typename... Args>
	inline Object* insert(Args... args) {
		Item* item = _items.construct(std::forward<Args>(args)...);
		_insert(item);
		return item;
	}

	void remove(Object* object) {
		Item* item = static_cast<Item*>(object);
		Cell* cell = item->cell;
		lairAssert(cell);
		_removeItem(cell, item);
	}

	template<typename Predicate>
	void filterIf(const Predicate& predicate) {
		_filterIf(_root, predicate);
	}

	void reset(const Box& box, unsigned maxDepth = 8) {
		clear();
		_root->min() = box.min();
		_root->max() = box.max();
		_maxDepth = maxDepth;
	}

	void clear() {
		_removeAllOwnItems(_root);
		_destroyAllSubCells(_root);
	}

	template<typename Callback>
	bool hitTest(const Box& box, const Callback& callback = Callback()) const {
		return _hitTest(_root, box, callback);
	}

protected:
	class Cell;

	class Item : public Object {
	public:
		template<typename... Args>
		inline Item(Args... args)
		    : Object(std::forward<Args>(args)...)
		    , cell(nullptr)
		    , next(nullptr)
		    , prev(nullptr)
		{}

		Cell* cell;
		Item* next;
		Item* prev;
	};

	class Cell : public Box {
	public:
		inline Cell()
		    : Box()
		{
			std::memset(&_cells, 0, sizeof(Cell*) * NCells);
			_sentinel->next = &_sentinel;
			_sentinel->prev = &_sentinel;
		}

		inline Cell(const Vector& min, const Vector& max)
		    : Box(min, max)
		{
			std::memset(&_cells, 0, sizeof(Cell*) * NCells);
			_sentinel->next = &_sentinel;
			_sentinel->prev = &_sentinel;
		}

		using Box::min;
		using Box::max;
		using Box::center;

	public:
		typedef Placeholder<Item> _Sentinel;

	public:
		Cell*      _cells[NCells];
		_Sentinel  _sentinel;
	};

	typedef MemoryPool<Item> ObjectPool;
	typedef MemoryPool<Cell> CellPool;

protected:

	void _insert(Item* item) {
		unsigned depth = _maxDepth;
		Cell*    cell  = _root;

		unsigned ciMin = _subCellIndex(cell, item->boundingBox().min());
		unsigned ciMax = _subCellIndex(cell, item->boundingBox().max());
		while(depth && ciMin == ciMax) {
			Cell* child = _subCell(cell, ciMin);
			if(!child)
				child = _createSubCell(cell, ciMin);

			cell   = child;
			depth -= 1;

			ciMin = _subCellIndex(cell, item->boundingBox().min());
			ciMax = _subCellIndex(cell, item->boundingBox().max());
		}

		_appendItem(cell, item);
	}


	inline unsigned _subCellIndex(const Cell* cell, const Vector& p) const {
		unsigned q = 0;
		Vector mid = cell->center();
		for(unsigned axis = 0; axis < Dim; ++axis) {
			q |= unsigned(p(axis) >= mid(axis)) << axis;
		}
		return q;
	}

	inline const Cell* _subCell(const Cell* cell, unsigned i) const {
		return cell->_cells[i];
	}

	inline Cell* _subCell(Cell* cell, unsigned i) {
		lairAssert(i < NCells);
		return cell->_cells[i];
	}

	inline Cell* _createSubCell(Cell* cell, unsigned i) {
		lairAssert(i < NCells);
		Cell* child = cell->_cells[i];
		if(!child) {
			child = _cells.construct();
			cell->_cells[i] = child;
			Vector mid = child->center();
			for(unsigned axis = 0; axis < Dim; ++axis) {
				unsigned cmp = i & (1 << axis);
				child->min()(axis) = cmp?       mid  (axis): cell->min()(axis);
				child->max()(axis) = cmp? cell->max()(axis):       mid  (axis);
			}
		}
		return child;
	}

	inline void _destroyAllSubCells(Cell* cell) {
		for(unsigned ci = 0; ci < NCells; ++ci) {
			Cell* child = cell->_cells[ci];
			if(child) {
				_removeAllOwnItems(child);
				_destroyAllSubCells(child);
				_cells.destroy(child);
			}
		}
	}

	inline void _appendItem(Cell* cell, Item* item) {
		lairAssert(!item->cell);

		item->cell = cell;
		item->prev = cell->_sentinel->prev;
		item->next = &cell->_sentinel;

		item->prev->next = item;
		item->next->prev = item;
	}

	inline void _removeItem(Cell* cell, Item* item) {
		lairAssert(item->cell == cell);

		item->prev->next = item->next;
		item->next->prev = item->prev;

		item->cell = nullptr;
		item->prev = nullptr;
		item->next = nullptr;

		_items.destroy(item);
	}

	inline void _removeAllOwnItems(Cell* cell) {
		while(cell->_sentinel->next != &cell->_sentinel)
			_removeItem(cell, cell->_sentinel->next);
	}

	template<typename Predicate>
	void _filterIf(Cell* cell, const Predicate& predicate) {
		for(Item* item = cell->_sentinel->next; item != &cell->_sentinel; item = item->next) {
			if(predicate(*item)) {
				Item* toRemove = item;
				item = item->prev;
				_removeItem(cell, toRemove);
			}
		}

		for(int ci = 0; ci < NCells; ++ci) {
			if(_subCell(cell, ci)) {
				_filterIf(_subCell(cell, ci), predicate);

			}
		}
	}

	template<typename Callback>
	bool _hitTest(const Cell* cell, const Box& box, const Callback& callback = Callback()) const {
		for(Item* item = cell->_sentinel->next; item != &cell->_sentinel; item = item->next) {
			if(intersect(box, item->boundingBox())
			&& callback(*item))
				return true;
		}

		unsigned ciMin = _subCellIndex(cell, box.min());
		unsigned ciMax = _subCellIndex(cell, box.max());
		unsigned mask  = ~(ciMin ^ ciMax);

		for(int ci = 0; ci < NCells; ++ci) {
			const Cell* child = _subCell(cell, ci);
			if(child && ((ci ^ ciMin) & mask) == 0
			&& _hitTest(child, box, callback))
				return true;
		}

		return false;
	}

protected:
	ObjectPool _items;
	CellPool   _cells;
	Cell*      _root;
	unsigned   _maxDepth;
};


}


#endif
