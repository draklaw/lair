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


from sys import argv
from pathlib import PurePath
import xml.etree.ElementTree as ET


def parse_html_color(html_color):
    l = len(html_color)
    assert l == 4 or l == 5 or l == 7 or l == 9
    n = 3 if l == 4 or l == 7 else 4
    w = 1 if l <= 6 else 2

    c = [ 0 ] * n
    for i in range(n):
        s = html_color[1 + i * w: 1 + (i+1) * w]
        v = int(s, 16)
        if w == 1:
            c[i] = v << 16
        else:
            c[i] = v

    return c

class Color:
    def __init__(self, html_color):
        self.html_color = html_color
        self.color = parse_html_color(html_color)

        if len(self.color) == 3:
            self.color.append(255)
        else:
            self.color.append(self.color[0])
            self.color.pop(0)

    def __repr__(self):
        # return 'Color({})'.format(self.html_color)
        return 'Color({!r})'.format(list(map(int, self.color)))


def attr(etree, type_, attr, default = None):
    value = etree.get(attr)
    if value is None:
        return default
    return type_(value)


class Map:
    __dump__ = True

    def __init__(self, elem, base_path, loader = None):
        self.version         = attr(elem, str, 'version')
        self.tiledVersion    = attr(elem, str, 'tiledversion')
        self.orientation     = attr(elem, str, 'orientation', 'orthogonal')
        self.renderorder     = attr(elem, str, 'renderorder', 'right-down')
        self.width           = attr(elem, float, 'width')
        self.height          = attr(elem, float, 'height')
        self.tilewidth       = attr(elem, int, 'tilewidth')
        self.tileheight      = attr(elem, int, 'tileheight')
        self.hexsidelength   = attr(elem, float, 'hexsidelength')
        self.staggeraxis     = attr(elem, str, 'staggeraxis')
        self.staggerindex    = attr(elem, str, 'staggerindex')
        self.backgroundcolor = attr(elem, Color, 'backgroundcolor', Color('#000000'))
        self.infinite        = attr(elem, bool, 'infinite', False)
        self.nextobjectid    = attr(elem, int, 'nextobjectid')

        if self.infinite:
            self.width  = 0
            self.height = 0

        self.properties      = read_properties(elem, base_path, loader = loader)
        self.tilesets        = parse_tilesets(elem, base_path, loader = loader)
        self.layers          = parse_layers(elem, base_path, loader = loader)


class Tileset:
    __dump__ = True

    def __init__(self, elem, base_path, loader = None):
        self.firstgid   = attr(elem, int, 'firstgid')
        self.name       = attr(elem, str, 'name')
        self.tilewidth  = attr(elem, int, 'tilewidth')
        self.tileheight = attr(elem, int, 'tileheight')
        self.spacing    = attr(elem, int, 'spacing', 0)
        self.margin     = attr(elem, int, 'margin', 0)
        self.tilecount  = attr(elem, int, 'tilecount')
        self.columns    = attr(elem, int, 'columns')

        # self.tileoffset
        # self.grid
        self.properties = read_properties(elem, base_path, loader = loader)
        self.image      = parse_image(elem.find('image'), base_path, loader = loader)
        # self.terraintypes
        # self.tiles
        # self.wangsets


class Image:
    __dump__ = True

    def __init__(self, elem, base_path, loader = None):
        self.source = base_path / attr(elem, PurePath, 'source', PurePath())
        self.width  = attr(elem, int, 'width')
        self.height = attr(elem, int, 'height')


class Layer:
    __dump__ = True

    def __init__(self, elem, base_path, loader = None):
        self.name    = elem.get('name')
        self.opacity = attr(elem, float, 'opacity', 1.0)
        self.visible = attr(elem, bool, 'visible', True)
        self.offsetx = attr(elem, float, 'offsetx', 0.0)
        self.offsety = attr(elem, float, 'offsety', 0.0)

        self.properties = read_properties(elem, base_path, loader = loader)

