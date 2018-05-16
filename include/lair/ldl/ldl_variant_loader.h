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


#ifndef LAIR_LDL_LDL_VARIANT_LOADER_H
#define LAIR_LDL_LDL_VARIANT_LOADER_H


#include <lair/core/lair.h>

#include <lair/meta/variant.h>

#include <lair/asset/loader.h>


namespace lair
{


typedef GenericAspect  <Variant>       VariantAspect;
typedef std::shared_ptr<VariantAspect> VariantAspectSP;
typedef std::weak_ptr  <VariantAspect> VariantAspectWP;


class LdlVariantLoader : public Loader {
public:
	typedef VariantAspect Aspect;

public:
	LdlVariantLoader(LoaderManager* manager, AspectSP aspect);
	LdlVariantLoader(const LdlVariantLoader&) = delete;
	LdlVariantLoader(LdlVariantLoader&&)      = delete;
	virtual ~LdlVariantLoader() = default;

	LdlVariantLoader& operator=(const LdlVariantLoader&) = delete;
	LdlVariantLoader& operator=(LdlVariantLoader&&)      = delete;

	virtual void commit();

protected:
	virtual void loadSyncImpl(Logger& log);

protected:
	Variant _variant;
};


}


#endif
