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


#ifndef _LAIR_CORE_PROPERTY_H
#define _LAIR_CORE_PROPERTY_H


#include <type_traits>
#include <unordered_map>

#include <lair/core/lair.h>
#include <lair/core/ldl.h>

#include <lair/meta/metatype.h>
#include <lair/meta/enum_info.h>
#include <lair/meta/flags_info.h>


namespace lair
{


/**
 * \brief The base class to represent a property.
 */
class Property {
public:
	Property(const MetaType* type, const std::string& name,
	         const EnumInfo* enumInfo, const FlagsInfo* flagsInfo);
	Property(const Property&)  = delete;
	Property(      Property&&) = delete;
	virtual ~Property() = default;

	Property& operator=(const Property&)  = delete;
	Property& operator=(      Property&&) = delete;

	const MetaType*     type() const;
	const EnumInfo*     enumInfo() const;
	const FlagsInfo*    flagsInfo() const;
	const std::string&  name() const;

	virtual Variant getVar(const void* obj) const = 0;
	virtual void    setVar(      void* obj, const Variant& var) const = 0;

	template < typename T >
	T get(const void* obj) const {
		return getVar(obj).as<T>();
	}

	template < typename T >
	void set(void* obj, const T& value) const {
		setVar(obj, Variant(value));
	}

protected:
	const MetaType*  _type;
	const EnumInfo*  _enumInfo;
	const FlagsInfo* _flagsInfo;
	std::string      _name;
};


/**
 * \brief The actuall implementation of a property, for a given class C and
 * type T.
 *
 * Allow to get / set a property using ConstVarRef objects. Behind the scene,
 * calls the setter / getter method of the object.
 */
template < typename C, typename T >
class GenericPropertyRef : public Property {
public:
	GenericPropertyRef(const std::string& name, const EnumInfo* enumInfo, const FlagsInfo* flagsInfo,
	                   const T& (C::*get)() const, void (C::*set)(const T&))
		: Property(metaTypes.get<T>(), name, enumInfo, flagsInfo)
		, _get(get)
		, _set(set)
	{
	}

	GenericPropertyRef(const GenericPropertyRef&)  = delete;
	GenericPropertyRef(      GenericPropertyRef&&) = delete;
	virtual ~GenericPropertyRef() = default;

	GenericPropertyRef& operator=(const GenericPropertyRef&)  = delete;
	GenericPropertyRef& operator=(      GenericPropertyRef&&) = delete;

	virtual Variant getVar(const void* obj) const {
		return (reinterpret_cast<const C*>(obj)->*_get)();
	}

	virtual void setVar(void* obj, const Variant& var) const {
		(reinterpret_cast<C*>(obj)->*_set)(var.as<T>());
	}

protected:
	const T& (C::*_get)() const;
	void (C::*_set)(const T&);
};

/// Same as GenericPropertyRef, but take accessor that work by-value instead of
/// by-reference.
template < typename C, typename T >
class GenericPropertyValue : public Property {
public:
	GenericPropertyValue(const std::string& name, const EnumInfo* enumInfo, const FlagsInfo* flagsInfo,
	                     T (C::*get)() const, void (C::*set)(T))
		: Property(metaTypes.get<T>(), name, enumInfo, flagsInfo)
		, _get(get)
		, _set(set)
	{
	}

	GenericPropertyValue(const GenericPropertyValue&)  = delete;
	GenericPropertyValue(      GenericPropertyValue&&) = delete;
	virtual ~GenericPropertyValue() = default;

	GenericPropertyValue& operator=(const GenericPropertyValue&)  = delete;
	GenericPropertyValue& operator=(      GenericPropertyValue&&) = delete;

	virtual Variant getVar(const void* obj) const {
		return (reinterpret_cast<const C*>(obj)->*_get)();
	}

	virtual void setVar(void* obj, const Variant& var) const {
		(reinterpret_cast<C*>(obj)->*_set)(var.as<T>());
	}

protected:
	T (C::*_get)() const;
	void (C::*_set)(T);
};


/// Same as GenericPropertyRef, but work directly with public a data member
/// instead of calling accessors.
template < typename C, typename T >
class GenericPropertyMember : public Property {
public:
	GenericPropertyMember(const std::string& name, const EnumInfo* enumInfo, const FlagsInfo* flagsInfo,
	                     T C::* member)
		: Property(metaTypes.get<T>(), name, enumInfo, flagsInfo)
		, _member(member)
	{
	}

	GenericPropertyMember(const GenericPropertyMember&)  = delete;
	GenericPropertyMember(      GenericPropertyMember&&) = delete;
	virtual ~GenericPropertyMember() = default;

