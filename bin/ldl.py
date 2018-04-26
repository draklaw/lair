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
from collections import namedtuple
import re


_undefined = object()


class TypedList(list):
    def __init__(self, type_, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.type = type_

    def __repr__(self):
        return 'TypedList({!r}, {})'.format(self.type, super().__repr__())

class TypedDict(dict):
    def __init__(self, type_, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.type = type_

    def __repr__(self):
        return 'TypedDict({!r}, {})'.format(self.type, super().__repr__())

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

        if hasattr(obj, 'as_ldl'):
            obj = obj.as_ldl()

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


_ws_re = re.compile(r'[ \t\n,]+', re.MULTILINE | re.VERBOSE)
_op_re = re.compile(r'[()[\]{}=]', re.MULTILINE | re.VERBOSE)
_int_re = re.compile(r'(?:[+-] | 0[bBoOxX])? [0-9]+', re.MULTILINE | re.VERBOSE)
_float_re = re.compile(r'''
    [+-]?
    (?:[0-9]+ \. [0-9]* | \.  [0-9]+)
    (?:[eE][+-]? [0-9]+)?
''', re.MULTILINE | re.VERBOSE)
_id_re = re.compile(r'[a-zA-Z_][a-zA-Z0-9_]*', re.MULTILINE | re.VERBOSE)
_s_string_re = re.compile(r"'(?:[^'\n]|(?<=\\)')*'", re.MULTILINE | re.VERBOSE)
_d_string_re = re.compile(r'"(?:[^"\n]|(?<=\\)")*"', re.MULTILINE | re.VERBOSE)
_ts_string_re = re.compile(r"'''(?:[^']|(?<=\\)'|'(?!''))*'''", re.MULTILINE | re.VERBOSE)
_td_string_re = re.compile(r'"""(?:[^"]|(?<=\\)"|"(?!""))*"""', re.MULTILINE | re.VERBOSE)

def parse_int(token):
    if token.startswith('0b') or token.startswith('0B'):
        return 'int', int(token[2:], 2)
    if token.startswith('0o') or token.startswith('0O'):
        return 'int', int(token[2:], 8)
    if token.startswith('0x') or token.startswith('0X'):
        return 'int', int(token[2:], 16)
    return 'int', int(token)

def parse_id(token):
    if token == 'null':
        return 'null', None
    if token == 'true':
        return 'bool', True
    if token == 'false':
        return 'bool', False
    return 'string', token

def parse_string(token, n_quotes):
#    token = token[n_quotes:-n_quotes]
    return 'string', eval(token)

_lexer = [
    (_ws_re, lambda t: ('ws', t)),
    (_op_re, lambda t: ('op', t)),
    (_id_re, parse_id),
    (_float_re, lambda t: ('float', float(t))),
    (_int_re, parse_int),
    (_ts_string_re, lambda t: parse_string(t, 3)),
    (_td_string_re, lambda t: parse_string(t, 3)),
    (_s_string_re, lambda t: parse_string(t, 1)),
    (_d_string_re, lambda t: parse_string(t, 1)),
]

class Pattern:
    def __init__(self, types, predicate = None, optional = False, target = None):
        if isinstance(types, str):
            types = [ types ]
        self.types     = types
        self.predicate = predicate
        self.optional  = optional
        self.target    = target

    def opt(self):
        return Pattern(self.types, self.predicate, True, self.target)

    def tgt(self, target):
        return Pattern(self.types, self.predicate, self.optional, target)

    def match(self, key, value):
        return key in self.types and (self.predicate is None or self.predicate(key, value))

    def __repr__(self):
        return 'Pattern({types!r}, {predicate!r}, {optional!r}, {target!r})'.format(**self.__dict__)

_ws = Pattern('ws', lambda t, v: ',' not in v and '\n' not in v, optional = True)
_sep = Pattern('ws', optional = True)
_value = Pattern(
    ['null', 'bool', 'int', 'float', 'string', 'op'],
    lambda t, v: t != 'op' or v in '([{',
)
_string = Pattern('string')
_compound_value = Pattern('op', lambda t, v: v in '([{')
_assign = Pattern('op', lambda t, v: v == '=')
_end = Pattern('op', lambda t, v: v in ')]}')

_parser = [
    (_sep, _string.tgt('key'), _ws, _assign,
        _ws, _string.tgt('type'), _ws, _compound_value.tgt('value')),
    (_sep, _string.tgt('key'), _ws, _assign,
        _ws, _value.tgt('value')),
    (_sep, _string.tgt('type'), _ws, _compound_value.tgt('value')),
    (_sep, _value.tgt('value')),
    (_sep, _end.tgt('end')),
    (_sep,),
]

class LdlParser:
    Token = namedtuple('Token', ['line', 'col', 'type', 'value'])

    def __init__(self, input_):
        self._ldl     = input_.read()
        self._lines   = self._ldl.split('\n')
        self._tokens  = []
        self._pos     = 0
        self._line    = 0
        self._col     = 0
        self._context = []
        self._value   = None

    def token(self, index = 0):
        while index >= len(self._tokens):
            if self._pos == len(self._ldl):
                break

            # Test all tokens in order
            # TODO: optimize this by looking at the first char and select the
            #   RE accordingly.
            for re, parse in _lexer:
                match = re.match(self._ldl, self._pos)
                if match:
                    t, v = parse(match.group(0))
                    self._push_token(t, v)
                    self._eat(len(match.group(0)))
                    break
            else:
                raise RuntimeError('{}:{}: Unrecognized token.\n{}'.format(
                    self._line + 1, self._col + 1, self._lines[self._line]))

        if index < len(self._tokens):
            return self._tokens[index]
        return None

    def _parse_value(self):
        # print('Parse')
        for pattern in _parser:
            i = 0
            v = {}
            # print('  Pattern')
            for p in pattern:
                tok = self.token(i)
                if not tok:
                    break
                # print('    {}: {}'.format(p, tok))
                if p.match(tok.type, tok.value):
                    if p.target:
                        v[p.target] = tok
                    i += 1
                elif not p.optional:
                    break
            else:
                self._pop(i)
                if v:
                    return v

        if self.token(0) is None:
            return None

        tok = self.token(0)
        self._pop()

        raise RuntimeError('{}:{}: Parse error: Unexpected token "{}".'.format(
            tok.line + 1, tok.col + 1, tok.value))

    # def next(self):
    #     self._value = self._parse_value()
    #
    # def enter(self):
    #     if not self._value:
    #         self._value = self._parse_value()
    #
    #     if self._value is None:
    #
    #
    #     if self._context:
    #         if self._value['type'] == 'op' and '([' in self._value['value']:
    #             self._context.append('list')
    #             self.next()
    #         elif self._value['type'] == 'op' and '{' in self._value['value']:
    #             self._context.append('map')
    #             self.next()
    #         else:
    #             tok = self._value.value
    #             raise RuntimeError('{}:{}: Cannot open non-compound value "{}" of type "{}"''
    #                 .format(value.line, value.col, value.value, value.type))
    #     elif 'key' in self._value:
    #         self._context.append('map')
    #     else
    #         self._context.append('list')
    #
    # def close(self):
    #     if self._value['type'] == 'op' and ')]}' in self._value['value']:
    #         if not self._context:
    #             raise RuntimeError()
    #         self._context.pop()
    #     else:
    #         raise RuntimeError

    # def parse_value(self):
    #     if not self._value:
    #         self._value = self._parse_value()

    def read(self):
        is_root = self._value is None
        if is_root:
            self._value = self._parse_value()
            print('Value 1:', self._value)

            if self._value is None:
                # We could return an empty list or dict, but we don't know which one...
                return None

        if self._value is None:
            raise RuntimeError('{}:{}: Unexpected end-of-file.'
                .format(self._line + 1, self._col + 1))

        if is_root:
            is_list = 'key' not in self._value
            type_   = None
        else:
            tok = self._value['value']
            if tok.type != 'op':
                value = tok.value
                self._value = self._parse_value()
                print('Value 2:', self._value)
                return value
            elif tok.value in '([{':
                is_list = tok.value in '(['
                type_   = None
                if 'type' in self._value:
                    type_ = self._value.get('type').value
            else:
                # Should not happen, except if bug in _parse_value()
                raise RuntimeError()
            self._value = self._parse_value()
            print('Value 3:', self._value)

        if is_list:
            value = TypedList(type_)
            print('Open list:', type_)
        else:
            value = TypedDict(type_)
            print('Open dict:', type_)

        while self._value is not None and 'end' not in self._value:
            tok = self._value.get('value')
            key = self._value.get('key')
            if is_list:
                if key is None:
                    value.append(self.read())
                    print('Append:', value[-1])
                else:
                    raise RuntimeError('{}:{}: Unexpected key "{}" in list'
                        .format(key.line + 1, key.col + 1, key.value))
            else:
                if key is None:
                    v = self._value['value']
                    raise RuntimeError('{}:{}: Expected key.'
                        .format(self._value.line + 1, self._value.col + 1))
                elif key.value in value:
                    raise RuntimeError('{}:{}: Duplicated key "{}"'
                        .format(key.line + 1, key.col + 1, key.value))
                else:
                    value[key.value] = self.read()
                    print('Insert:', key.value, '=', value[key.value])

        if not is_root:
            tok = self._value.get('end')
            if (is_list and tok.value not in ')]') or (not is_list and tok.value not in '}'):
                raise RuntimeError('{}:{}: Unexpected token "{}".'
                    .format(tok.line + 1, tok.col + 1, tok.value))

            self._value = self._parse_value()
            print('Value 4:', self._value)

        return value


    def _push_token(self, type_, value):
        self._tokens.append(self.Token(self._line, self._col, type_, value))

    def _pop(self, count = 1):
        self._tokens = self._tokens[count:]

    def _eat(self, n_chars):
        token = self._ldl[self._pos:self._pos + n_chars]
        self._pos += n_chars
        n_lines = token.count('\n')
        if n_lines:
            self._line += n_lines
            self._col   = n_chars - token.rindex('\n')
        else:
            self._col += n_chars

    def error(self, message):
        raise RuntimeError('{}:{}: {}\n{}'.format(
            self._line + 1, self._col + 1, message,
            self._lines[self._line]))


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

    test = '''
        null
        true, false
        123
        +789
        .12e-45
        -15.
        ,
        _hello_world
        'hello\\'world'
        "foo \\"bar\\" baz"
        \'''test \\\''' test\'''
        """test \\""" test"""

        []
        {}

        [ 1, 2 3]
        {
            foo = 0
            bar = 1
        }

        foo(3a b)
        bar{ x = 0, y = 1 }
    '''

    from io import StringIO

    parser = LdlParser(StringIO(test))

    print('Tokens:')
    i = 0
    while True:
        tok = parser.token(i)
        if tok is None:
            break
        print(tok)
        i += 1

    print('Values:')
    while True:
        try:
            p = parser._parse_value()
            if p is None:
                break
            print(p)
        except RuntimeError as e:
            print(e)

    print('Read:')
    parser = LdlParser(StringIO(test))
    print(parser.read())
