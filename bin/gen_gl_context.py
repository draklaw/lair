#!/usr/bin/env python3

from sys import stderr
import re
from string import ascii_letters
from itertools import chain
import xml.etree.ElementTree as ElementTree
from pprint import pprint


###############################################################################
# Utility functions

def iter_text_until_tag(elem, tag):
	if elem.text:
		yield elem.text
	for c in elem:
		if c.tag == tag:
			return
		for t in iter_text_until_tag(c, tag):
			if t is None:
				return
			yield t
		if c.tail:
			yield c.tail

def get_text(elem):
	"""Returns the text content of an element even if it contains sub-elements"""
	if elem is None:
		return None
	return ''.join(elem.itertext())

def get_text_before(elem, tag):
	return ''.join(iter_text_until_tag(elem, tag))

def get_attr_or_child(elem, id):
	"""Returns the value of attr 'id' if it exists, or the text of the first
	descendent with tag 'id' otherwise."""
	ret = elem.get(id)
	if ret is None:
		ret = elem.find(id)
		if ret is not None:
			ret = get_text(ret)
	return ret

attr_warned = set()
def check_attr(elem, known_attrs):
	"""Prints a warning if elem contains attributes not in known_attrs. Only
	print each warning once."""
	unknown = set(elem.keys()) - set(known_attrs)
	for attr in unknown:
		key = elem.tag, attr
		if key not in attr_warned:
			attr_warned.add(key)
			print("Element {}: Unknown attribute {}".format(elem.tag, attr), file=stderr)
			print("  ", ElementTree.tostring(elem), file=stderr)

elem_warned = set()
def check_elem(elem, known_elems):
	"""Prints a warning if elem descendent not in known_elems. Only
	print each warning once."""
	tags = list(map(lambda e: e.tag, elem.iter()))[1:]
	unknown = set(tags) - set(known_elems)
	for e in unknown:
		key = elem.tag, e
		if key not in elem_warned:
			elem_warned.add(key)
			print("Element {}: Unknown descendent {}".format(elem.tag, e), file=stderr)
			print("  ", ElementTree.tostring(elem), file=stderr)


###############################################################################
# Spec objects

class Type:
	def __init__(self, xml):
		self.spec = get_text(xml)
		self.name = get_attr_or_child(xml, 'name')
		self.api = xml.get('api')
		self.requires = xml.get('requires')
		self.comment = xml.get('comment')
		check_attr(xml, ['name', 'api', 'requires', 'comment'])
		check_elem(xml, ['name'])

	def __repr__(self):
		repr = [ '<Type {name}' ]
		if self.api:
			repr.append(' api={api}')
		if self.requires:
			repr.append(' requires={requires}')
		repr.append('>')
		return ''.join(repr).format_map(self.__dict__)

def get_enum_name(xml):
	check_attr(xml, ['name'])
	check_elem(xml, [])
	return xml.get('name')

class Group:
	def __init__(self, xml):
		self.name = xml.get('name')
		self.comment = xml.get('comment')
		self.enums = list(map(get_enum_name, xml.iter('enum')))
		check_attr(xml, ['name', 'comment'])
		check_elem(xml, ['enum'])

	def __repr__(self):
		return '<Group {name}>'.format_map(self.__dict__)

class EnumItem:
	def __init__(self, xml):
		self.name = xml.get('name')
		self.alias = xml.get('alias')
		self.value = xml.get('value')
		self.type = xml.get('type')
		self.api = xml.get('api')
		self.comment = xml.get('comment')
		check_attr(xml, ['name', 'alias', 'value', 'comment', 'type', 'api'])
		check_elem(xml, [])

	def __repr__(self):
		return '<Enum {name} {value}>'.format_map(self.__dict__)

