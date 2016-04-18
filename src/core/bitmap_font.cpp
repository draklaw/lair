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

#include <lair/core/json.h>
#include <lair/core/image.h>

#include <lair/render_gl2/renderer.h>

#include "lair/core/bitmap_font.h"


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

BitmapFont::BitmapFont(const Json::Value& json, AssetSP image)
    : _image(image),
      _fontSize(),
      _height(),
      _baselineToTop(),
      _glyphMap() {
	setFromJson(json);
}


const BitmapFont::Glyph& BitmapFont::glyph(int cp) const {
	auto git = _glyphMap.find(cp);
	if(git != _glyphMap.end()) {
		return git->second;
	}
	return _glyphMap.find(-1)->second;
}


int BitmapFont::kerning(int cp0, int cp1) const {
	auto it = _kerning.find(CharPair(cp0, cp1));
	if(it == _kerning.end()) {
		return 0;
	}
	return it->second;
}


void BitmapFont::setFromJson(const Json::Value& json) {
	auto aspect = _image->aspect<ImageAspect>();
	lairAssert(bool(aspect));

	_fontSize = json.get("size", 0).asInt();
	_height = json.get("height", 0).asInt();
	_baselineToTop = json.get("base", _height / 2).asInt();

	Vector2 texSize(aspect->get()->width(), aspect->get()->height());
	for(const Json::Value& c: json.get("chars", Json::nullValue)) {
		unsigned cp = c[0].asInt();
		Glyph g;
		Vector2 pos = Vector2(c[1].asInt(), c[2].asInt()).array()
		        / texSize.array();
		g.size = Vector2(c[3].asInt(), c[4].asInt());
		g.region = Box2(pos, pos + (g.size.array()
		                            / texSize.array()).matrix());
		g.offset = Vector2(c[5].asInt(), c[6].asInt());
		g.advance = c[7].asInt();
		_glyphMap.emplace(cp, g);
	}
	for(const Json::Value& k: json.get("kern", Json::nullValue)) {
		unsigned cp0  = k[0].asInt();
		unsigned cp1  = k[1].asInt();
		unsigned kern = k[2].asInt();
		_kerning.emplace(CharPair(cp0, cp1), kern);
	}
}

void BitmapFont::setImage(AssetSP image) {
	_image = image;
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
	// TODO: Use local to support UTF-8
	TextLayout layout;
	int x = 0;
	int y = 0;
	unsigned i = 0;
	while(i < msg.size()) {
		int wstart = i;
		int ww = 0;
		while(i < msg.size() && !std::isspace(msg[i])) {
			ww += glyph(msg[i]).advance;
			++i;
		}
		if(x + ww > int(maxWidth)) {
			x = 0;
			y -= _height;
		}
		i = wstart;
		while(i < msg.size() && !std::isspace(msg[i])) {
			x += (i? kerning(msg[i-1], msg[i]): 0);
			layout.addGlyph(msg[i], Vector2(x, y - int(_baselineToTop)));
			x += glyph(msg[i]).advance;
			layout.grow(Vector2(x, y - int(_height)));
			++i;
		}
		if(i < msg.size()) {
			if(msg[i] == '\n') {
				x = 0;
				y -= _height;
			}
			else {
				layout.addGlyph(msg[i], Vector2(x, -y));
				x += glyph(msg[i]).advance;
			}
		}
		++i;
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