	GenericPropertyMember& operator=(const GenericPropertyMember&)  = delete;
	GenericPropertyMember& operator=(      GenericPropertyMember&&) = delete;

	virtual Variant getVar(const void* obj) const {
		return reinterpret_cast<const C*>(obj)->*_member;
	}

	virtual void setVar(void* obj, const Variant& var) const {
		reinterpret_cast<C*>(obj)->*_member = var.as<T>();
	}

protected:
	T C::* _member;
};


/**
 * \brief Store a list of properties, as well as an index to access them by name.
 */
class PropertyList {
public:
	PropertyList()  = default;
	PropertyList(const PropertyList&)  = default;
	PropertyList(      PropertyList&&) = default;
	~PropertyList() = default;

	PropertyList& operator=(const PropertyList&)  = default;
	PropertyList& operator=(      PropertyList&&) = default;

	unsigned nProperties() const;
	/// Return -1 if there is no property with that name.
	int propertyIndex(const std::string& propertyName) const;
	const Property& property(unsigned index) const;

	// By reference
	template < typename C, typename T >
	void addProperty(const std::string& name, const T& (C::*get)() const, void (C::*set)(const T&)) {
		_addProperty(name, nullptr, nullptr, get, set);
	}

	template < typename C, typename T >
	void addProperty(const std::string& name, const EnumInfo* enumInfo,
	                 const T& (C::*get)() const, void (C::*set)(const T&)) {
		_addProperty(name, enumInfo, nullptr, get, set);
	}

	template < typename C, typename T >
	void addProperty(const std::string& name, const FlagsInfo* flagsInfo,
	                 const T& (C::*get)() const, void (C::*set)(const T&)) {
		_addProperty(name, nullptr, flagsInfo, get, set);
	}

	template < typename C, typename T >
	void _addProperty(const std::string& name,
	                  const EnumInfo* enumInfo, const FlagsInfo* flagsInfo,
	                  const T& (C::*get)() const, void (C::*set)(const T&)) {
		_propertyMap.emplace(name, nProperties());
		_properties.emplace_back(
		            new GenericPropertyRef<C, T>(name, enumInfo, flagsInfo, get, set));
	}

	// By value
	template < typename C, typename T >
	void addProperty(const std::string& name, T (C::*get)() const, void (C::*set)(T)) {
		_addProperty(name, nullptr, nullptr, get, set);
	}

	template < typename C, typename T >
	void addProperty(const std::string& name, const EnumInfo* enumInfo,
	                 T (C::*get)() const, void (C::*set)(T)) {
		_addProperty(name, enumInfo, nullptr, get, set);
	}

	template < typename C, typename T >
	void addProperty(const std::string& name, const FlagsInfo* flagsInfo,
	                 T (C::*get)() const, void (C::*set)(T)) {
		_addProperty(name, nullptr, flagsInfo, get, set);
	}

	template < typename C, typename T >
	void _addProperty(const std::string& name,
	                  const EnumInfo* enumInfo, const FlagsInfo* flagsInfo,
	                  T (C::*get)() const, void (C::*set)(T)) {
		_propertyMap.emplace(name, nProperties());
		_properties.emplace_back(
		            new GenericPropertyValue<C, T>(name, enumInfo, flagsInfo, get, set));
	}

	// By field accessor
	template < typename C, typename T >
	void addProperty(const std::string& name, T C::* member) {
		_addProperty(name, nullptr, nullptr, member);
	}

	template < typename C, typename T >
	void addProperty(const std::string& name, const EnumInfo* enumInfo,
	                 T C::* member) {
		_addProperty(name, enumInfo, nullptr, member);
	}

	template < typename C, typename T >
	void addProperty(const std::string& name, const FlagsInfo* flagsInfo,
	                 T C::* member) {
		_addProperty(name, nullptr, flagsInfo, member);
	}

	template < typename C, typename T >
	void _addProperty(const std::string& name,
	                  const EnumInfo* enumInfo, const FlagsInfo* flagsInfo,
	                  T C::* member) {
		_propertyMap.emplace(name, nProperties());
		_properties.emplace_back(
		            new GenericPropertyMember<C, T>(name, enumInfo, flagsInfo, member));
	}

private:
	typedef std::shared_ptr<Property>                 PropertySP;
	typedef std::vector<PropertySP>                   PropertyVector;
	typedef std::unordered_map<std::string, unsigned> PropertyMap;

private:
	PropertyVector _properties;
	PropertyMap    _propertyMap;
};


/**
 * \brief A mixin class using the CRTP to implement utility method for
 * property-able classes.
 *
 * The leaf class(es) must implement the method properties() that returns a
 * `const PropertyList&` containing a list of all supported properties.
 */
template < typename C >
class WithProperties {
public:
	unsigned nProperties() const {
		return static_cast<const C*>(this)->properties().nProperties();
	}

