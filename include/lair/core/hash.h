/*
 *  Copyright (C) 2015-2017 Simon Boyé
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


#ifndef _LAIR_CORE_HASH_H
#define _LAIR_CORE_HASH_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <system_error>
#include <bitset>
#include <memory>
#include <typeindex>
#include <vector>
#include <thread>


namespace lair {


#if SIZE_MAX == 18446744073709551615u
    // 64 bits std::size_t
    #define LAIR_HASH_COMBINE_CONSTANT 0x9E3779B97F4A7C15u
#else
    // Assume 32 bits std::size_t
    #define LAIR_HASH_COMBINE_CONSTANT 0x9e3779b9u
#endif

/**
 * @brief Combine two hash values in an order-dependent way.
 */
inline std::size_t combineHash(std::size_t h0, std::size_t h1) {
	return h1 + LAIR_HASH_COMBINE_CONSTANT + (h0 << 6) + (h0 >> 2);
}



#define LAIR_DECL_STD_HASH(_type) inline std::size_t hash(const _type& v) { return std::hash<_type>()(v); }

LAIR_DECL_STD_HASH(bool)
LAIR_DECL_STD_HASH(char)
LAIR_DECL_STD_HASH(signed char)
LAIR_DECL_STD_HASH(unsigned char)
LAIR_DECL_STD_HASH(char16_t)
LAIR_DECL_STD_HASH(char32_t)
LAIR_DECL_STD_HASH(wchar_t)
LAIR_DECL_STD_HASH(short)
LAIR_DECL_STD_HASH(unsigned short)
LAIR_DECL_STD_HASH(int)
LAIR_DECL_STD_HASH(unsigned int)
LAIR_DECL_STD_HASH(long)
LAIR_DECL_STD_HASH(unsigned long)
LAIR_DECL_STD_HASH(long long)
LAIR_DECL_STD_HASH(unsigned long long)
LAIR_DECL_STD_HASH(float)
LAIR_DECL_STD_HASH(double)
LAIR_DECL_STD_HASH(long double)

LAIR_DECL_STD_HASH(std::string)
LAIR_DECL_STD_HASH(std::u16string)
LAIR_DECL_STD_HASH(std::u32string)
LAIR_DECL_STD_HASH(std::wstring)
LAIR_DECL_STD_HASH(std::error_code)
LAIR_DECL_STD_HASH(std::type_index)
LAIR_DECL_STD_HASH(std::vector<bool>)
LAIR_DECL_STD_HASH(std::thread::id)


#undef LAIR_DECL_STD_HASH

template<typename T>
inline std::size_t hash(const T*& v) {
	return std::hash<T*>()(v);
}

template<typename T>
inline std::size_t hash(const std::unique_ptr<T>& v) {
	return std::hash<std::unique_ptr<T>>()(v);
}

template<typename T>
inline std::size_t hash(const std::shared_ptr<T>& v) {
	return std::hash<std::shared_ptr<T>>()(v);
}

template<std::size_t N>
inline std::size_t hash(const std::bitset<N>& v) {
	return std::hash<std::bitset<N>>()(v);
}

template<typename T0, typename T1>
inline std::size_t hash(const std::pair<T0, T1>& v) {
	return combineHash(hash(v.first), hash(v.second));
}


template<typename T>
struct Hash {
	inline std::size_t operator()(const T& v) const {
		return hash(v);
	}
};


}

#endif
