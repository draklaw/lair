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
#include <lair/core/metatype.h>
#include <lair/core/ldl.h>


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

	virtual ConstVarRef getVar(const void* obj) const = 0;
	virtual void        setVar(      void* obj, const ConstVarRef& var) const = 0;

	template < typename T >
	const T& get(const void* obj) const {
		return getVar(obj).as<T>();
	}

	template < typename T >
	void set(void* obj, const T& value) const {
		setVar(obj, ConstVarRef(value));
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
class GenericProperty : public Property {
public:
	GenericProperty(const std::string& name, const EnumInfo* enumInfo, const FlagsInfo* flagsInfo,
	                const T& (C::*get)() const, void (C::*set)(const T&))
		: Property(metaTypes.get<T>(), name, enumInfo, flagsInfo)
		, _get(get)
		, _set(set)
	{
	}

	GenericProperty(const GenericProperty&)  = delete;
	GenericProperty(      GenericProperty&&) = delete;
	virtual ~GenericProperty() = default;

	GenericProperty& operator=(const GenericProperty&)  = delete;
	GenericProperty& operator=(      GenericProperty&&) = delete;

	virtual ConstVarRef getVar(const void* obj) const {
		return (reinterpret_cast<const C*>(obj)->*_get)();
	}

	virtual void setVar(void* obj, const ConstVarRef& var) const {
		(reinterpret_cast<C*>(obj)->*_set)(var.as<T>());
	}

protected:
	const T& (C::*_get)() const;
	void (C::*_set)(const T&);
};


/**
 * \brief Store a list of properties, as well as an index to access them by name.
 */
class PropertyList {
public:
	PropertyList()  = default;
	PropertyList(const PropertyList&)  = delete;
	PropertyList(      PropertyList&&) = delete;
	~PropertyList() = default;

	PropertyList& operator=(const PropertyList&)  = delete;
	PropertyList& operator=(      PropertyList&&) = delete;

	unsigned nProperties() const;
	/// Return -1 if there is no property with that name.
	int propertyIndex(const std::string& propertyName) const;
	const Property& property(unsigned index) const;

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
		            new GenericProperty<C, T>(name, enumInfo, flagsInfo, get, set));
	}

private:
	typedef std::unique_ptr<Property>                 PropertyUP;
	typedef std::vector<PropertyUP>                   PropertyVector;
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
	const T& get(unsigned index) const {
		lairAssert(index < nProperties());
		return property(index).get<T>(this);
	}

	template < typename T >
	const T& get(const std::string& property) const {
		return get<T>(propertyIndex(property));
	}

	template < typename T >
	void set(unsigned index, const T& value) {
		lairAssert(index < nProperties());
		return property(index).set<T>(this, value);
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

//class JsonPropertySerializer {
//public:
//	typedef bool (*Serialize)(Json::Value& json, const void* obj, const Property& property, ErrorList* errors);
//	typedef bool (*Deserialize)(void* obj, const Property& property, const Json::Value& json, ErrorList* errors);

//public:
//	static JsonPropertySerializer createWithLairTypes();

//	void _registerType(const MetaType* type, Serialize serialize, Deserialize deserialize);
//	bool _serialize(Json::Value& json, const void* obj, const Property& property, ErrorList* errors) const;
//	bool _deserialize(void* obj, const Property& property, const Json::Value& json, ErrorList* errors) const;

//	template<typename T>
//	void registerType() {
//		_registerType(metaTypes.get<T>(), &_jsonFromProperty<T>, &_propertyFromJson<T>);
//	}

//	template < typename C >
//	bool serialize(Json::Value& json, const C& obj, unsigned propertyIndex, ErrorList* errors = 0) const {
//		return _serialize(json, &obj, obj.property(propertyIndex), errors);
//	}

//	template < typename C >
//	bool deserialize(C& obj, unsigned propertyIndex, const Json::Value& json, ErrorList* errors = 0) const {
//		return _deserialize(&obj, obj.property(propertyIndex), json, errors);
//	}

//private:
//	struct Functions {
//		Serialize   serialize;
//		Deserialize deserialize;
//	};

//	typedef std::unordered_map<const MetaType*, Functions> FunctionMap;

//public:
//	FunctionMap _functions;
//};

class LdlPropertySerializer {
public:
	LdlPropertySerializer();
	LdlPropertySerializer(const LdlPropertySerializer&)  = delete;
	LdlPropertySerializer(      LdlPropertySerializer&&) = delete;
	~LdlPropertySerializer() = default;

	LdlPropertySerializer& operator=(const LdlPropertySerializer&)  = delete;
	LdlPropertySerializer& operator=(      LdlPropertySerializer&&) = delete;

	template<typename T>
	inline void registerType() {
		_registerType(metaTypes.get<T>(), _wrapLdlRead<T>, _wrapLdlWrite<T>);
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
	typedef bool (*ReadFn)(LdlParser& parser, void* value, const Property& property);
	typedef bool (*WriteFn)(LdlWriter& writer, const void* value, const Property& property);

	struct ReadWrite {
		ReadFn  read;
		WriteFn write;
	};
	typedef std::unordered_map<const MetaType*, ReadWrite> TypeMap;

private:
	template<typename T>
	static bool _wrapLdlRead(LdlParser& parser, void* obj, const Property& property) {
		T tmp;
		if(ldlRead(parser, tmp)) {
			property.set<T>(obj, tmp);
			return true;
		}
		return false;
	}

	template<typename T>
	static bool _wrapLdlWrite(LdlWriter& writer, const void* obj, const Property& property) {
		return ldlWrite(writer, property.get<T>(obj));
	}

	void _registerType(const MetaType* metatype, ReadFn read, WriteFn write);

	const ReadWrite* _getReadWrite(const MetaType* metatype) const;

private:
	TypeMap _map;
};


}


#endif
