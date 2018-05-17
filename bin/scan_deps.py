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
from pathlib import Path
import re

import graphviz


_include_re = re.compile(r'^\s*#\s*include\s[<"](.*)[>"]\s*$')
class SourceManager:
    def __init__(self):
        self.sources = {}

    def add_source(self, path):
        if path in self.sources:
            return self.sources[path]

        source = SourceFile(path)
        self.sources[path] = source
        self._add_dependencies(source)

        return source

    def add_sources(self, sources):
        for source in sources:
            self.add_source(source)

    def module_graph(self, out_path):
        graph = graphviz.Digraph(engine = 'dot', format='svg')
        graph.graph_attr['rankdir'] = 'BT'
        graph.node_attr['shape'] = 'rectangle'

        modules = set(map(lambda s: s.module, self.sources.values()))

        # Json stuff
        modules.discard('src')
        modules.discard('cpptl')

        edges = set()
        for source in self.sources.values():
            if source.module not in modules:
                continue

            for dep in source.depends_of:
                if dep.module not in modules or dep.module == source.module:
                    continue

                edges.add((source.module, dep.module))

        for e in edges:
            graph.edge(*e)

        graph.save(out_path)
        graph.render(filename = out_path)


    def draw_graph(self, out_path):
        graph = graphviz.Digraph(engine = 'dot', format='svg')
        #graph.graph_attr['concentrate'] = 'true'

        cluster_map = {}
        def g(source):
            if source.module not in cluster_map:
                g = graphviz.Digraph('cluster_'+source.module)
                cluster_map[source.module] = g
#                graph.subgraph(g)
            return cluster_map[source.module]

        node_map = {}
        def name(source):
            if source not in node_map:
                node_map[source] = '{}/{}'.format(source.module, source.path.name)
            return node_map[source]

        def skip(source):
            return source.module not in ('core', 'sys_sdl2')

        for source in self.sources.values():
            if skip(source):
                continue
            g(source).node(name(source))

        for source in self.sources.values():
            if skip(source):
                continue
            for dep in source.depends_of:
                if skip(dep):
                    continue
                graph.edge(node_map[source], node_map[dep])

        for g in cluster_map.values():
            graph.subgraph(g)

        graph.save(out_path)
        graph.render(filename = out_path)

    def _add_dependencies(self, source):
        if not source.path.is_file():
            return
        with source.path.open() as f:
            for line in f:
                match = _include_re.match(line)
                if match:
                    dep = self.add_source(Path(match.group(1)))
                    source.depends_of.add(dep)
                    dep.included_in.add(source)

class SourceFile:
    def __init__(self, path):
        self.path        = path
        self.module      = self.get_module(path)
        self.depends_of  = set()
        self.included_in = set()
        self.node        = None

    _module_map = {
        'Python.h': 'python',
        'json_tool.h':            'json',
        'json_valueiterator.inl': 'json',
        'math.h':              'stdlib',
        'float.h':             'stdlib',
        'ieeefp.h':            'stdlib',
        'io.h':                'stdlib',
        'unistd.h':            'stdlib',
        'ext/stdio_filebuf.h': 'stdlib',
    }
    @classmethod
    def get_module(cls, path):
        if str(path) in cls._module_map:
            return cls._module_map[str(path)]
        elif path.parent != Path('.'):
            return path.parent.name
        elif path.name.startswith("SDL"):
            return 'SDL'
        elif path.suffix:
            print('Warning: source file "{}": unkonwn module.'.format(path), file = stderr)
            return '<unknown>'
        else:
            return 'stdlib'

    def __hash__(self):
        return hash(self.path)

    def __eq__(self, other):
        return self.path == other.path

    def __repr__(self):
        return '<SourceFile {!r} {!r}>'.format(self.module, self.path)

    def __lt__(self, other):
        return self.path < other.path


def print_deps(sources, module):
    module_deps = {}
    ignore = set((module, 'stdlib', '<unknown>'))
    for source in sources.sources.values():
        if source.module != module:
            continue

        modules = set()
        for dep in source.depends_of:
            modules.add(dep.module)
        modules -= ignore

        if modules:
            module_deps[source] = modules

    print("{} deps:".format(module))
    for source, deps in sorted(module_deps.items(), key = lambda x: x[0]):
        print('  {}: {}'.format(source, ', '.join(deps)))

def main(argv):
    dirs = []

    arg_it = iter(argv[1:])
    for arg in arg_it:
        dir = Path(arg)
        if not dir.is_dir():
            print("\"{}\" is not a directory.".format(dir), file = stderr)
            exit(1)
        dirs.append(dir)

    print("Dirs:")
    for dir in dirs:
        print('  {}'.format(dir))

    sources = SourceManager()
    for dir in dirs:
        sources.add_sources(dir.glob('**/*.h'))
        sources.add_sources(dir.glob('**/*.cpp'))

    sources.module_graph('deps')

    print_deps(sources, 'core')
    # core_deps = {}
    # for source in sources.sources.values():
    #     if source.module != 'core':
    #         continue
    #
    #     modules = set()
    #     for dep in source.depends_of:
    #         modules.add(dep.module)
    #     modules -= set(('core', 'stdlib', '<unknown>'))
    #
    #     if modules:
    #         core_deps[source] = modules
    #
    # print("core deps:")
    # for source, deps in sorted(core_deps.items(), key = lambda x: x[0]):
    #     print('  {}: {}'.format(source, ', '.join(deps)))


if __name__ == '__main__':
    main(argv)
