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


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include "lair/meta/var_map.h"


namespace lair
{


VarMap::VarMap(unsigned style)
    : _type()
    , _style(style) {
}


VarMap::VarMap(const String& type, unsigned style)
    : _type(type)
    , _style(style) {
}


VarMap::~VarMap() {
}


VarMap::iterator VarMap::begin() {
	return _list.begin();
}


VarMap::iterator VarMap::end() {
	return _list.end();
}


VarMap::const_iterator VarMap::begin() const {
	return _list.begin();
}


VarMap::const_iterator VarMap::end() const {
	return _list.end();
}


unsigned VarMap::empty() const {
	return _list.empty();
}


unsigned VarMap::size() const {
	return _list.size();
}


void VarMap::clear() {
	_map.clear();
	_list.clear();
}


std::pair<VarMap::iterator,bool> VarMap::insert(const Pair& pair) {
	auto it = _map.find(pair.first);
	if(it != _map.end()) {
		return std::make_pair(_list.begin() + it->second, false);
	}

	_map.emplace(pair.first, _list.size());
	_list.push_back(pair);

	return std::make_pair(_list.end() - 1, true);
}


std::pair<VarMap::iterator,bool> VarMap::insert(Pair&& pair) {
	auto it = _map.find(pair.first);
	if(it != _map.end()) {
		return std::make_pair(_list.begin() + it->second, false);
	}

	_map.emplace(pair.first, _list.size());
	_list.push_back(std::move(pair));

	return std::make_pair(_list.end() - 1, true);
}


VarMap::iterator VarMap::erase(const_iterator pos) {
	unsigned index = pos - _list.begin();
	Map::iterator it = _map.begin();
	for(; it != _map.end(); ++it) {
		if(it->second == index) {
			break;
		}
	}

	return _erase(it);
}


unsigned VarMap::erase(const String& key) {
	_erase(_map.find(key));
	return 1;
}


void VarMap::swap(VarMap& other) {
	_map.swap(other._map);
	_list.swap(other._list);
	_type.swap(other._type);
	std::swap(_style, other._style);
}


Variant& VarMap::at(const String& key) {
	return _list.at(_map.at(key)).second;
}


const Variant& VarMap::at(const String& key) const {
	return _list.at(_map.at(key)).second;
}


Variant& VarMap::operator[](const String& key) {
	auto it = _map.find(key);
	if(it == _map.end()) {
		it = _map.emplace(key, _list.size()).first;
		_list.emplace_back(key, Variant());
	}
	return _list[it->second].second;
}


unsigned VarMap::count(const String& key) const {
	return _map.count(key);
}


VarMap::iterator VarMap::find(const String& key) {
	auto it = _map.find(key);
	if(it == _map.end()) {
		return _list.end();
	}
	return _list.begin() + it->second;
}


VarMap::const_iterator VarMap::find(const String& key) const {
	return const_cast<VarMap*>(this)->find(key);
}


const Variant& VarMap::get(const String& id) const {
	auto it = find(id);
	if(it != end())
		return it->second;
	return Variant::null;
}


const String& VarMap::type() const {
	return _type;
}


String& VarMap::type() {
	return _type;
}


unsigned VarMap::style() const {
	return _style;
}


bool VarMap::isInline() const {
	return _style & INLINE;
}


VarMap::iterator VarMap::_erase(Map::iterator it) {
	lairAssert(it != _map.end());

	unsigned index = it->second;

	_list.erase(_list.begin() + index);
	_map.erase(it);

	for(auto&& pair: _map) {
		if(pair.second > index) {
			pair.second -= 1;
		}
	}

	return _list.begin() + index;
}


}
