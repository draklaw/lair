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


#ifndef LAIR_META_PROPERTY_LIST_H
#define LAIR_META_PROPERTY_LIST_H


#include <lair/core/lair.h>


namespace lair
{


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


}


#endif
