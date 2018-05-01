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
 *  along with lair.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef LAIR_META_VARIANT_H
#define LAIR_META_VARIANT_H


#include <lair/core/lair.h>
#include <lair/core/parse.h>

#include <lair/meta/metatype.h>


namespace lair
{


// Uncomment this for a very verbose debug output.
//#define LAIR_DEBUG_VARIANT
#ifdef LAIR_DEBUG_VARIANT
    inline unsigned _ptrIndex(const void* ptr) {
		typedef std::unordered_map<const void*, unsigned> Map;
		static Map map;
		return map.emplace(ptr, map.size()).first->second;
	}

    #define LAIR_VARIANT_ID(_ptr) _ptrIndex(_ptr) << ":Variant<" << ((_ptr)->type()? (_ptr)->type()->identifier: "null") << ">"
    #define LAIR_VARIANT_PRINT(_ptr, ...) std::cerr << LAIR_VARIANT_ID(_ptr) << ": " << __VA_ARGS__ << "\n";
#else
    #define LAIR_VARIANT_ID(_ptr)
    #define LAIR_VARIANT_PRINT(_ptr, ...)
#endif


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
	typedef typename std::aligned_union<16, Byte*, double, Vector4, String>::type VariantData;


	static const Variant null;
	static const String  _nullTypename;


	struct ParseInfo {
		String   filename;
		unsigned line;
		unsigned col;
		String   type;
	};


public:
	inline Variant()
	    : _type(nullptr)
	    , _parseInfo(nullptr)
	{
		LAIR_VARIANT_PRINT(this, "Variant()")
		_clearData();
	}


	inline Variant(const Variant& other)
	    : _type(nullptr)
	    , _parseInfo(nullptr)
	{
		LAIR_VARIANT_PRINT(this, "Variant(const " << LAIR_VARIANT_ID(&other) << ">&)")
		if(other._type) {
			_set(other._type, other._getDataBlock());
		}
		else {
			_clearData();
		}

		if(other._parseInfo) {
			_parseInfo = new ParseInfo(*other._parseInfo);
		}
	}


	inline Variant(Variant&& other)
	    : _type(nullptr)
	    , _parseInfo(nullptr)
	{
		if(!other._type) {
			LAIR_VARIANT_PRINT(this, "Variant(" << LAIR_VARIANT_ID(&other) << "&&)")
			_clearData();
		}
		else if(useExternalStorage() || other._type->isTriviallyCopyable) {
			LAIR_VARIANT_PRINT(this, "Variant(" << LAIR_VARIANT_ID(&other) << "&&) -> memcpy data")
			_type = other._type;
			std::memcpy(&_data, &other._data, sizeof(VariantData));
			other._type = nullptr;
			other._clearData();
		}
		else if(other._type->moveConstruct) {
			LAIR_VARIANT_PRINT(this, "Variant(" << LAIR_VARIANT_ID(&other) << "&&) -> moveConstruct")
			_malloc(other._type);
			_type->moveConstruct(_getDataBlock(), other._getDataBlock());
			other._clear();
		}
		else {
			LAIR_VARIANT_PRINT(this, "Variant(" << LAIR_VARIANT_ID(&other) << "&&) -> copyConstruct")
			_set(other._type, other._getDataBlock());
			other._clear();
		}

		if(other._parseInfo) {
			_parseInfo = other._parseInfo;
			other._parseInfo = nullptr;
		}
	}


	template<typename T>
	inline Variant(const T& object)
	    : _type(nullptr)
	    , _parseInfo(nullptr)
	{
		LAIR_VARIANT_PRINT(this, "Variant(const " << metaTypes.get<T>()->identifier << "&)")
		_set(metaTypes.get<T>(), &object);
	}


	// Template T&& is a forwarding reference that accepts any type (T&, const T&, etc.)
	// This cause this overload to be selected when it should not, for instance
	// with non-const T&. The enable_if is required to makes sure this overload
	// is used only if T in an rvalue.
	template<typename T,
	         typename std::enable_if<std::is_rvalue_reference<T&&>::value, int>::type = 0>
	inline Variant(T&& object)
	    : _type(nullptr)
	    , _parseInfo(nullptr)
	{
		const MetaType* type = metaTypes.get<T>();
		if(type->moveConstruct) {
			LAIR_VARIANT_PRINT(this, "Variant(" << type->identifier << "&&) -> moveConstruct")
			Byte* ptr = _malloc(type);
			type->moveConstruct(ptr, &object);
		}
		else {
			LAIR_VARIANT_PRINT(this, "Variant(" << type->identifier << "&&) -> copyConstruct")
			_set(type, &object);
		}
	}


	inline ~Variant() {
		LAIR_VARIANT_PRINT(this, "~Variant()")
		clear();
	}


	inline Variant& operator=(const Variant& other) {
		if(&other == this)
			return *this;

		LAIR_VARIANT_PRINT(this, "= const " << LAIR_VARIANT_ID(&other) << "&")
		if(other._type) {
			_set(other._type, other._getDataBlock());
		}
		else {
			clear();
		}

		delete _parseInfo;
		_parseInfo = nullptr;
		if(other._parseInfo) {
			_parseInfo = new ParseInfo(*other._parseInfo);
		}

		return *this;
	}


