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
#include <lair/core/ldl.h>

#include "lair/meta/variant_loader.h"


namespace lair
{


bool parseLdl(Variant& value, std::istream& in, const Path& localPath, Logger& log) {
	ErrorList errors;
	LdlParser parser(&in, localPath.utf8String(), &errors, LdlParser::CTX_MAP);
	bool success = ldlRead(parser, value);
	errors.log(log);
	return success;
}


bool parseLdl(Variant& value, const Path& realPath, const Path& localPath, Logger& log) {
	Path::IStream in(realPath.native().c_str());
	if(!in.good()) {
		log.error("Unable to read \"", localPath, "\" (", realPath, ").");
		return false;
	}
	return parseLdl(value, in, localPath, log);
}


bool parseLdl(Variant& value, const VirtualFile& file, const Path& localPath, Logger& log) {
	if(!file) {
		log.error("Invalid file: \"", localPath, "\".");
		return false;
	}

	Path realPath = file.realPath();
	if(!realPath.empty()) {
		return parseLdl(value, realPath, localPath, log);
	}

	const MemFile* memFile = file.fileBuffer();
	if(memFile) {
		String buffer((const char*)memFile->data, memFile->size);
		std::istringstream in(buffer);
		return parseLdl(value, in, localPath, log);
	}

	lairAssert(false);
	return false;
}



VariantLoader::VariantLoader(LoaderManager* manager, AspectSP aspect)
    : Loader(manager, aspect) {
}


void VariantLoader::commit() {
	VariantAspectSP aspect = std::static_pointer_cast<VariantAspect>(_aspect);
	aspect->_get() = std::move(_variant);
	Loader::commit();
}


void VariantLoader::loadSyncImpl(Logger& log) {
	parseLdl(_variant, file(), asset()->logicPath(), log);
}


}
