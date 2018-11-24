/*
 *  Copyright (C) 2018 Simon Boyé
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
 *  auint with lair.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef LAIR_CORE_INTRUSIVE_POINTER_H
#define LAIR_CORE_INTRUSIVE_POINTER_H


#include <atomic>

#include <lair/core/lair.h>


namespace lair
{


template<typename T>
class IntrusivePointer;

template<typename T>
class IntrusiveWeakPointer;


struct _dont_inc_ref {};


template<typename T>
class IntrusiveBlock {
public:
	using Pointer          = IntrusivePointer<T>;
	using ConstPointer     = IntrusivePointer<const T>;
	using WeakPointer      = IntrusiveWeakPointer<T>;
	using ConstWeakPointer = IntrusiveWeakPointer<const T>;

public:
	IntrusiveBlock();
	IntrusiveBlock(const IntrusiveBlock& ) = delete;
	IntrusiveBlock(      IntrusiveBlock&&) = delete;

	IntrusiveBlock& operator=(const IntrusiveBlock& ) = delete;
	IntrusiveBlock& operator=(      IntrusiveBlock&&) = delete;

public:
	Pointer      pointer();
	ConstPointer pointer() const;
	ConstPointer constPointer() const;

public:
	friend class IntrusivePointer<T>;
	friend class IntrusiveWeakPointer<T>;

	uint _use_count() const;

	void _incRef();
	bool _incRefLock();
	void _decRef();

	void _incWeakRef();
	void _decWeakRef();

private:
	mutable std::atomic_uint _refCount;
	mutable std::atomic_uint _weakRefCount;
};


template<typename T>
class IntrusivePointer {
public:
	typedef T element_type;
	typedef IntrusiveWeakPointer<T> weak_type;

public:
	constexpr IntrusivePointer() noexcept;
	constexpr IntrusivePointer(std::nullptr_t) noexcept;

public:
	template<typename U>
	explicit IntrusivePointer(U* ptr) noexcept;

	template<typename U>
	explicit IntrusivePointer(U* ptr, _dont_inc_ref) noexcept;

public:
	template<typename U>
	IntrusivePointer(const IntrusivePointer<U>& ptr);

	template<typename U>
	explicit IntrusivePointer(const IntrusiveWeakPointer<U>& ptr);

	IntrusivePointer(const IntrusivePointer&  other) noexcept;
	IntrusivePointer(      IntrusivePointer&& other) noexcept;
	~IntrusivePointer();

	// TODO: convertion copy operators ?
	IntrusivePointer& operator=(const IntrusivePointer&  other) noexcept;
	IntrusivePointer& operator=(      IntrusivePointer&& other) noexcept;

	void reset() noexcept;

	void swap(IntrusivePointer& other) noexcept;

	element_type* get() const noexcept;

	T& operator*() const noexcept;
	T* operator->() const noexcept;

	uint use_count() const noexcept;

	explicit operator bool() const noexcept;

	static IntrusivePointer _create(T* ptr) noexcept;

private:
	using MutableType = typename std::remove_const<T>::type;

private:
	void _incRef() noexcept;
	void _decRef() noexcept;
	bool _incRefLock() noexcept;

private:
	T* _ptr;

	friend class IntrusiveBlock<T>;
	friend class IntrusiveWeakPointer<T>;
};

template<typename T, typename U>
bool operator==(const IntrusivePointer<T>& lhs, const IntrusivePointer<U>& rhs) noexcept { return lhs.get() == rhs.get(); }
template<typename T, typename U>
bool operator!=(const IntrusivePointer<T>& lhs, const IntrusivePointer<U>& rhs) noexcept { return lhs.get() != rhs.get(); }
template<typename T, typename U>
bool operator<(const IntrusivePointer<T>& lhs, const IntrusivePointer<U>& rhs) noexcept { return lhs.get() < rhs.get(); }
template<typename T, typename U>
bool operator>(const IntrusivePointer<T>& lhs, const IntrusivePointer<U>& rhs) noexcept { return lhs.get() > rhs.get(); }
template<typename T, typename U>
bool operator<=(const IntrusivePointer<T>& lhs, const IntrusivePointer<U>& rhs) noexcept { return lhs.get() <= rhs.get(); }
template<typename T, typename U>
bool operator>=(const IntrusivePointer<T>& lhs, const IntrusivePointer<U>& rhs) noexcept { return lhs.get() >= rhs.get(); }

template<typename T>
bool operator==(const IntrusivePointer<T>& ptr, std::nullptr_t) noexcept { return ptr.get() == nullptr; }
template<typename T>
bool operator!=(const IntrusivePointer<T>& ptr, std::nullptr_t) noexcept { return ptr.get() != nullptr; }
template<typename T>
bool operator<(const IntrusivePointer<T>& ptr, std::nullptr_t) noexcept { return ptr.get() < nullptr; }
template<typename T>
bool operator>(const IntrusivePointer<T>& ptr, std::nullptr_t) noexcept { return ptr.get() > nullptr; }
template<typename T>
bool operator<=(const IntrusivePointer<T>& ptr, std::nullptr_t) noexcept { return ptr.get() <= nullptr; }
template<typename T>
bool operator>=(const IntrusivePointer<T>& ptr, std::nullptr_t) noexcept { return ptr.get() >= nullptr; }

template<typename T>
bool operator==(std::nullptr_t, const IntrusivePointer<T>& ptr) noexcept { return nullptr == ptr.get(); }
template<typename T>
bool operator!=(std::nullptr_t, const IntrusivePointer<T>& ptr) noexcept { return nullptr != ptr.get(); }
template<typename T>
bool operator<(std::nullptr_t, const IntrusivePointer<T>& ptr) noexcept { return nullptr < ptr.get(); }
template<typename T>
bool operator>(std::nullptr_t, const IntrusivePointer<T>& ptr) noexcept { return nullptr > ptr.get(); }
template<typename T>
bool operator<=(std::nullptr_t, const IntrusivePointer<T>& ptr) noexcept { return nullptr <= ptr.get(); }
template<typename T>
bool operator>=(std::nullptr_t, const IntrusivePointer<T>& ptr) noexcept { return nullptr >= ptr.get(); }

template<typename T>
std::ostream& operator<<(std::ostream& out, const IntrusivePointer<T>& ptr) {
	out << ptr.get();
	return out;
}

template<typename T>
bool swap(const IntrusivePointer<T>& ptr0, const IntrusivePointer<T>& ptr1) {
	return ptr0.swap(ptr1);
}

template<typename T>
std::size_t hash(const IntrusivePointer<T>& ptr) {
	using std::hash;
	return hash<T*>()(ptr.get());
}

template<typename T, typename U>
IntrusivePointer<T> static_pointer_cast(const IntrusivePointer<U>& ptr) {
	return IntrusivePointer<T>::_create(static_cast<T*>(ptr.get()));
}

template<typename T, typename U>
IntrusivePointer<T> dynamic_pointer_cast(const IntrusivePointer<U>& ptr) {
	return IntrusivePointer<T>::_create(dynamic_cast<T*>(ptr.get()));
}

template<typename T, typename U>
IntrusivePointer<T> const_pointer_cast(const IntrusivePointer<U>& ptr) {
	return IntrusivePointer<T>::_create(const_cast<T*>(ptr.get()));
}

template<typename T, typename... Args>
IntrusivePointer<T> makeIntrusive(Args... args) {
	T* ptr = static_cast<T*>(malloc(sizeof(T)));
	return makeIntrusiveAt(ptr, std::forward<Args>(args)...);
}

template<typename T, typename... Args>
IntrusivePointer<T> makeIntrusiveAt(T* dst, Args... args) {
	T* ptr = new(dst) T(std::forward<Args>(args)...);
	return static_pointer_cast<T>(ptr->pointer());
}


template<typename T>
class IntrusiveWeakPointer {
public:
	typedef T element_type;
	typedef IntrusivePointer<T> shared_type;

public:
	constexpr IntrusiveWeakPointer() noexcept;

	template<typename U>
	IntrusiveWeakPointer(const IntrusivePointer<U>& ptr);

	IntrusiveWeakPointer(const IntrusiveWeakPointer&  other) noexcept;
	IntrusiveWeakPointer(      IntrusiveWeakPointer&& other) noexcept;
	~IntrusiveWeakPointer();

	IntrusiveWeakPointer& operator=(const IntrusiveWeakPointer&  other) noexcept;
	IntrusiveWeakPointer& operator=(      IntrusiveWeakPointer&& other) noexcept;
	IntrusiveWeakPointer& operator=(const IntrusivePointer<T>&   other) noexcept;

	void reset() noexcept;

	void swap(IntrusiveWeakPointer& other) noexcept;

	uint use_count() const noexcept;

	shared_type lock() const noexcept;

private:
	using MutableType = typename std::remove_const<T>::type;

private:
	void _incWeakRef() noexcept;
	void _decWeakRef() noexcept;

private:
	T* _ptr;
};


// ////////////////////////////////////////////////////////////////////////////


template<typename T>
IntrusiveBlock<T>::IntrusiveBlock()
    : _refCount(0)
    , _weakRefCount(0)
{
//	std::cerr << "IB(" << _refCount << ", " << _weakRefCount << ")\n";
}


template<typename T>
typename IntrusiveBlock<T>::Pointer
IntrusiveBlock<T>::pointer() {
	return Pointer(static_cast<T*>(this));
}


template<typename T>
typename IntrusiveBlock<T>::ConstPointer
IntrusiveBlock<T>::pointer() const {
	return ConstPointer(static_cast<const T*>(this));
}


template<typename T>
typename IntrusiveBlock<T>::ConstPointer
IntrusiveBlock<T>::constPointer() const {
	return ConstPointer(static_cast<const T*>(this));
}


template<typename T>
uint IntrusiveBlock<T>::_use_count() const {
	return _refCount.load(std::memory_order_relaxed);
}


template<typename T>
void IntrusiveBlock<T>::_incRef() {
//	std::cerr << "IB::_incRef(" << _refCount << ", " << _weakRefCount << ")\n";

	_refCount    .fetch_add(1, std::memory_order_relaxed);
	_weakRefCount.fetch_add(1, std::memory_order_relaxed);

//	std::cerr << "           (" << _refCount << ", " << _weakRefCount << ")\n";
}


template<typename T>
bool IntrusiveBlock<T>::_incRefLock() {
//	std::cerr << "IB::_incRefLock(" << _refCount << ", " << _weakRefCount << ")\n";

	unsigned count = _refCount.load(std::memory_order_relaxed);
	do {
		if(count == 0)
			return false;
	} while(!_refCount.compare_exchange_weak(count, count + 1,
	                                         std::memory_order_acq_rel,
	                                         std::memory_order_relaxed));

	count = _weakRefCount.fetch_add(1, std::memory_order_relaxed);
	lairAssert(count != 0);

//	std::cerr << "               (" << _refCount << ", " << _weakRefCount << ")\n";

	return true;
}


template<typename T>
void IntrusiveBlock<T>::_decRef() {
//	std::cerr << "IB::_decRef(" << _refCount << ", " << _weakRefCount << ")\n";

	if(_refCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
		static_cast<T*>(this)->_destroy();
	}

	if(_weakRefCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
		static_cast<T*>(this)->_delete();
	}

//	std::cerr << "           (" << _refCount << ", " << _weakRefCount << ")\n";
}


template<typename T>
void IntrusiveBlock<T>::_incWeakRef() {
//	std::cerr << "IB::_incWeakRef(" << _refCount << ", " << _weakRefCount << ")\n";

	unsigned count;
	count = _weakRefCount.fetch_add(1, std::memory_order_relaxed);
	lairAssert(count != 0);

//	std::cerr << "               (" << _refCount << ", " << _weakRefCount << ")\n";
}


template<typename T>
void IntrusiveBlock<T>::_decWeakRef() {
//	std::cerr << "IB::_incWeakRef(" << _refCount << ", " << _weakRefCount << ")\n";

	if(_weakRefCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
		static_cast<T*>(this)->_delete();
	}

//	std::cerr << "               (" << _refCount << ", " << _weakRefCount << ")\n";
}


// ////////////////////////////////////////////////////////////////////////////


template<typename T>
constexpr IntrusivePointer<T>::IntrusivePointer() noexcept
    : _ptr(nullptr)
{}


template<typename T>
constexpr IntrusivePointer<T>::IntrusivePointer(std::nullptr_t) noexcept
    : _ptr(nullptr)
{}


template<typename T>
template<typename U>
IntrusivePointer<T>::IntrusivePointer(U* ptr) noexcept
    : IntrusivePointer(ptr, _dont_inc_ref{})
{
	_incRef();
}


template<typename T>
template<typename U>
IntrusivePointer<T>::IntrusivePointer(U* ptr, _dont_inc_ref) noexcept
    : _ptr(ptr)
{
	static_assert(std::is_convertible<U*, T*>::value,
	              "Incompatible pointer types");
}


template<typename T>
template<typename U>
IntrusivePointer<T>::IntrusivePointer(const IntrusivePointer<U>& ptr)
    : _ptr(ptr.get()) {
	static_assert(std::is_convertible<U*, T*>::value,
	              "Incompatible pointer types");

	_incRef();
}


template<typename T>
template<typename U>
IntrusivePointer<T>::IntrusivePointer(const IntrusiveWeakPointer<U>& ptr)
    : _ptr(ptr._ptr)
{
	static_assert(std::is_convertible<U*, T*>::value,
	              "Incompatible pointer types");

	if(!_ptr || !_incRefLock())
		throw std::bad_weak_ptr();
}


template<typename T>
IntrusivePointer<T>::IntrusivePointer(const IntrusivePointer& other) noexcept
    : _ptr(other._ptr)
{
	_incRef();
}


template<typename T>
IntrusivePointer<T>::IntrusivePointer(IntrusivePointer&& other) noexcept
    : _ptr(other._ptr)
{
	other._ptr = nullptr;
}


template<typename T>
IntrusivePointer<T>::~IntrusivePointer() {
	_decRef();
}


template<typename T>
IntrusivePointer<T>& IntrusivePointer<T>::operator=(const IntrusivePointer& other) noexcept {
	if(this != &other) {
		_decRef();
		_ptr = other._ptr;
		_incRef();
	}
	return *this;
}


template<typename T>
IntrusivePointer<T>& IntrusivePointer<T>::operator=(IntrusivePointer&& other) noexcept {
	if(this != &other) {
		_decRef();
		_ptr = other._ptr;
		other._ptr = nullptr;
	}
	return *this;
}


template<typename T>
void IntrusivePointer<T>::reset() noexcept {
	_decRef();
	_ptr = nullptr;
}


template<typename T>
void IntrusivePointer<T>::swap(IntrusivePointer& other) noexcept {
	std::swap(_ptr, other._ptr);
}


template<typename T>
typename IntrusivePointer<T>::element_type*
IntrusivePointer<T>::get() const noexcept {
	return _ptr;
}


template<typename T>
T& IntrusivePointer<T>::operator*() const noexcept {
	return *_ptr;
}


template<typename T>
T* IntrusivePointer<T>::operator->() const noexcept {
	return _ptr;
}


template<typename T>
uint IntrusivePointer<T>::use_count() const noexcept {
	return _ptr? _ptr->_use_count(): 0;
}


template<typename T>
IntrusivePointer<T>::operator bool() const noexcept {
	return _ptr != nullptr;
}


template<typename T>
IntrusivePointer<T> IntrusivePointer<T>::_create(T* ptr) noexcept {
	return IntrusivePointer<T>(ptr);
}


template<typename T>
void IntrusivePointer<T>::_incRef() noexcept {
	if(_ptr) {
		const_cast<MutableType*>(_ptr)->_incRef();
	}
}

template<typename T>
void IntrusivePointer<T>::_decRef() noexcept {
	if(_ptr) {
		const_cast<MutableType*>(_ptr)->_decRef();
	}
}


// ////////////////////////////////////////////////////////////////////////////


template<typename T>
constexpr IntrusiveWeakPointer<T>::IntrusiveWeakPointer() noexcept
    : _ptr(nullptr)
{
}


template<typename T>
template<typename U>
IntrusiveWeakPointer<T>::IntrusiveWeakPointer(const IntrusivePointer<U>& ptr)
    : _ptr(ptr._ptr)
{
	static_assert(std::is_convertible<U*, T*>::value,
	              "Incompatible pointer types");

	_incWeakRef();
}


template<typename T>
IntrusiveWeakPointer<T>::IntrusiveWeakPointer(const IntrusiveWeakPointer& other) noexcept
    : _ptr(other._ptr)
{
	_incWeakRef();
}


template<typename T>
IntrusiveWeakPointer<T>::IntrusiveWeakPointer(IntrusiveWeakPointer&& other) noexcept
    : _ptr(other._ptr)
{
	other._ptr = nullptr;
}


template<typename T>
IntrusiveWeakPointer<T>::~IntrusiveWeakPointer() {
	_decWeakRef();
}


template<typename T>
IntrusiveWeakPointer<T>& IntrusiveWeakPointer<T>::operator=(const IntrusiveWeakPointer& other) noexcept {
	if(this != &other) {
		_decWeakRef();
		_ptr = other._ptr;
		_incWeakRef();
	}
	return *this;
}


template<typename T>
IntrusiveWeakPointer<T>& IntrusiveWeakPointer<T>::operator=(IntrusiveWeakPointer&& other) noexcept {
	if(this != &other) {
		_decWeakRef();
		_ptr = other._ptr;
		other._ptr = nullptr;
	}
	return *this;
}


template<typename T>
IntrusiveWeakPointer<T>& IntrusiveWeakPointer<T>::operator=(const IntrusivePointer<T>& other) noexcept {
	_decWeakRef();
	_ptr = other._ptr;
	_incWeakRef();
	return *this;
}


template<typename T>
void IntrusiveWeakPointer<T>::reset() noexcept {
	_decWeakRef();
	_ptr = nullptr;
}


template<typename T>
void IntrusiveWeakPointer<T>::swap(IntrusiveWeakPointer& other) noexcept {
	std::swap(_ptr, other._ptr);
}


template<typename T>
uint IntrusiveWeakPointer<T>::use_count() const noexcept {
	return _ptr? _ptr->_use_count(): 0;
}


template<typename T>
typename IntrusiveWeakPointer<T>::shared_type
IntrusiveWeakPointer<T>::lock() const noexcept {
	if(_ptr && const_cast<MutableType*>(_ptr)->_incRefLock())
		return IntrusivePointer<T>(_ptr, _dont_inc_ref());

	// Release the pointer so memory can be released asap.
	// Note: Doing this is dangerous. If the pointer is used as a key in a map,
	// it can cause undefined behavior.
	// const_cast<IntrusiveWeakPointer<T>*>(this)->reset();

	return IntrusivePointer<T>();
}


template<typename T>
void IntrusiveWeakPointer<T>::_incWeakRef() noexcept {
	if(_ptr) {
		const_cast<MutableType*>(_ptr)->_incWeakRef();
	}
}


template<typename T>
void IntrusiveWeakPointer<T>::_decWeakRef() noexcept {
	if(_ptr) {
		const_cast<MutableType*>(_ptr)->_decWeakRef();
	}
}


}


#endif
