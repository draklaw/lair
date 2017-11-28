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
#include <lair/core/shapes.h>


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
	Octree(const Box& box, unsigned maxDepth = 8,
	       size_t blockSize = 1024)
	    : _items(blockSize)
	    , _root(new Cell(box.min(), box.max()))
	    , _maxDepth(maxDepth)
	{}

	Octree(const Vector& min, const Vector& max, unsigned maxDepth = 8,
	       size_t blockSize = 1024)
	    : Octree(Box(min, max), maxDepth, blockSize)
	{}

	Octree(const Octree& ) = delete;
	Octree(      Octree&&) = delete;
	~Octree() = default;

	Octree& operator=(const Octree& ) = delete;
	Octree& operator=(      Octree&&) = delete;

	template<typename... Args>
	inline Object* insert(Args... args) {
		Item* item = _items.construct(std::forward<Args>(args)...);
		_insert(item);
		return item;
	}

	void remove(Object* object) {
		Item* item = static_cast<Item*>(object);
		lairAssert(item->cell);
		item->cell->removeItem(object);
		_items.destroy(item);
	}

	template<typename Predicate>
	void filterIf(const Predicate& predicate) {
		_root->filterIf(predicate);
	}

	void reset(const Box& box, unsigned maxDepth = 8) {
		clear();
		_root->min() = box.min();
		_root->max() = box.max();
		_maxDepth = maxDepth;
	}

	void clear() {
		_root->removeAllOwnItems();
		_root->destroyAllSubCells();
	}

	template<typename Callback>
	bool hitTest(const Box& box, const Callback& callback = Callback()) const {
		return _root->hitTest(box, callback);
	}

protected:
	class Cell;
	typedef std::unique_ptr<Cell> CellUP;

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
			_sentinel->next = &_sentinel;
			_sentinel->prev = &_sentinel;
		}

		inline Cell(const Vector& min, const Vector& max)
		    : Box(min, max)
		{
			_sentinel->next = &_sentinel;
			_sentinel->prev = &_sentinel;
		}

		inline ~Cell() {
			removeAllOwnItems();
		}

		using Box::min;
		using Box::max;
		using Box::center;

		inline unsigned subCellIndex(const Vector& p) const {
			unsigned q = 0;
			Vector mid = center();
			for(unsigned axis = 0; axis < Dim; ++axis) {
				q |= unsigned(p(axis) >= mid(axis)) << axis;
			}
			return q;
		}

		inline const Cell* subCell(unsigned i) const {
			return _cells[i].get();
		}

		inline Cell* subCell(unsigned i) {
			lairAssert(i < NCells);
			return _cells[i].get();
		}

		inline Cell* createSubCell(unsigned i) {
			lairAssert(i < NCells);
			Cell* cell = _cells[i].get();
			if(!cell) {
				cell = new Cell();
				_cells[i].reset(cell);
				Vector mid = center();
				for(unsigned axis = 0; axis < Dim; ++axis) {
					unsigned cmp = i & (1 << axis);
					cell->min()(axis) = cmp? mid  (axis): min()(axis);
					cell->max()(axis) = cmp? max()(axis): mid  (axis);
				}
			}
			return cell;
		}

		inline void destroyAllSubCells() {
			for(unsigned ci = 0; ci < NCells; ++ci)
				_cells[ci].reset();
		}

		inline void appendItem(Item* item) {
			lairAssert(!item->cell);

			item->cell = this;
			item->prev = _sentinel->prev;
			item->next = &_sentinel;

			item->prev->next = item;
			item->next->prev = item;
		}

		inline void removeItem(Item* item) {
			lairAssert(item->cell == this);

			item->prev->next = item->next;
			item->next->prev = item->prev;

			item->cell = nullptr;
			item->prev = nullptr;
			item->next = nullptr;
		}

		inline void removeAllOwnItems() {
			while(_sentinel->next != &_sentinel)
				removeItem(_sentinel->next);
		}

		template<typename Predicate>
		void filterIf(const Predicate& predicate) {
			for(Item* item = _sentinel->next; item != &_sentinel; item = item->next) {
				if(predicate(*item)) {
					Item* toRemove = item;
					item = item->prev;
					removeItem(toRemove);
				}
			}

			for(int ci = 0; ci < NCells; ++ci) {
				if(subCell(ci)) {
					subCell(ci)->filterIf(predicate);
				}
			}
		}

		template<typename Callback>
		bool hitTest(const Box& box, const Callback& callback = Callback()) const {
			for(Item* item = _sentinel->next; item != &_sentinel; item = item->next) {
				if(intersect(box, item->boundingBox())
				&& callback(*item))
					return true;
			}

			unsigned ciMin = subCellIndex(box.min());
			unsigned ciMax = subCellIndex(box.max());
			unsigned mask  = ~(ciMin ^ ciMax);

			for(int ci = 0; ci < NCells; ++ci) {
				if(subCell(ci) && ((ci ^ ciMin) & mask) == 0) {
					if(subCell(ci)->hitTest(box, callback))
						return true;
				}
			}

			return false;
		}


	private:
		typedef Placeholder<Item> Sentinel;

	private:
		CellUP     _cells[NCells];
		Sentinel   _sentinel;
	};

	typedef MemoryPool<Item> ObjectPool;

protected:

	void _insert(Item* item) {
		unsigned depth = _maxDepth;
		Cell*    cell  = _root.get();

		unsigned ciMin = cell->subCellIndex(item->boundingBox().min());
		unsigned ciMax = cell->subCellIndex(item->boundingBox().max());
		while(depth && ciMin == ciMax) {
			Cell* child = cell->subCell(ciMin);
			if(!child)
				child = cell->createSubCell(ciMin);

			cell   = child;
			depth -= 1;

			ciMin = cell->subCellIndex(item->boundingBox().min());
			ciMax = cell->subCellIndex(item->boundingBox().max());
		}

		cell->appendItem(item);
	}

protected:
	ObjectPool _items;
	CellUP     _root;
	unsigned   _maxDepth;
};


}


#endif
