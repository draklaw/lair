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


from sys import argv, stderr
from collections import OrderedDict
from os.path import basename, dirname, join, relpath, splitext
from pathlib import PurePath
import re
from pprint import pprint

from tmx import (
    Color as TmxColor, GroupLayer, Layer, Object, ObjectGroup, Property, TileMap
)

from ldl import LdlWriter, Typed


def properties_as_dict(properties):
    d = OrderedDict()

    for prop in properties:
        d[prop.name] = prop.value

    return d

def get_tile_set_from_gid(tile_map, gid):
    prev = None
    for tile_set in tile_map.tilesets:
        if tile_set.firstgid >= gid:
            return prev
        prev = tile_set
    return prev

_vectorize_re = re.compile(r'^(.*)_([xyzw])$')
_vectorize_index = { 'x': 0, 'y': 1, 'z': 2, 'w': 3 }
def vectorize(properties):
    d = OrderedDict()

    for k, v in sorted(properties.items(), key=lambda t: t[0]):
        match = _vectorize_re.match(k)
        if match:
            vec = d.setdefault(match.group(1), Vector())
            i = _vectorize_index[match.group(2)]
            while len(vec.coeffs) < i + 1:
                vec.coeffs.append(0)
            vec.coeffs[i] = v
        else:
            d[k] = v

    return d


class Vector:
    def __init__(self, *args):
        self.coeffs = list(args)
        if len(args) == 1:
            try:
                iter(args[0])
                self.coeffs = args[0]
            except TypeError:
                pass

    def as_ldl(self):
        return Typed('Vector', self.coeffs, inline = True)

    def __repr__(self):
        return 'Vector({})'.format(', '.join(map(repr, self.coeffs)))

Color = Vector

class ABox:
    def __init__(self, min, size):
        self.min = min
        self.size = size

    def as_ldl(self):
        return Typed('ABox', OrderedDict([
            ('min', self.min),
            ('size', self.size),
        ]), inline = True)

    def __repr__(self):
        return 'ABox({!r}, {!r})'.format(self.min, self.size)

class Transform:
    def __init__(self, translation, rotation = 0, scale = None):
        self.translation = translation
        self.rotation = rotation
        self.scale = scale

    def as_ldl(self):
        ops = []
        ops.append(Typed('translate', self.translation, inline = True))
        if self.rotation != 0:
            ops.append(Typed('rotate', [self.rotation], inline = True))
        if self.scale is not None:
            ops.append(Typed('scale', self.scale, inline = True))
        if len(ops) == 1:
            return ops[0]
        return Typed(None, ops, inline = True)

class Sampler:
    def __init__(self, flags = ['bilinear_no_mipmap', 'clamp'], anisotropy = None,
                 lod_bias = None, min_lod = None, max_lod = None):
        if isinstance(flags, str):
            flags = [ flags ]
        self._flags = ' | '.join(flags)
        self._anisotropy = anisotropy
        self._lod_bias = lod_bias
        self._min_lod = min_lod
        self._max_lod = max_lod

        self._params = [ self._flags, self._anisotropy, self._lod_bias,
                         self._min_lod, self._max_lod ]
        self._n_params = 0
        for i, p in enumerate(self._params):
            if p is not None:
                self._n_params = i + 1
            else:
                # To avoid null values.
                self._params[i] = 0

    def as_ldl(self):
        return Typed('Sampler', self._params[:self._n_params], inline = True)

    def __repr__(self):
        return 'Sampler({})'.format(', '.join(self._params[:self._n_params]))

class Texture:
    def __init__(self, source, sampler, target = 'sprite_color'):
        self._target = target
        self._source = source
        self._sampler = sampler

    def as_ldl(self):
        return Typed('Texture', [self._target, self._source, self._sampler], inline = True)

    def __repr__(self):
        return 'Texture({_source!r}, {_sampler!r}, {_target!r})'.format(**self.__dict__)


