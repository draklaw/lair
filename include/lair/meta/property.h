/*
 *  Copyright (C) 2017-2018 Simon Boyé
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


#ifndef _LAIR_META_PROPERTY_H
#define _LAIR_META_PROPERTY_H


#include <type_traits>
#include <unordered_map>

#include <lair/core/lair.h>

#include <lair/meta/metatype.h>
#include <lair/meta/variant.h>
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


}


#endif
