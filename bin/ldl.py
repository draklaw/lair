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
from collections import OrderedDict, namedtuple
import re


_undefined = object()


class TypedList(list):
    def __init__(self, type_ = None, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._type   = type_
        self._inline = False
        self._style  = '['

    def inline(self):
        self._inline = True
        return self

    def function(self):
        self._inline = True
        self._style  = '('
        return self

    def __repr__(self):
        return 'TypedList({!r}, {})'.format(self._type, super().__repr__())

    def pprint(self, indent = '  ', depth = 0, file = stdout, no_indent = False):
        if not no_indent:
            file.write(indent * depth)
        if self._type is not None:
            file.write(self._type)
        file.write('[\n')

        depth += 1
        for v in self:
            file.write(indent * depth)
            if hasattr(v, 'pprint'):
                v.pprint(indent = indent, depth = depth , file = file, no_indent = True)
            else:
                file.write(repr(v))
                file.write('\n')
        depth -= 1

        file.write(indent * depth)
        file.write(']\n')

class TypedDict(OrderedDict):
    def __init__(self, type_ = None, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._type   = type_
        self._inline = False

    def __repr__(self):
        return 'TypedDict({!r}, {})'.format(self._type, super().__repr__())

    def inline(self):
        self._inline = True
        return self

    def pprint(self, indent = '  ', depth = 0, file = stdout, no_indent = False):
        if not no_indent:
            file.write(indent * depth)
        if self._type is not None:
            file.write(self._type)
        file.write('{\n')

        depth += 1
        for k, v in self.items():
            file.write(indent * depth)
            file.write(repr(k))
            file.write(' = ')
            if hasattr(v, 'pprint'):
                v.pprint(indent = indent, depth = depth , file = file, no_indent = True)
            else:
                file.write(repr(v))
                file.write('\n')
        depth -= 1

        file.write(indent * depth)
        file.write('}\n')

class LdlWriter:
    LIST = 'list'
    MAP  = 'map'

    _keywords = {
        None:  'null',
        True:  'true',
        False: 'false',
    }

    _id_re = re.compile(r'^[a-zA-Z_][a-zA-Z_0-9]*$')

    def __init__(self, out, indent = '\t'):
        self.out    = out
        self.indent_string = indent
        self.indent = 0
        self.context = []
        self.close_char = []
        self.inline = []
        self.count  = []

    def new_line(self):
        self.out.write('\n')
        self.out.write(self.indent_string * self.indent)

    def sep(self):
        if not self.context:
            return

        if len(self.context) != 1 or self.count[0]:
            if self.inline[-1]:
                if self.count[-1]:
                    self.out.write(', ')
                elif self.close_char[-1] != ')':
                    self.out.write(' ')
            elif self.count[-1] != 0 or self.close_char:
                self.new_line()
        self.count[-1] += 1

    def open_map(self, type = None, inline = False):
        if self.context:
            if type is not None:
                self.write_string(type)
            self.out.write('{')
            if not inline:
                self.indent += 1
            self.close_char.append('}')
        else:
            self.close_char.append(None)

        self.context.append(LdlWriter.MAP)

        self.count.append(0)
        self.inline.append(inline)

    def open_list(self, type = None, style = '[', inline = False):
        assert style == '[' or style == '('

        if self.context:
            if type is not None:
                self.write_string(type)
            self.out.write(style)
            if not inline:
                self.indent += 1
            self.close_char.append(']' if style == '[' else ')')
        else:
            self.close_char.append(None)

        self.context.append(LdlWriter.LIST)

        self.count.append(0)
        self.inline.append(inline)

    def close(self):
        if not self.close_char:
            self.out.write('\n')
            return

        inline = self.inline.pop()
        char = self.close_char.pop()

        if not inline:
            self.indent -= 1

        if self.count.pop():
            if not inline:
                self.new_line()
            elif char != ')':
                self.out.write(' ')

        if char:
            self.out.write(char)
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
            assert not self.context or self.context[-1] == LdlWriter.LIST
            obj = obj_or_key
        else:
            assert not self.context or self.context[-1] == LdlWriter.MAP
            key = obj_or_key

        self.sep()

        if hasattr(obj, 'as_ldl'):
            obj = obj.as_ldl()

        if isinstance(obj, TypedList):
            type       = obj._type
            inline     = obj._inline
            list_style = obj._style
        elif isinstance(obj, TypedDict):
            type       = obj._type
            inline     = obj._inline

        if key is not None:
            self.write_key(key)

        if isinstance(obj, dict):
            self.open_map(type = type, inline = inline)
            for k, v in obj.items():
                self.write(k, v, inline = inline)
            self.close()
        elif isinstance(obj, (list, tuple)):
            self.open_list(type = type, style = list_style, inline = inline)
            for v in obj:
                self.write(v, inline = inline)
            self.close()
        elif isinstance(obj, str):
            self.write_string(obj)
        elif isinstance(obj, bool) or obj is None:
            self.out.write(self._keywords[obj])
        else:
            self.out.write(repr(obj))


#_ws_re = re.compile(r'[ \t\n,]+ | (?://|\#) [^\n]* \n | /\* (?:[^*]|\*(?!/))* \*/', re.MULTILINE | re.VERBOSE)
_ws_re = re.compile(r'[ \t]+ | /\* (?:[^*]|\*(?!/))* \*/', re.MULTILINE | re.VERBOSE)
_sep_re = re.compile(r'[\n,] | (?://|\#) [^\n]* \n', re.MULTILINE | re.VERBOSE)
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
    (_sep_re, lambda t: ('sep', t)),
    (_op_re, lambda t: ('op', t)),
    (_id_re, parse_id),
    (_float_re, lambda t: ('float', float(t))),
    (_int_re, parse_int),
    (_ts_string_re, lambda t: parse_string(t, 3)),
    (_td_string_re, lambda t: parse_string(t, 3)),
    (_s_string_re, lambda t: parse_string(t, 1)),
    (_d_string_re, lambda t: parse_string(t, 1)),
]

class LdlReader:
    Token = namedtuple('Token', ['line', 'col', 'type', 'value'])

    def __init__(self, input_):
        self._ldl     = input_.read()
        self._lines   = self._ldl.split('\n')
        self._tokens  = []
        self._pos     = 0
        self._line    = 0
        self._col     = 0
        self._value   = None

    def token(self, index = 0):
        while index >= len(self._tokens) and self._pos != len(self._ldl):
            t, v = None, None
            while t is None and self._pos != len(self._ldl):
                # Test all tokens in order
                # TODO: optimize this by looking at the first char and select the
                #   RE accordingly.
                for re, parse in _lexer:
                    match = re.match(self._ldl, self._pos)
                    if match:
                        t, v = parse(match.group(0))
                        if t != 'ws':
                            self._push_token(t, v)
                        self._eat(len(match.group(0)))
                        break
                else:
                    raise RuntimeError('{}:{}: Unrecognized token.\n{}'.format(
                        self._line + 1, self._col + 1, self._lines[self._line]))

        if index < len(self._tokens):
            return self._tokens[index]
        return None

    def _parse_value(self, need_sep = True, accept_key = True):
        # print('Parse')
        sep_count = 0
        # print('TOKEN: {!r}'.format(self.token(0)))
        while self.token(0) is not None and self.token(0).type == 'sep':
            # print('SKIP')
            self._pop()
            # print('TOKEN: {!r}'.format(self.token(0)))
            sep_count += 1

        token = self.token(0)

        if token is None:
            return None

        self._pop()

        # Sep is never required for end
        if token.type == 'op' and token.value in ')]}':
            return { 'end': token }

        # Raise error if sep is required but not there
        if need_sep and sep_count == 0:
            raise RuntimeError('{}:{}: Expected "," or new line, got {!r}.'.format(
                token.line + 1, token.col + 1, token.value))

        if token.type == 'string':
            token1 = self.token(0)

            if token1 is None:
                return { 'value': token }

            if accept_key and token1.type == 'op' and token1.value == '=':
                self._pop()
                token2 = self.token(0)

                if token2 is None:
                    raise RuntimeError('{}:{}: Unexpected end-of-file.'
                        .format(self._line + 1, self._col + 1))
                if token2.type == 'sep':
                    raise RuntimeError('{}:{}: Expected value, got {!r}.'.format(
                        token2.line + 1, token2.col + 1, token2.value))

                value = self._parse_value(need_sep = False, accept_key = False)
                value['key'] = token
                return value

            if token1.type == 'op' and token1.value in '([{':
                self._pop()
                return { 'type': token, 'value': token1}

            return { 'value': token }
        elif token.type == 'op' and token.value == '=':
            raise RuntimeError('{}:{}: Expected value, got {!r}.'.format(
                token.line + 1, token.col + 1, token.value))
        else:
            return { 'value': token }

        print(repr(token))
        raise RuntimeError("Should not happen")

    def read(self):
        is_root = self._value is None
        if is_root:
            self._value = self._parse_value(need_sep = False)

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
                return value
            elif tok.value in '([{':
                is_list = tok.value in '(['
                type_   = None
                if 'type' in self._value:
                    type_ = self._value.get('type').value
                self._value = self._parse_value(need_sep = False)
            else:
                # Should not happen, except if bug in _parse_value()
                raise RuntimeError()

        if is_list:
            value = TypedList(type_)
        else:
            value = TypedDict(type_)

        while self._value is not None and 'end' not in self._value:
            tok = self._value.get('value')
            key = self._value.get('key')
            if is_list:
                if key is None:
                    value.append(self.read())
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

        if not is_root:
            tok = self._value.get('end')
            if (is_list and tok.value not in ')]') or (not is_list and tok.value not in '}'):
                raise RuntimeError('{}:{}: Unexpected token "{}".'
                    .format(tok.line + 1, tok.col + 1, tok.value))

            self._value = self._parse_value()

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
    w = LdlWriter(stdout, indent = '  ')

    w.open_list()

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

    w.write(TypedList('Vector', [1, 2, 3]).function())

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

        [] # comment
        {/* comment
        on several
        lines*/}

        [ 1, 2
          3]
        {
            foo = 0 // Comment
            bar = 1
        }

        foo(3, a, b)
        bar{ x = 0, y = 1 }
    '''

    from io import StringIO

    parser = LdlReader(StringIO(test))

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
            p = parser._parse_value(need_sep = False)
            if p is None:
                break
            print(p)
        except RuntimeError as e:
            print(e)

    print('Read:')
    parser = LdlReader(StringIO(test))
    print(parser.read())