class TileMapAsDict:
    _halign_anchor = { 'left': 0, 'center': .5, 'right': 1 }
    _valign_anchor = { 'bottom': 0, 'center': .5, 'top': 1 }

    def __init__(self, tile_map, filename, target_dir, sampler, font):
        self._tile_map = tile_map
        self._filename = filename
        self._target_dir = target_dir
        self._sampler = sampler
        self._font = font

        self._height = 0
        self._tile_layers = []
        self._fetch_tile_layers(self._tile_map.layers)

    def _fetch_tile_layers(self, layers):
        for layer in layers:
            if isinstance(layer, Layer):
                self._tile_layers.append(layer)
            elif isinstance(layer, GroupLayer):
                self._fetch_tile_layers(layer.layers)

    def convert(self):
        return self.tile_map(self._tile_map)

    def tile_map(self, tile_map):
        d = OrderedDict()

        self._height = tile_map.height * tile_map.tileheight

        d['width'] = tile_map.width
        d['height'] = tile_map.height
        d['properties'] = self.properties(tile_map.properties)
        d['tilesets'] = list(map(self.tile_set, tile_map.tilesets))
        d['tile_layers'] = list(map(self.tile_layer, self._tile_layers))
        d['objects'] = list(filter(bool, map(self.object, tile_map.layers)))

        return d

    def tile_set(self, tile_set):
        d = OrderedDict()

        d['h_tiles'] = tile_set.columns
        d['v_tiles'] = tile_set.tilecount // tile_set.columns
        d['image'] = self.path(tile_set.image.source)

        return d

    def tile_layer(self, tile_layer):
        return Typed(None, list(map(lambda t: t.gid, tile_layer.tiles)), inline = True)

    def object(self, object):
        d = OrderedDict()

        # Test if the object is in fact a layer
        if isinstance(object, Layer):
            d['type'] = 'tile_layer'
        elif isinstance(object, ObjectGroup) or isinstance(object, GroupLayer):
            d['type'] = 'group'
        elif isinstance(object, Object):
            d['type'] = object.type
        else:
            print("Warning: unsupported object type: {}".format(type(layer)), file = stderr)
            return None

        properties = self.properties(object.properties)

        text_object = None
        if hasattr(object, 'text'):
            text_object = object.text

        # Get useful properties
        if isinstance(object, Object):
            anchor_x = properties.get('anchor_x')
            if anchor_x is None:
                anchor_x = properties.get('sprite.anchor_x')
            if anchor_x is None and text_object:
                anchor_x = self._halign_anchor[text_object.halign]
            if anchor_x is None:
                anchor_x = 0.5

            anchor_y = properties.get('anchor_y')
            if anchor_y is None:
                anchor_y = properties.get('sprite.anchor_y')
            if anchor_y is None and text_object:
                anchor_y = self._valign_anchor[text_object.valign]
            if anchor_y is None:
                anchor_y = 0.5
            anchor_y = 1 - anchor_y

            base_x = object.__dict__.get('x', 0.0)
            base_y = object.__dict__.get('y', 0.0)
        else:
            anchor_x = 0
            anchor_y = 0
            base_x = object.__dict__.get('offsetx', 0.0)
            base_y = self._height + object.__dict__.get('offsety', 0.0)

        z = properties.get('z', 0.0)
        width  = object.__dict__.get('width',  0)
        height = object.__dict__.get('height', 0)
        rotation = -object.__dict__.get('rotation', 0.0) # Rotation is inverted

        if hasattr(object, 'gid') and object.gid:
            # Sprite object coordinate is bottom-left, for some reason...
            base_y -= height

        # Tiled origin is top left, Lair is bottom left
        x = base_x + anchor_x * width
        y = self._height - base_y - anchor_y * height

        # Set base properties
        d['name'] = object.name
        d['enabled'] = properties.get('enabled', True)
        d['transform'] = Transform([x, y, z], rotation)

        # Group properties in vectors
        properties = vectorize(properties)

        # Separate properties by components
        for k, v in properties.items():
            comp_prop = k.split('.', 2)
            if len(comp_prop) < 2:
                comp_prop = [ 'properties', comp_prop[0] ]
            d.setdefault(comp_prop[0], OrderedDict())[comp_prop[1]] = v

        # Set sprite properties if object is a sprite object.
        sprite_enabled = properties.get('sprite.enabled', True)
        if 'gid' in object.__dict__ and object.gid is not None and sprite_enabled:
            tile_set = get_tile_set_from_gid(self._tile_map, object.gid)
            sprite = d.setdefault('sprite', OrderedDict())
            sprite['texture'] = Texture(self.path(tile_set.image.source), self._sampler)
            sprite['tile_grid'] = Vector(
                tile_set.columns,
                tile_set.tilecount // tile_set.columns,
            )
            sprite['tile_index'] = object.gid - tile_set.firstgid

        # Add default sprite parameters
        if 'sprite' in d:
            sprite = d['sprite']
            # Convert filename to Texture object
            texture = sprite.get('texture')
            if isinstance(texture, str):
                sprite['texture'] = Texture(texture, self._sampler)
            sprite.setdefault('anchor', Vector(anchor_x, anchor_y))

        # Set collisions properties
        if 'collision' in d:
            collision = d['collision']
            if 'shape' not in collision:
                collision['shape'] = ABox(
                    Vector(
                        - object.width  * anchor_x,
                        - object.height * anchor_y,
                    ),
                    Vector(
                        object.width,
                        object.height,
                    )
                )

        # Support text objects
        if text_object:
            text = d.setdefault('text', OrderedDict())
            text.setdefault('font', self._font)
            text['text'] = text_object.text
            if text_object.wrap:
                text['size'] = Vector(width, height)
            text['color'] = self.property(text_object.color)
            text['anchor'] = Vector(
                self._halign_anchor[text_object.halign],
                self._valign_anchor[text_object.valign],
            )

        # Tile map
        if isinstance(object, Layer):
            tile_layer = d.setdefault('tile_layer', OrderedDict())
            tile_layer['tile_map'] = self.path(self._filename)
            tile_layer['layer_index'] = self._tile_layers.index(object)

        if isinstance(object, GroupLayer):
            d['children'] = list(map(self.object, object.layers))
        elif isinstance(object, ObjectGroup):
            d['children'] = list(map(self.object, object.objects))

        return d

    def properties(self, properties):
        if not isinstance(properties, dict):
            properties = properties_as_dict(properties)

        d = OrderedDict()

        for k, v in properties.items():
            d[k] = self.property(v)

        return d

    def property(self, property):
        if isinstance(property, TmxColor):
            color = [
                property.red   / 255,
                property.green / 255,
                property.blue  / 255,
                property.alpha / 255,
            ]
            return Color(color)
        elif isinstance(property, PurePath):
            return self.path(str(property))

        return property

    def path(self, path):
        return relpath(path, self._target_dir)


