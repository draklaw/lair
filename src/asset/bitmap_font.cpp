/*
 *  Copyright (C) 2015, 2016 Simon Boyé
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


#include <cctype>

#include <lair/core/text.h>

#include <lair/asset/image.h>

#include "lair/asset/bitmap_font.h"


namespace lair {


TextLayout::TextLayout()
	: 	_box(Vector2(0, 0), Vector2(0, 0)) {
}


void TextLayout::clear() {
	_glyphs.clear();
	_box = Box2(Vector2(0, 0), Vector2(0, 0));
}


void TextLayout::addGlyph(unsigned codepoint, const Vector2& pos) {
	_glyphs.emplace_back(PlacedGlyph{ codepoint, pos });
}


void TextLayout::grow(const Vector2& p) {
	_box.extend(p);
}


//---------------------------------------------------------------------------//


BitmapFont::BitmapFont()
    : _image(),
      _fontSize(0),
      _height(0),
      _baselineToTop(0),
      _glyphMap() {
}


const BitmapFont::Glyph& BitmapFont::glyph(int cp) const {
	auto git = _glyphMap.find(cp);
	if(git != _glyphMap.end()) {
		return git->second;
	}
	return _glyphMap.find(-1)->second;
}


void BitmapFont::setGlyph(int cp, const Glyph& glyph) {
	_glyphMap[cp] = glyph;
}


int BitmapFont::kerning(int cp0, int cp1) const {
	auto it = _kerning.find(CharPair(cp0, cp1));
	if(it == _kerning.end()) {
		return 0;
	}
	return it->second;
}


void BitmapFont::setKerning(int cp0, int cp1, int kern) {
	_kerning[CharPair(cp0, cp1)] = kern;
}


unsigned BitmapFont::textWidth(const std::string& msg) const {
	unsigned w = 0;
	for(char c: msg) {
		auto git = _glyphMap.find(c);
		if(git != _glyphMap.end()) {
			w += git->second.advance;
		}
	}
	return w;
}


TextLayout BitmapFont::layoutText(const std::string& msg, unsigned maxWidth) const {
	TextLayout layout;
	int      x = 0;
	int      y = 0;
	Utf8CodepointIterator cpIt(msg);
	Codepoint cp  = -1;
	while(cpIt.hasNext()) {
		Utf8CodepointIterator cpIt2(cpIt);
		int ww = 0;
		while(cpIt2.hasNext() && !std::isspace(cp = cpIt2.next())) {
			ww += glyph(cp).advance;
		}
		if(x + ww > int(maxWidth)) {
			x = 0;
			y -= _height;
		}
		Codepoint pcp = -1;
		while(cpIt.hasNext() && !std::isspace(cp = cpIt.next())) {
			x += kerning(pcp, cp);
			layout.addGlyph(cp, Vector2(x, y - int(_baselineToTop)));
			x += glyph(cp).advance;
			layout.grow(Vector2(x, y - int(_height)));
			pcp = cp;
		}
		if(std::isspace(cp)) {
			if(cp == '\n') {
				x = 0;
				y -= _height;
				layout.grow(Vector2(x, y - int(_height)));
			}
			else {
				layout.addGlyph(cp, Vector2(x, -y));
				x += glyph(cp).advance;
			}
		}
	}
	return layout;
}


unsigned BitmapFont::wordWidth(const std::string& msg, unsigned i, unsigned* ci) const {
	unsigned w = 0;
	do {
		auto git = _glyphMap.find(msg[i]);
		if(git != _glyphMap.end()) {
			w += git->second.advance;
		}
		++i;
	} while(i < msg.size() && !std::isspace(msg[i]));
	if(ci) *ci = i;
	return w;
}


}
