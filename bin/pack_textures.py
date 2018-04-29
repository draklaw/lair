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


from sys import argv, stderr, stdout
from pathlib import Path

from PIL import Image
from PIL.ImageDraw import Draw

from ldl import LdlReader, LdlWriter, TypedDict, TypedList
from lair import Vector, Box as LairBox, Sampler
from pack import Box, pack_dense


def set_border(img, border, rect):
	"""
	Add a 1px border in `img` around `rect`.

	If `border` is "clamp" or "transparent", the border extends the imgage
	(it copies border pixels). if `border` is "repeat", it copy the opposite
	border pixels.
	"""

	x0, y0, x1, y1 = rect

	first_line_src =   x0,   y0,   x1, y0+1
	last_line_src  =   x0, y1-1,   x1,   y1
	first_row_src  =   x0, y0-1, x0+1, y1+1
	last_row_src   = x1-1, y0-1,   x1, y1+1

	first_line_dst =   x0, y0-1
	last_line_dst  =   x0,   y1
	first_row_dst  = x0-1, y0-1
	last_row_dst   =   x1, y0-1

	if border in ('clamp', 'transparent'):
		img.paste(img.crop(first_line_src), first_line_dst)
		img.paste(img.crop(last_line_src),  last_line_dst)
		img.paste(img.crop(first_row_src),  first_row_dst)
		img.paste(img.crop(last_row_src),   last_row_dst)
	elif border == 'repeat':
		img.paste(img.crop(first_line_src), last_line_dst)
		img.paste(img.crop(last_line_src),  first_line_dst)
		img.paste(img.crop(first_row_src),  last_row_dst)
		img.paste(img.crop(last_row_src),   first_row_dst)
	else:
		raise RuntimeError("Unknown border type: '{}'".format(border))

def draw_border(d, rect, color):
	"""
	Draw a 1px border around `rect` using the drawer `d`
	"""

	x0, y0, x1, y1 = rect
	x0 -= 1
	y0 -= 1

	line_top    = (x0, y0, x1, y0)
	line_bottom = (x0, y1, x1, y1)
	line_left   = (x0, y0, x0, y1)
	line_right  = (x1, y0, x1, y1)

	d.line(line_top,    fill = color)
	d.line(line_bottom, fill = color)
	d.line(line_left,   fill = color)
	d.line(line_right,  fill = color)


class SourceImage:
	"""
	Represent an image in an atlas.
	"""

	def __init__(self, image, index):
		self.image = image
		self.index = index
		self.pos   = None


SOURCE_LIST   = 'list'
SOURCE_MAP    = 'map'
class Source:
	"""
	A source image used to build an atlas. Source images can contain several
	sub images represented by the class `SourceImage`.

	There is two kind of source images: "list" contains sub-images indexed from
	0 to n, and "map" where indices are arbitraty integers.
	"""

	def __init__(self, name, sampler, border, type = SOURCE_LIST):
		self.name    = name
		self.type    = type
		self.sampler = sampler
		self.border  = border
		self.images  = []
		self.first   = 0

	def add_image(self, img, index = None):
		if index is None:
			index = len(self.images)
		self.images.append(SourceImage(img, index))

