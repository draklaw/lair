/*
 *  Copyright (C) 2015 Simon Boyé
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

#include "lair/render_gl3/orthographic_camera.h"


namespace lair
{


OrthographicCamera::OrthographicCamera()
    : _viewBox() {
}


OrthographicCamera::~OrthographicCamera() {
}


void OrthographicCamera::setViewBox(const Box3& viewBox) {
	_viewBox = viewBox;
}


Matrix4 OrthographicCamera::transform() const {
	float rpl = _viewBox.max()(0) + _viewBox.min()(0);
	float tpb = _viewBox.max()(1) + _viewBox.min()(1);
	float fpn = _viewBox.max()(2) + _viewBox.min()(2);

	float rml = _viewBox.max()(0) - _viewBox.min()(0);
	float tmb = _viewBox.max()(1) - _viewBox.min()(1);
	float fmn = _viewBox.max()(2) - _viewBox.min()(2);

	Matrix4 viewMatrix;
	viewMatrix << 2.f/rml,       0,        0, -rpl/rml,
	                    0, 2.f/tmb,        0, -tpb/tmb,
	                    0,       0, -2.f/fmn,  fpn/fmn,
	                    0,       0,        0,        1;

	return viewMatrix;
}


}
