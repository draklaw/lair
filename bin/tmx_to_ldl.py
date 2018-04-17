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
from os.path import dirname, relpath, splitext
from pathlib import PurePath
from pprint import pprint

from tmx import Color, Layer, ObjectGroup, Property, TileMap

from ldl import LdlWriter, Typed


def get_property(properties, name, default = None):
    for prop in properties:
        if prop.name == name:
            return prop.value
    return default

class TileMapAsDict:
    def __init__(self, target_dir):
        self._target_dir = target_dir

    def tile_map(self, tile_map):
        d = OrderedDict()

        d['width'] = tile_map.width
        d['height'] = tile_map.height
        d['properties'] = self.properties(tile_map.properties)
        d['tilesets'] = list(map(self.tile_set, tile_map.tilesets))
        d['layers'] = list(filter(bool, map(self.layer, tile_map.layers)))

        return d

    def tile_set(self, tile_set):
        d = OrderedDict()

        d['h_tiles'] = tile_set.columns
        d['v_tiles'] = tile_set.tilecount // tile_set.columns
        d['image'] = self.path(tile_set.image.source)

        return d

    def layer(self, layer):
        d = OrderedDict()

        if isinstance(layer, Layer):
            d['tiles'] = Typed(None, list(map(lambda t: t.gid, layer.tiles)), inline = True)
        elif isinstance(layer, ObjectGroup):
            d['objects'] = list(map(self.object, layer.objects))
        else:
            print("Warning: unsupported layer type: {}".format(type(layer)), file = stderr)
            return None

        return d

    def object(self, object):
        d = OrderedDict()

        # anchor_x = ( get_property(object.properties, 'anchor_x')
        #           or get_property(object.properties, 'sprite.anchor_x', .5) )
        # anchor_y = ( get_property(object.properties, 'anchor_y')
        #           or get_property(object.properties, 'sprite.anchor_y', .5) )
        # z = get_property(object.properties, 'z', 0)

        d['type'] = object.type
        d['name'] = object.name
        d['is_visible'] = object.visible
        d['x'] = object.x
        d['y'] = object.y
        d['width'] = object.width
        d['height'] = object.height
        d['rotation'] = object.rotation
        # d['transform'] = Typed(None, [
        #     Typed('translate', [
        #         object.x + object.width  * anchor_x,
        #         object.y + object.height * anchor_y,
        #         z,
        #     ], inline = True),
        #     Typed('rotate', [ object.rotation ], inline = True),
        # ], inline = True)

        prop_groups = {}
        for prop in object.properties:
            comp_prop = prop.name.split('.', 2)
            if len(comp_prop) < 2:
                comp_prop = [ 'properties', comp_prop[0] ]
            prop_groups.setdefault(comp_prop[0], []).append(Property(comp_prop[1], prop.value))

        # if 'sprite' in prop_groups:
        #     prop_groups['sprite']

        for k, v in prop_groups.items():
            d[k] = self.properties(v)

        return d

    def properties(self, properties):
        d = OrderedDict()

        for prop in properties:
            d[prop.name] = self.property(prop.value)

        return d

    def property(self, property):
        if isinstance(property, Color):
            color = [
                property.red   / 255,
                property.green / 255,
                property.blue  / 255,
                property.alpha / 255,
            ]
            return Typed('Color', color, inline = True)
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
    out_filename = None

    for arg in argv[1:]:
        if in_filename is None:
            in_filename = arg
        elif out_filename is None:
            out_filename = arg
        else:
            usage()

    if in_filename is None:
        usage()

    if out_filename is None:
        out_filename = splitext(in_filename)[0] + '.ldl'

    tile_map = TileMap.load(in_filename)

    with open(out_filename, 'w') as out_file:
        out = LdlWriter(out_file)

        converter = TileMapAsDict(dirname(out_filename))
        tile_map_as_dict = converter.tile_map(tile_map)
        for k, v in tile_map_as_dict.items():
            out.write(k, v)