class Enum:
	def __init__(self, xml):
		self.namespace = xml.get('namespace')
		self.group = xml.get('group')
		self.type = xml.get('type')
		self.bitmask = (self.type == 'bitmask')
		self.vendor = xml.get('vendor')
		self.comment = xml.get('comment')
		self.enums = list(map(EnumItem, xml.iter('enum')))
		check_attr(xml, ['namespace', 'group', 'type', 'vendor', 'comment',
						 'start', 'end'])
		check_elem(xml, ['enum', 'unused'])

	def __repr__(self):
		repr = [ '<Enums {namespace} {group}' ]
		if self.type:
			repr.append(' {type}')
		repr.append('>')
		return ''.join(repr).format_map(self.__dict__)

def get_alias_name(xml):
	check_attr(xml, ['name'])
	check_elem(xml, [])
	return xml.get('name')

class Param:
	def __init__(self, xml):
		self.spec = get_text(xml)
		self.name = get_text(xml.find('name'))
		self.type = get_text_before(xml, 'name')
		self.ptype = get_text(xml.find('ptype'))
		self.group = xml.get('group')
		self.len = xml.get('len')

		check_attr(xml, ['group', 'len'])
		check_elem(xml, ['ptype', 'name'])

	def as_c(self):
		return self.spec

	def __repr__(self):
		if self.group:
			return '<Param {type} {name} grp:{group}>'.format_map(self.__dict__)
		return '<Param {type} {name}>'.format_map(self.__dict__)

class Command:
	def __init__(self, xml):
		proto_elem = xml.find('proto')
		self.proto = get_text(proto_elem)
		self.name = get_text(proto_elem.find('name'))
		self.type = get_text_before(proto_elem, 'name')
		self.ptype = get_text(proto_elem.find('ptype'))

		self.comment = xml.get('comment')
		self.params = list(map(Param, xml.iter('param')))
		self.alias = list(map(get_alias_name, xml.iter('alias')))

		check_attr(xml, ['comment'])
		check_elem(xml, ['proto', 'param', 'name', 'ptype', 'alias',
						 'glx', 'vecequiv'])

	def __repr__(self):
		repr = [ '<Command {proto}(' ]
		repr.append(', '.join(map(Param.as_c, self.params)))
		repr.append(')>')
		return ''.join(repr).format_map(self.__dict__)

def get_ref(xml):
	check_attr(xml, ['name', 'comment'])
	# if 'comment' in xml.attrib:
	#     print(xml.get('comment'))
	check_elem(xml, [])
	return xml.get('name')

class FeatureSet:
	def __init__(self, xml=None):
		if xml is not None:
			self.type = xml.tag
			self.api = xml.get('api')
			self.profile = xml.get('profile')
			self.comment = xml.get('comment')

			self.types = list(map(get_ref, xml.iter('type')))
			self.enums = list(map(get_ref, xml.iter('enum')))
			self.commands = list(map(get_ref, xml.iter('command')))

			check_attr(xml, ['api', 'profile', 'comment'])
			check_elem(xml, ['type', 'enum', 'command'])
		else:
			self.types = set()
			self.enums = set()
			self.commands = set()

	def has_profile(self, profile):
		return self.profile is None or self.profile == profile

	def has_api(self, api):
		return self.api is None or self.api == api

	def __repr__(self):
		repr = [ '<FeatureSet {type}' ]
		if self.api:
			repr.append(' api={api}')
		if self.profile:
			repr.append(' profile={profile}')
		repr.append('>')
		return ''.join(repr).format_map(self.__dict__)

class Feature:
	def __init__(self, xml):
		self.api = xml.get('api')
		self.name = xml.get('name')
		self.number = xml.get('number')

		self.feature_set = (
			list(map(FeatureSet, xml.iter('require'))) +
			list(map(FeatureSet, xml.iter('remove'))) +
			list(map(FeatureSet, xml.iter('deprecate')))
		)

		check_attr(xml, ['api', 'name', 'number'])
		check_elem(xml, ['require', 'remove', 'deprecate',
						 'type', 'enum', 'command'])

	def is_in_api(self, api, version):
		return self.api == api and self.number <= version

	def __repr__(self):
		return '<Feature {api} {name} {number}>'.format_map(self.__dict__)

