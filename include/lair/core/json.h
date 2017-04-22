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


#ifndef _LAIR_CORE_JSON_H
#define _LAIR_CORE_JSON_H


#include <istream>

#include <json/json.h>

#include <lair/core/lair.h>
#include <lair/core/parse.h>
#include <lair/core/log.h>
#include <lair/core/path.h>


namespace lair {


bool parseJson(Json::Value& value, std::istream& in, const Path& localPath, Logger& log);
bool parseJson(Json::Value& value, const Path& realPath, const Path& localPath, Logger& log);


Matrix4 parseMatrix4(const Json::Value& json, bool* ok = nullptr);


struct JsonError {
	enum Type {
		ERROR,
		WARNING,
		SUCCESS,
	};

	Type        type;
	std::string message;

	static JsonError error  (const std::string& message) { return JsonError{ ERROR,   message       }; }
	static JsonError warning(const std::string& message) { return JsonError{ WARNING, message       }; }
	static JsonError success()                           { return JsonError{ SUCCESS, std::string() }; }
};

inline std::string jsonTypeName(const Json::Value& json) {
	switch(json.type()) {
	case Json::nullValue:    return "null";
	case Json::intValue:     return "int";
	case Json::uintValue:    return "uint";
	case Json::realValue:    return "real";
	case Json::stringValue:  return "string";
	case Json::booleanValue: return "boolean";
	case Json::arrayValue:   return "array";
	case Json::objectValue:  return "object";
	}
}

template < typename T >
struct JsonTrait {
	static bool get(T& value, const Json::Value& json, ErrorList* errors);
	static bool set(Json::Value& json, const T& value, ErrorList* errors);
};

template < typename T >
inline bool fromJson(T& value, const Json::Value& json, ErrorList* errors = 0) {
	return JsonTrait<T>::get(value, json, errors);
}

template < typename T >
inline T fromJson(const Json::Value& json, ErrorList* errors = 0) {
	T value;
	JsonTrait<T>::get(value, json, errors);
	return value;
}

template < typename T >
inline bool toJson(Json::Value& json, const T& value, ErrorList* errors = 0) {
	return JsonTrait<T>::set(json, value, errors);
}

template < typename T >
inline Json::Value toJson(const T& value, ErrorList* errors = 0) {
	Json::Value json;
	JsonTrait<T>::set(json, value, errors);
	return json;
}


template<>
struct JsonTrait<bool> {
	static bool get(bool& value, const Json::Value& json, ErrorList* errors) {
		if(json.isBool() || json.isNumeric()) {
			value = json.asBool();
			return true;
		}
		if(errors) {
			errors->error("Invalid type while parsing Bool: ", jsonTypeName(json));
		}
		return false;
	}

