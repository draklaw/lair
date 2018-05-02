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


#ifndef LAIR_META_WITH_PROPERTIES_H
#define LAIR_META_WITH_PROPERTIES_H


#include <lair/core/lair.h>

#include <lair/meta/property.h>
#include <lair/meta/property_list.h>


namespace lair
{


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


}


#endif