class Extension:
	def __init__(self, xml):
		self.name = xml.get('name')
		self.supported = xml.get('supported').split('|')
		self.comment = xml.get('comment')

		self.feature_set = (
			list(map(FeatureSet, xml.iter('require'))) +
			list(map(FeatureSet, xml.iter('remove'))) +
			list(map(FeatureSet, xml.iter('deprecate')))
		)

		check_attr(xml, ['name', 'supported', 'comment'])
		check_elem(xml, ['require', 'remove', 'deprecate',
						 'type', 'enum', 'command'])

	def __repr__(self):
		return '<Extension {name}>'.format_map(self.__dict__)

class Specs:
	def __init__(self, xml=None):
		self.types = [ Type(elem) for elem in xml.iter('type') ]
		self.groups = [ Group(elem) for elem in xml.iter('group') ]
		self.enums = [ Enum(elem) for elem in xml.iter('enums') ]

		commands = xml.find('commands')
		self.commands = [ Command(elem) for elem in commands.iter('command') ]
		self.features = [ Feature(elem) for elem in xml.iter('feature') ]
		self.extensions = [ Extension(elem) for elem in xml.iter('extension') ]

		self.type_index = {}
		for t in self.types:
			self.type_index.setdefault(t.name, []).append(t)

		self.group_index = {}
		for group in self.groups:
			for enum in group.enums:
				self.group_index.setdefault(enum, []).append(group)

		self.enum_index = {}
		for enumGroup in self.enums:
			for enum in enumGroup.enums:
				ei = self.enum_index.setdefault(enum.name, [])
				ei.append((enumGroup, enum))

		self.command_index = {}
		for command in self.commands:
			assert command.name not in self.command_index
			self.command_index[command.name] = command

		self.extension_index = {}
		for ext in self.extensions:
			assert ext.name not in self.extension_index
			self.extension_index[ext.name] = ext

class ApiItem:
	def __init__(self, feat, name, index):
		self.feat = feat
		self.name = name
		self.index = index

	def __eq__(self, other):
		return self.name == other.name

	def __hash__(self):
		return hash(self.name)

