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


uniform highp mat4 viewMatrix;

in highp   vec4 vx_position;
in lowp    vec4 vx_color;
in mediump vec2 vx_texCoord;

out highp   vec4 position;
out lowp    vec4 color;
out mediump vec2 texCoord;

void main() {
	gl_Position = viewMatrix * vx_position;
	position    = vx_position;
	color       = vx_color;
	texCoord    = vx_texCoord;
};
