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


#ifndef LAIR_META_VARIANT_H
#define LAIR_META_VARIANT_H


#include <lair/core/lair.h>

#include <lair/meta/metatype.h>


namespace lair
{


/**
 * \brief Union used to compute the size of Variant data block.
 *
 * Any type stored in this union will be storable directly in a Variant, without
 * allocating a new data block.
 */
union _VariantDataContent {
	void*  ptr;
	double real;
};

enum {
	/// The size of the data stored in a Variant.
	VARIANT_DATA_SIZE = sizeof(_VariantDataContent),
};


class Variant;

/// A vector of Variant.
typedef std::vector<Variant> VarList;

/// A map String -> Variant.
typedef std::unordered_map<String, Variant> VarMap;


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
	static Variant null;

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

	// TODO: Implement move contructor using moveConstruct
//	inline Variant(Variant&& other)
//		: _type(nullptr)
//	{
//		dbgLogger.warning("Move constructor ", this, ", ", &other);
//		if(other._type) {
//			_type = other._type;
//			std::memcpy(_data, other._data, VARIANT_DATA_SIZE);
//			other._type = nullptr;
//			other._clearData();
//		}
//		else {
//			_clearData();
//		}
//	}

	template<typename T>
	inline Variant(const T& object)
	    : _type(nullptr)
	{
		_set(metaTypes.get<T>(), &object);
	}

	inline ~Variant() {
		clear();
	}

	inline Variant& operator=(Variant other) {
		other.swap(*this);
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

	inline bool isNull() const {
		return isEmpty();
	}

	inline bool isBool() const {
		return _type && _type->toInt;
	}

	inline bool isInt() const {
		return _type && _type->toInt;
	}

	inline bool isFloat() const {
		return isInt() || is<float>() || is<double>();
	}

	inline bool isString() const {
		return is<String>();
	}

	inline bool isVarList() const {
		return is<VarList>();
	}

	inline bool isVarMap() const {
		return is<VarMap>();
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

	inline bool asBool() const {
		if(is<bool>())
			return as<bool>();
		if(isInt())
			return asInt();
		lairAssert(false);
		return false;
	}

	inline int asInt() const {
		lairAssert(_type && _type->toInt);
		return _type->toInt(_getDataBlock());
	}

	inline double asFloat() const {
		if(isInt())
			return asInt();
		if(is<float>())
			return as<float>();
		if(is<double>())
			return as<double>();
		lairAssert(false);
		return 0;
	}

	inline const String& asString() const {
		return as<String>();
	}

	inline const VarList& asVarList() const {
		return as<VarList>();
	}

	inline VarList& asVarList() {
		return as<VarList>();
	}

	inline const VarMap& asVarMap() const {
		return as<VarMap>();
	}

	inline VarMap& asVarMap() {
		return as<VarMap>();
	}

	inline const Variant& operator[](unsigned i) const {
		return const_cast<Variant*>(this)->operator[](i);
	}

	inline Variant& operator[](unsigned i) {
		lairAssert(isVarList());
		lairAssert(i < asVarList().size());
		return asVarList()[i];
	}

	inline const Variant& operator[](const String& key) const {
		return const_cast<Variant*>(this)->operator[](key);
	}

	inline Variant& operator[](const String& key) {
		lairAssert(isVarMap());
		auto it = asVarMap().find(key);
		lairAssert(it != asVarMap().end());
		return it->second;
	}

	inline const Variant& get(const String& key) const {
		lairAssert(isVarMap());
		auto it = asVarMap().find(key);
		if(it != asVarMap().end())
			return it->second;
		return null;
	}

//	template<typename T>
//	inline const Variant& get(const String& key, const T& defaultValue) const {
//		lairAssert(isVarMap());
//		auto it = asVarMap().find(key);
//		if(it != asVarMap().end())
//			return it->second;
//		return Variant(defaultValue);
//	}

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
				if(_type->destroy) {
					_type->destroy(ptr);
				}

				if(useExternalStorage()) {
					free(ptr);
					ptr = nullptr;
				}
			}

			_type = nullptr;
			_clearData();
		}
	}

	void swap(Variant& other) noexcept {
		std::swap(_type, other._type);

		uint8 tmp[VARIANT_DATA_SIZE];
		std::memcpy(tmp,        _data,        VARIANT_DATA_SIZE);
		std::memcpy(_data,       other._data, VARIANT_DATA_SIZE);
		std::memcpy(other._data, tmp,         VARIANT_DATA_SIZE);
	}


private:
	inline bool useExternalStorage() const {
		return !_type || !_type->isPod || _type->size > VARIANT_DATA_SIZE;
	}

	template<typename T>
	inline T& _get() {
		return *reinterpret_cast<T*>(_data);
	}

	template<typename T>
	inline const T& _get() const {
		return *reinterpret_cast<const T*>(_data);
	}

	inline void* _getDataBlock() {
		if(useExternalStorage())
			return _get<void*>();
		return (void*)&_data;
	}

	inline const void* _getDataBlock() const {
		if(useExternalStorage())
			return _get<void*>();
		return (void*)&_data;
	}

	inline void _set(const MetaType* type, const void* obj) {
		clear();

		_type = type;

		void* ptr = (void*)&_data;
		if(useExternalStorage()) {
			// Allocate a memory block to store the object
			ptr = malloc(_type->size);
			if(!ptr)
				throw std::bad_alloc();
			_get<void*>() = ptr;
		}

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


std::ostream& operator<<(std::ostream& out, const Variant& var);
std::ostream& operator<<(std::ostream& out, const VarList& list);
std::ostream& operator<<(std::ostream& out, const VarMap& map);


}


LAIR_REGISTER_METATYPE(lair::VarList, "VarList");
LAIR_REGISTER_METATYPE(lair::VarMap,  "VarMap");


#endif
