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


#ifndef _LAIR_CORE_LAIR_H
#define _LAIR_CORE_LAIR_H

#define _USE_MATH_DEFINES

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <sstream>

// FIXME: Enable vectorization
#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Geometry>

#include <json/json-forwards.h>


namespace lair {


class AssertionFailedError : public std::logic_error {
public:
	AssertionFailedError(const char* testCode, const char* file, int line);

	static std::string _formatWhat(const char* testCode, const char* file, int line);
};


extern bool throwOnAssert;
void _assertCrash(const char* testCode, const char* file, int line);


inline void _lairAssert(bool result, const char* testCode, const char* file, int line) {
	if(!result) {
		AssertionFailedError error(testCode, file, line);
		if(throwOnAssert) {
			throw error;
		}
		else {
			_assertCrash(testCode, file, line);
		}
	}
}

#define lairAssert(_test) lair::_lairAssert(bool(_test), #_test, __FILE__, __LINE__)


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

typedef Eigen::Matrix<int, 2, 1>         Vector2i;
typedef Eigen::Matrix<int, 3, 1>         Vector3i;
typedef Eigen::Matrix<int, 4, 1>         Vector4i;

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

typedef std::string String;
class Path;

class ErrorOutput;
class ErrorList;

/// Functions to write a representation of `v` to `out`.
inline void writeRepr(std::ostream& out,  int8  v) { out << int(v) << "i8"; }
inline void writeRepr(std::ostream& out,  int16 v) { out << v << "i16"; }
inline void writeRepr(std::ostream& out,  int32 v) { out << v << "i32"; }
inline void writeRepr(std::ostream& out,  int64 v) { out << v << "i64"; }
inline void writeRepr(std::ostream& out, uint8  v) { out << int(v) << "u8"; }
inline void writeRepr(std::ostream& out, uint16 v) { out << v << "u16"; }
inline void writeRepr(std::ostream& out, uint32 v) { out << v << "u32"; }
inline void writeRepr(std::ostream& out, uint64 v) { out << v << "u64"; }
inline void writeRepr(std::ostream& out, float         v) { out << v << "f"; }
inline void writeRepr(std::ostream& out, double        v) { out << v << "d"; }
inline void writeRepr(std::ostream& out, const String& v) { out << '"' << v << '"'; }

/// Return a textual representation of `v`.
template<typename T>
std::string repr(const T& v) {
	std::ostringstream out;
	writeRepr(out, v);
	return out.str();
}


/// Make a unique_ptr that will be destroyed with the deleter D.
/// Useful when using C APIs that provide "destructor" functions.
template < typename T, typename D >
inline std::unique_ptr<T, D> make_unique(T* ptr, D deleter) {
	return std::unique_ptr<T, D>(ptr, deleter);
}

template < typename T >
inline std::unique_ptr<T> make_unique(T* ptr) {
	return std::unique_ptr<T>(ptr);
}


template < typename S, typename T >
inline T lerp(S x, const T& v0, const T& v1) {
	return (1-x) * v0 + x * v1;
}

template < typename T >
inline T clamp(const T& value, const T& min, const T& max) {
	return (value < min)? min:
	       (value > max)? max:
	                      value;
}

template < typename T >
inline T normalize(T f, T fmin, T fmax) {
	return (f - fmin) / (fmax - fmin);
}

template < typename I, typename F >
inline I intFromFloat(F f, F fmin, F fmax, I imin, I imax) {
	return clamp(I(normalize(f, fmin, fmax) * (imax - imin)) + imin, imin, imax);
}

template < typename T, typename B >
inline bool bitsEnabled(T flags, B bits) {
	return (flags & T(bits)) == T(bits);
}

template < typename T, typename B >
inline T setBits(T flags, B bits, bool enable) {
	if(enable)
		return flags | T(bits);
	return flags & ~T(bits);
}


inline float srgbFromLinear(float coeff) {
	return (coeff <= 0.0031308f)? 12.92f * coeff:
	                              1.055f * pow(coeff, 1.0f/2.4f) - 0.055f;
}

inline float linearFromSrgb(float coeff) {
	return (coeff <= 0.04045f)? coeff / 12.92:
	                            pow((coeff + 0.055f) / 1.055f, 2.4f);
}

template<typename Derived>
inline Derived srgbFromLinear(const Eigen::DenseBase<Derived>& color) {
	typedef typename Derived::Index Index;
	Derived v = color;
	Index rows = std::min(Index(3), color.rows());
	Index cols = color.cols();
	for(Index c = 0; c < cols; ++c) {
		for(Index r = 0; r < rows; ++r) {
			v(r, c) = srgbFromLinear(v(r, c));
		}
	}
	return v;
}

template<typename Derived>
inline Derived linearFromSrgb(const Eigen::DenseBase<Derived>& color) {
	typedef typename Derived::Index Index;
	Derived v = color;
	Index rows = std::min(Index(3), color.rows());
	Index cols = color.cols();
	for(Index c = 0; c < cols; ++c) {
		for(Index r = 0; r < rows; ++r) {
			v(r, c) = linearFromSrgb(v(r, c));
		}
	}
	return v;
}

inline Vector3 srgb(float r, float g, float b) {
	return Vector3(linearFromSrgb(r), linearFromSrgb(g), linearFromSrgb(b));
}

inline Vector4 srgba(float r, float g, float b, float a = 1.0f) {
	return Vector4(linearFromSrgb(r), linearFromSrgb(g), linearFromSrgb(b), a);
}

template<typename Derived>
Eigen::WithFormat<Derived> fmt(const Eigen::DenseBase<Derived>& m) {
	static Eigen::IOFormat vFormat(Eigen::StreamPrecision,
	                               Eigen::DontAlignCols,
	                               ", ", ", ", "", "", "[", "]");
	static Eigen::IOFormat mFormat(Eigen::StreamPrecision);
	if(m.cols() == 1 || m.rows() == 1)
		return m.format(vFormat);
	return m.format(mFormat);
}


template<typename It>
class IteratorRange {
public:
	inline IteratorRange(It begin, It end)
	    : _begin(begin), _end(end) {}

	inline It begin() const {
		return _begin;
	}

	inline It end() const {
		return _end;
	}

private:
	It _begin;
	It _end;
};


}

#endif