def usage(ret=1):
    stderr.write("""Usage: {} <tmx-input> [<ldl-output])
Convert a tmx file into a ldl file compatible with Lair.
""".format(argv[0]))
    exit(ret)

if __name__ == '__main__':
    in_filename  = None
    out_dir      = None
    out_filename = None
    sampler = 'bilinear_no_mipmap|clamp'
    font = 'droid_sans_24.json'

    arg_it = iter(argv[1:])
    for arg in arg_it:
        if arg[0] == '-':
            if arg == '-d':
                out_dir = next(arg_it)
            elif arg == '-f':
                font = next(arg_it)
            elif arg == '-s':
                sampler = next(arg_it)
            else:
                print("Unknown option {}.".format(arg), file=stderr)
                usage()
        elif in_filename is None:
            in_filename = arg
        elif out_filename is None:
            out_filename = arg
        else:
            usage()

    if in_filename is None:
        usage()

    if out_filename is None:
        out_filename = splitext(in_filename)[0] + '.ldl'
        if out_dir:
            out_filename = join(out_dir, basename(out_filename))

    if out_dir is None:
        out_dir = dirname(out_filename)

    tile_map = TileMap.load(in_filename)

    with open(out_filename, 'w') as out_file:
        out = LdlWriter(out_file)

        sampler = Sampler(sampler)

        converter = TileMapAsDict(tile_map, out_filename, out_dir, sampler, font)
        tile_map_as_dict = converter.convert()
        for k, v in tile_map_as_dict.items():
            out.write(k, v)
