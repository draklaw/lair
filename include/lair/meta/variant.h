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
#include <lair/ldl/parse.h>

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


class VarList;
class VarMap;


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
	};


public:
	Variant();
	Variant(const Variant& other);
	Variant(Variant&& other);

	template<typename T>
	explicit Variant(const T& object);

	// Template T&& is a forwarding reference that accepts any type (T&, const T&, etc.)
	// This cause this overload to be selected when it should not, for instance
	// with non-const T&. The enable_if is required to makes sure this overload
	// is used only if T in an rvalue.
	template<typename T,
	         typename std::enable_if<std::is_rvalue_reference<T&&>::value, int>::type = 0>
	explicit Variant(T&& object);


	~Variant();

	Variant& operator=(const Variant& other);
	Variant& operator=(Variant&& other);

	template<typename T>
	Variant& operator=(const T& object);

	template<typename T,
	         typename std::enable_if<std::is_rvalue_reference<T&&>::value, int>::type = 0>
	Variant& operator=(T&& object);

	const MetaType* type() const;

	const String& typeName() const;

	bool isValid() const;

	template<typename T>
	inline bool is() const {
		return _type == metaTypes.get<T>();
	}

	bool isNull() const;
	bool isBool() const;
	bool isInt() const;
	bool isFloat() const;
	bool isString() const;
	bool isVarList() const;
	bool isVarMap() const;

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

	bool asBool() const;
	int asInt() const;
	double asFloat() const;
	const String& asString() const;
	String& asString();
	const VarList& asVarList() const;
	VarList& asVarList();
	const VarMap& asVarMap() const;
	VarMap& asVarMap();

	const Variant& operator[](unsigned i) const;
	Variant& operator[](unsigned i);
	const Variant& operator[](const String& key) const;
	Variant& operator[](const String& key);

	const Variant& get(const String& key) const;

//	template<typename T>
//	inline const Variant& get(const String& key, const T& defaultValue) const {
//		lairAssert(isVarMap());
//		auto it = asVarMap().find(key);
//		if(it != asVarMap().end())
//			return it->second;
//		return Variant(defaultValue);
//	}

	void setInt(int i);

	inline void* data();
	inline const void* data() const;

	void clear();

	void setParseInfo(const ParseInfo& parseInfo);
	void setParseInfo(const String& filename, unsigned line, unsigned col = 0);
	const ParseInfo* parseInfo() const;
	String parseInfoDesc() const;


private:
	bool useExternalStorage() const;

	template<typename T>
	inline T& _get() {
		return *reinterpret_cast<T*>(&_data);
	}

	template<typename T>
	inline const T& _get() const {
		return *reinterpret_cast<const T*>(&_data);
	}

	Byte* _getDataBlock();
	const Byte* _getDataBlock() const;
	Byte* _malloc(const MetaType* type);
	void _set(const MetaType* type, const void* obj);

	void _clear();
	void _clearData();

private:
	VariantData     _data;
	const MetaType* _type;
	ParseInfo*      _parseInfo;
};


template<typename T>
inline Variant::Variant(const T& object)
    : _type(nullptr)
    , _parseInfo(nullptr)
{
	LAIR_VARIANT_PRINT(this, "Variant(const " << metaTypes.get<T>()->identifier << "&)")
	_set(metaTypes.get<T>(), &object);
}


template<typename T,
         typename std::enable_if<std::is_rvalue_reference<T&&>::value, int>::type = 0>
inline Variant::Variant(T&& object)
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


template<typename T>
inline Variant& Variant::operator=(const T& object)
{
	LAIR_VARIANT_PRINT(this, "= const " << metaTypes.get<T>()->identifier << "&")
	_set(metaTypes.get<T>(), &object);

	delete _parseInfo;
	_parseInfo = nullptr;

	return *this;
}


template<typename T,
         typename std::enable_if<std::is_rvalue_reference<T&&>::value, int>::type = 0>
inline Variant& Variant::operator=(T&& object)
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




std::ostream& operator<<(std::ostream& out, const Variant& var);
std::ostream& operator<<(std::ostream& out, const VarList& list);
std::ostream& operator<<(std::ostream& out, const VarMap& map);


}


LAIR_DECLARE_METATYPE(lair::VarList);
LAIR_DECLARE_METATYPE(lair::VarMap);


#endif
