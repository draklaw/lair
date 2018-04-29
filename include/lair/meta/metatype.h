/*
 *  Copyright (C) 2017 Simon Boyé
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


#ifndef _LAIR_META_METATYPE_H
#define _LAIR_META_METATYPE_H


#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <memory>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <string>
#include <ostream>

#include <lair/core/lair.h>


/**
 * \brief A trait that map a C++ type to a meta-type.
 *
 * Defined outside of the namespace lair so that other software can easily
 * declare new metatypes without adding dubious "namespace lair {...}".
 */
template<typename T>
struct _MetaTypeTrait;


namespace lair {


// Function pointers to implement meta-type functionalities. Mostly like methods.

/// Copy-construct `dst` from `src`.
typedef void (*CopyConstructorFunc)(void* dst, const void* src);

/// Move-construct `dst` from `src`.
typedef void (*MoveConstructorFunc)(void* dst, void* src);

/// Destroy `obj`.
typedef void (*DestructorFunc)(const void* obj);

/// Assign `value` to `obj`.
typedef void (*AssignFunc)(void* dst, const void* value);

/// Swap two values
typedef void (*SwapFunc)(void* value0, void* value1);

/// Converts `obj` to `int`.
typedef int (*ToIntFunc)(const void* obj);

/// Set the value of `obj` from an `int`.
typedef void (*FromIntFunc)(void* obj, int value);

/// Write a representation of `obj` to `out`.
typedef void (*WriteReprFunc)(std::ostream& out, const void* obj);

/**
 * \brief The meta-type structure: stores all information required by a meta-type.
 *
 *  Note that functions pointers might be null, meaning that the type do not
 *  support the feature.
 */
struct MetaType {
	unsigned            index;
	std::string         identifier;
	size_t              size;
	bool                isPod;
	CopyConstructorFunc copyConstruct;
	MoveConstructorFunc moveConstruct;
	DestructorFunc      destroy;
	AssignFunc          assign;
	SwapFunc            swap;
	ToIntFunc           toInt;
	FromIntFunc         fromInt;
	WriteReprFunc       writeRepr;
};

/// The singleton meta-type manager: registers meta-types and gives access to them.
class MetaTypeManager {
public:
	MetaTypeManager(const MetaTypeManager& ) = delete;
	MetaTypeManager(      MetaTypeManager&&) = delete;
	~MetaTypeManager() = default;

	MetaTypeManager& operator=(const MetaTypeManager& ) = delete;
	MetaTypeManager& operator=(      MetaTypeManager&&) = delete;

	inline const MetaType* get(unsigned index) const {
		lairAssert(index < _metaTypes.size());
		return _metaTypes[index].get();
	}

	template<typename T>
	inline const MetaType* get() const {
		typedef typename std::remove_reference<T>::type NoRef;
		typedef typename std::remove_pointer<NoRef>::type NoRefPtr;
		typedef typename std::remove_const<NoRefPtr>::type Stripped;
		return get(_MetaTypeTrait<Stripped>::index());
	}

	inline const MetaType* get(const std::string& identifier) const {
		return _metaTypeMap.at(identifier);
	}

	unsigned _register(MetaType metaType);

