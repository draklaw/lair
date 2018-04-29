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
from random import Random
from pprint import pprint
from colorsys import hsv_to_rgb

from PIL import Image
from PIL.ImageDraw import Draw


def halton(i, base=2):
	"""Return the ith element of the Halton sequence of a given base."""

	f = 1
	r = 0
	while i > 0:
		f  = f / base
		r += f * (i % base)
		i  = i // base
	return r


def hsva(h, s, v, a):
	"""Converts float hsva color to int rgba usable with PIL."""

	hsva = hsv_to_rgb(h, s, v) + (a,)
	return tuple(map(lambda x: int(x * 255), hsva))


class Vector(list):
	def __init__(self, *args):
		if len(args) == 1:
			super().__init__(*args)
		else:
			super().__init__(args)

	def apply(self, func, *args):
		coeffs = self[:]
		for i, c in enumerate(coeffs):
			extra = [ c[i] for c in args ]
			coeffs[i] = func(c, *extra)
		return Vector(coeffs)

	def __add__(self, rhs):
		return self.apply(lambda a, b: a + b, rhs)

	def __sub__(self, rhs):
		return self.apply(lambda a, b: a - b, rhs)

	def __mul__(self, rhs):
		return self.apply(lambda a: a * rhs)

	def __rmul__(self, lhs):
		return self.apply(lambda a: a * lhs)


class Box:
	def __init__(self, pos, size, page = 0, data = None):
		self._min  = Vector(pos)
		self._size = Vector(size)
		self._page = page
		self.data  = data

	def __repr__(self):
		return 'Box({!r}, {!r}, {!r})'.format(self._min, self._size, self._page)

	def min(self, index = None):
		if index is None:
			return Vector(self.min(0), self.min(1))
		return self._min[index]

	def max(self, index = None):
		if index is None:
			return Vector(self.max(0), self.max(1))
		return self._min[index] + self._size[index]

	def size(self, index = None):
		if index is None:
			return Vector(self.size(0), self.size(1))
		return self._size[index]

	def page(self):
		return self._page

	def place(self, index_pos, value = None):
		if value is None:
			self.place(0, index_pos[0])
			self.place(1, index_pos[1])
		else:
			self._min[index_pos] = value

	def set_min(self, index_pos, value = None):
		if value is None:
			self.set_min(0, index_pos[0])
			self.set_min(1, index_pos[1])
		else:
			self._size[index_pos] = self.max(index_pos) - value
			self._min[index_pos]  = value

	def set_max(self, index_pos, value = None):
		if value is None:
			self.set_max(0, index_pos[0])
			self.set_max(1, index_pos[1])
		else:
			self._size[index_pos] = value - self._min[index_pos]

	def set_size(self, index_size, value = None):
		if value is None:
			self.set_size(0, index_size[0])
			self.set_size(1, index_size[1])
		else:
			self._size[index_size] = value

	def set_page(self, page):
		self._page = page

	def area(self):
		return self.size(0) * self.size(1)

	def intersects(self, other):
		return (self.page() == other.page() and
		        self.min(0) < other.max(0) and self.max(0) > other.min(0) and
		        self.min(1) < other.max(1) and self.max(1) > other.min(1))


def random_boxes(rng, count, min_size, max_size, mean, variance):
	"""Returns a list of boxes of size included between min_size and max_size
	with the given distribution (mean/variance)."""

	u   = (mean - min_size) / (max_size - min_size)
	var = variance / (max_size - min_size)

	if var >= u * (1 - u):
		print("Warning: invalid variance")

	v     = (u * (1 - u)) / var - 1
	alpha = u * v
	beta  = (1 - u) * v

	def rand_size():
		return min_size + int(round(rng.betavariate(alpha, beta) * (max_size - min_size)))

	boxes = []
	for i in range(count):
		boxes.append(Box((0, 0), (rand_size(), rand_size())))

	return boxes


def pack_naive(boxes, width, height, incremental = None, validate = False):
	"""A naive pack implementation."""

	packed = sorted(boxes, key=lambda b: (b.size(1), b.size(0)), reverse=True)

	x = 0
	y = 0
	next_row = 0
	for box in packed:
		x2 = x + box.size(0)
		if x2 > width:
			x = 0
			y = next_row
			next_row = 0
		box.place(Vector(x, y))
		x += box.size(0)
		next_row = max(next_row, y + box.size(1))

	return packed