	static bool set(Json::Value& json, const bool& value, ErrorList*) {
		json = value;
		return true;
	}
};

#define LAIR_DECL_JSON_INT(_type, _get, _set) \
template <> \
struct JsonTrait<_type> { \
	static bool get(_type& value, const Json::Value& json, ErrorList* errors) { \
		if(json.isNumeric()) { \
			value = json._get(); \
		} \
		else { \
			if(errors) {\
				errors->error("Invalid type while parsing " #_type ": ", jsonTypeName(json)); \
			} \
			return false; \
		} \
		if(errors && value != json._get()) { \
			if(json.isDouble()) { \
				errors->warning("Double rounded to " #_type); \
			} \
			else { \
				errors->warning("Integer out of bound (" #_type ")"); \
			} \
		} \
		return true; \
	} \
 \
	static bool set(Json::Value& json, const _type& value, ErrorList*) { \
		json = Json::_set(value); \
		return true; \
	} \
}

LAIR_DECL_JSON_INT(int8,   asInt64,  Int64);
LAIR_DECL_JSON_INT(int16,  asInt64,  Int64);
LAIR_DECL_JSON_INT(int32,  asInt64,  Int64);
LAIR_DECL_JSON_INT(int64,  asInt64,  Int64);
LAIR_DECL_JSON_INT(uint8,  asUInt64, UInt64);
LAIR_DECL_JSON_INT(uint16, asUInt64, UInt64);
LAIR_DECL_JSON_INT(uint32, asUInt64, UInt64);
LAIR_DECL_JSON_INT(uint64, asUInt64, UInt64);

#undef LAIR_DECL_JSON_INT


#define LAIR_DECL_JSON_FLOAT(_type, _get) \
template <> \
struct JsonTrait<_type> { \
	static bool get(_type& value, const Json::Value& json, ErrorList* errors) { \
		if(json.isNumeric()) { \
			value = json._get(); \
		} \
		else { \
			if(errors) { \
				errors->error("Invalid type while parsing " #_type ": ", jsonTypeName(json)); \
			} \
			return false; \
		} \
		if(errors && json.isIntegral() && int64(value) != json.asInt64()) { \
			errors->warning("Integer rounded to " #_type); \
		} \
		return true; \
	} \
 \
	static bool set(Json::Value& json, const _type& value, ErrorList*) { \
		json = value; \
		return true; \
	} \
}

LAIR_DECL_JSON_FLOAT(float,  asFloat);
LAIR_DECL_JSON_FLOAT(double, asDouble);

#undef LAIR_DECL_JSON_FLOAT


template < typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols >
struct JsonTrait<Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>> {
	typedef Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols> Matrix;

	static bool get(Matrix& value, const Json::Value& json, ErrorList* errors) {
		assert(value.rows() > 0 && value.cols() > 0);
		if(value.rows() == 1 || value.cols() == 1) {
			int dim = (value.rows() == 1)? value.cols(): value.rows();
			if(!json.isArray()) {
				if(errors) {
					errors->error("Invalid type while parsing a vector of dimension ",
					              dim, ": ", jsonTypeName(json));
				}
				return false;
			}
			int size = std::min<int>(dim, json.size());
			int i = 0;
			for( ; i < size; ++i) {
				if(!fromJson(value(i), json[i], errors)) {
					return false;
				}
			}
			for( ; i < dim; ++i) {
				value(i) = (i == 3)? Scalar(1): Scalar(0);
			}
			if(errors && dim < json.size()) {
				errors->warning("To much coefficient in vector: expected ",
				                dim, ", got ", json.size());
			}
		}
		else {
			int rows = value.rows();
			int cols = value.cols();
			if(!json.isArray()) {
				if(errors) {
					errors->error("Invalid type while parsing a ", rows, "x", cols,
					              " matrix: ", jsonTypeName(json));
				}
				return false;
			}

			int nc = std::min<int>(cols, json.size());
			int c  = 0;
			for( ; c < nc; ++c) {
				const Json::Value& col = json[c];
				if(!col.isArray()) {
					if(errors) {
						errors->error("Invalid type while parsing a column: expected an array, got ", jsonTypeName(col));
					}
					return false;
				}
				int nr = std::min<int>(rows, col.size());
				int r  = 0;
				for( ; r < nr; ++r) {
					if(!fromJson(value(r, c), col[r], errors)) {
						return false;
					}
				}
				for( ; r < rows; ++r)
					value(r, c) = (r == c)? Scalar(1): Scalar(0);
			}
			for( ; c < cols; ++c) {
				for(int r = 0; r < rows; ++r)
					value(r, c) = (r == c)? Scalar(1): Scalar(0);
			}
		}

		return true;
	}

	static bool set(Json::Value& json, const Matrix& value, ErrorList* errors) {
		assert(value.rows() > 0 && value.cols() > 0);
		json = Json::Value(Json::arrayValue);
		if(value.rows() == 1 || value.cols() == 1) {
			int dim = std::max(value.cols(), value.rows());
			for(int i = 0; i < dim; ++i) {
				json.append(toJson(value(i), errors));
			}
		}
		else {
			int rows = value.rows();
			int cols = value.cols();
			for(int c = 0; c < cols; ++c) {
				Json::Value col(Json::arrayValue);
				json.append(col);
				for(int r = 0; r < rows; ++r) {
					col.append(toJson(value(r, c), errors));
				}
			}
		}
		return true;
	}
};


template <>
struct JsonTrait<std::string> {
	static bool get(std::string& value, const Json::Value& json, ErrorList* errors) {
		if(json.isString()) {
			value = json.asString();
		}
		else {
			if(errors) {
				errors->error("Invalid type while parsing string: ", jsonTypeName(json));
			}
			return false;
		}
		return true;
	}

	static bool set(Json::Value& json, const std::string& value, ErrorList*) {
		json = value;
		return true;
	}
};

template <>
struct JsonTrait<Path> {
	static bool get(Path& value, const Json::Value& json, ErrorList* errors) {
		if(json.isString()) {
			value = Path(json.asString());
		}
		else {
			if(errors) {
				errors->error("Invalid type while parsing path: ", jsonTypeName(json));
			}
			return false;
		}
		return true;
	}

	static bool set(Json::Value& json, const Path& value, ErrorList*) {
		json = value.utf8String();
		return true;
	}
};


template < typename Scalar, int Dim >
struct JsonTrait<Eigen::AlignedBox<Scalar, Dim>> {
	static bool get(Eigen::AlignedBox<Scalar, Dim>& value, const Json::Value& json, ErrorList* errors) {
		if(!json.isArray()) {
			if(errors) {
				errors->error("Invalid type while parsing a box: expected array, got ", jsonTypeName(json));
			}
			return false;
		}
		if(json.size() != 2) {
			if(errors) {
				errors->error("Invalid type while parsing a box: expected array of dimension 2, "
				              "got array of dimension ", json.size());
			}
			return false;
		}
		return fromJson(value.min(), json[0], errors)
		    && fromJson(value.max(), json[1], errors);
	}

	static void set(Json::Value& json, const Eigen::AlignedBox<Scalar, Dim>& value) {
		json = Json::Value(Json::arrayValue);
		json.append(toJson(value.min()));
		json.append(toJson(value.max()));
	}
};


}

#endif
