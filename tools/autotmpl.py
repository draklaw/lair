#!/usr/bin/python3

##
##  Copyright (C) 2015 Simon Boyé
##
##  This file is part of lair.
##
##  lair is free software: you can redistribute it and/or modify it
##  under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  lair is distributed in the hope that it will be useful, but
##  WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
##  General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with lair.  If not, see <http://www.gnu.org/licenses/>.
##

from io import StringIO
import re


def _ctx(ctx, obj, field):
	return ctx[field]

def _attr(ctx, obj, attr):
	return getattr(obj, attr)

def _sub(ctx, obj, expr):
	return obj[expr(ctx)]

def _call(ctx, obj, args):
	return obj(*map(lambda a: a(ctx), args))

def _str(ctx, obj, args):
	return args

class TmplExpr:
	_expr_re = re.compile(r'\s*([.,()[\]]|(?:"[^"]*")|(?:\'[^\']*\'))\s*')

	def __init__(self, line_nb, expr = None, *, _expr = None):
		self._line_nb = line_nb
		self._expr = _expr
		if expr is not None:
			self._expr, tok = self._parse_expr(iter(self._expr_re.split(expr.strip())))
			assert tok == None
		assert self._expr is not None

	def __call__(self, ctx, obj = None):
		assert self._expr
		if obj is None:
			obj = ctx
		for cmd, param in self._expr:
			obj = cmd(ctx, obj, param)
		return obj

	def _parse_expr(self, tok_it):
		test = tok_it
		cmd = []
		for tok in tok_it:
			if tok == '': # side-effect of the tokenization
				continue
			elif tok in (',', ')', ']'):
				return cmd, tok
			elif tok[0] in ('"', "'"):
				cmd.append((_str, tok[1:-1]))
			elif tok == '(':
				args = []
				for sub, tok in iter(lambda: self._parse_expr(tok_it), None):
					if sub:
						args.append(TmplExpr(_expr = sub))
					if tok == ')':
						cmd.append((_call, args))
						break;
					elif tok != ',':
						raise RuntimeError("Unexpected token {}".format(repr(tok)))
			elif tok == '[':
				sub, tok = self._parse_expr(tok_it)
				if tok == ']':
					cmd.append((_sub, TmplExpr(_expr = sub)))
				else:
					raise RuntimeError("Unexpected token {}".format(repr(tok)))
			elif tok == '.':
				cmd.append((_attr, next(tok_it)))
			else:
				cmd.append((_ctx, tok))
		return cmd, None

	def __repr__(self):
		out = StringIO()
		self._repr_helper(out)
		return out.getvalue()

	def _repr_helper(self, out):
		for cmd, param in self._expr:
			if cmd == _ctx:
				out.write(param)
			elif cmd == _str:
				out.write("'")
				out.write(param)
				out.write("'")
			elif cmd == _attr:
				out.write('.')
				out.write(param)
			elif cmd == _sub:
				out.write('[')
				param._repr_helper(out)
				out.write(']')
			elif cmd == _call:
				out.write('(')
				for i, arg in enumerate(param):
					if i:
						out.write(', ')
					arg._repr_helper(out)
				out.write(')')
			else:
				raise RuntimeError("Unexpected cmd: {}".format(repr(cmd)))


def _tmpl_indent(out, ctx, indent):
	out.write(indent)

def is_indent(s):
	if not isinstance(s, str):
		return False
	for c in s:
		if c not in (' ', '\t'):
			return False
	return True

class _TmplRepl:
	def __init__(self, line_nb, expr):
		self._expr = TmplExpr(line_nb, expr)

	def __call__(self, out, ctx, indent):
		repl = self._expr(ctx)
		if not isinstance(repl, str):
			raise RuntimeError("{}: {} must return a string, {} found"
			                   .format(self._expr._line_nb, self._expr, type(repl).__name__))
		out.write(repl)

	def __repr__(self):
		return "_TmplRepl({})".format(repr(self._expr))


class _TmplWrite:
	def __init__(self, line_nb, expr, indent):
		self._expr   = TmplExpr(line_nb, expr)
		self._indent = indent

	def __call__(self, out, ctx, indent):
		self._expr(ctx)(out, indent = indent + self._indent)

	def __repr__(self):
		return "_TmplWrite({})".format(repr(self._key))


class Template:
	_sep  = re.compile(r'((?:{{<?)|(?:}}))')
	_ws   = re.compile(r'^\s*$')

	_STRING = 0
	_REPL   = 1
	_CALL   = 2

	def __init__(self, code):
		self._tmpl_list = []

		line_nb = 0
		for line in code.splitlines(True):
			line_nb += 1
			if self._ws.match(line):
				self._tmpl_list.append('\n')
				continue

			self._tmpl_list.append(_tmpl_indent)
			state = self._STRING
#			print("TOKS:", self._sep.split(line))
			skip_lf = False
			for tok in self._sep.split(line):
				if skip_lf and tok == '\n':
					break
				tok2 = tok.strip()
				if state == self._STRING:
					if tok2 == '{{':
						state = self._REPL
					elif tok2 == '{{<':
						state = self._CALL
					elif tok:
						self._tmpl_list.append(tok)
				elif tok2 == '}}':
					state = self._STRING
				elif state == self._REPL:
					self._tmpl_list.append(_TmplRepl(line_nb, tok2))
				elif state == self._CALL:
					indent = ''
					if self._tmpl_list and (is_indent(self._tmpl_list[-1]) or
					                        self._tmpl_list[-1] == _tmpl_indent):
						indent = self._tmpl_list.pop()
						if indent == _tmpl_indent:
							indent = ''
						skip_lf = True
					self._tmpl_list.append(_TmplWrite(line_nb, tok2, indent))

	def render(self, out, ctx, indent = ''):
		for part in self._tmpl_list:
			if isinstance(part, str):
				out.write(part)
			else:
				part(out, ctx, indent)

	def dump(self):
		out = StringIO()
		for part in self._tmpl_list:
			if isinstance(part, str):
				out.write("STR {}\n".format(repr(part)))
			elif part == _tmpl_indent:
				out.write("INDENT\n")
			elif isinstance(part, _TmplRepl):
				out.write("REPL {}\n".format(repr(part._expr)))
			elif isinstance(part, _TmplWrite):
				out.write("WRITE {} indent = {}\n".format(repr(part._expr), repr(part._indent)))
		return out.getvalue()