class DensePacker:
	"""Implementation of a bin packing algorithm that tries to fill empty space."""

	def __init__(self, size):
		self.size = size
		self.pi = 0
		self.places = []
		self.packed = []
		self.page_count = 0
		self.fill_space = False # Doesn't work.

	def add_place(self, box):
		box._i = self.pi
		self.pi += 1
		self.places.append(box)

	def add_page(self):
		self.add_place(Box((0, 0), self.size, self.page_count))
		self.page_count += 1
		return len(self.places) - 1, self.places[-1]

	def add_packed(self, box):
		self.packed.append(box)

	def insert(self, box):
		# print('  Places', box)
		# for p in self.places:
		# 	print('   ', p)

		i, p = self.find_place(box.size())
		if i < 0:
			i, p = self.add_page()

		# print('  PLACE', p)
		box.place(p.min())
		box.set_page(p.page())
		self.add_packed(box)

		self.add_top_box(box, i, p)
		self.update_side_box(box, i, p)

		self.sort()
		self.resize_intersections(box)
		self.remove_dead_places()

	def find_place(self, size):
		for i, p in enumerate(self.places):
			if size[0] <= p.size(0)  and size[1] <= p.size(1):
				return i, p
		return -1, None

	def cast_ray(self, page, start, axis, dir):
		orth = 1 - axis
		p    = 0 if dir < 0 else self.size[axis]
		for box in self.packed:
			if box.page() == page and box.min(orth) <= start[orth] < box.max(orth):
				if dir < 0:
					if box.max(axis) <= start[axis]:
						p = max(p, box.max(axis))
					elif box.min(axis) <= start[axis]:
						p = start[axis]
				else:
					if start[axis] < box.min(axis):
						p = min(p, box.min(axis))
					elif start[axis] < box.max(axis):
						p = start[axis]
		return p

	def add_top_box(self, box, i, p):
		if box.max(1) < p.max(1):
			pos  = Vector(box.min(0), box.max(1))
			size = Vector(p.size(0), p.size(1) - box.size(1))
			if self.fill_space:
				max_x  = self.cast_ray(box.page(), pos, 0, 1)
				pos[0] = self.cast_ray(box.page(), pos, 0, -1)
				size[0] = max_x - pos[0]
			self.add_place(Box(pos, size, p.page()))
			# print('  TOP', self.places[-1])

	def update_side_box(self, box, i, p):
		if box.max(0) < p.max(0):
			cursor = Vector(box.max(0), box.min(1))
			best_enclosing = None
			# print('  SEARCH SIDE', cursor)
			for j in range(i-1, -1, -1):
				q = self.places[j]
				if q.page() != p.page():
					break

				# print('   ', q)
				if q.min(0) <= cursor[0] < q.max(0):
					if q.max(1) >= cursor[1]:
						cursor[1] = q.min(1)
						best_enclosing = q
						# print('    Move:', cursor)
					else:
						# print('    Break')
						break

			if best_enclosing and best_enclosing.min() == cursor:
				del self.places[i]
				if self.fill_space:
					best_enclosing.set_max(1, p.max(1))
				else:
					best_enclosing.set_max(1, box.max(1))
			else:
				p.set_min(cursor)
				if self.fill_space:
					p.set_max(1, p.max(1))
				else:
					p.set_max(1, box.max(1))
				# print('  SIDE ', p)
		else:
			del self.places[i]

	def resize_intersections(self, box):
		for p in self.places:
			if p.page() == box.page() and p.intersects(box):
				for i in range(2):
					if p.min(i) < box.min(i):
						p.set_size(i, box.min(i) - p.min(i))
				if p.min() == box.min():
					p.set_page(-1)
				elif p.intersects(box):
					p.set_min(1, box.max(1))
					if p.size(1) <= 0:
						p.set_page(-1)

	def remove_dead_places(self):
		self.places = list(filter(lambda p: p.page() >= 0, self.places))

	def sort(self):
		self.places.sort(key=lambda p: (p.page(), p.min(1), p.min(0)))


