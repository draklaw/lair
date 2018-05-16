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


#ifndef LAIR_META_LDL_PROPERTY_SERIALIZER_H
#define LAIR_META_LDL_PROPERTY_SERIALIZER_H


#include <lair/core/lair.h>

#include <lair/ldl/ldl.h>

#include <lair/meta/metatype.h>
#include <lair/meta/property.h>
#include <lair/meta/property_list.h>


namespace lair
{


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
