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

from autotmpl import Template


def dump(obj, indent = ''):
	indent2 = indent + '\t'
	if isinstance(obj, list):
		print(indent, '[', sep = '')
		for e in obj:
			dump(e, indent2)
		print(indent, ']', sep = '')
	elif isinstance(obj, tuple):
		print(indent, '(', sep = '')
		for e in obj:
			dump(e, indent2)
		print(indent, ')', sep = '')
	else:
		print(indent, repr(obj), sep='')


def underscore_from_camel(camel):
	out = StringIO()
	can_split = False
	for c in camel:
		if c.isupper():
			if can_split:
				out.write('_')
				can_split = False
			out.write(c.lower())
		else:
			can_split = True
			out.write(c)
	return out.getvalue()


class AutoType:
	_default_c_from_py_tmpl = Template(
"""{{ self.decl() }} {{ out }} = ({{ self.decl() }}){{ self.c_from_py_fn() }}({{ in }});
if(PyErr_Occurred()) {
	return {{ err_return }};
}
""")
	_default_py_from_c_tmpl = Template(
"""PyObject* {{ out }} = {{ self.py_from_c_fn() }}({{ self.py_from_c_cast() }}{{ in }});
if({{ out }} == NULL) {
	return {{ err_return }};
}
""")
	def __init__(self, py_id, c_id, parse_fmt, py_from_c_fn, c_from_py_fn,
	             py_from_c_cast = None, py_from_c_tmpl = None, c_from_py_tmpl = None,
	             class_ = None):
		self._py_id          = py_id
		self._c_id           = c_id
		self._parse_fmt      = parse_fmt
		self._py_from_c_fn   = py_from_c_fn
		self._c_from_py_fn   = c_from_py_fn
		self._py_from_c_cast = py_from_c_cast
		self._py_from_c_tmpl = py_from_c_tmpl if py_from_c_tmpl is not None \
		                                      else self._default_py_from_c_tmpl
		self._c_from_py_tmpl = c_from_py_tmpl if c_from_py_tmpl is not None \
		                                      else self._default_c_from_py_tmpl
		self._class          = class_

		self._const = None

	def const(self):
		if self._const is None:
			self._const = AutoType(
				self._py_id, 'const '+self._c_id, self._parse_fmt,
				self._py_from_c_fn, self._c_from_py_fn, self._py_from_c_cast,
				self._py_from_c_tmpl, self._c_from_py_tmpl)
		return self._const

	def value(self):
		return _AutoTypeValue(self)

	def decl(self):
		return self._c_id

	def py_fmt(self):
		return self._parse_fmt

	def fmt_args(self, var):
		if self._class:
			return (self._class.converter_fn(), '&'+var)
		return ('&'+var,)

	def to_param(self, var):
		return var

	def c_from_py_fn(self):
		return self._c_from_py_fn

	def py_from_c_fn(self):
		return self._py_from_c_fn

	def py_from_c_cast(self):
		if self._py_from_c_cast is not None:
			return '(' + self._py_from_c_cast + ')'
		return ''

	def write_c_from_py(self, out, in_id, out_id, err_return = 'NULL', indent = ''):
		self._c_from_py_tmpl.render(out, { 'self':       self,
		                                   'in':         in_id,
		                                   'out':        out_id,
		                                   'err_return': err_return }, indent = indent)

	def write_py_from_c(self, out, in_id, out_id, err_return = 'NULL', indent = ''):
		self._py_from_c_tmpl.render(out, { 'self':       self,
		                                   'in':         in_id,
		                                   'out':        out_id,
		                                   'err_return': err_return }, indent = indent)

class _AutoTypeValue:
	def __init__(self, type_):
		self._type = type_

	def __getattr__(self, name):
		return getattr(self._type, name)

	def to_param(self, var):
		return '*' + var


auto_bool   = AutoType('bool', 'int',      'p', 'PyBool_FromLong',      'PyBool_Check')
auto_int    = AutoType('int', 'long',      'i', 'PyLong_FromLong',      'PyLong_AsLong')
auto_int64  = AutoType('int', 'long long', 'i', 'PyLong_FromLongLong',  'PyLong_AsLongLong')
auto_string = AutoType('str', 'char*',     's', 'PyUnicode_FromString', 'PyUnicode_AsUTF8')
auto_object = AutoType('object', 'PyObject*', 'O', None, None)


