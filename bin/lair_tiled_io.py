#!/usr/bin/env python
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


from sys import stdout
from os.path import dirname, relpath, splitext
import re
from pprint import pprint

from ldl import LdlWriter, Typed


_snake_from_camel_re = re.compile(r'(?<!^)([a-z])([A-Z])')
def snake_from_camel(text):
    return _snake_from_camel_re.sub(r'\1_\2', text).lower()

def dump(obj):
    map = {}
    for p in dir(obj):
        map[p] = getattr(obj, p)
    return map

if __name__ != '__main__':
    from tiled import *

    class Lair(Plugin):
        _color_re = re.compile(r'^#([0-9a-fA-F]+)$')

        @classmethod
        def nameFilter(cls):
            return "Lair files (*.ldl)"

        @classmethod
        def shortName(cls):
            return "lair"

        @classmethod
        def convert_property(cls, prop):
            match = cls._color_re.match(prop)
            if match:
                html = match.group(1)
                if len(html) == 3:
                    html = html[0] + html[0] + hmtl[1] + html[1] + html[2] + html[2]
                if len(html) == 6:
                    html = html + 'ff'
                if len(html) == 8:
                    return Typed('Color', [ int(html[0:2], 16) / 255.0,
                                            int(html[2:4], 16) / 255.0,
                                            int(html[4:6], 16) / 255.0,
                                            int(html[6:8], 16) / 255.0 ], inline = True)

            try:
                return int(prop)
            except ValueError:
                pass

            try:
                return float(prop)
            except ValueError:
                pass

            return prop

        @classmethod
        def write_properties(cls, out, obj):
            props = {}
            for p in obj.properties().keys():
                value = cls.convert_property(obj.propertyAsString(p))
                comp_prop = p.split('.', 1)
                if len(comp_prop) == 1:
                    comp, prop = 'properties', p
                else:
                    comp, prop = comp_prop

                props.setdefault(comp, {})[prop] = value

            for comp, properties in props.items():
                out.write(comp, properties)

            # out.sep()
            # out.write_key('properties')
            # out.open_map()
            # for p in obj.properties().keys():
            #     prop = obj.propertyAsString(p)
            #     type, prop = cls.convert_property(prop)
            #
            #     out.write(p, prop, type=type, list_style='(', inline=True)
            # out.close()

        @classmethod
        def write(cls, tileMap, fileName):
            with open(fileName, 'w') as fileHandle:
                out = LdlWriter(fileHandle)
                out.write('width', tileMap.width())
                out.write('height', tileMap.height())

                out.sep()
                out.write_key('tilesets')
                out.open_list()
                for i in range(tileMap.tilesetCount()):
                    ts = tileMap.tilesetAt(i).data()
                    # attrs = [ 'name', 'fileName', 'isExternal', 'tileWidth',
                    #           'tileHeight', 'tileSpacing', 'margin', 'tileCount',
                    #           'columnCount', 'imageWidth', 'imageHeight' ]
                    out.sep()
                    out.open_map()
                    # for attr in attrs:
                    #     out.write(snake_from_camel(attr), getattr(ts, attr)())
                    out.write('h_tiles', ts.columnCount())
                    out.write('v_tiles', ts.tileCount() // ts.columnCount())

                    if ts.fileName():
                        rel = relpath(ts.fileName(), dirname(fileName))
                    else:
                        rel = ts.name()
                    out.write('image', './' + splitext(rel)[0] + '.png')
                    # tree = ET.parse(ts.fileName())
                    # out.write('image', tree.find('tileset/image').get('source'))

                    out.close()
                out.close()
                cls.write_properties(out, tileMap)

                out.sep()
                out.write_key('layers')
                out.open_list()
                for i in range(tileMap.layerCount()):
                    if isTileLayerAt(tileMap, i):
                        tl = tileLayerAt(tileMap, i)
                        # attrs = [ 'name', 'opacity', 'isVisible', 'x', 'y',
                        #           'width', 'height', ]
                        out.sep()
                        out.open_map()
                        # out.write('type', 'tile_layer')
                        # for attr in attrs:
                        #     out.write(snake_from_camel(attr), getattr(tl, attr)())

                        out.sep()
                        out.write_key('tiles')
                        out.open_list(inline=True)
                        for y in range(tl.height()):
                            for x in range(tl.width()):
                                tile = tl.cellAt(x, y).tile()
                                if tile:
                                    out.write(tile.id() + 1)
                                else:
                                    out.write(0)
                        out.close()

                        # cls.write_properties(out, tl)
                        out.close()

                    elif isObjectGroupAt(tileMap, i):
                        og = objectGroupAt(tileMap, i)
                        # attrs = [ 'name', 'opacity', 'isVisible', 'x', 'y', ]
                        out.sep()
                        out.open_map()
                        # out.write('type', 'object_layer')
                        # for attr in attrs:
                        #     out.write(snake_from_camel(attr), getattr(og, attr)())

                        # cls.write_properties(out, og)

                        out.sep()
                        out.write_key('objects')
                        out.open_list()
                        for i in range(og.objectCount()):
                            obj = og.objectAt(i)
                            attrs = [ 'type', 'name', 'isVisible', 'x', 'y',
                                      'width', 'height', 'rotation' ]
                            out.sep()
                            out.open_map()
                            for attr in attrs:
                                out.write(snake_from_camel(attr), getattr(obj, attr)())
                            cls.write_properties(out, obj)
                            out.close()
                        out.close()
                        out.close()
                out.close()

            return True