_extension_suffixes = [ '', '_ARB', '_EXT', '_KHR' ]
class ApiItems:
	def __init__(self, specs, api, version, profile='core', extensions=[]):
		self.specs = specs
		self.api = api
		self.version = version
		self.profile = profile
		self.extensions = extensions

		self.type_names = set()
		self.enum_names = set()
		self.command_names = set()

		self.features = []
		self.main_feature = None
		exts = map(lambda e: specs.extension_index[e], extensions);
		index = 0
		for feat in chain(specs.features, exts):
			if isinstance(feat, Feature):
				if not feat.is_in_api(api, version):
					continue
				self.main_feature = feat
			self.features.append(feat)
			for fs in feat.feature_set:
				if not fs.has_profile(profile) or not fs.has_api(api):
					continue
				method = set.union
				if fs.type == 'remove':
					method = set.difference

				def make_item(i):
					nonlocal index
					item = ApiItem(feat, i, index)
					index += 1
					return item
				def as_items(items):
					return map(make_item, items)

				self.type_names = method(self.type_names, fs.types)
				self.enum_names = method(self.enum_names, as_items(fs.enums))
				self.command_names = method(self.command_names, as_items(fs.commands))

		# Add parameter types / groups:
		self.group_names = set([ 'ErrorCode' ])
		for cname in self.command_names:
			command = specs.command_index[cname.name]
			for param in command.params:
				if param.ptype:
					self.type_names.add(param.ptype)
				if param.group:
					self.group_names.add(param.group)

		# Build type list:
		type_done = set()
		self.types = []
		for t in specs.types:
			if t.name not in self.type_names or t.name in type_done:
				continue

			# FIXME: Support api other than 'gl'
			# It looks like we should take the api-specific type if it exists, and
			# the type with no api specification otherwise.
			if t.api:
				continue

			# Don't insert requires as we are inside a namespace. This stuff should
			# already be included by lair.h anyway.
			# if t.requires and t.requires not in type_done:
			# 	done.add(t.requires)
			# 	self.types.append(specs.type_index[t.requires][0])

			type_done.add(t.name)
			self.types.append(t)

		# Build enum list:
		enum_done = set()
		self.enums = []
		for enum in sorted(self.enum_names, key=lambda i: i.index):
			e = specs.enum_index[enum.name]
			assert len(e) == 1
			self.enums.append((enum.feat, e[0][1]))

		# Build command list
		command_done = set()
		self.extension_with_commands = set()
		self.commands = []
		for command in sorted(self.command_names, key=lambda i: i.index):
			c = specs.command_index[command.name]
			self.extension_with_commands.add(command.feat.name)
			self.commands.append((command.feat, c))

		# Groups
		self.groups = []
		self.groups_by_name = {}
		for group in specs.groups:
			if group.name not in self.group_names:
				continue
			items = list(filter(bool, map(self.enum_alias, group.enums)))
			if items:
				self.groups.append((group, items))
				self.groups_by_name[group.name] = self.groups[-1]

		# Bitsets
		self.bitset_names = set()
		self.bitsets = []
		for enum in specs.enums:
			if enum.type != 'bitmask':
				continue

			items = self.groups_by_name.get(enum.group, [None, []])[1]
			def test_enum(e):
				return self.has_enum(e) and e not in items
			items.extend(filter(test_enum, map(lambda e: e.name, enum.enums)))
			if items:
				self.bitset_names.add(enum.group)
				self.bitsets.append((enum, items))

	# Some accessors (enum_names, ...)
	def has_enum(self, enum_name):
		return ApiItem(None, enum_name, 0) in self.enum_names

	def enum_alias(self, enum_name):
		for suffix in _extension_suffixes:
			if ApiItem(None, enum_name + suffix, 0) in self.enum_names:
				return enum_name + suffix
		return None

###############################################################################
# Templates

header = """/*
 *  Copyright (C) 2016 Simon Boyé
 *
 *  This file is part of lair.
 *
 *  lair is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  lair is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lair.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 *  This file is generated automatically by gl_context.py.
 */
"""

h_template = """>>> header


#ifndef _LAIR_RENDER_GL3_CONTEXT_H
#define _LAIR_RENDER_GL3_CONTEXT_H


#include <lair/core/lair.h>
#include <lair/core/log.h>


#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif


namespace lair {

>>> render_types(types)

namespace gl {
enum GL {
	>>> render_enums(enums)
};
}


struct GLOutOfMemory : public std::runtime_error {
	GLOutOfMemory();
};


class Context {
public:
	typedef void* (*GlGetProcAddress)(const char*);

public:
	Context(GlGetProcAddress glGetProcAddress, Logger* logger);

	Context(const Context&) = delete;
	Context(Context&&)      = default;
	~Context() = default;

	Context& operator=(const Context&) = delete;
	Context& operator=(Context&&)      = default;

	bool initialize(bool debugGl = false);
	void shutdown();

	inline void setAbortOnError(bool enable) {
		_abortOnError = enable;
	}

	inline void setLogErrors(bool enable) {
		_logErrors = enable;
	}

	inline void setLogCalls(bool enable) {
		_logCalls = enable;
	}

	Logger& log();

	// Enum names

	const char* getEnumName(GLenum enum_) const;
	>>> render_enum_name_methods_decl(groups)

	// Bitset to string

	String bitsetAsString(GLbitfield bitfield) const;
	>>> render_bitset_to_string_method_decl(bitsets)

	// Custom methods

	bool hasExtension(const char* ext) const;
	void checkGlErrors();
	void setEnabled(GLenum cap, bool enabled);
	>>> render_commands_decl(commands)

public:
	>>> render_feat_vars_decl(features)

public:
	>>> render_commands_ptr_decl(commands)

private:
	void* _getProcAddress(const char* proc);

private:
	Logger*  _log;

	bool     _abortOnError;
	bool     _logErrors;

	bool     _logCalls;
	LogLevel _logLevel;

	GlGetProcAddress _glGetProcAddress;
	unsigned _procCount;
};

}

#endif
"""


