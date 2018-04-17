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


from sys import stdout
import re


_undefined = object()

class Typed:
    def __init__(self, type, value, inline = False):
        self.type = type
        self.value = value
        self.inline = inline

    def __repr__(self):
        if self.inline:
            return 'Typed({type!r}, {value!r}, inline=True)'.format(**self.__dict__)
        return 'Typed({type!r}, {value!r})'.format(**self.__dict__)

class LdlWriter:
    LIST = 'list'
    MAP  = 'map'

    _keywords = {
        None:  'null',
        True:  'true',
        False: 'false',
    }

    _id_re = re.compile(r'^[a-zA-Z_][a-zA-Z_0-9]*$')

    def __init__(self, out, type = 'map'):
        self.out    = out
        self.indent = 0
        self.context = [ type ]
        self.close_char = []
        self.inline = [ False ]
        self.count  = [ 0 ]

    def new_line(self):
        self.out.write('\n')
        self.out.write('\t' * self.indent)

    def sep(self):
        if self.inline[-1]:
            if self.count[-1]:
                self.out.write(', ')
            else:
                self.out.write(' ')
        elif self.count[-1] != 0 or self.close_char:
            self.new_line()
        self.count[-1] += 1

    def open_map(self, inline = False):
        self.context.append(LdlWriter.MAP)
        self.out.write('{')

        if not inline:
            self.indent += 1
        self.count.append(0)
        self.close_char.append('}')
        self.inline.append(inline)

    def open_list(self, char = '[', inline = False):
        assert char == '[' or char == '('

        self.context.append(LdlWriter.LIST)
        self.out.write(char)

        if not inline:
            self.indent += 1
        self.count.append(0)
        self.close_char.append(']' if char == '[' else ')')
        self.inline.append(inline)

    def close(self):
        if not self.close_char:
            self.out.write('\n')
            return

        inline = self.inline.pop()

        if not inline:
            self.indent -= 1

        if self.count.pop():
            if inline:
                self.out.write(' ')
            else:
                self.new_line()

        self.out.write(self.close_char.pop())
        self.context.pop()

    def write_key(self, key):
        assert self.context[-1] == self.MAP
        self.write_string(key)
        self.out.write(' = ')

    def write_string(self, string):
        if self._id_re.match(string):
            self.out.write(string)
        else:
            self.out.write(repr(string))

    def write(self, obj_or_key, obj = _undefined, type = None, list_style = '[', inline = False):
        key = None
        if obj is _undefined:
            assert self.context[-1] == LdlWriter.LIST
            obj = obj_or_key
        else:
            assert self.context[-1] == LdlWriter.MAP
            key = obj_or_key

        self.sep()

        if isinstance(obj, Typed):
            type = obj.type
            inline = obj.inline
            obj = obj.value
            if inline:
                list_style = '('

        if key is not None:
            self.write_key(key)

        if isinstance(obj, dict):
            if type is not None:
                self.write_string(type)
            self.open_map(inline=inline)
            for k, v in obj.items():
                self.write(k, v, inline=inline)
            self.close()
        elif isinstance(obj, (list, tuple)):
            if type is not None:
                self.write_string(type)
            self.open_list(char=list_style, inline=inline)
            for v in obj:
                self.write(v, inline=inline)
            self.close()
        elif isinstance(obj, str):
            self.write_string(obj)
        elif isinstance(obj, bool) or obj is None:
            self.out.write(self._keywords[obj])
        else:
            self.out.write(repr(obj))


if __name__ == '__main__':
    w = LdlWriter(stdout, LdlWriter.LIST)

    w.write(None)
    w.write(True)
    w.write(False)
    w.write(123)
    w.write(456.789)
    w.write("hello_world")
    w.write("Hello World !")

    w.write([ 0, 1, 2 ])
    w.write([ 3, 4, 5 ], type = 'Test', inline = True)

    w.write({ 'a': 0, 'b': 1, 'c': 2 })

    w.write([])
    w.write({})

    w.write([
        { 'a': 0, 'b': 1, 'c': 2 }, 1, 2
    ])

    w.close()
