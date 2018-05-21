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


#ifndef LAIR_META_VAR_MAP_H
#define LAIR_META_VAR_MAP_H


#include <unordered_map>

#include <lair/core/lair.h>

#include <lair/meta/variant.h>


namespace lair
{


class VarMap {
public:
	typedef std::pair<String, Variant> Pair;
	typedef std::unordered_map<String, unsigned> Map;
	typedef std::vector<Pair> List;

	typedef List::iterator iterator;
	typedef List::const_iterator const_iterator;

	enum Style {
		INLINE = 0x01,
	};

public:
	VarMap(unsigned style = 0);
	VarMap(const String& type, unsigned style = 0);
	VarMap(const VarMap&) = default;
	VarMap(VarMap&&)      = default;
	~VarMap();

	VarMap& operator=(const VarMap&) = default;
	VarMap& operator=(VarMap&&)      = default;

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	unsigned empty() const;
	unsigned size() const;

	void clear();

	std::pair<iterator,bool> insert(const Pair& pair);
	std::pair<iterator,bool> insert(Pair&& pair);

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args);

	iterator erase(const_iterator pos);
	unsigned erase(const String& key);

	void swap(VarMap& other);

	Variant& at(const String& key);
	const Variant& at(const String& key) const;

	Variant& operator[](const String& key);

	unsigned count(const String& key) const;

	iterator find(const String& key);
	const_iterator find(const String& key) const;

	const Variant& get(const String& id) const;

	const String& type() const;
	String& type();

	unsigned style() const;
	bool isInline() const;

protected:
	iterator _erase(Map::iterator it);

protected:
	Map _map;
	List _list;
	String   _type;
	unsigned _style;
};


template<typename... Args>
std::pair<VarMap::iterator, bool> VarMap::emplace(Args&&... args) {
	return insert(Pair(std::forward<Args>(args)...));
}


}


#endif