class AutoParam:
	def __init__(self, param_type, param_id, default = None, flags = 0):
		self._param_type = param_type
		self._param_id   = param_id
		self._default    = default
		self._flags      = flags

	def param_type(self):
		return self._param_type

	def param_id(self):
		return self._param_id

	def func_param(self):
		return self.param_type().to_param(self.param_id())

	def default_value(self):
		return self._default

	def write_decl(self, out, indent):
		if self.default_value() is None:
			out.write("{}{} {};\n".format(indent,
								          self.param_type().decl(),
			                              self.param_id()))
		else:
			out.write("{}{} {} = {};\n".format(indent,
			                                   self.param_type().decl(),
			                                   self.param_id(),
			                                   self.default_value()))


REQUIRED_PARAM = 0
OPTIONAL_PARAM = 1
KEYWORD_PARAM  = 2

class AutoParamList:
	_parser_tmpl = Template(
"""{{< self.write_decl }}

static char* kwlist[] = { {{< self.write_str_ids }}, NULL };
if(!PyArg_ParseTupleAndKeywords(args, kwargs, "{{< self.write_fmt }}:{{ py_id }}", kwlist, {{< self.write_refs }})) {
	return {{ err_return }};
}
""")
	_parser_noparam_tmpl = Template(
"""if(PyTuple_Size(args) || (kwargs && PyDict_Size(kwargs))) {
	PyErr_SetString(PyExc_TypeError, "{{ py_id }} expected 0 arguments");
	return {{ err_return }};
}
""")

	def __init__(self, *args):
		self._params = list(args)

		for i, p in enumerate(self._params):
			if not isinstance(p, AutoParam) and not isinstance(p, int):
				self._params[i] = AutoParam(*p)

	def params(self):
		return self._params

	def real_params(self):
		return filter(lambda p: isinstance(p, AutoParam), self._params)

	def write_decl(self, out, indent=''):
		for param in self.real_params():
			param.write_decl(out, indent=indent)

	def write_str_ids(self, out, indent=''):
		if self._params:
			out.write('(char*)"')
			out.write('", (char*)"'.join(map(AutoParam.param_id, self.real_params())))
			out.write('"')

	def write_fmt(self, out, indent=''):
		state = REQUIRED_PARAM
		for param in self._params:
			if param in (OPTIONAL_PARAM, KEYWORD_PARAM):
				if state == REQUIRED_PARAM:
					out.write('|')
				if param == KEYWORD_PARAM:
					out.write('$')
				state = param
				continue
			out.write(param.param_type().py_fmt())

	def write_refs(self, out, indent=''):
		if self._params:
			not_first = False
			for param in self.real_params():
				for arg in param.param_type().fmt_args(param.param_id()):
					if not_first:
						out.write(', ')
					not_first = True
					out.write(arg)

	def write_params(self, out, indent=''):
		for i, param in enumerate(self.real_params()):
			if i:
				out.write(', ')
			out.write(param.func_param())

	def write_parser(self, out, py_id, err_return = 'NULL', indent=''):
		if self._params:
			self._parser_tmpl.render(out, { 'self': self,
			                                'py_id': py_id,
			                                'err_return': err_return}, indent = indent)
		else:
			self._parser_noparam_tmpl.render(out, { 'self': self,
			                                        'py_id': py_id,
			                                        'err_return': err_return}, indent = indent)