	static MetaTypeManager& _getInstance();

private:
	typedef std::unique_ptr<const MetaType>                  MetaTypePtr;
	typedef std::vector<MetaTypePtr>                         MetaTypeVector;
	typedef std::unordered_map<std::string, const MetaType*> MetaTypeMap;

private:
	MetaTypeManager() = default;

private:
	MetaTypeVector _metaTypes;
	MetaTypeMap    _metaTypeMap;
};

/// A reference to the unique instance of MetaTypeManager, for convenience.
extern MetaTypeManager& metaTypes;


/// Templates that return a CopyConstructorFunc that calls the copy constructor
/// of T, or null if there is no such constructor.
template<typename T>
struct _CopyConstructorFunc {
	static void copy(void* dst, const void* src) {
		T*       dstPtr = reinterpret_cast<      T*>(dst);
		const T* srcPtr = reinterpret_cast<const T*>(src);
		new(dstPtr) T(*srcPtr);
	}
};
template<typename T>
CopyConstructorFunc makeCopyConstructor(std::true_type) {
	return _CopyConstructorFunc<T>::copy;
}
template<typename T>
CopyConstructorFunc makeCopyConstructor(std::false_type) {
	return nullptr;
}
template<typename T>
CopyConstructorFunc makeCopyConstructor() {
	return makeCopyConstructor<T>(std::is_copy_constructible<T>());
}

/// Templates that return a MoveConstructorFunc that calls the move constructor
/// of T, or null if there is no such constructor.
template<typename T>
struct _MoveConstructorFunc {
	static void move(void* dst, void* src) {
		T* dstPtr = reinterpret_cast<T*>(dst);
		T* srcPtr = reinterpret_cast<T*>(src);
		new(dstPtr) T(std::move(*srcPtr));
	}
};
template<typename T>
MoveConstructorFunc makeMoveConstructor(std::true_type) {
	return _MoveConstructorFunc<T>::move;
}
template<typename T>
MoveConstructorFunc makeMoveConstructor(std::false_type) {
	return nullptr;
}
template<typename T>
MoveConstructorFunc makeMoveConstructor() {
	return makeMoveConstructor<T>(std::is_move_constructible<T>());
}

/// Templates that return a DestructorFunc that calls the destructor of T,
/// or null if the destructor is deleted (who would want to do that ?).
template<typename T>
struct _DestructorFunc {
	static void destroy(const void* obj) {
		const T* objPtr = reinterpret_cast<const T*>(obj);
		objPtr->~T();
	}
};
template<typename T>
DestructorFunc makeDestructor(std::true_type) {
	return _DestructorFunc<T>::destroy;
}
template<typename T>
DestructorFunc makeDestructor(std::false_type) {
	return nullptr;
}
template<typename T>
DestructorFunc makeDestructor() {
	return makeDestructor<T>(std::is_destructible<T>());
}

/// Templates that return an AssignFunc that calls the copy assignment operator
/// *obj = *value, or null if not supported.
template<typename T>
struct _AssignFunc {
	static void assign(void* obj, const void* value) {
		T* objPtr = reinterpret_cast<T*>(obj);
		const T* valPtr = reinterpret_cast<const T*>(value);
		*objPtr = *valPtr;
	}
};
template<typename T>
AssignFunc makeAssignFunc(std::true_type) {
	return _AssignFunc<T>::assign;
}
template<typename T>
AssignFunc makeAssignFunc(std::false_type) {
	return nullptr;
}
template<typename T>
AssignFunc makeAssignFunc() {
	// We need the & so that C++ test lvalue instead of rvalue.
	return makeAssignFunc<T>(std::is_assignable<T&, T&>());
}

template<typename T>
static auto _hasSwap(int)
    -> decltype(swap(std::declval<T>(), std::declval<T>()), // <- Test this experssion...
                std::true_type{});                          // <- and return this one.
template<typename T>
static std::false_type _hasSwap(long);
/// A reflection trait that test if writeRepr(std::ostream&, T) exists.
template<typename T>
struct HasSwap: decltype(_hasSwap<T>(0)) {};

/// Templates that return a SwapFunc that calls std::swap() on two objects.
template<typename T>
struct _SwapFunc {
	static void swapFunc(void* value0, void* value1) {
		T* ptr0 = reinterpret_cast<const T*>(value0);
		T* ptr1 = reinterpret_cast<const T*>(value1);
		swap(*ptr0, *ptr1);
	}
};
template<typename T>
SwapFunc makeSwapFunc(std::true_type) {
	return _SwapFunc<T>::swapFunc;
}
template<typename T>
SwapFunc makeSwapFunc(std::false_type) {
	return nullptr;
}
template<typename T>
SwapFunc makeSwapFunc() {
	return makeSwapFunc<T>(HasSwap<T>());
}

/// Templates that return a ToIntFunc that converts the object to an integer if
/// T is a number type.
template<typename T>
struct _ToIntFunc {
	static int toInt(const void* obj) {
		return int(*reinterpret_cast<const T*>(obj));
	}
};
template<typename T>
ToIntFunc makeToIntFunc(std::true_type) {
	return _ToIntFunc<T>::toInt;
}
template<typename T>
ToIntFunc makeToIntFunc(std::false_type) {
	return nullptr;
}
template<typename T>
ToIntFunc makeToIntFunc() {
	return makeToIntFunc<T>(std::is_assignable<int&, T&>());
}

/// Templates that return a FromIntFunc sets the object from an integer if
/// T is a number type.
template<typename T>
struct _FromIntFunc {
	static void fromInt(void* obj, int value) {
		*reinterpret_cast<T*>(obj) = T(value);
	}
};
template<typename T>
FromIntFunc makeFromIntFunc(std::true_type) {
	return _FromIntFunc<T>::fromInt;
}
template<typename T>
FromIntFunc makeFromIntFunc(std::false_type) {
	return nullptr;
}
template<typename T>
FromIntFunc makeFromIntFunc() {
	return makeFromIntFunc<T>(std::is_assignable<int&, T&>());
}

// Explanations: HasWriteRepr inerits the return type of the "best" overload of
// _hasWriteRepr. As 0 is int, the best one is _hasWriteRepr(int), however,
// if the expression of the return type is invalid, C++ fall back on the next
// best match instead of throwing an error (SFINAE). This expression tries to
// call writeRepr(std::ostream&, T), so if it is valid, it returns
// std::true_type (note the use of the operator ,). Otherwise, the second best
// guess returns std::false_type. std::declval<T> creates a value of type T
// without calling a (potentially inexistent) constructor.
template<typename T>
static auto _hasWriteRepr(int)
	-> decltype(writeRepr(std::declval<std::ostream&>(), std::declval<T>()), // <- Test this experssion...
	            std::true_type{});                                           // <- and return this one.
template<typename T>
static std::false_type _hasWriteRepr(long);
/// A reflection trait that test if writeRepr(std::ostream&, T) exists.
template<typename T>
struct HasWriteRepr: decltype(_hasWriteRepr<T>(0)) {};

/// Templates that return a WriteReprFunc that calls writeRepr(std::ostream, T)
/// if it exists, or null otherwise.
template<typename T>
struct _WriteReprFunc {
	static void writeReprFunc(std::ostream& out, const void* obj) {
		const T* objPtr = reinterpret_cast<const T*>(obj);
		writeRepr(out, *objPtr);
	}
};
template<typename T>
WriteReprFunc makeWriteReprFunc(std::true_type) {
	return _WriteReprFunc<T>::writeReprFunc;
}
template<typename T>
WriteReprFunc makeWriteReprFunc(std::false_type) {
	return nullptr;
}
template<typename T>
WriteReprFunc makeWriteReprFunc() {
	return makeWriteReprFunc<T>(HasWriteRepr<T>());
}

/**
 * \brief Register _type in the meta-type system.
 *
 * Note that this macro must be called in the global namespace.
 */
#define LAIR_REGISTER_METATYPE(_type, _id) \
	template<> \
	struct _MetaTypeTrait<_type> { \
		static unsigned index() { \
			static unsigned index = lair::metaTypes._register(lair::MetaType { \
				0, \
				_id, \
				sizeof(_type), \
				std::is_pod<_type>(), \
				lair::makeCopyConstructor<_type>(), \
				lair::makeMoveConstructor<_type>(), \
				lair::makeDestructor<_type>(), \
				lair::makeAssignFunc<_type>(), \
				lair::makeSwapFunc<_type>(), \
				lair::makeToIntFunc<_type>(), \
				lair::makeFromIntFunc<_type>(), \
				lair::makeWriteReprFunc<_type>() \
			}); \
			return index; \
		} \
	}


}