class TileLayer(Layer):
    __dump__ = True

    def __init__(self, elem, base_path, loader = None):
        super().__init__(elem, base_path)

        self.width   = attr(elem, int, 'width')
        self.height  = attr(elem, int, 'height')

        self.tile_offset_x = 0
        self.tile_offset_y = 0
        self.chunks = None
        self.tiles  = None

        data = elem.find('data')
        encoding = data.get('encoding')

        if data.find('chunk') is not None:
            self.chunks = list(map(lambda e: Chunk(e, encoding), children(data, 'chunk')))

            # Find min and max, update width and height
            l_min = None
            l_max = None
            for coords, tile in self.iter_chunk_tiles():
                if l_min is None:
                    l_min, l_max = coords[:], coords[:]
                else:
                    for i in range(2):
                        l_min[i] = min(l_min[i], coords[i])
                        l_max[i] = max(l_max[i], coords[i])

            if l_min:
                self.tile_offset_x = l_min[0]
                self.tile_offset_y = l_min[1]
                self.width  = l_max[0] - l_min[0] + 1
                self.height = l_max[1] - l_min[1] + 1
            else:
                self.width  = 0
                self.height = 0
        else:
            self.tiles = decode_tiles(data.text, encoding)

    def convert_chunks_to_tiles(self):
        if self.chunks is None or self.tiles is not None:
            return

        self.tiles = [ 0 ] * (self.width * self.height)
        for coords, tile in self.iter_chunk_tiles():
            x = coords[0] - self.tile_offset_x
            y = coords[1] - self.tile_offset_y
            self.tiles[x + y * self.width] = tile

    def iter_chunk_tiles(self):
        for chunk in self.chunks:
            for i, tile in enumerate(chunk.tiles):
                if tile:
                    x = chunk.x + i %  chunk.width
                    y = chunk.y + i // chunk.height
                    yield [x, y], tile

class Chunk:
    def __init__(self, elem, encoding):
        self.x      = attr(elem, int, 'x')
        self.y      = attr(elem, int, 'y')
        self.width  = attr(elem, int, 'width')
        self.height = attr(elem, int, 'height')
        self.tiles  = decode_tiles(elem.text, encoding)

    def __repr__(self):
        return '<Chunk {}, {} - {}, {}>'.format(self.x, self.y, self.width, self.height)


def decode_tiles(tiles, encoding):
    if encoding == 'csv':
        return list(map(int, tiles.split(',')))
    else:
        raise RuntimeError("Unsupported tile data encoding: {}".format(encoding))

class ObjectLayer(Layer):
    __dump__ = True

    def __init__(self, elem, base_path, loader = None):
        super().__init__(elem, base_path)
        self.color     = attr(elem, Color, 'color', Color('#000000'))
        self.draworder = attr(elem, str, 'draworder', 'topdown')

        self.children = parse_objects(elem, base_path, loader = loader)


class GroupLayer(Layer):
    __dump__ = True

    def __init__(self, elem, base_path, loader = None):
        super().__init__(elem, base_path)

        self.children = parse_layers(elem, base_path, loader = loader)


class Object:
    __dump__ = True

    def __init__(self, elem, base_path, loader = None):
        self.id       = attr(elem, int, 'id')
        self.name     = elem.get('name')
        self.type     = elem.get('type')
        self.x        = attr(elem, float, 'x')
        self.y        = attr(elem, float, 'y')
        self.width    = attr(elem, float, 'width')
        self.height   = attr(elem, float, 'height')
        self.rotation = attr(elem, float, 'rotation', 0.0)
        self.gid      = attr(elem, int, 'gid')
        self.visible  = attr(elem, bool, 'visible', True)
        self.template = None
        if 'template' in elem.keys():
            self.template = base_path / PurePath(elem.get('template'))

        self.properties = read_properties(elem, base_path, loader = loader)
        self.text = None
        text_elem = elem.find('text')
        if text_elem is not None:
            self.text = parse_text(text_elem, base_path, loader = loader)


class Text:
    __dump__ = True

    def __init__(self, elem, base_path, loader = None):
        self.text       = elem.text
        self.fontfamily = attr(elem, str, 'fontfamily', 'sans-serif')
        self.pixelsize  = attr(elem, float, 'pixelsize', 16.0)
        self.wrap       = attr(elem, bool, 'wrap', True)
        self.color      = attr(elem, Color, 'color', Color('#000000'))
        self.bold       = attr(elem, bool, 'bold', False)
        self.italic     = attr(elem, bool, 'italic', False)
        self.underline  = attr(elem, bool, 'underline', False)
        self.strikeout  = attr(elem, bool, 'strikeout', False)
        self.kerning    = attr(elem, bool, 'kerning', True)
        self.halign     = attr(elem, str, 'halign', 'left')
        self.valign     = attr(elem, str, 'valign', 'top')


class Template:
    __dump__ = True

    def __init__(self, elem, base_path, loader = None):
        self.tilesets = parse_tilesets(elem, base_path, loader = loader)
        self.object = parse_object(elem.find('object'), base_path, loader = loader)