	unsigned propertyIndex(const std::string& property) const {
		return static_cast<const C*>(this)->properties().propertyIndex(property);
	}

	const Property& property(unsigned index) const {
		return static_cast<const C*>(this)->properties().property(index);
	}

	template < typename T >
	T get(unsigned index) const {
		lairAssert(index < nProperties());
		return property(index).template get<T>(this);
	}

	template < typename T >
	T get(const std::string& property) const {
		return get<T>(propertyIndex(property));
	}

	template < typename T >
	void set(unsigned index, const T& value) {
		lairAssert(index < nProperties());
		return property(index).template set<T>(this, value);
	}

	template < typename T >
	void set(const std::string& property, const T& value) {
		return set<T>(propertyIndex(property), value);
	}
};


template<typename T>
bool _jsonFromProperty(Json::Value& json, const void* obj, const Property& property, ErrorList* errors) {
	return toJson(json, property.get<T>(obj), errors);
}

template<typename T>
bool _propertyFromJson(void* obj, const Property& property, const Json::Value& json, ErrorList* errors) {
	T tmp;
	bool result = fromJson(tmp, json, errors);
	if(result) {
		property.set<T>(obj, tmp);
	}
	return result;
}


class LdlPropertySerializer {
private:
	typedef std::function<bool(LdlParser&, void*, const Property&)> ReadFn;
	typedef std::function<bool(LdlWriter&, const void*, const Property&)> WriteFn;
public:
	LdlPropertySerializer();
	LdlPropertySerializer(const LdlPropertySerializer&)  = delete;
	LdlPropertySerializer(      LdlPropertySerializer&&) = delete;
	~LdlPropertySerializer() = default;

	LdlPropertySerializer& operator=(const LdlPropertySerializer&)  = delete;
	LdlPropertySerializer& operator=(      LdlPropertySerializer&&) = delete;

	template<typename T>
	inline void registerType() {
		bool (*read )(LdlParser&,       T&) = &ldlRead;
		bool (*write)(LdlWriter&, const T&) = &ldlWrite;
		_registerType(metaTypes.get<T>(),
		              ReadFn(ReadWrapper<T, bool(*)(LdlParser&, T&)>(read)),
		              WriteFn(WriteWrapper<T, bool(*)(LdlWriter&, const T&)>(write)));
	}

	template<typename T, typename Read, typename Write>
	inline void registerType(const Read& read, const Write& write) {
		_registerType(metaTypes.get<T>(),
		              ReadFn(ReadWrapper<T, Read>(read)),
		              WriteFn(WriteWrapper<T, Write>(write)));
	}

	template<typename T>
	inline void registerEnum() {
		_registerType(metaTypes.get<T>(), nullptr, nullptr);
	}

	template<typename T>
	inline bool read(LdlParser& parser, T& obj) {
		return _read(parser, obj.properties(), &obj);
	}

	template<typename T>
	inline bool write(LdlWriter& writer, const T& obj) {
		return _write(writer, obj.properties(), &obj);
	}

	bool _read(LdlParser& parser, const PropertyList& properties, void* obj);
	bool _write(LdlWriter& writer, const PropertyList& properties, const void* obj);

private:
	struct ReadWrite {
		ReadFn  read;
		WriteFn write;
	};
	typedef std::unordered_map<const MetaType*, ReadWrite> TypeMap;

private:
	template<typename T, typename ReadFn>
	struct ReadWrapper {
		ReadWrapper(const ReadFn& read): _read(read) {}

		bool operator()(LdlParser& parser, void* obj, const Property& property) const {
			T tmp;
			if(_read(parser, tmp)) {
				property.set<T>(obj, tmp);
				return true;
			}
			return false;
		}

		ReadFn _read;
	};

	template<typename T, typename WriteFn>
	struct WriteWrapper {
		WriteWrapper(const WriteFn& write): _write(write) {}

		bool operator()(LdlWriter& writer, const void* obj, const Property& property) const {
			return _write(writer, property.get<T>(obj));
		}

		WriteFn _write;
	};

	void _registerType(const MetaType* metatype, ReadFn read, WriteFn write);

	const ReadWrite* _getReadWrite(const MetaType* metatype) const;

private:
	TypeMap _map;
};


}


#endif
