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


#ifndef LAIR_META_VARIANT_LOADER_H
#define LAIR_META_VARIANT_LOADER_H


#include <lair/core/lair.h>
#include <lair/core/loader.h>

#include <lair/meta/variant.h>


namespace lair
{


bool parseLdl(Variant& value, std::istream& in, const Path& localPath, Logger& log);
bool parseLdl(Variant& value, const Path& realPath, const Path& localPath, Logger& log);
bool parseLdl(Variant& value, const VirtualFile& file, const Path& localPath, Logger& log);


typedef GenericAspect  <Variant>       VariantAspect;
typedef std::shared_ptr<VariantAspect> VariantAspectSP;
typedef std::weak_ptr  <VariantAspect> VariantAspectWP;


class VariantLoader : public Loader {
public:
	typedef VariantAspect Aspect;

public:
	VariantLoader(LoaderManager* manager, AspectSP aspect);
	VariantLoader(const VariantLoader&) = delete;
	VariantLoader(VariantLoader&&)      = delete;
	virtual ~VariantLoader() = default;

	VariantLoader& operator=(const VariantLoader&) = delete;
	VariantLoader& operator=(VariantLoader&&)      = delete;

	virtual void commit();

protected:
	virtual void loadSyncImpl(Logger& log);

protected:
	Variant _variant;
};


}


#endif