cpp_template = """>>> header


#include <sstream>

#include <lair/render_gl3/context.h>


namespace lair {


struct BitMask {
	GLbitfield  mask;
	const char* label;
};

String bitsetAsString(const BitMask* masks, GLbitfield value) {
	std::ostringstream out;
	int count = 0;
	while(masks->label) {
		if((value & masks->mask) == masks->mask) {
			value &= ~masks->mask;
			if(count) out << " | ";
			out << masks->label;
			++count;
		}
		++masks;
	}
	if(value) {
		if(count) out << " | ";
		out << "0x" << std::hex << value;
	}
	return out.str();
}


GLOutOfMemory::GLOutOfMemory()
	: std::runtime_error("out of graphic memory") {
}


Context::Context(GlGetProcAddress glGetProcAddress, Logger* logger):
	>>> render_feat_vars_constructor(features)
	>>> render_commands_ptr_constructor(commands)

	_log(logger),
	_abortOnError(false),
	_logErrors(true),
	_logCalls(false),
	_logLevel(LogLevel::Info),
	_glGetProcAddress(glGetProcAddress),
	_procCount(0) {
}


bool Context::initialize(bool debugGl) {
	_abortOnError = debugGl;
	_logCalls = debugGl;

	>>> render_commands_ptr_initialize(commands)

	>>> render_double_check_extensions(extensions)

	if(!{{feature_var_name(api_items.main_feature)}}) {
		log().error("Failed to load {{api}} {{version}}.");
		return false;
	}

	log().info("OpenGL version: ",      getString(gl::VERSION));
	log().info("OpenGL GLSL version: ", getString(gl::SHADING_LANGUAGE_VERSION));
	log().info("OpenGL vendor: ",       getString(gl::VENDOR));
	log().info("OpenGL renderer: ",     getString(gl::RENDERER));
	>>> render_log_enabled_extensions(extensions)

	return {{feature_var_name(api_items.main_feature)}};
}


void Context::shutdown() {
}


Logger& Context::log() {
	return *_log;
}


const char* Context::getEnumName(GLenum enum_) const {
	switch(enum_) {
		>>> render_get_enum_name_cases(enums)
	}
	return "<Unknown GLenum>";
}

>>> render_enum_name_methods_impl(groups)


String Context::bitsetAsString(GLbitfield bitfield) const {
	static const BitMask masks[] = { { 0, nullptr } };
	return lair::bitsetAsString(masks, bitfield);
}

>>> render_bitset_to_string_method_impl(bitsets)


bool Context::hasExtension(const char* ext) const {
	GLint count = 0;
	_glGetIntegerv(gl::NUM_EXTENSIONS, &count);
	for(GLint i = 0; i < count; ++i) {
		const char* e = (const char*)_glGetStringi(gl::EXTENSIONS, i);
		if(strcmp(e, ext) == 0)
			return true;
	}
	return false;
}

void Context::checkGlErrors() {
	GLenum error;
	while((error = _glGetError()) != gl::NO_ERROR) {
		if(error == gl::OUT_OF_MEMORY)
			throw GLOutOfMemory();

		if(_logErrors) {
			_log->error("OpenGL error: ", getErrorCodeName(error));
		}

		if(_abortOnError) {
			abort();
		}
	}
}


void Context::setEnabled(GLenum cap, bool enabled) {
	if(enabled) {
		enable(cap);
	} else {
		disable(cap);
	}
}

>>> render_commands_impl(commands)

void* Context::_getProcAddress(const char* proc) {
	void* ptr = _glGetProcAddress(proc);
	if(ptr) ++_procCount;
	return ptr;
}

}
"""


