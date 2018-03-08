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


#ifndef _LAIR_CORE_METATYPE_H
#define _LAIR_CORE_METATYPE_H


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

/// Destroy `obj`.
typedef void (*DestructorFunc)(const void* obj);

/// Assign `value` to `obj`.
typedef void (*AssignFunc)(void* dst, const void* value);

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
	CopyConstructorFunc copyConstruct;
	DestructorFunc      destroy;
	AssignFunc          assign;
	// TODO: Add a move operation that calls the move constructor.
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
				lair::makeCopyConstructor<_type>(), \
				lair::makeDestructor<_type>(), \
				lair::makeAssignFunc<_type>(), \
				lair::makeToIntFunc<_type>(), \
				lair::makeFromIntFunc<_type>(), \
				lair::makeWriteReprFunc<_type>() \
			}); \
			return index; \
		} \
	}

/**
 * \brief Union used to compute the size of Variant data block.
 *
 * Any type stored in this union will be storable directly in a Variant, without
 * allocating a new data block.
 */
union _VariantDataContent {
	void*       ptr;
	double      real;
	std::string str;
};

enum {
	/// The size of the data stored in a Variant.
	VARIANT_DATA_SIZE = sizeof(_VariantDataContent),
};


class VarRef;
class ConstVarRef;
class Variant;

/**
 * \brief A dynamically-typed reference.
 *
 * This works as a C++ reference: once constructed, it cannot reference another
 * variable (unless by calling reset()). More specifically, the assignment
 * operator set the value of the referenced variable and _never_ change the
 * internal pointer.
 *
 * Also note that the referenced value's lifetime is not tied to the reference
 * lifetime. It mean that the referenced value can be deleted before the
 * reference, thus producing a dangling reference.
 *
 * The main use of this class is to avoid useless copy and allocations in
 * context where dynamic types are necessary.
 */
class VarRef {
public:
	inline VarRef()
		: _type(nullptr)
		, _data(nullptr)
	{}

	VarRef(VarRef&  other) = default;
	VarRef(VarRef&& other) = default;

	inline VarRef(Variant& variant);

	template<typename T>
	inline VarRef(T& ref)
		: _type(metaTypes.get<T>())
		, _data(&ref)
	{}

	~VarRef() = default;

	VarRef& operator=(const VarRef&  other) = delete;
	VarRef& operator=(      VarRef&& other) = delete;

	inline VarRef& operator=(const ConstVarRef&  other);

	template<typename T>
	inline VarRef& operator=(const T& value) {
		lairAssert(metaTypes.get<T>() == _type);
		lairAssert(_type->assign);
		_type->assign(_data, &value);
		return *this;
	}

	inline const MetaType* type() const {
		return _type;
	}

	inline bool isEmpty() const {
		return !_type || !_data;
	}

	template<typename T>
	inline bool is() const {
		return _type == metaTypes.get<T>();
	}

	template<typename T>
	inline T& as() {
		lairAssert(is<T>());
		return *reinterpret_cast<T*>(_data);
	}

	template<typename T>
	inline const T& as() const {
		lairAssert(is<T>());
		return *reinterpret_cast<const T*>(_data);
	}

	inline int asInt() const {
		lairAssert(_type && _type->toInt);
		return _type->toInt(_data);
	}

	inline void setInt(int i) {
		lairAssert(_type && _type->fromInt);
		_type->fromInt(_data, i);
	}

	inline void* data() {
		lairAssert(!isEmpty());
		return _data;
	}

	inline const void* data() const {
		lairAssert(!isEmpty());
		return _data;
	}

	inline void reset(const VarRef& ref = VarRef()) {
		_type = ref._type;
		_data = ref._data;
	}

	inline void swap(VarRef& other) noexcept {
		std::swap(_type, other._type);
		std::swap(_data, other._data);
	}

private:
	const MetaType* _type;
	void*           _data;

	friend class ConstVarRef;
	friend class Variant;
};


/**
 * \brief A dynamically-typed const reference.
 *
 * This is basically the same as VarRef, except that the value referenced cannot
 * be mutated.
 */
class ConstVarRef {
public:
	inline ConstVarRef()
		: _type(nullptr)
		, _data(nullptr)
	{}

	ConstVarRef(const ConstVarRef&  other) = default;
	ConstVarRef(ConstVarRef&& other) = default;

	inline ConstVarRef(const VarRef& ref)
		: _type(ref._type)
		, _data(ref._data)
	{}

	inline ConstVarRef(const Variant& variant);

	template<typename T>
	inline ConstVarRef(const T& ref)
		: _type(metaTypes.get<T>())
		, _data(&ref)
	{}

	~ConstVarRef() = default;

	ConstVarRef& operator=(const ConstVarRef&  other) = delete;
	ConstVarRef& operator=(      ConstVarRef&& other) = delete;