	inline Variant& operator=(Variant&& other) {
		if(&other == this)
			return *this;

		if(!other._type) {
			LAIR_VARIANT_PRINT(this, "= " << LAIR_VARIANT_ID(&other) << "&&")
			clear();
		}
		else if(useExternalStorage() || other._type->isTriviallyCopyable) {
			LAIR_VARIANT_PRINT(this, "= " << LAIR_VARIANT_ID(&other) << "&& -> memcpy data")
			_type = other._type;
			std::memcpy(&_data, &other._data, sizeof(VariantData));
			other._type = nullptr;
			other._clearData();
		}
		else if(other._type->moveConstruct) {
			LAIR_VARIANT_PRINT(this, "= " << LAIR_VARIANT_ID(&other) << "&& -> moveConstruct")
			_malloc(other._type);
			_type->moveConstruct(_getDataBlock(), other._getDataBlock());
			other._clear();
		}
		else {
			LAIR_VARIANT_PRINT(this, "= " << LAIR_VARIANT_ID(&other) << "&& -> copyConstruct")
			_set(other._type, other._getDataBlock());
			other._clear();
		}

		delete _parseInfo;
		_parseInfo = other._parseInfo;
		other._parseInfo = nullptr;

		return *this;
	}


	template<typename T>
	inline Variant& operator=(const T& object)
	{
		LAIR_VARIANT_PRINT(this, "= const " << metaTypes.get<T>()->identifier << "&")
		_set(metaTypes.get<T>(), &object);

		delete _parseInfo;
		_parseInfo = nullptr;

		return *this;
	}


	template<typename T,
	         typename std::enable_if<std::is_rvalue_reference<T&&>::value, int>::type = 0>
	inline Variant& operator=(T&& object)
	{
		const MetaType* type = metaTypes.get<T>();
		if(type->moveConstruct) {
			LAIR_VARIANT_PRINT(this, "= " << type->identifier << "&& -> moveConstruct")
			Byte* ptr = _malloc(type);
			type->moveConstruct(ptr, &object);
		}
		else {
			LAIR_VARIANT_PRINT(this, "= " << type->identifier << "&& -> copyConstruct")
			_set(type, &object);
		}

		delete _parseInfo;
		_parseInfo = nullptr;

		return *this;
	}


	inline const MetaType* type() const {
		return _type;
	}


	inline const String& typeName() const {
		if(_type) {
			return _type->identifier;
		}
		return _nullTypename;
	}


	inline bool isValid() const {
		return _type && _getDataBlock();
	}


	template<typename T>
	inline bool is() const {
		return _type == metaTypes.get<T>();
	}


	inline bool isNull() const {
		return !isValid();
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
		lairAssert(isValid());
		return _getDataBlock();
	}


	inline const void* data() const {
		lairAssert(isValid());
		return _getDataBlock();
	}


	void clear() {
		_clear();

		delete _parseInfo;
		_parseInfo = nullptr;
	}


	void setParseInfo(const ParseInfo& parseInfo) {
		if(!_parseInfo) {
			_parseInfo = new ParseInfo(parseInfo);
		}
		else {
			*_parseInfo = parseInfo;
		}
	}


	void setParseInfo(const String& filename, unsigned line, unsigned col = 0,
	                  const String& type = String()) {
		setParseInfo(ParseInfo{ filename, line, col, type  });
	}


	const ParseInfo* parseInfo() const {
		return _parseInfo;
	}


	String parseInfoDesc() const {
		if(_parseInfo) {
			return cat(_parseInfo->filename, ": ", _parseInfo->line,
			           ": ", _parseInfo->col, ": ");
		}
		return String();
	}


private:
	inline bool useExternalStorage() const {
		return _type && _type->size > sizeof(VariantData);
	}

	template<typename T>
	inline T& _get() {
		return *reinterpret_cast<T*>(&_data);
	}

	template<typename T>
	inline const T& _get() const {
		return *reinterpret_cast<const T*>(&_data);
	}

	inline Byte* _getDataBlock() {
		if(useExternalStorage())
			return _get<Byte*>();
		return reinterpret_cast<Byte*>(&_data);
	}

	inline const Byte* _getDataBlock() const {
		if(useExternalStorage())
			return _get<Byte*>();
		return reinterpret_cast<const Byte*>(&_data);
	}

	inline Byte* _malloc(const MetaType* type) {
		_clear();

		_type = type;

		Byte* ptr = reinterpret_cast<Byte*>(&_data);
		if(useExternalStorage()) {
			// Allocate a memory block to store the object
			ptr = reinterpret_cast<Byte*>(malloc(_type->size));
			if(!ptr)
				throw std::bad_alloc();
			_get<Byte*>() = ptr;
		}

		return ptr;
	}

	inline void _set(const MetaType* type, const void* obj) {
		Byte* ptr = _malloc(type);

		lairAssert(_type->copyConstruct);
		_type->copyConstruct(ptr, obj);
	}

	void _clear() {
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

	inline void _clearData() {
		std::memset(&_data, 0, sizeof(VariantData));
	}

private:
	VariantData     _data;
	const MetaType* _type;
	ParseInfo*      _parseInfo;
};


std::ostream& operator<<(std::ostream& out, const Variant& var);
std::ostream& operator<<(std::ostream& out, const VarList& list);
std::ostream& operator<<(std::ostream& out, const VarMap& map);


}


LAIR_DECLARE_METATYPE(lair::VarList);
LAIR_DECLARE_METATYPE(lair::VarMap);


#endif