_protected_enums = set() #set([ 'GL_TRUE', 'GL_FALSE' ])
def enum_name(enum_name):
	if enum_name[3] in ascii_letters and enum_name not in _protected_enums:
		return enum_name[3:]
	return enum_name[2:]

def get_enum_name_method_name(group_name):
	return 'get{group_name}Name'.format_map(vars())

def bitset_to_string_method_name(bitset_name):
	return 'bitset{bitset_name}AsString'.format_map(vars())

def command_method_name(command_name):
	return command_name[2].lower() + command_name[3:]

def command_ptr_type_name(command_name):
	return ''.join([ '_Pfn', command_name[0].upper(), command_name[1:] ])

def command_ptr_name(command_name):
	return '_' + command_name

def feature_var_name(feat):
	if isinstance(feat, Feature):
		version = feat.number.replace('.', '_')
		return '_{feat.api}_{version}'.format_map(vars());
	return '_' + feat.name.lower();


def render_types(types):
	return join_lines(map(lambda i: i.spec, types))

def render_enums(enums):
	width = max(map(lambda e: len(enum_name(e[1].name)), enums))
	render = lambda e: '{:{width}} = {},'.format(enum_name(e.name), e.value, width=width)
	return render_by_feature(enums, render)

def render_enum_name_methods_decl(groups):
	def render(g):
		method_name = get_enum_name_method_name(g[0].name)
		return 'const char* {method_name}(GLenum enum_) const;'.format_map(vars())
	return join_lines(map(render, groups))

def render_bitset_to_string_method_decl(bitsets):
	def render(g):
		method_name = bitset_to_string_method_name(g[0].group)
		return 'String {method_name}(GLbitfield bitfield) const;'.format_map(vars())
	return join_lines(map(render, bitsets))

def render_commands_decl(commands):
	return render_by_feature(commands, render_command_decl)

def render_command_decl(command):
	type = command.type.strip()
	name = command_method_name(command.name)
	params = ', '.join(map(Param.as_c, command.params))
	return '{type} {name}({params});'.format_map(vars())

def render_feat_vars_decl(features):
	lines = []
	for feat in features:
		name = feature_var_name(feat)
		lines.append('bool {name};'.format_map(vars()))
	return join_lines(lines)

def render_feat_vars_constructor(features):
	lines = []
	for feat in features:
		name = feature_var_name(feat)
		lines.append('{name}(false),'.format_map(vars()))
	return join_lines(lines)

def render_commands_ptr_decl(commands):
	return render_by_feature(commands, render_command_ptr_decl)

def render_command_ptr_decl(command):
	type = command.type.strip()
	tname = command_ptr_type_name(command.name)
	fname = command_ptr_name(command.name)
	params = ', '.join(map(Param.as_c, command.params))
	return 'typedef {type} (APIENTRYP {tname})({params});\n{tname} {fname};'.format_map(vars())


def render_commands_ptr_constructor(commands):
	def render(c):
		name = command_ptr_name(c.name)
		return '{name}(nullptr),'.format_map(vars())
	return render_by_feature(commands, render)