	inline const MetaType* type() const {
		return _type;
	}

	inline bool isEmpty() const {
		return !_type || !_data;
	}

	template<typename T>
	inline bool is() const {
		return _type == metaTypes.get<T>();
	}

	template<typename T>
	inline const T& as() const {
		lairAssert(is<T>());
		return *reinterpret_cast<const T*>(_data);
	}

	inline int asInt() const {
		lairAssert(_type && _type->toInt);
		return _type->toInt(_data);
	}

	inline const void* data() const {
		lairAssert(!isEmpty());
		return _data;
	}

	inline void reset(const VarRef& ref = VarRef()) {
		_type = ref._type;
		_data = ref._data;
	}

	inline void swap(ConstVarRef& other) noexcept {
		std::swap(_type, other._type);
		std::swap(_data, other._data);
	}

private:
	const MetaType* _type;
	const void*     _data;

	friend class VarRef;
	friend class Variant;
};


/**
 * \brief Variant implement a dynamically-typed variable.
 *
 * Variant can represent the value of any type registered in the meta-type
 * system and that implements the copy constructor.
 *
 * Variant works by-value, no matter which type it contains. It mean that each
 * time a Variant is created, a value is copied (and memory allocation takes
 * place if required). For instance, unlike what happen in most scripting
 * languages, creating a Variant from a Variant that contains as string will
 * copy the string (unless some lower level mechanism like copy-on-write
 * occurs, but it still have a by-value semantic).
 */
class Variant {
public:
	inline Variant()
		: _type(nullptr)
	{
		_clearData();
	}

	inline Variant(const Variant& other)
		: _type(nullptr)
	{
		if(other._type) {
			_set(other._type, other._getDataBlock());
		}
		else {
			_clearData();
		}
	}

	inline Variant(Variant&& other)
		: _type(nullptr)
	{
		if(other._type) {
			_type = other._type;
			std::memcpy(_data, other._data, VARIANT_DATA_SIZE);
			other._type = nullptr;
			other._clearData();
		}
		else {
			_clearData();
		}
	}

	inline Variant(const VarRef& ref)
		: _type(nullptr)
	{
		if(ref.isEmpty()) {
			_clearData();
		}
		else {
			_set(ref._type, ref._data);
		}
	}

	inline Variant(const ConstVarRef& ref)
		: _type(nullptr)
	{
		if(ref.isEmpty()) {
			_clearData();
		}
		else {
			_set(ref._type, ref._data);
		}
	}

	template<typename T>
	Variant(const T& object)
		: _type(nullptr)
	{
		_set(metaTypes.get<T>(), &object);
	}

	~Variant() {
		clear();
	}

	Variant& operator=(Variant other) {
		other.swap(*this);
		return *this;
	}

	template<typename T>
	Variant& operator=(const ConstVarRef& ref) {
		clear();
		if(ref._type) {
			_set(ref._type, ref._data);
		}
		return *this;
	}

	inline const MetaType* type() const {
		return _type;
	}

	inline bool isEmpty() const {
		return !_type || !_getDataBlock();
	}

	template<typename T>
	inline bool is() const {
		return _type == metaTypes.get<T>();
	}

	template<typename T>
	inline T& as() {
		lairAssert(is<T>());
		return *reinterpret_cast<T*>(_getDataBlock());
	}

	template<typename T>
	inline const T& as() const {
		lairAssert(is<T>());
		return *reinterpret_cast<const T*>(_getDataBlock());
	}

	inline int asInt() const {
		lairAssert(_type && _type->toInt);
		return _type->toInt(_getDataBlock());
	}

	inline void setInt(int i) {
		lairAssert(_type && _type->fromInt);
		_type->fromInt(_getDataBlock(), i);
	}

	inline void* data() {
		lairAssert(!isEmpty());
		return _getDataBlock();
	}

	inline const void* data() const {
		lairAssert(!isEmpty());
		return _getDataBlock();
	}

	void clear() {
		if(_type) {
			void* ptr = _getDataBlock();

			if(ptr) {
				// Seriously, who would delete the destructor ???
				if(_type->destroy)
					_type->destroy(ptr);

				if(_type->size > VARIANT_DATA_SIZE) {
					free(ptr);
				}
			}

			_type = nullptr;
			_clearData();
		}
	}

	void swap(Variant& other) noexcept {
		std::swap(_type, other._type);

		std::uint8_t tmp[VARIANT_DATA_SIZE];
		std::memcpy(tmp,        _data,        VARIANT_DATA_SIZE);
		std::memcpy(_data,       other._data, VARIANT_DATA_SIZE);
		std::memcpy(other._data, tmp,         VARIANT_DATA_SIZE);
	}

private:
	template<typename T>
	inline T& _get() {
		return *reinterpret_cast<T*>(_data);
	}

	template<typename T>
	inline const T& _get() const {
		return *reinterpret_cast<const T*>(_data);
	}

