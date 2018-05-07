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


#ifndef LAIR_META_PROPERTY_SERIALIZER_H
#define LAIR_META_PROPERTY_SERIALIZER_H


#include <lair/core/lair.h>

#include <lair/meta/metatype.h>
#include <lair/meta/variant.h>
#include <lair/meta/variant_reader.h>
#include <lair/meta/variant_writer.h>
#include <lair/meta/property.h>
#include <lair/meta/property_list.h>


namespace lair
{


class PropertySerializer {
private:
	typedef std::function<bool(void*, const Property&, const Variant&, Logger&)> ReadFn;
	typedef std::function<bool(Variant&, const void*, const Property&, Logger&)> WriteFn;

public:
	PropertySerializer();
	PropertySerializer(const PropertySerializer&)  = delete;
	PropertySerializer(      PropertySerializer&&) = delete;
	~PropertySerializer() = default;

	PropertySerializer& operator=(const PropertySerializer&)  = delete;
	PropertySerializer& operator=(      PropertySerializer&&) = delete;

	template<typename T>
	inline void registerType() {
		typedef bool (*Read )(T&,       const Variant&, Logger&);
		typedef bool (*Write)(Variant&, const T&      , Logger&);
//		bool (*read )(T&,       const Variant&, Logger&) = &varRead;
//		bool (*write)(Variant&, const T&      , Logger&) = &varWrite;
		Read  read  = &varRead;
		Write write = &varWrite;
		_registerType(metaTypes.get<T>(),
		              ReadFn (ReadWrapper <T, Read >(read)),
		              WriteFn(WriteWrapper<T, Write>(write)));
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
	inline bool read(T& obj, const Variant& var, Logger& logger) {
		return _read(&obj, obj.properties(), var, logger);
	}

	template<typename T>
	inline bool write(Variant& var, const T& obj, Logger& logger) {
		return _write(var, &obj, obj.properties(), logger);
	}

	bool _read(void* obj, const PropertyList& properties, const Variant& var, Logger& logger);
	bool _write(Variant& var, const void* obj, const PropertyList& properties, Logger& logger);

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

		bool operator()(void* obj, const Property& property, const Variant& var, Logger& logger) const {
			if(var.type() == metaTypes.get<T>()) {
				// If the variant contains the exact same type than the property
				// just set it without conversion.
				property.setVar(obj, var);
			}
			else {
				T tmp;
				if(!_read(tmp, var, logger)) {
					return false;
				}
				property.set<T>(obj, tmp);
			}
			return true;
		}

		ReadFn _read;
	};

	template<typename T, typename WriteFn>
	struct WriteWrapper {
		WriteWrapper(const WriteFn& write): _write(write) {}

		bool operator()(Variant& var, const void* obj, const Property& property, Logger& logger) const {
			return _write(var, property.get<T>(obj), logger);
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
