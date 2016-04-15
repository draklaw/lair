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


#ifndef _LAIR_CORE_BITMAP_FONT_H
#define _LAIR_CORE_BITMAP_FONT_H


#include <unordered_map>

#include <boost/functional/hash.hpp>

#include <lair/core/lair.h>
#include <lair/core/log.h>
#include <lair/core/asset_manager.h>


namespace lair {


class TextLayout {
public:
	struct PlacedGlyph {
		unsigned codepoint;
		Vector2  pos;
	};

public:
	TextLayout();
	TextLayout(const TextLayout&)  = delete;
	TextLayout(      TextLayout&&) = default;
	~TextLayout() = default;

	TextLayout& operator=(const TextLayout&)  = delete;
	TextLayout& operator=(      TextLayout&&) = delete;

	unsigned nGlyphs() const { return _glyphs.size(); }
	const PlacedGlyph& glyph(unsigned i) const { return _glyphs.at(i); }

	void clear();
	void addGlyph(unsigned codepoint, const Vector2& pos);

private:
	typedef std::vector<PlacedGlyph> GlyphVector;

private:
	GlyphVector _glyphs;
};


class BitmapFont {
public:
	struct Glyph {
		Box2     region;
		Vector2  size;
		Vector2  offset;
		unsigned advance;
	};

public:
	BitmapFont();
	BitmapFont(const Json::Value& json, AssetSP image);
	BitmapFont(const BitmapFont&)  = delete;
	BitmapFont(      BitmapFont&&) = default;
	~BitmapFont() = default;

	BitmapFont& operator=(const BitmapFont&)  = delete;
	BitmapFont& operator=(      BitmapFont&&) = delete;

	bool     isValid()  const { return bool(_image); }
	unsigned fontSize() const { return _fontSize; }
	unsigned height()   const { return _height; }
	AssetSP  image()    const { return _image; }

	const Glyph& glyph(int cp) const;
	int kerning(int cp0, int cp1) const;

	void setFromJson(const Json::Value& json);
	void setImage(AssetSP image);

	unsigned textWidth(const std::string& msg) const;
	TextLayout layoutText(const std::string& msg, unsigned maxWidth = 999999) const;

protected:
	typedef std::unordered_map<int, Glyph, std::hash<int>, std::equal_to<int>,
	                           Eigen::aligned_allocator<std::pair<int, Glyph>>> GlyphMap;
	typedef std::pair<int, int> CharPair;
	typedef std::unordered_map<CharPair, int, boost::hash<CharPair>> KerningMap;

protected:
	unsigned wordWidth(const std::string& msg, unsigned i, unsigned* ci = nullptr) const;

protected:
	AssetSP    _image;

	unsigned   _fontSize;
	unsigned   _height;
	unsigned   _baselineToTop;

	GlyphMap   _glyphMap;
	KerningMap _kerning;
};

typedef std::shared_ptr<BitmapFont> BitmapFontSP;
typedef std::weak_ptr  <BitmapFont> BitmapFontWP;


typedef GenericAspect  <BitmapFont>       BitmapFontAspect;
typedef std::shared_ptr<BitmapFontAspect> BitmapFontAspectSP;
typedef std::weak_ptr  <BitmapFontAspect> BitmapFontAspectWP;


}

#endif