class AutoFunction:
	_func_tmpl = Template(
"""static PyObject*
{{ self.wrapper_id() }}({{ self.self_type() }}* self, PyObject* args, PyObject* kwargs) {
	{{< self.write_param_parser }}

	try {
		{{< self.write_self_check }}
		{{< self.write_body }}
	} catch(std::exception& e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return NULL;
	}
}
""")
	_self_check_tmpl = Template(
"""if(!self->obj) {
	PyErr_SetString(PyExc_ValueError, "self is not initialized");
	return NULL;
}
""")
	_body_noret_tmpl = Template(
"""{{ self.object_prefix() }}{{ self.c_id() }}({{< self.params().write_params }});
Py_RETURN_NONE;
""")
	_body_ret_tmpl = Template(
"""{{ self.return_type().decl() }} tmp = {{ self.object_prefix() }}{{ self.c_id() }}({{< self.params().write_params }});
{{< self.write_ret_conv }}
return ret;
""")
	_def_tmpl = Template(
"""{ "{{ self.py_id() }}", (PyCFunction){{ self.wrapper_id() }}, METH_VARARGS | METH_KEYWORDS,
  "{{ self.doc() }}" },
""")

	def __init__(self, c_id, return_type = None, params = AutoParamList(),
		         py_id = None, body_tmpl = None, doc = ''):
		self._c_id        = c_id
		self._py_id       = py_id if py_id is not None else underscore_from_camel(c_id)
		self._return_type = return_type
		self._params      = params if isinstance(params, AutoParamList) else AutoParamList(*params)
		self._self_type   = 'PyObject'
		self._is_method   = False
		self._body_tmpl   = body_tmpl
		self._doc         = doc

		if self._body_tmpl is None:
			self._body_tmpl = self._body_noret_tmpl if self.return_type() is None \
			                                        else self._body_ret_tmpl

	def c_id(self):
		return self._c_id

	def py_id(self):
		return self._py_id

	def return_type(self):
		return self._return_type

	def params(self):
		return self._params

	def self_type(self):
		return self._self_type

	def doc(self):
		return self._doc

	def wrapper_id(self):
		if self._self_type is not None:
			return self._self_type + '_' + self.c_id() + '_wrapper'
		return self.c_id() + '_wrapper'

	def object_prefix(self):
		return 'self->obj->' if self._is_method else ''

	def write_param_parser(self, out, indent = ''):
		self.params().write_parser(out, self.py_id(), indent = indent)

	def write_self_check(self, out, indent = ''):
		if self._is_method:
			self._self_check_tmpl.render(out, { 'self': self }, indent = indent)

	def write_body(self, out, indent = ''):
		self._body_tmpl.render(out, { 'self': self }, indent = indent)

	def write_ret_conv(self, out, indent = ''):
		self.return_type().write_py_from_c(out, 'tmp', 'ret', indent = indent)

	def write_wrapper_function(self, out, indent = ''):
		self._func_tmpl.render(out, { 'self': self }, indent = indent)

	def write_def(self, out, indent = ''):
		self._def_tmpl.render(out, { 'self': self }, indent = indent)


