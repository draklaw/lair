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
from math import cos, radians, sin
from collections import OrderedDict
from pathlib import Path, PurePath
import re
from pprint import pprint

from tiled import (
    Color as TiledColor,
    Loader, Map, Layer, TileLayer, ObjectLayer, GroupLayer, Object,
)

from ldl import LdlWriter, Typed


def get_tile_set_from_gid(tilemap, gid):
    prev = None
    for tileset in tilemap.tilesets:
        if gid < tileset.firstgid:
            return prev
        prev = tileset
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

def rotate(angle, x, y):
    c = cos(angle)
    s = sin(angle)
    return x * c + y * s, x * -s + y * c

_align_attr = { 'x': 'halign', 'y': 'valign' }
_align_anchor = {
    'left': 0, 'center': .5, 'right': 1,
    'bottom': 0, 'top': 1
}
def get_anchor(obj, axis, default = None):
    assert axis == 'x' or axis == 'y'
    prop = 'anchor_' + axis

    anchor = obj.properties.get(prop)
    if anchor is None:
        anchor = obj.properties.get('sprite.' + prop)
    if anchor is None and getattr(obj, 'text', None):
        anchor = _align_anchor[getattr(obj.text, _align_attr[axis])]
    if anchor is None:
        anchor = default

    return anchor

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