LAIR_REGISTER_METATYPE(bool,  "Bool");

LAIR_REGISTER_METATYPE(lair::int8,  "Int8");
LAIR_REGISTER_METATYPE(lair::int16, "Int16");
LAIR_REGISTER_METATYPE(lair::int32, "Int32");
LAIR_REGISTER_METATYPE(lair::int64, "Int64");

LAIR_REGISTER_METATYPE(lair::uint8,  "UInt8");
LAIR_REGISTER_METATYPE(lair::uint16, "UInt16");
LAIR_REGISTER_METATYPE(lair::uint32, "UInt32");
LAIR_REGISTER_METATYPE(lair::uint64, "UInt64");

LAIR_REGISTER_METATYPE(float,  "Float");
LAIR_REGISTER_METATYPE(double, "Double");

LAIR_REGISTER_METATYPE(lair::String, "String");

LAIR_REGISTER_METATYPE(lair::Vector2, "Vector2f");
LAIR_REGISTER_METATYPE(lair::Vector3, "Vector3f");
LAIR_REGISTER_METATYPE(lair::Vector4, "Vector4f");

LAIR_REGISTER_METATYPE(lair::Vector2i, "Vector2i");
LAIR_REGISTER_METATYPE(lair::Vector3i, "Vector3i");
LAIR_REGISTER_METATYPE(lair::Vector4i, "Vector4i");

LAIR_REGISTER_METATYPE(lair::Matrix2, "Matrix2f");
LAIR_REGISTER_METATYPE(lair::Matrix3, "Matrix3f");
LAIR_REGISTER_METATYPE(lair::Matrix4, "Matrix4f");

LAIR_REGISTER_METATYPE(lair::Transform,   "Transform");
LAIR_REGISTER_METATYPE(lair::Quaternion,  "Quaternion");
LAIR_REGISTER_METATYPE(lair::AngleAxis,   "AngleAxis");
LAIR_REGISTER_METATYPE(lair::Translation, "Translation");

LAIR_REGISTER_METATYPE(lair::Box2,  "Box2f");
LAIR_REGISTER_METATYPE(lair::Box3,  "Box3f");
LAIR_REGISTER_METATYPE(lair::Box2i, "Box2i");
LAIR_REGISTER_METATYPE(lair::Box3i, "Box3i");

#endif