class AutoGetSet:
	_get_tmpl = Template(
"""static PyObject *
{{ self.getter_id() }}({{ self.get_class().object_type() }}* self, void* /*closure*/)
{
	try {
		{{ self.getset_type().decl() }} tmp = self->obj->{{ self.c_getter() }}();
		{{< self.write_ret_conv }}
		return ret;
	} catch(std::exception& e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return NULL;
	}
}
""")
	_set_tmpl = Template(
"""static int
{{ self.setter_id() }}({{ self.get_class().object_type() }}* self, PyObject* value, void* /*closure*/)
{
	if (value == NULL) {
		PyErr_SetString(PyExc_TypeError, "Cannot delete the {{ self.getset_id() }} attribute");
		return -1;
	}

	try {
		{{< self.write_value_conv }}
		self->obj->{{ self.c_setter() }}(cValue);
	} catch(std::exception& e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return -1;
	}

	return 0;
}
""")
	_no_set_tmpl = Template(
"""static int
{{ self.setter_id() }}({{ self.get_class().object_type() }}* /*self*/, PyObject* value, void* /*closure*/)
{
	if (value == NULL) {
		PyErr_SetString(PyExc_TypeError, "Cannot delete the {{ self.getset_id() }} attribute");
	} else {
		PyErr_SetString(PyExc_TypeError, "Attribute {{ self.getset_id() }} is read-only");
	}
	return -1;
}
""")
	_getseter_def_tmpl = Template(
"""{ (char*)"{{ self.getset_id() }}",
  (getter){{ self.getter_id() }}, (setter){{ self.setter_id() }},
  (char*)"{{ self.doc() }}",
  NULL},
""")
	def __init__(self, getset_id, getset_type, c_getter = None, c_setter = None, doc = ''):
		self._getset_id   = getset_id
		self._getset_type = getset_type
		self._c_getter    = c_getter if c_getter is not None else getset_id
		self._c_setter    = c_setter
		self._class       = None
		self._doc         = doc

	def getset_id(self):
		return self._getset_id

	def getset_type(self):
		return self._getset_type

	def c_getter(self):
		return self._c_getter

	def c_setter(self):
		return self._c_setter

	def get_class(self):
		return self._class

	def doc(self):
		return self._doc

	def getter_id(self):
		return "{}_get_{}".format(self.get_class().class_id(), self.getset_id())

	def setter_id(self):
		return "{}_set_{}".format(self.get_class().class_id(), self.getset_id())

	def write_ret_conv(self, out, indent = ''):
		self.getset_type().write_py_from_c(out, 'tmp', 'ret', indent = indent)

	def write_value_conv(self, out, indent = ''):
		self.getset_type().write_c_from_py(out, 'value', 'cValue', '-1', indent = indent)

	def write_getseters(self, out, indent = ''):
		self._get_tmpl.render(out, { 'self': self }, indent = indent)
		out.write('\n')
		set_tmpl = self._set_tmpl if self.c_setter() is not None else self._no_set_tmpl
		set_tmpl.render(out, { 'self': self }, indent = indent)

	def write_def(self, out, indent = ''):
		self._getseter_def_tmpl.render(out, { 'self': self }, indent = indent)


