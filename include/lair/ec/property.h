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


#ifndef _LAIR_EC_PROPERTY_H
#define _LAIR_EC_PROPERTY_H


#include <type_traits>
#include <unordered_map>

#include <lair/core/lair.h>

#include <lair/render_gl2/render_pass.h>


namespace lair
{


struct _EnumInfo {
	int         value;
	std::string label;
};
typedef std::vector<_EnumInfo> _EnumMap;

struct _FlagInfo {
	unsigned    mask;
	unsigned    flags;
	std::string label;
};
typedef std::vector<_FlagInfo> _FlagMap;

struct PropertyType {
	std::string name;
	_EnumMap    enumMap;
	_FlagMap    flagMap;

	void addEnum(int value, const std::string& label);
	void addFlag(unsigned flag, const std::string& label);
	void addFlagSet(unsigned mask, unsigned flags, const std::string& label);

	const std::string* findEnum(int value) const;
	const std::string* matchFlag(unsigned flags, unsigned* mask) const;
};


template < typename T >
struct PropertyTrait {
};


}


#define LAIR_EC_DECL_PROPERTY_TYPE(_type, _name) \
namespace lair { \
template <> \
struct PropertyTrait<_type> { \
	static const PropertyType* type() { \
		static PropertyType type = { \
		    _name \
		}; \
		return &type; \
	} \
}; \
}

#define LAIR_EC_DECL_PROPERTY_TYPE_EXT(_type, _name) \
namespace lair { \
template <> \
struct PropertyTrait<_type> { \
	static void setup(PropertyType& type); \
	static const PropertyType* type() { \
		static PropertyType type = { \
		    _name \
		}; \
		setup(type); \
		return &type; \
	} \
}; \
} \
inline void lair::PropertyTrait<_type>::setup(PropertyType& type)

// Property type declaration should be done in the global namespace.

LAIR_EC_DECL_PROPERTY_TYPE(lair::uint8,  "UInt8")
LAIR_EC_DECL_PROPERTY_TYPE(lair:: int8,   "Int8")
LAIR_EC_DECL_PROPERTY_TYPE(lair::uint16, "UInt16")
LAIR_EC_DECL_PROPERTY_TYPE(lair:: int16,  "Int16")
LAIR_EC_DECL_PROPERTY_TYPE(lair::uint32, "UInt32")
LAIR_EC_DECL_PROPERTY_TYPE(lair:: int32,  "Int32")
LAIR_EC_DECL_PROPERTY_TYPE(lair::uint64, "UInt64")
LAIR_EC_DECL_PROPERTY_TYPE(lair:: int64,  "Int64")

LAIR_EC_DECL_PROPERTY_TYPE(lair::Scalar, "Scalar")

LAIR_EC_DECL_PROPERTY_TYPE(std::string, "String")

LAIR_EC_DECL_PROPERTY_TYPE(lair::Vector2,  "Vector2")
LAIR_EC_DECL_PROPERTY_TYPE(lair::Vector3,  "Vector3")
LAIR_EC_DECL_PROPERTY_TYPE(lair::Vector4,  "Vector4")
LAIR_EC_DECL_PROPERTY_TYPE(lair::Vector2i, "Vector2i")
LAIR_EC_DECL_PROPERTY_TYPE(lair::Vector3i, "Vector3i")
LAIR_EC_DECL_PROPERTY_TYPE(lair::Vector4i, "Vector4i")
LAIR_EC_DECL_PROPERTY_TYPE(lair::Matrix2,  "Matrix2")
LAIR_EC_DECL_PROPERTY_TYPE(lair::Matrix3,  "Matrix3")
LAIR_EC_DECL_PROPERTY_TYPE(lair::Matrix4,  "Matrix4")

LAIR_EC_DECL_PROPERTY_TYPE(lair::Transform,  "Transform")
LAIR_EC_DECL_PROPERTY_TYPE(lair::Quaternion, "Quaternion")

