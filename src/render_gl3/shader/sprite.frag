/*
 *  Copyright (C) 2015-2018 Simon Boyé
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


uniform sampler2D texture;
uniform vec4      tileInfo;

in highp   vec4 position;
in lowp    vec4 color;
in mediump vec2 texCoord;

out vec4 out_color;

void main() {
	// Is this useful ?
	vec2 nTiles    = tileInfo.xy;
	vec2 texSize   = tileInfo.zw;
	vec2 texCoord2 = texCoord * nTiles;
	vec2 margin    = nTiles / (2.0 * texSize);
	vec2 tileCoord = fract(texCoord2);
	vec2 tile      = texCoord2 - tileCoord;
	tileCoord      = clamp(tileCoord, margin, vec2(1.0) - margin);
	texCoord2      = (tile + tileCoord) / nTiles;

	vec4 fcolor = color * texture2D(texture, texCoord2);
	if(fcolor.a < .01){
		discard;
	}
	out_color = fcolor;
//	out_color = vec4(texCoord, 0., 1.);
//	out_color = vec4(1., 0., 0., 1.);
//	out_color = vec4(tileCoord, 0., 1.);
};