class AutoClass(AutoType):
	_decl_tmpl = Template(
"""extern PyTypeObject {{ self.type_object() }};
PyObject* {{ self.py_from_c() }}({{ self.c_class() }}* cObj);
{{ self.c_class() }}* {{ self.c_from_py() }}(PyObject* pyObj);
int {{ self.converter_fn() }}(PyObject* obj, void* addr);
""")
	_binding_tmpl = Template(
"""struct {{ self.object_type() }} {
	PyObject_HEAD
	{{ self.c_class() }}* obj;
	bool _owned;
};

static void
{{ self.dealloc_func() }}({{ self.object_type() }}* self) {
	if(self->_owned) {
		delete self->obj;
	}
	Py_TYPE(self)->tp_free((PyObject*)self);
}

{{< self.write_new }}

{{< self.write_init }}

{{< self.write_methods }}

static PyMethodDef {{ self.methods_def_id() }}[] = {
	{{< self.write_methods_def }}
	{ NULL, NULL, 0, NULL }
};

{{< self.write_getseters }}

static PyGetSetDef {{ self.getseter_def_id() }}[] = {
	{{< self.write_getseters_def }}
	{NULL, NULL, NULL, NULL, NULL}  /* Sentinel */
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
PyTypeObject {{ self.type_object() }} = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"{{ self.full_id() }}",		/* tp_name */
	sizeof({{ self.object_type() }}),		/* tp_basicsize */
	0,		/* tp_itemsize */
	(destructor){{ self.dealloc_func() }},		/* tp_dealloc */
	0,		/* tp_print */
	0,		/* tp_getattr */
	0,		/* tp_setattr */
	0,		/* tp_reserved */
	0,		/* tp_repr */
	0,		/* tp_as_number */
	0,		/* tp_as_sequence */
	0,		/* tp_as_mapping */
	0,		/* tp_hash */
	0,		/* tp_call */
	0,		/* tp_str */
	0,		/* tp_getattro */
	0,		/* tp_setattro */
	0,		/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT |
		Py_TPFLAGS_BASETYPE,		/* tp_flags */
	"{{ self.doc() }}",		/* tp_doc */
	0,		/* tp_traverse */
	0,		/* tp_clear */
	0,		/* tp_richcompare */
	0,		/* tp_weaklistoffset */
	0,		/* tp_iter */
	0,		/* tp_iternext */
	{{ self.methods_def_id() }},		/* tp_methods */
	0,		/* tp_members */
	{{ self.getseter_def_id() }},		/* tp_getset */
	0,		/* tp_base */
	0,		/* tp_dict */
	0,		/* tp_descr_get */
	0,		/* tp_descr_set */
	0,		/* tp_dictoffset */
	(initproc){{ self.init_func() }},		/* tp_init */
	0,		/* tp_alloc */
	{{ self.new_func() }},		/* tp_new */
};
#pragma GCC diagnostic pop

PyObject* {{ self.py_from_c() }}({{ self.c_class() }}* cObj) {
	if(cObj == NULL) {
		Py_RETURN_NONE;
	}
	PyObject* pyObj = {{ self.new_func() }}(&{{ self.type_object() }}, NULL, NULL);
	if(pyObj != NULL) {
		(({{ self.object_type() }}*)pyObj)->obj = cObj;
	}
	return pyObj;
}

{{ self.c_class() }}* {{ self.c_from_py() }}(PyObject* pyObj) {
	if(PyObject_IsInstance(pyObj, (PyObject*)&{{ self.type_object() }})) {
		return (({{ self.object_type() }}*)pyObj)->obj;
	}
	if(pyObj == Py_None) {
		return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "expected object of type {{ self.full_id() }}");
	return NULL;
}

int {{ self.converter_fn() }}(PyObject* obj, void* addr) {
	{{ self.c_class() }}* tmp = {{ self.c_from_py() }}(obj);
	if(PyErr_Occurred()) {
		return 0;
	}
	*({{ self.c_class() }}**)addr = tmp;
	return 1;
}
""")
	_new_tmpl = Template(
"""static PyObject*
{{ self.new_func() }}(PyTypeObject* type, PyObject* /*args*/, PyObject* /*kwargs*/) {
	{{ self.object_type() }}* self = ({{ self.object_type() }}*)type->tp_alloc(type, 0);
	self->obj = NULL;
	self->_owned = false;
	return (PyObject*)self;
}
""")
	_init_tmpl = Template(
"""static int
{{ self.init_func() }}({{ self.object_type() }}* self, PyObject* args, PyObject* kwargs) {
	{{< self.write_init_parser }}

	try {
		if(self->_owned) {
			delete self->obj;
		}
		self->obj = new {{ self.c_class() }}({{< self.init_params().write_params }});
		self->_owned = true;
	} catch(std::exception& e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return -1;
	}

	return 0;
}
""")

	def __init__(self, class_id, init_params = AutoParamList(), c_struct = None,
			     abstract = False, doc = ''):
		self._class_id    = class_id
		self._c_struct    = c_struct if c_struct is not None else class_id
		self._init_params = init_params if init_params is None \
										or isinstance(init_params, AutoParamList) \
										else AutoParamList(*init_params)
		self._methods     = []
		self._getseters   = []
		self._doc         = doc
		self._module      = None

		AutoType.__init__(self, self._class_id, self._c_struct+'*',
						  'O&', self.py_from_c(), self.c_from_py(),
						  class_ = self)

	def class_id(self):
		return self._class_id

	def c_class(self):
		return self._c_struct

	def init_params(self):
		return self._init_params

	def methods(self):
		return self._methods

	def doc(self):
		return self._doc

	def module(self):
		return self._module

	def full_id(self):
		return "{}.{}".format(self.module().module_id(), self.class_id())

	def add_method(self, *args, **kwargs):
		if len(args) == 1 and not kwargs and isinstance(args[0], AutoFunction):
			method = args[0]
		else:
			method = AutoFunction(*args, **kwargs)
		self._methods.append(method)
		method._self_type = self.object_type()
		method._is_method = True
		return self

	def add_getset(self, *args, **kwargs):
		if len(args) == 1 and not kwargs and isinstance(args[0], AutoGetSet):
			getset = args[0]
		else:
			getset = AutoGetSet(*args, **kwargs)
		self._getseters.append(getset)
		getset._class = self
		return self

	def object_type(self):
		return self.class_id() + "_object"

	def type_object(self):
		return self.class_id() + "_class"

	def dealloc_func(self):
		return self.class_id() + "_dealloc"

	def new_func(self):
		return self.class_id() + "_new"

	def init_func(self):
		return self.class_id() + "_init" if self.init_params() is not None else '0'

	def methods_def_id(self):
		return self.class_id() + "_methods"

	def getseter_def_id(self):
		return self.class_id() + "_def"

	def c_from_py(self):
		return self.class_id() + "_FromObject"

	def py_from_c(self):
		return self.class_id() + "_From" + self.class_id()

	def converter_fn(self):
		return self.class_id() + "_converter"

	def write_new(self, out, indent = ''):
		self._new_tmpl.render(out, { 'self': self }, indent = indent)

	def write_init_parser(self, out, indent = ''):
		self.init_params().write_parser(out, self.full_id() + '.__init__', err_return = '-1', indent = indent)

	def write_init(self, out, indent = ''):
		if self.init_params() is not None:
			self._init_tmpl.render(out, { 'self': self }, indent = indent)

	def write_methods(self, out, indent = ''):
		for i, method in enumerate(self.methods()):
			if i:
				out.write('\n')
			method.write_wrapper_function(out, indent = indent)

	def write_methods_def(self, out, indent = ''):
		for method in self.methods():
			method.write_def(out, indent = indent)

	def write_getseters(self, out, indent = ''):
		for getset in self._getseters:
			getset.write_getseters(out, indent = indent)

	def write_getseters_def(self, out, indent = ''):
		for getset in self._getseters:
			getset.write_def(out, indent = indent)

	def write_decl(self, out, indent = ''):
		self._decl_tmpl.render(out, { 'self': self }, indent = indent)

	def write_binding(self, out, indent = ''):
		self._binding_tmpl.render(out, { 'self': self }, indent = indent)


