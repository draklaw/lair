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


#ifndef LAIR_META_FLAGS_INFO_H
#define LAIR_META_FLAGS_INFO_H


#include <vector>
#include <unordered_map>

#include <lair/core/lair.h>
#include <lair/core/log.h>


namespace lair
{


class FlagsInfo {
public:
	struct FlagSet {
		unsigned flags;
		unsigned mask;
		String   label;
	};

public:
	FlagsInfo(const String& name);
	FlagsInfo(const FlagsInfo& ) = delete;
	FlagsInfo(      FlagsInfo&&) = delete;
	~FlagsInfo() = default;

	FlagsInfo& operator=(const FlagsInfo& ) = delete;
	FlagsInfo& operator=(      FlagsInfo&&) = delete;

	FlagsInfo& add(unsigned flag, const String& label);
	FlagsInfo& add(unsigned flags, unsigned mask, const String& label);

	const String& name() const;
	unsigned nFlagSet() const;
	const FlagSet* flagSet(const String& label) const;

	unsigned parse(std::istream& in, ErrorOutput* errors = 0) const;
	unsigned parse(const String& in, ErrorOutput* errors = 0) const;
	unsigned parse(std::istream& in, Logger& logger = noopLogger) const;
	unsigned parse(const String& in, Logger& logger = noopLogger) const;
	void write(std::ostream& out, unsigned flags) const;
	String format(unsigned flags) const;

private:
	typedef std::unique_ptr<FlagSet>                     FlagSetPtr;
	typedef std::vector<FlagSetPtr>                      FlagSetVector;
	typedef std::unordered_map<unsigned, const FlagSet*> FlagsFlagSetMap;
	typedef std::unordered_map<String, const FlagSet*>   LabelFlagSetMap;

private:
	String          _name;
	FlagSetVector   _flagSets;
	LabelFlagSetMap _flagSetFromLabel;
};


}


#endif