def render_commands_ptr_initialize(commands):
	def render(c):
		ptr_type = command_ptr_type_name(c.name)
		ptr_name = command_ptr_name(c.name)
		return '{ptr_name} = ({ptr_type})_getProcAddress("{c.name}");'.format_map(vars())

	lines = []
	cur_feat = None
	count = 0
	prev_feat_var = ''
	feat_var = ''
	def render_group_header(name):
		lines.append('')
		lines.append('// {name}'.format_map(vars()))
		lines.append('')
		lines.append('_procCount = 0;')
	def render_group_footer(feat_var, count, prev_feat_var):
		if prev_feat_var:
			lines.append('{feat_var} = ({prev_feat_var} && _procCount == {count});'.format_map(vars()))
		else:
			lines.append('{feat_var} = (_procCount == {count});'.format_map(vars()))

	for feat, item in commands:
		if feat != cur_feat:
			if cur_feat:
				render_group_footer(feat_var, count, prev_feat_var)

			cur_feat = feat
			count = 0
			prev_feat_var = feat_var if isinstance(feat, Feature) else ''
			feat_var = feature_var_name(cur_feat)

			render_group_header(cur_feat.name)
		count += 1
		lines.append(render(item))
	render_group_footer(feat_var, count, prev_feat_var)

	return join_lines(lines)

def render_double_check_extensions(extensions):
	lines = []
	for name in extensions:
		ext = specs.extension_index[name]
		var = feature_var_name(ext)
		if name in api_items.extension_with_commands:
			lines.append('{var} = {var} && hasExtension("{name}");'.format_map(vars()))
		else:
			lines.append('{var} = hasExtension("{name}");'.format_map(vars()))
	return join_lines(lines)

def render_log_enabled_extensions(extensions):
	lines = []
	for name in extensions:
		ext = specs.extension_index[name]
		var = feature_var_name(ext)
		lines.append('if({var}) log().info("OpenGL enable extension: {ext.name}");'.format_map(vars()))
	return join_lines(lines)

def render_get_enum_name_cases(enums):
	enum_by_value = {}
	for f, e in enums:
		if e.value.startswith('0x'):
			value = int(e.value[2:], 16)
		else:
			value = int(e.value)
		enum_by_value.setdefault(value, []).append(e.name)

	lines = []
	max_k = 1 << 32
	for k, v in sorted(enum_by_value.items(), key=lambda e: e[0]):
		if k > max_k:
			continue
		if len(v) == 1:
			values = v[0]
		else:
			values = '[{}]'.format('|'.join(v))
		lines.append('case 0x{k:08x}: return "{values}";'.format_map(vars()))

	return join_lines(lines)

enum_name_methods_impl_tmpl = """const char* Context::{{name}}(GLenum enum_) const {
	switch(enum_) {
		>>> render_group_cases(items)
	}
	return "<Unknown {{group_name}}>";
}
"""
def render_enum_name_methods_impl(groups):
	lines = []
	for group, items in groups:
		context = dict(
			name = get_enum_name_method_name(group.name),
			group_name = group.name,
			items = items,
		)
		lines.append(render_template(enum_name_methods_impl_tmpl, context))
	return join_lines(lines)

def render_group_cases(items):
	lines = []
	done = set()
	for e in items:
		value = specs.enum_index[e][0][1].value
		if value in done:
			continue
		done.add(value)
		lines.append('case {value}: return "{e}";'.format_map(vars()))
	return join_lines(lines)

bitset_to_string_method_impl_tmpl = """String Context::{{name}}(GLbitfield bitfield) const {
	static const BitMask masks[] = {
		>>> render_bitset_bitmasks(values)
		{ 0, nullptr }
	};
	return lair::bitsetAsString(masks, bitfield);
}
"""
def render_bitset_to_string_method_impl(bitsets):
	lines = []
	for bitset, enames in bitsets:
		context = dict(
			name = bitset_to_string_method_name(bitset.group),
			values = enames,
		)
		lines.append(render_template(bitset_to_string_method_impl_tmpl, context))
	return join_lines(lines)

def render_bitset_bitmasks(values):
	lines = []
	for e in values:
		value = specs.enum_index[e][0][1].value
		lines.append('{{ {value}, "{e}" }},'.format_map(vars()))
	return join_lines(lines)

def render_commands_impl(commands):
	return render_by_feature(commands, render_command_impl)