class Loader:
    __dump__ = True

    def __init__(self, source):
        self._source = source

        self._maps      = {}
        self._tilesets  = {}
        self._templates = {}

    def load_map(self, source):
        source = str(source)
        if source not in self._maps:
            self._maps[source] = read_map(source, loader = self)
        return self._maps[source]

    def load_tileset(self, source):
        source = str(source)
        if source not in self._tilesets:
            self._tilesets[source] = read_tileset(source, loader = self)
        return self._tilesets[source]

    def load_template(self, source):
        source = str(source)
        if source not in self._templates:
            self._templates[source] = read_template(source, loader = self)
        return self._templates[source]

    def __repr__(self):
        return '<Loader\n  maps: {!r}\n  tilesets: {!r}\n  templates: {!r}>'.format(
            list(self._maps.keys()),
            list(self._tilesets.keys()),
            list(self._templates.keys()),
        )


_tiled_types = {
    'bool': bool,
    'int': int,
    'float': float,
    'string': str,
    'color': Color,
    'file': PurePath,
}
def read_properties(elem, base_path, loader = None):
    props = {}
    for properties in children(elem, 'properties'):
        for property in children(properties, 'property'):
            value = _tiled_types[property.get('type', 'string')](property.get('value'))
            if isinstance(value, PurePath):
                value = base_path / value
            props[property.get('name')] = value
    return props


def read_map(source, loader = None):
    et = ET.parse(str(source))
    return Map(et.getroot(), PurePath(source).parent, loader = loader)


def read_tileset(source, loader = None):
    et = ET.parse(str(source))
    return Tileset(et.getroot(), PurePath(source).parent, loader = loader)


def read_template(source, loader = None):
    et = ET.parse(str(source))
    return Template(et.getroot(), PurePath(source).parent, loader = loader)


def parse_tileset(elem, base_path, loader = None):
    source = attr(elem, str, 'source')
    if source is None:
        return Tileset(elem, base_path)
    if loader:
        ts = loader.load_tileset(base_path / PurePath(source))
    else:
        ts = read_tileset(base_path / PurePath(source))
    ts.firstgid = int(elem.get('firstgid'))
    return ts


def parse_tilesets(elem, base_path, loader = None):
    return list(map(lambda e: parse_tileset(e, base_path, loader = loader),
                    children(elem, 'tileset')))


def parse_image(elem, base_path, loader = None):
    return Image(elem, base_path, loader = loader)


_layer_types = {
    'layer': TileLayer,
    'objectgroup': ObjectLayer,
    # 'imagelayer': ImageLayer,
    'group': GroupLayer,
}
def parse_layer(elem, base_path, loader = None):
    return _layer_types[elem.tag](elem, base_path, loader = loader)

def parse_layers(elem, base_path, loader = None):
    return list(map(lambda e: parse_layer(e, base_path, loader = loader),
                    children(elem, _layer_types.keys())))


def parse_object(elem, base_path, loader = None):
    return Object(elem, base_path, loader = loader)

def parse_objects(elem, base_path, loader = None):
    return list(map(lambda e: parse_object(e, base_path, loader = loader),
                    children(elem, 'object')))


def parse_text(elem, base_path, loader = None):
    return Text(elem, base_path, loader = loader)


def children(elem, tags):
    if isinstance(tags, str):
        tags = [ tags ]
    return filter(lambda e: e.tag in tags, elem)


def dump(obj, indent = 1):
    if getattr(obj, '__dump__', False):
        print('{}('.format(type(obj).__name__))
        for k in dir(obj):
            if k[0] != '_':
                print('{}{} = '.format('  ' * indent, k), end='')
                dump(getattr(obj, k), indent + 1)
        print('{})'.format('  ' * (indent - 1)))
    elif isinstance(obj, (tuple, list)):
        print('[')
        for v in obj:
            print('{}'.format('  ' * indent), end='')
            dump(v, indent + 1)
        print('{}]'.format('  ' * (indent - 1)))
    elif isinstance(obj, dict):
        print('{')
        for k, v in obj.items():
            print('{}{}: '.format('  ' * indent, k), end='')
            dump(v, indent)
        print('{}}}'.format('  ' * (indent - 1)))
    else:
        print(repr(obj))


def main():
    loader = Loader(argv[1])
    map = loader.load_map(argv[1])
    dump(map)
    print(loader)

if __name__ == '__main__':
    main()