LAIR_EC_DECL_PROPERTY_TYPE(lair::Box2, "Box2")
LAIR_EC_DECL_PROPERTY_TYPE(lair::Box3, "Box3")
LAIR_EC_DECL_PROPERTY_TYPE(lair::Box2i, "Box2i")
LAIR_EC_DECL_PROPERTY_TYPE(lair::Box3i, "Box3i")

LAIR_EC_DECL_PROPERTY_TYPE(lair::Path, "Path")


LAIR_EC_DECL_PROPERTY_TYPE_EXT(lair::BlendingMode, "BlendingMode") {
	type.addEnum(lair::BLEND_NONE,     "none");
	type.addEnum(lair::BLEND_ALPHA,    "alpha");
	type.addEnum(lair::BLEND_ADD,      "add");
	type.addEnum(lair::BLEND_MULTIPLY, "multiply");
}

#define LAIR_EC_PROPERTY_TYPE(_type) lair::PropertyTrait<typename std::remove_cv<typename std::remove_reference<_type>::type>::type>::type()


namespace lair
{


class Property {
public:
	Property(const PropertyType* type, const std::string& name);
	Property(const Property&)  = delete;
	Property(      Property&&) = delete;
	virtual ~Property() = default;

	Property& operator=(const Property&)  = delete;
	Property& operator=(      Property&&) = delete;

	const PropertyType* type() const;
	const std::string&  name() const;

	template < typename T >
	const T& get(const void* obj) const {
		lairAssert(_type == PropertyTrait<T>::type());
		return *reinterpret_cast<const T*>(_getPtr(obj));
	}

	template < typename T >
	void set(void* obj, const void* value) const {
		lairAssert(_type == PropertyTrait<T>::type());
		_setPtr(obj, value);
	}

	virtual const void* _getPtr(const void* obj) const = 0;
	virtual void _setPtr(void* obj, const void* value) const = 0;

protected:
	const PropertyType* _type;
	std::string         _name;
};


template < typename C, typename T >
class GenericProperty : public Property {
public:
	GenericProperty(const std::string& name, const T& (C::*get)() const, void (C::*set)(const T&))
		: Property(LAIR_EC_PROPERTY_TYPE(T), name)
		, _get(get)
		, _set(set)
	{
	}

	GenericProperty(const GenericProperty&)  = delete;
	GenericProperty(      GenericProperty&&) = delete;
	virtual ~GenericProperty() = default;

	GenericProperty& operator=(const GenericProperty&)  = delete;
	GenericProperty& operator=(      GenericProperty&&) = delete;

protected:
	virtual const void* _getPtr(const void* obj) const {
		return &(reinterpret_cast<const C*>(obj)->*_get)();
	}

	virtual void _setPtr(void* obj, const void* value) const {
		(reinterpret_cast<C*>(obj)->*_set)(*reinterpret_cast<const T*>(value));
	}

protected:
	const T& (C::*_get)() const;
	void (C::*_set)(const T&);
};


class PropertyList {
public:
	PropertyList()  = default;
	PropertyList(const PropertyList&)  = delete;
	PropertyList(      PropertyList&&) = delete;
	~PropertyList() = default;

	PropertyList& operator=(const PropertyList&)  = delete;
	PropertyList& operator=(      PropertyList&&) = delete;

	unsigned nProperties() const;
	unsigned propertyIndex(const std::string& property) const;
	const Property& property(unsigned index) const;

	template < typename C, typename T >
	void addProperty(const std::string& name, const T& (C::*get)() const, void (C::*set)(const T&)) {
		_propertyMap.emplace(name, nProperties());
		_properties.emplace_back(new GenericProperty<C, T>(name, get, set));
	}

private:
	typedef std::unique_ptr<Property>                 PropertyUP;
	typedef std::vector<PropertyUP>                   PropertyVector;
	typedef std::unordered_map<std::string, unsigned> PropertyMap;

private:
	PropertyVector _properties;
	PropertyMap    _propertyMap;
};


template < typename C >
class Properties {
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
		return property(index).set<T>(this, &value);
	}

	template < typename T >
	void set(const std::string& property, const T& value) {
		return set<T>(propertyIndex(property), value);
	}
};


}


#endif