class MapAsDict:
    def __init__(self, map_, filename, target_dir, sampler, font, loader):
        self._map = map_
        self._filename = filename
        self._target_dir = target_dir
        self._sampler = sampler
        self._font = font
        self._loader = loader

        self._height = 0
        self._tile_width = 0
        self._tile_height = 0
        self._tile_layers = []
        self._fetch_tile_layers(self._map.layers)

    def _fetch_tile_layers(self, layers):
        for layer in layers:
            if isinstance(layer, TileLayer):
                self._tile_layers.append(layer)
            elif isinstance(layer, GroupLayer):
                self._fetch_tile_layers(layer.children)

    def convert(self):
        return self.map(self._map)

    def map(self, map_):
        d = OrderedDict()

        self._height = map_.height * map_.tileheight
        self._tile_width  = map_.tilewidth
        self._tile_height = map_.tileheight

        d['width'] = map_.width
        d['height'] = map_.height
        d['properties'] = self.properties(map_.properties)
        d['tilesets'] = list(map(self.tile_set, map_.tilesets))
        d['tile_layers'] = list(map(self.tile_layer, self._tile_layers))
        d['objects'] = list(filter(bool, map(self.object, map_.layers)))

        return d

    def tile_set(self, tileset):
        d = OrderedDict()

        d['h_tiles'] = tileset.columns
        d['v_tiles'] = tileset.tilecount // max(1, tileset.columns)
        d['image'] = self.path(tileset.image.source)

        return d

    def tile_layer(self, tile_layer):
        d = OrderedDict()

        tile_layer.convert_chunks_to_tiles()

        d['offset']    = Vector(tile_layer.tile_offset_x,
                                tile_layer.height + tile_layer.tile_offset_y)
        d['size']      = Vector(tile_layer.width, tile_layer.height)
        d['tile_size'] = Vector(self._tile_width, self._tile_height)
        d['tiles']     = Typed(None, tile_layer.tiles, inline = True)

        return d

    def object(self, object):
        d = OrderedDict()

        # Test if the object is in fact a layer
        if isinstance(object, TileLayer):
            type_ = 'tile_layer'
        elif isinstance(object, ObjectLayer) or isinstance(object, GroupLayer):
            type_ = 'group'
        elif isinstance(object, Object):
            type_ = object.type
        else:
            print("Warning: unsupported object type: {}".format(type(layer)), file = stderr)
            return None

        properties = self.properties(object.properties)

        template = None
        if getattr(object, 'template', None):
            template = self._loader.load_template(object.template)

        text_object = None
        if hasattr(object, 'text'):
            text_object = object.text

        gid = getattr(object, 'gid', None)
        if gid is None and template:
            gid = getattr(template.object, 'gid', None)

        # Get useful properties
        if isinstance(object, Object):
            anchor_x = get_anchor(object, 'x')
            if anchor_x is None and template:
                anchor_x = get_anchor(template.object, 'x')
            if anchor_x is None:
                anchor_x = 0.5

            anchor_y = get_anchor(object, 'y')
            if anchor_y is None and template:
                anchor_y = get_anchor(template.object, 'y')
            if anchor_y is None:
                anchor_y = 0.5

            base_x = getattr(object, 'x', 0.0)
            base_y = getattr(object, 'y', 0.0)

            width  = getattr(object, 'width',  None)
            if width is None and template:
                width = template.object.width

            height = getattr(object, 'height', None)
            if height is None and template:
                height = template.object.height
        else:
            anchor_x = 0
            anchor_y = 0
            base_x = getattr(object, 'offsetx', 0.0)
            base_y = getattr(object, 'offsety', 0.0)
            width  = 0 #self._width
            height = self._height

        z = properties.get('z', 0.0)
        rotation = -getattr(object, 'rotation', 0.0) # Rotation is inverted

        # For some reason, tile objects origin is bottom-left instead of top-left
        if gid is None:
            # Move base coordinate bottom-left
            base_y += height

        # Tiled origin is top left of the map, Lair is bottom left
        x, y = rotate(-radians(rotation), anchor_x * width, anchor_y * height)
        x += base_x
        y += self._height - base_y

        # Set base properties
        if getattr(object, 'template', None) is not None:
            d['model'] = object.template.stem
        if type_ is not None:
            d['type'] = type_
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
        if getattr(object, 'gid', 0) and sprite_enabled:
            tileset = get_tile_set_from_gid(self._map, object.gid)
            sprite = d.setdefault('sprite', OrderedDict())
            sprite['texture'] = Texture(self.path(tileset.image.source), self._sampler)
            sprite['tile_grid'] = Vector(
                tileset.columns,
                tileset.tilecount // tileset.columns,
            )
            sprite['tile_index'] = object.gid - tileset.firstgid

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
                _align_anchor[text_object.halign],
                _align_anchor[text_object.valign],
            )

        # Tile map
        if isinstance(object, TileLayer):
            tile_layer = d.setdefault('tile_layer', OrderedDict())
            tile_layer['tile_map'] = self.path(self._filename)
            tile_layer['layer_index'] = self._tile_layers.index(object)

        if isinstance(object, GroupLayer):
            d['children'] = list(map(self.object, object.children))
        elif isinstance(object, ObjectLayer):
            d['children'] = list(map(self.object, object.children))

        return d

    def properties(self, properties):
        if not isinstance(properties, dict):
            properties = properties_as_dict(properties)

        d = OrderedDict()

        for k, v in properties.items():
            d[k] = self.property(v)

        return d

    def property(self, property):
        if isinstance(property, TiledColor):
            color = list(map(lambda c: c / 255, property.color))
            return Color(color)
        elif isinstance(property, PurePath):
            return self.path(property)

        return property

    def path(self, path):
        path = Path(path).resolve()
        target = Path(self._target_dir).resolve()
        return str(path.relative_to(target))


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
                out_dir = PurePath(next(arg_it))
            elif arg == '-f':
                font = next(arg_it)
            elif arg == '-s':
                sampler = next(arg_it)
            else:
                print("Unknown option {}.".format(arg), file=stderr)
                usage()
        elif in_filename is None:
            in_filename = PurePath(arg)
        elif out_filename is None:
            out_filename = PurePath(arg)
        else:
            usage()

    if in_filename is None:
        usage()

    if out_filename is None:
        out_filename = in_filename.with_suffix('.ldl')
        if out_dir:
            out_filename = out_dir / out_filename

    if out_dir is None:
        out_dir = out_filename.parent

    loader = Loader(in_filename)
    tilemap = loader.load_map(in_filename)

    with open(str(out_filename), 'w') as out_file:
        out = LdlWriter(out_file)

        sampler = Sampler(sampler)

        converter = MapAsDict(tilemap, out_filename, out_dir, sampler, font, loader)
        tile_map_as_dict = converter.convert()
        for k, v in tile_map_as_dict.items():
            out.write(k, v)
