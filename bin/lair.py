#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# Copyright (C) 2018 Simon Boyé
#
# This file is part of lair.
#
# lair is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# lair is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with lair.  If not, see <http://www.gnu.org/licenses/>.
#


from ldl import TypedDict, TypedList


class Vector:
    """A lair vector, can be converted to/from ldl."""

    _ldl_type = 'Vector'

    def __init__(self, *args):
        self.coeffs = list(args)
        if len(args) == 1:
            try:
                iter(args[0])
                self.coeffs = args[0]
            except TypeError:
                pass

    def as_ldl(self):
        return TypedList(self._ldl_type, self.coeffs).function()

    def __repr__(self):
        return 'Vector({})'.format(', '.join(map(repr, self.coeffs)))


class Box:
    """A lair box, can be converted to/from ldl."""

    _ldl_type = 'Box'

    def __init__(self, min_or_dict, size = None):
        if size is None:
            self.min = min_or_dict.get('min')
            self.size = min_or_dict.get('size')
        else:
            self.min = min_or_dict
            self.size = size

    def as_ldl(self):
        return TypedDict(self._ldl_type, [
            ('min',  self.min),
            ('size', self.size),
        ]).inline()

    def __repr__(self):
        return 'Box({!r}, {!r})'.format(self.min, self.size)


class ABox(Box):
    """A lair ABox, pretty much the same as Box."""

    _ldl_type = 'ABox'


class Transform:
    """A transform that can be converted to ldl. Converting ldl to transform is
    not supported yet."""

    _ldl_type = None

    def __init__(self, translation, rotation = 0, scale = None):
        self.translation = translation
        self.rotation = rotation
        self.scale = scale

    def as_ldl(self):
        ops = []
        ops.append(TypedList('translate', self.translation).function())
        if self.rotation != 0:
            ops.append(TypedList('rotate', [self.rotation]).function())
        if self.scale is not None:
            ops.append(TypedList('scale', self.scale).function())
        if len(ops) == 1:
            return ops[0]
        return TypedList(self._ldl_type, ops).inline()

class Sampler:
    """A Lair sampler, can be converted to/from ldl."""

    _ldl_type = 'Sampler'

    _default_params = [ None, 1.0, 0.0, -1000.0, 1000.0 ]

    def __init__(self, flags_or_ldl = ['bilinear_no_mipmap', 'clamp'], anisotropy = 1.0,
                 lod_bias = 0.0, min_lod = -1000.0, max_lod = 1000.0):
        if (isinstance(flags_or_ldl, TypedList)
            and flags_or_ldl._type == self._ldl_type):
            self.__init__(*flags_or_ldl)
            return

        if isinstance(flags_or_ldl, str):
            flags_or_ldl = list(map(str.strip, flags_or_ldl.split('|')))

        self._flags = flags_or_ldl
        self._anisotropy = anisotropy
        self._lod_bias = lod_bias
        self._min_lod = min_lod
        self._max_lod = max_lod

    def _params(self):
        params = [ ' | '.join(self._flags), self._anisotropy, self._lod_bias,
                   self._min_lod, self._max_lod ]
        n_params = 0
        for i, p in enumerate(params):
            if p != self._default_params[i]:
                n_params = i + 1

        return params[:n_params]

    def as_ldl(self):
        return TypedList(self._ldl_type, self._params()).function()

    def __repr__(self):
        return 'Sampler({})'.format(', '.join(self._params()))


class Texture:
    """A lair texture, can be converted to/from ldl."""

    _ldl_type = 'Texture'

    def __init__(self, source_or_ldl, sampler = None, target = 'sprite_color'):
        if sampler is None:
            self._target = source_or_ldl[0]
            self._source = source_or_ldl[1]
            if len(source_or_ldl) > 2:
                self._sampler = source_or_ldl[2]
            else:
                self._sampler = None
        else:
            self._target = target
            self._source = source_or_ldl
            self._sampler = sampler

    def as_ldl(self):
        return TypedList(self._ldl_type, [self._target, self._source, self._sampler]).function()

    def __repr__(self):
        return 'Texture({_source!r}, {_sampler!r}, {_target!r})'.format(**self.__dict__)
