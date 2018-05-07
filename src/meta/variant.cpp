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

#include <lair/meta/var_list.h>
#include <lair/meta/var_map.h>

#include "lair/meta/variant.h"


namespace lair
{


const Variant Variant::null = Variant();
const String Variant::_nullTypename = "null";


Variant::Variant()
    : _type(nullptr)
    , _parseInfo(nullptr)
{
	LAIR_VARIANT_PRINT(this, "Variant()")
	_clearData();
}


Variant::Variant::Variant(const Variant& other)
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


Variant::Variant(Variant&& other)
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


Variant::~Variant() {
	LAIR_VARIANT_PRINT(this, "~Variant()")
	clear();
}


Variant& Variant::operator=(const Variant& other) {
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


Variant& Variant::operator=(Variant&& other) {
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


const MetaType* Variant::type() const {
	return _type;
}


const String& Variant::typeName() const {
	if(_type) {
		return _type->identifier;
	}
	return _nullTypename;
}


bool Variant::isValid() const {
	return _type && _getDataBlock();
}


bool Variant::isNull() const {
	return !isValid();
}


bool Variant::isBool() const {
	return _type && _type->toInt;
}


bool Variant::isInt() const {
	return _type && _type->toInt;
}


bool Variant::isFloat() const {
	return isInt() || is<float>() || is<double>();
}


bool Variant::isString() const {
	return is<String>();
}


bool Variant::isVarList() const {
	return is<VarList>();
}


bool Variant::isVarMap() const {
	return is<VarMap>();
}


bool Variant::asBool() const {
	if(is<bool>())
		return as<bool>();
	if(isInt())
		return asInt();
	lairAssert(false);
	return false;
}


int Variant::asInt() const {
	lairAssert(_type && _type->toInt);
	return _type->toInt(_getDataBlock());
}


double Variant::asFloat() const {
	if(isInt())
		return asInt();
	if(is<float>())
		return as<float>();
	if(is<double>())
		return as<double>();
	lairAssert(false);
	return 0;
}


const String& Variant::asString() const {
	return as<String>();
}


String& Variant::asString() {
	return as<String>();
}


const VarList& Variant::asVarList() const {
	return as<VarList>();
}


VarList& Variant::asVarList() {
	return as<VarList>();
}


const VarMap& Variant::asVarMap() const {
	return as<VarMap>();
}


VarMap& Variant::asVarMap() {
	return as<VarMap>();
}


const Variant& Variant::operator[](unsigned i) const {
	return const_cast<Variant*>(this)->operator[](i);
}


Variant& Variant::operator[](unsigned i) {
	lairAssert(isVarList());
	lairAssert(i < asVarList().size());
	return asVarList()[i];
}


const Variant& Variant::operator[](const String& key) const {
	return const_cast<Variant*>(this)->operator[](key);
}


Variant& Variant::operator[](const String& key) {
	lairAssert(isVarMap());
	auto it = asVarMap().find(key);
	lairAssert(it != asVarMap().end());
	return it->second;
}


const Variant& Variant::get(const String& key) const {
	lairAssert(isVarMap());
	auto it = asVarMap().find(key);
	if(it != asVarMap().end())
		return it->second;
	return null;
}


void Variant::setInt(int i) {
	lairAssert(_type && _type->fromInt);
	_type->fromInt(_getDataBlock(), i);
}


void* Variant::data() {
	lairAssert(isValid());
	return _getDataBlock();
}


const void* Variant::data() const {
	lairAssert(isValid());
	return _getDataBlock();
}


void Variant::clear() {
	_clear();

	delete _parseInfo;
	_parseInfo = nullptr;
}


void Variant::setParseInfo(const ParseInfo& parseInfo) {
	if(!_parseInfo) {
		_parseInfo = new ParseInfo(parseInfo);
	}
	else {
		*_parseInfo = parseInfo;
	}
}


void Variant::setParseInfo(const String& filename, unsigned line, unsigned col) {
	setParseInfo(ParseInfo{ filename, line, col  });
}


const Variant::ParseInfo* Variant::parseInfo() const {
	return _parseInfo;
}


String Variant::parseInfoDesc() const {
	if(_parseInfo) {
		return cat(_parseInfo->filename, ": ", _parseInfo->line,
		           ": ", _parseInfo->col, ": ");
	}
	return String();
}


bool Variant::useExternalStorage() const {
	return _type && _type->size > sizeof(VariantData);
}


Byte* Variant::_getDataBlock() {
	if(useExternalStorage())
		return _get<Byte*>();
	return reinterpret_cast<Byte*>(&_data);
}

const Byte* Variant::_getDataBlock() const {
	if(useExternalStorage())
		return _get<Byte*>();
	return reinterpret_cast<const Byte*>(&_data);
}


Byte* Variant::_malloc(const MetaType* type) {
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


void Variant::_set(const MetaType* type, const void* obj) {
	Byte* ptr = _malloc(type);

	lairAssert(_type->copyConstruct);
	_type->copyConstruct(ptr, obj);
}


void Variant::_clear() {
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


void Variant::_clearData() {
	std::memset(&_data, 0, sizeof(VariantData));
}




std::ostream& operator<<(std::ostream& out, const Variant& var) {
	if(var.isNull())
		out << "null";
	else if(var.type()->writeRepr)
		var.type()->writeRepr(out, var.data());
	else
		out << "<" << var.type()->identifier << " at " << var.data() << ">";
	return out;
}


std::ostream& operator<<(std::ostream& out, const VarList& list) {
	out << "VarList(";
	bool count = 0;
	for(const Variant& var: list) {
		if(count)
			out << ", ";
		out << var;
		count += 1;
	}
	out << ")";
	return out;
}


std::ostream& operator<<(std::ostream& out, const VarMap& map) {
	out << "VarMap(";
	bool count = 0;
	for(const auto& var: map) {
		if(count)
			out << ", ";
		out << var.first << " = " << var.second;
		count += 1;
	}
	out << ")";
	return out;
}


}

LAIR_IMPLEMENT_METATYPE(lair::VarList, "VarList");
LAIR_IMPLEMENT_METATYPE(lair::VarMap,  "VarMap");