class TexturePacker:
	"""
	Take a set of `Source` images and pack all their sub-images in a single,
	multi-layer atlas texture. The atlas has a pre-defined size `page_size`.

	Can also generate mipmaps. Mipmaps are a bit tricky with atlases though.
	In order to avoid bleeding from one image to the other, `TexturePacker`
	makes sure to not place more than one texture in each cell of `grid_size`.
	Then mipmap are computed to so there is no bleeding between images. This
	limits the maximum number of mipmaps to `log2(min(grid_size))`.

	TODO: implement grid_size and mipmaps
	"""

	def __init__(self, page_size, grid_size = (1, 1), mipmaps = None):
		self.page_size = page_size
		self.grid_size = grid_size
		self.mipmaps   = mipmaps
		self.sources   = []
		self.count     = 0

		if self.mipmaps is None:
			self.mipmaps = min(*grid_size).bit_length() - 1

	def add_source(self, source):
		source.first = self.count
		self.count  += len(source.images)
		self.sources.append(source)

	def pack(self, path, format = 'png'):
		"""Pack all sub-images in a texture array and output a ldl describing
		the atlas."""

		boxes = self._get_boxes()
		boxes = pack_dense(boxes, self.page_size[0], self.page_size[1])

		pages = self._create_pages(boxes)

		page_names = self._write_pages(pages, path, format)

		ldl = self._create_ldl(page_names)

		writer = LdlWriter(path.open('w'))
		writer.write(ldl)

	def _get_boxes(self):
		"""Create one box per sub-image and returns the list."""

		boxes = []
		for source in self.sources:
			for src_image in source.images:
				size = src_image.image.size
				if source.border is not None:
					size = size[0] + 2, size[1] + 2
				box = Box((0, 0), size, data = (source, src_image))
				boxes.append(box)
		return boxes

	def _create_pages(self, boxes):
		"""
		Create the images by pasting sub-images into the atlas layers.

		If required, add a border to the images that respect the way the image
		will be drawn.
		"""

		page_count = max(map(lambda b: b.page(), boxes)) + 1

		page_color = (0, 0, 0, 255)
		pages = [ Image.new('RGBA', self.page_size, page_color) for i in range(page_count) ]

		transparent_borders = {}
		for box in boxes:
			source, src_image = box.data
			image = src_image.image
			page = pages[box.page()]

			p = tuple(box.min())
			if source.border is None:
				page.paste(image, p)
			else:
				p = (p[0] + 1, p[1] + 1)
				page.paste(image, p)

				img_box = p[0], p[1], p[0] + image.width, p[1] + image.height
				set_border(page, source.border, img_box)

				if source.border == 'transparent':
					# Setting alpha channel to 0 needs to separate channels
					transparent_borders.setdefault(box.page(), []).append(img_box)
			src_image.pos = (box.page(),) + p

		# Clear alpha channel for "transparent" borders
		for i, rects in transparent_borders.items():
			page = pages[i]
			r, g, b, a = page.split()
			d = Draw(a)
			for rect in rects:
				draw_border(d, rect, 0)
			pages[i] = Image.merge('RGBA', (r, g, b, a))

		return pages

	def _write_pages(self, pages, path, format):
		"""Write the layers into numbered files."""

		stem = path.stem
		page_names = TypedList()
		for i, page in enumerate(pages):
			name = '{}_{:02d}.{}'.format(stem, i, format)
			page.save(path.with_name(name))
			page_names.append(name)
		return page_names

	def _create_ldl(self, page_names):
		"""Generate the ldl structure that describe the atlas."""

		virtual_textures = TypedDict()
		sprites = TypedList()
		for source in self.sources:
			v_tex = TypedDict(None, [
				('begin',   source.first),
				('count',   len(source.images)),
				('sampler', source.sampler),
			])
			virtual_textures[source.name] = v_tex

			for src_image in source.images:
				sprite = TypedDict(None, [
					('layer', src_image.pos[0]),
					('box',   LairBox(Vector(src_image.pos[1:]),
					                  Vector(src_image.image.size))),
				])
				sprites.append(sprite)

		atlas = TypedDict(None, [
			('size',             Vector(self.page_size)),
			('mipmaps',          self.mipmaps),
			('images',           page_names),
			('virtual_textures', virtual_textures),
			('sprites',          sprites),
		])

		return atlas



def read_cfg(path):
	"""Read the config file and return a ldl structure."""

	with path.open() as in_file:
		reader = LdlReader(in_file)

	return reader.read()


def process_source(packer, source, base_path = Path(),
                   sampler = None, border = None, grid = None, count = None):
	"""Read a portion of the cfg as a source. The root acts like a source with
	extra info. Sources can contains sources, so this function is recursive.
	This is done so that sources can be grouped by property (sampler, border)
	in a hierarchical way."""

	# Deal with a single source
	if isinstance(source, str):
		path = base_path / Path(source)
		img = Image.open(path)

		s = Source(source, sampler, border, SOURCE_LIST)
		if grid is None:
			s.add_image(img)
		else:
			tile_width  = img.size[0] // grid[0]
			tile_height = img.size[1] // grid[1]
			for i in range(count):
				y, x = divmod(i, grid[0])
				rect = (
					x * tile_width,
					y * tile_height,
					(x+1) * tile_width,
					(y+1) * tile_height,
				)
				s.add_image(img.crop(rect))
		packer.add_source(s)
		return

	if 'sampler' in source:
		sampler = Sampler(source['sampler'])
	border  = source.get('border',  border)
	if source.get('sources') is not None:
		grid  = source.get('grid', None)
		count = source.get('count', None)
		if count is None and grid is not None:
			count = grid[0] * grid[1]
		for s in source.get('sources'):
			process_source(packer, s, base_path = base_path,
			               sampler = sampler, border = border,
			               grid = grid, count = count)
	elif source.get('font'):
		raise NotImplemented()
	elif source.get('map'):
		raise NotImplemented()


def usage(argv, exit_code = 1, file = stderr):
	print("Usage: {} [-h] <cfg_path> <texture_path>"
		.format(argv[0]), file = file)
	exit(exit_code)

def main(argv):
	cfg_path     = None
	texture_path = None

	arg_it = iter(argv[1:])
	for arg in arg_it:
		if arg and arg[0] == '-':
			if arg == '-h' or arg == '--help':
				usage(argv, 0, stdout)
			else:
				print("Invalid option '{}'".format(arg), file = stderr)
				usage(argv)
		elif cfg_path is None:
			cfg_path = Path(arg)
		elif texture_path is None:
			texture_path = Path(arg)
		else:
			usage(argv)

	if cfg_path is None:
		print("<cfg_path> is mandatory.", file = stderr)
		usage(argv)

	if texture_path is None:
		print("<texture_path> is mandatory.", file = stderr)
		usage(argv)

	if not cfg_path.is_file():
		print("{} does not exists or is not a file.".format(cfg_path), file = stderr)

	cfg = read_cfg(cfg_path)

	packer = TexturePacker(
		page_size = tuple(cfg.get('page_size', (1024, 1024))),
		grid_size = tuple(cfg.get('grid_size', (1, 1))),
		mipmaps   = cfg.get('mipmaps', False),
	)

	process_source(packer, cfg, base_path = cfg_path.parent)

	packer.pack(texture_path)

if __name__ == '__main__':
	main(argv)