	inline void* _getDataBlock() {
		if(_type->size > VARIANT_DATA_SIZE)
			return _get<void*>();
		return _data;
	}

	inline const void* _getDataBlock() const {
		if(_type->size > VARIANT_DATA_SIZE)
			return _get<void*>();
		return _data;
	}

	inline void _set(const MetaType* type, const void* obj) {
		clear();

		_type = type;

		if(_type->size > VARIANT_DATA_SIZE) {
			// Allocate a memory block to store the object
			_get<void*>() = malloc(_type->size);
			if(!_get<void*>())
				throw std::bad_alloc();
		}

		void* ptr = _getDataBlock();

		lairAssert(_type->copyConstruct);
		_type->copyConstruct(ptr, obj);
	}

	inline void _clearData() {
		std::memset(_data, 0, VARIANT_DATA_SIZE);
	}

private:
	const MetaType* _type;
	std::uint8_t    _data[VARIANT_DATA_SIZE];

	friend class VarRef;
	friend class ConstVarRef;
};


VarRef::VarRef(Variant& variant)
	: _type(variant._type)
	, _data(_type? variant._getDataBlock(): nullptr)
{}

VarRef& VarRef::operator=(const ConstVarRef&  other) {
	lairAssert(_type == other._type);
	if(_type) {
		lairAssert(_type->assign);
		_type->assign(_data, other._data);
	}
	return *this;
}

ConstVarRef::ConstVarRef(const Variant& variant)
	: _type(variant._type)
	, _data(_type? variant._getDataBlock(): nullptr)
{}

inline std::ostream& operator<<(std::ostream& out, const VarRef& var) {
	if(var.isEmpty())
		out << "null";
	else if(var.type()->writeRepr)
		var.type()->writeRepr(out, var.data());
	else
		out << "<" << var.type()->identifier << " at " << var.data() << ">";
	return out;
}

inline std::ostream& operator<<(std::ostream& out, const ConstVarRef& var) {
	if(var.isEmpty())
		out << "null";
	else if(var.type()->writeRepr)
		var.type()->writeRepr(out, var.data());
	else
		out << "<" << var.type()->identifier << " at " << var.data() << ">";
	return out;
}

inline std::ostream& operator<<(std::ostream& out, const Variant& var) {
	if(var.isEmpty())
		out << "null";
	else if(var.type()->writeRepr)
		var.type()->writeRepr(out, var.data());
	else
		out << "<" << var.type()->identifier << " at " << var.data() << ">";
	return out;
}


class EnumInfo {
public:
	EnumInfo(const String& name);
	EnumInfo(const EnumInfo& ) = delete;
	EnumInfo(      EnumInfo&&) = delete;
	~EnumInfo() = default;

	EnumInfo& operator=(const EnumInfo& ) = delete;
	EnumInfo& operator=(      EnumInfo&&) = delete;

	EnumInfo& add(int value, const String& label);

	const String& name() const;
	unsigned nFields() const;
	String label(int value) const;
	bool label(String& label, int value) const;
	int value(const String& label, int defaultValue = 0) const;
	bool value(int& value, const String& label) const;

private:
	typedef std::unordered_map<int, String> IntStringMap;
	typedef std::unordered_map<String, int> StringIntMap;

private:
	String       _name;
	IntStringMap _labelFromValue;
	StringIntMap _valueFromLabel;
};

class FlagsInfo {
public:
	struct FlagSet {
		unsigned flags;
		unsigned mask;
		String   label;
	};

public:
	FlagsInfo(const String& name);
	FlagsInfo(const FlagsInfo& ) = delete;
	FlagsInfo(      FlagsInfo&&) = delete;
	~FlagsInfo() = default;

	FlagsInfo& operator=(const FlagsInfo& ) = delete;
	FlagsInfo& operator=(      FlagsInfo&&) = delete;

	FlagsInfo& add(unsigned flag, const String& label);
	FlagsInfo& add(unsigned flags, unsigned mask, const String& label);

	const String& name() const;
	unsigned nFlagSet() const;
	const FlagSet* flagSet(const String& label) const;

	unsigned parse(std::istream& in, ErrorOutput* errors = 0) const;
	unsigned parse(const String& in, ErrorOutput* errors = 0) const;
	void write(std::ostream& out, unsigned flags) const;
	String format(unsigned flags) const;

private:
	typedef std::unique_ptr<FlagSet>                     FlagSetPtr;
	typedef std::vector<FlagSetPtr>                      FlagSetVector;
	typedef std::unordered_map<unsigned, const FlagSet*> FlagsFlagSetMap;
	typedef std::unordered_map<String, const FlagSet*>   LabelFlagSetMap;

private:
	String          _name;
	FlagSetVector   _flagSets;
	LabelFlagSetMap _flagSetFromLabel;
};


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
