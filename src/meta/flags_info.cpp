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
#include "lair/core/parse.h"

#include "lair/meta/flags_info.h"


namespace lair
{


FlagsInfo::FlagsInfo(const String& name)
    : _name(name)
{}

FlagsInfo& FlagsInfo::add(unsigned flag, const String& label) {
	return add(flag, flag, label);
}

FlagsInfo& FlagsInfo::add(unsigned flags, unsigned mask, const String& label) {
	lairAssert(_flagSetFromLabel.count(label) == 0);

	_flagSets.emplace_back(new FlagSet{ flags, mask, label });

	const FlagSet* flagSet = _flagSets.back().get();
	_flagSetFromLabel.emplace(label, flagSet);

	return *this;
}

const String& FlagsInfo::name() const {
	return _name;
}

unsigned FlagsInfo::nFlagSet() const {
	return _flagSets.size();
}

const FlagsInfo::FlagSet* FlagsInfo::flagSet(const String& label) const {
	auto it = _flagSetFromLabel.find(label);
	return (it == _flagSetFromLabel.end())? nullptr: it->second;
}

unsigned FlagsInfo::parse(std::istream& in, ErrorOutput* errors) const {
	unsigned flags = 0;
	unsigned mask  = 0;

	eatWhitespaces(in);

	if(std::isdigit(in.peek())) {
		char* end = 0;
		String tok;
		in >> tok;
		unsigned flags = std::strtoul(tok.data(), &end, 0);
		if(end == tok.data() + tok.size()) {
			eatWhitespaces(in);
			if(errors && !in.eof()) {
				errors->warning("Extra data after numerical flags are being ignored.");
			}
			return flags;
		}
	}

	String tok;
	while(parseIdentifier(tok, in, "-")) {
		const FlagSet* flagSet = this->flagSet(tok);
		if(flagSet) {
			if(errors && (mask & flagSet->mask) != 0) {
				errors->warning("Flagset \"", flagSet->label, "\" overlap previous flagsets.");
			}

			flags |= flagSet->flags;
			mask  |= flagSet->mask;
		}
		else if(errors) {
			errors->warning("Unknown label \"", tok, "\".");
		}

		eatWhitespaces(in);

		if(in.eof()) {
			break;
		}

		if(isNextIn(in, ",|")) {
			in.get();
			eatWhitespaces(in);
		}
	}
	return flags;
}

unsigned FlagsInfo::parse(const String& in, ErrorOutput* errors) const {
	std::istringstream inStream(in);
	return parse(inStream, errors);
}

void FlagsInfo::write(std::ostream& out, unsigned flags) const {
	out << format(flags);
}

String FlagsInfo::format(unsigned flags) const {
	std::ostringstream out;

	unsigned mask = 0;
	for(const FlagSetPtr& flagSet: _flagSets) {
		if((mask  & flagSet->mask) == 0
		&& (flags & flagSet->mask) == flagSet->flags) {
			if(mask != 0) {
				out << " | ";
			}
			out << flagSet->label;
			mask  |= flagSet->mask;
		}
	}

	if(flags & ~mask) {
		// Some bits are not covered, fall back to hexadecimal output.
		char buff[19];
		std::snprintf(buff, sizeof(buff), "0x%08x", flags);
		return String(buff);
	}

	return out.str();
}


}