def pack_dense(boxes, width, height, incremental = None, validate = False):
	"""Pack boxes in a set of pages of size width, height using DensePacker.
	Extra arguments are for debugging."""

	packed = sorted(boxes, key=lambda b: (b.size(1), b.size(0)), reverse=True)

	packer = DensePacker((width, height))
	for i, box in enumerate(packed):
		if incremental:
			print('INSERT', i, box)
		packer.insert(box)

		if incremental:
			img = Image.new('RGB', (width * packer.page_count, height), (0, 0, 0, 255))
			d = Draw(img, 'RGBA')

			for j, box in enumerate(packed[:i+1]):
				hue = halton(j, 3)
				offset = Vector(width * box.page(), 0)
				draw_box(d, box, hsva(hue, .5, .5, 1), hsva(hue, .2, 1, 1), offset)

			for j, p in enumerate(packer.places):
				hue = halton(p._i)
				offset = Vector(width * p.page(), 0)
				draw_box(d, p, hsva(hue, 1, 1, 1), hsva(hue, 1, 1, .25), offset)
			img.save('{}_{:04d}.png'.format(incremental, i))

		if validate:
			for i1 in range(i):
				box1 = packed[i1]
				assert not box.intersects(box1)

			for box in packed[:i+1]:
				for p in packer.places:
					assert not box.intersects(p)

	return packed


def draw_box(d, box, outline = None, fill = None, offset = None):
	p0 = box.min()
	p1 = box.max()
	if outline:
		p1 = p1 - Vector(1, 1)
	if offset:
		p0 = p0 + offset
		p1 = p1 + offset
	d.rectangle([tuple(p0), tuple(p1)], outline = outline, fill = fill)


def have_intersection(boxes):
	for i0, box0 in enumerate(boxes):
		for i1 in range(i0):
			box1 = boxes[i1]
			if box0.intersects(box1):
				return True
	return False


def usage(argv):
	print("""Usage: {} [-c count] [-p pack-method] [-s seed] <out-filename>
pack-method can be: naive (default), dense""".format(argv[0]), file=stderr)
	exit(1)

def main(argv):
	count = 32
	limit = None
	seed  = None
	validate = False
	incremental = None
	out_filename = None
	img_size = 256
	min_size = 2
	max_size = 192
	mean     = 24
	# variance = 2
	variance = 2
	pack     = pack_dense

	arg_it = iter(argv[1:])
	for arg in arg_it:
		if arg[0] == '-':
			if arg == '-c':
				count = int(next(arg_it))
			elif arg == '-i':
				incremental = next(arg_it)
			elif arg == '-l':
				limit = int(next(arg_it))
			elif arg == '-p':
				arg = next(arg_it)
				pack = globals().get('pack_' + arg)
				if pack is None:
					print("Unknown pack method '{}'".format(arg), file=stderr)
					usage(argv)
			elif arg == '-s':
				seed = next(arg_it)
			elif arg == '-v':
				validate = True
			else:
				print("Unknown opiton '{}'.".format(arg), file=stderr)
				usage(argv)
		elif out_filename is None:
			out_filename = arg
		else:
			usage(argv)

	if out_filename is None:
		usage(argv)

	rng = Random()
	rng.seed(seed)

	boxes = random_boxes(rng, count, min_size, max_size, mean, variance)
	if limit is not None:
		boxes = sorted(boxes, key=lambda b: (b.size(1), b.size(0)), reverse=True)
		boxes = boxes[:limit]

	packed = pack(boxes, img_size, img_size, incremental = incremental, validate = validate)

	if validate:
		assert not have_intersection(packed)

	page_count = 0
	for box in packed:
		page_count = max(page_count, box.page() + 1)

	img = Image.new('RGBA', (img_size * page_count, img_size), (0, 0, 0, 255))
	d = Draw(img)
	for i, box in enumerate(packed):
		color = hsv_to_rgb(halton(i, 2), 1, 1)
		outline = tuple(map(lambda x: int(x * 255), color)) + (255,)
		fill    = tuple(map(lambda x: int(x * 63 + 192), color)) + (255,)

		offset = Vector(img_size * box.page(), 0)
		p0 = tuple(box.min() + offset)
		p1 = tuple(box.max() + offset - Vector(1, 1))
		d.rectangle([p0, p1], outline = outline, fill = fill)
	img.save('{}.png'.format(out_filename))

	area = sum(map(Box.area, packed))

	max_height = 0
	for box in packed:
		if box.page() == page_count - 1:
			max_height = max(max_height, box.max(1))
	atlas_area = img_size * (img_size * (page_count - 1) + max_height)

	print("Area:       {:7d}px^2".format(area))
	print("Atlas area: {:7d}px^2".format(atlas_area))
	print("Occupancy:  {:4.2f}%".format(area / atlas_area * 100))

if __name__ == '__main__':
	main(argv)
