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


#ifndef _LAIR_CORE_LAIR_H
#define _LAIR_CORE_LAIR_H


#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <sstream>

#include <Eigen/Geometry>

#include <json/json-forwards.h>


namespace lair {


class AssertionFailedError : public std::logic_error {
public:
	inline AssertionFailedError(const char* testCode, const char* file, int line)
	    : std::logic_error(_formatWhat(testCode, file, line)) {
	}

private:
	inline static std::string _formatWhat(
	        const char* testCode, const char* file, int line) {
		std::ostringstream out;
		out << file << ":" << line << ": Assertion failed: '" << testCode << "'.\n";
		return out.str();
	}
};


inline void _lairAssert(bool result, const char* testCode, const char* file, int line) {
	if(!result) {
		throw AssertionFailedError(testCode, file, line);
	}
}

#define lairAssert(_test) lair::_lairAssert(_test, #_test, __FILE__, __LINE__)


typedef std::size_t    Size;
typedef std::ptrdiff_t PtrDiff;

typedef std::uint8_t   Byte;
typedef std::uint8_t   uint8;
typedef std:: int8_t    int8;
typedef std::uint16_t  uint16;
typedef std:: int16_t   int16;
typedef std::uint32_t  uint32;
typedef std:: int32_t   int32;
typedef std::uint64_t  uint64;
typedef std:: int64_t   int64;
typedef std::uintptr_t uintptr;

typedef float                            Scalar;

typedef Eigen::Matrix<Scalar, 2, 1>      Vector2;
typedef Eigen::Matrix<Scalar, 3, 1>      Vector3;
typedef Eigen::Matrix<Scalar, 4, 1>      Vector4;

typedef Eigen::Matrix<Scalar, 2, 2>      Matrix2;
typedef Eigen::Matrix<Scalar, 3, 3>      Matrix3;
typedef Eigen::Matrix<Scalar, 4, 4>      Matrix4;

typedef Eigen::Transform<Scalar, 3,
                         Eigen::Affine>  Transform;
typedef Eigen::Quaternion<Scalar>        Quaternion;
typedef Eigen::AngleAxis<Scalar>         AngleAxis;
typedef Eigen::Translation<Scalar, 3>    Translation;

typedef Eigen::AlignedBox2f Box2;
typedef Eigen::AlignedBox3f Box3;
typedef Eigen::AlignedBox2i Box2i;
typedef Eigen::AlignedBox3i Box3i;

typedef unsigned ScanCode;

class Path;


// Make a unique_ptr that will be destroyed with the deleter D.
// Useful when using C APIs that provide "destructor" functions.
template < typename T, typename D >
inline std::unique_ptr<T, D> make_unique(T* ptr, D deleter) {
	return std::unique_ptr<T, D>(ptr, deleter);
}


template < typename S, typename T >
T lerp(S x, const T& v0, const T& v1) {
	return (1-x) * v0 + x * v1;
}


}

#endif
