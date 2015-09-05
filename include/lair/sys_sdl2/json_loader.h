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


#ifndef _LAIR_SYS_SDL2_JSON_LOADER_H
#define _LAIR_SYS_SDL2_JSON_LOADER_H


#include <lair/core/lair.h>
#include <lair/core/json.h>
#include <lair/core/path.h>

#include <lair/utils/loader.h>


namespace lair
{


class JsonLoader : public Loader {
public:
	JsonLoader(LoaderManager* manager, const Path& path);
	JsonLoader(const JsonLoader&) = delete;
	JsonLoader(JsonLoader&&)      = delete;
	~JsonLoader();

	JsonLoader& operator=(const JsonLoader&) = delete;
	JsonLoader& operator=(JsonLoader&&)      = delete;

	const Json::Value& getValue() const { return _value; }

protected:
	virtual void loadSyncImpl(Logger& log);

protected:
	Json::Value _value;
};


}


#endif