void_command_impl_tmpl = """void Context::{{name}}({{params_decl}}) {
	if(_logCalls) {
		>>> render_log_params(command)
	}
	{{gl_name}}({{params_call}});
	checkGlErrors();
}
"""
ret_command_impl_tmpl = """{{ret_type}} Context::{{name}}({{params_decl}}) {
	if(_logCalls) {
		>>> render_log_params(command)
	}
	{{ret_type}} result = {{gl_name}}({{params_call}});
	checkGlErrors();
	return result;
}
"""
def render_command_impl(command):
	ret_type = command.type.strip()
	template = ret_command_impl_tmpl
	if ret_type == 'void':
		template = void_command_impl_tmpl

	context = dict(
		command = command,
		ret_type = ret_type,
		name = command_method_name(command.name),
		gl_name = command_ptr_name(command.name),
		params_decl = ', '.join(map(Param.as_c, command.params)),
		params_call = ', '.join(map(lambda p: p.name, command.params)),
	)
	return render_template(template, context)

def render_log_params(command):
	name = command.name
	if command.params:
		params = ', ", ", '.join(map(render_log_param, command.params))
		return '_log->write(_logLevel, "{name}(", {params}, ")");\n'.format_map(vars())
	return  '_log->write(_logLevel, "{name}()");\n'.format_map(vars())

def render_log_param(param):
	if '*' not in param.type and (param.group or param.ptype == 'GLbitfield'):
		get_name = None
		if param.group in api_items.bitset_names:
			get_name = bitset_to_string_method_name(param.group)
		elif param.ptype == 'GLbitfield':
			get_name = "bitsetAsString"
		elif param.group in api_items.groups_by_name:
			get_name = get_enum_name_method_name(param.group)
		if get_name:
			return '{get_name}({param.name})'.format_map(vars())
	return param.name

def render_by_feature(items, render):
	lines = []
	cur_feat = None
	for feat, item in items:
		if feat != cur_feat:
			cur_feat = feat
			lines.append('')
			lines.append('// {feat.name}'.format_map(vars()))
			lines.append('')
		lines.append(render(item))
	return join_lines(lines)


_whitespace_re = re.compile(r'^\w*$')
def eval_template_command(match, context):
	if match.group('lcmd'):
		indent = match.group('indent')
		code = eval(match.group('lcmd').strip(), None, context)
		if indent:
			code = code.split('\n')
			for i, line in enumerate(code):
				if _whitespace_re.fullmatch(line):
					code[i] = ''
				else:
					code[i] = indent + code[i]
			code = '\n'.join(code)
		return code
	return eval(match.group('icmd').strip(), None, context)

_template_re = re.compile(r'^(?P<indent>[ \t]*?)>>>(?P<lcmd>.*)\n|\{\{(?P<icmd>.*?)\}\}', re.MULTILINE)
def render_template(template, context):
	return _template_re.sub(lambda m: eval_template_command(m, context), template)

def join_lines(lines):
	return '\n'.join(chain(lines, ['']))

###############################################################################
# Main

if __name__ == '__main__':
	print("Read specs...")

	specs = Specs(ElementTree.parse("gl.xml"))

	api     = 'gl'
	version = '3.3'
	profile = 'core'
	extensions = [
		'GL_EXT_texture_filter_anisotropic',
		'GL_KHR_debug',
		'GL_ARB_conservative_depth',
		#'GL_ARB_ES2_compatibility',
		#'GL_ARB_texture_buffer_range',
		'GL_ARB_texture_storage',
		#'GL_ARB_vertex_attrib_binding',
		#'GL_ARB_viewport_array',
	]

	print("Select features...")
	api_items = ApiItems(specs, api, version, profile, extensions)

	params = api_items.__dict__
	with open('context.h', 'w') as h_out:
		print("Write context.h...")
		h_out.write(render_template(h_template, params))

	with open('context.cpp', 'w') as cpp_out:
		print("Write context.cpp...")
		cpp_out.write(render_template(cpp_template, params))