class AutoModule:
	_header_tmpl = Template(
"""
#ifndef {{ self.include_guard() }}
#define {{ self.include_guard() }}

#include <Python.h>

{{< self.write_includes }}

{{< self.write_use_namespaces }}

extern "C" {

{{< self.write_classes_decl }}

PyMODINIT_FUNC
PyInit_{{ self.module_id() }}();

}

#endif
""")
	_module_file_tmpl = Template(
"""#include "{{ self.header_file() }}"

extern "C" {

{{< self.write_methods }}

{{< self.write_classes_binding }}

static PyMethodDef {{ self.methods_def_id() }}[] = {
	{{< self.write_methods_def }}
	{ NULL, NULL, 0, NULL }
};

static PyModuleDef {{ self.module_def_id() }} = {
	PyModuleDef_HEAD_INIT,
	"{{ self.module_id() }}",
	"{{ self.doc() }}",
	-1,
	{{ self.methods_def_id() }},
	0,
	0,
	0,
	0
};

PyMODINIT_FUNC
PyInit_{{ self.module_id() }}() {
	PyObject* module = NULL;

	{{< self.write_classes_ready }}

	module = PyModule_Create(&{{ self.module_def_id() }});
	if(module == NULL) {
		return NULL;
	}

	{{< self.write_submodules_register }}
	{{< self.write_classes_register }}

	return module;
}

}
""")
	_submodules_register_tmpl = Template(
"""PyModule_AddObject(module, \"{{ submodule }}\", PyInit_{{ submodule }}());
""")
	_class_ready_tmpl = Template(
"""if(PyType_Ready(&{{ self.type_object() }}) < 0) {
	return NULL;
}
""")
	_class_register_tmpl = Template(
"""Py_INCREF(&{{ self.type_object() }});
PyModule_AddObject(module, \"{{ self.class_id() }}\", (PyObject*)&{{ self.type_object() }});
""")

	def __init__(self, module_id, doc = ''):
		self._module_id  = module_id
		self._doc        = doc
		self._methods    = []
		self._classes    = []
		self._submodules = []
		self._includes   = [ "exception", "memory" ]
		self._use_ns     = []
		self._base_file  = None

	def module_id(self):
		return self._module_id

	def doc(self):
		return self._doc

	def methods(self):
		return self._methods

	def classes(self):
		return self._classes

	def submodules(self):
		return self._submodules

	def includes(self):
		return self._includes

	def use_namespaces(self):
		return self._use_ns

	def add_method(self, *args, **kwargs):
		if len(args) == 1 and not kwargs and isinstance(args[0], AutoFunction):
			method = args[0]
		else:
			method = AutoFunction(*args, **kwargs)
		self._methods.append(method)
		return self

	def add_class(self, *args, **kwargs):
		if len(args) == 1 and not kwargs and isinstance(args[0], AutoClass):
			class_ = args[0]
		else:
			class_ = AutoClass(*args, **kwargs)
		self._classes.append(class_)
		class_._module = self
		return self

	def add_submodule(self, submodule):
		self._submodules.append(submodule)
		return self

	def add_include(self, include):
		self._includes.append(include)
		return self

	def add_use_namespace(self, ns):
		self._use_ns.append(ns)
		return self

	def methods_def_id(self):
		return self.module_id() + '_methods'

	def module_def_id(self):
		return self.module_id() + '_module'

	def header_file(self):
		return self._base_file + '.h'

	def source_file(self):
		return self._base_file + '.cpp'

	def include_guard(self):
		return '_' + self.module_id().upper() + '_PY_H'

	def write_includes(self, out, indent = ''):
		for inc in self.includes():
			out.write("#include \"{}\"\n".format(inc))

	def write_use_namespaces(self, out, indent = ''):
		for ns in self.use_namespaces():
			out.write("using namespace {};\n".format(ns))

	def write_methods(self, out, indent = ''):
		for i, method in enumerate(self.methods()):
			if i:
				out.write('\n')
			method.write_wrapper_function(out, indent = indent)

	def write_classes_decl(self, out, indent = ''):
		for i, class_ in enumerate(self.classes()):
			if i:
				out.write('\n')
			class_.write_decl(out, indent = indent)

	def write_classes_binding(self, out, indent = ''):
		for i, class_ in enumerate(self.classes()):
			if i:
				out.write('\n')
			class_.write_binding(out, indent = indent)

	def write_methods_def(self, out, indent = ''):
		for method in self.methods():
			method.write_def(out, indent = indent)

	def write_classes_ready(self, out, indent = ''):
		for i, class_ in enumerate(self.classes()):
			if i:
				out.write('\n')
			self._class_ready_tmpl.render(out, { 'self': class_ }, indent)

	def write_classes_register(self, out, indent = ''):
		for i, class_ in enumerate(self.classes()):
			if i:
				out.write('\n')
			self._class_register_tmpl.render(out, { 'self': class_ }, indent)

	def write_submodules_register(self, out, indent = ''):
		for i, mod in enumerate(self.submodules()):
			if i:
				out.write('\n')
			self._submodules_register_tmpl.render(out, { 'submodule': mod }, indent)

	def write_module(self, base_file):
		self._base_file = base_file
		with open(self.header_file(), 'w') as out:
			self._header_tmpl.render(out, { 'self': self })
		with open(self.source_file(), 'w') as out:
			self._module_file_tmpl.render(out, { 'self': self })


if __name__ == '__main__':
	from sys import stdout, stderr

	module = AutoModule("test_autopy", "This is a test")
	module.add_include("test_autopy.h")
	module.add_include("iostream")

	module.add_method(
		AutoFunction("testFunc", None, AutoParamList(
			AutoParam(auto_int, 'a'),
			OPTIONAL_PARAM,
			AutoParam(auto_int, 'b', 0)), "test_func"))

	module.add_method(
		AutoFunction("test2", auto_int, AutoParamList(
			KEYWORD_PARAM,
			AutoParam(auto_string, 'b', '""')), doc = "Do stuff."))

	test_class = AutoClass("TestClass", "TestStruct", doc = "Hello world !")
#	test_class.add_method(AutoFunction("setFoo", None, AutoParamList(
#		AutoParam(auto_string, 'foo')), py_id = 'set_foo', doc = 'Sets foo'))
	test_class.add_getseter(AutoGetSet('foo', auto_string, 'foo', 'setFoo', doc = "Foofoo"))
	module.add_class(test_class)

	module.write_module(stdout)
