#!/usr/bin/env python3

#
#    el-copy-map.py - Copy Eternal Lands map regions
#    Copyright (C) 2014 Cole Minor
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

__version__ = '0.2.1'

import sys
import os
import re
import struct
import gzip
from argparse import ArgumentParser
from os.path import basename, dirname, abspath
from array import array
from io import BytesIO
from copy import deepcopy
from math import floor

_line_pattern = re.compile(r'^(\d*)([xcbB?hHiIlLqQfds])\s+([a-zA-Z_]\w*)$')
_reserved = ('size', 'fields')

def namedstruct(clsname, fdef):
    _fields = []
    _fmts = []
    for line in fdef.splitlines():
        s = line.split('#', 1)[0].strip()
        if not s:
            continue
        m = _line_pattern.match(s)
        if not m:
            e = 'Invalid field definition: %s' % s
            raise ValueError(e)
        c, t, n = m.groups()
        if n in _reserved:
            e = 'Invalid field name: %s' % n
            raise ValueError(e)
        _fmts.append(c + t if c else t)
        c = int(c) if c else 1
        if t != 'x':
            _fields.append((c, t, n))

    _st = struct.Struct('<' + ''.join(_fmts))
    _dict = {
        'size': _st.size,
        'fields': [e[2] for e in _fields],
    }

    def method(f):
        _dict[f.__name__] = f
        return f
    
    @method
    def __init__(self, f=None):
        if f is not None:
            self.fromfile(f)
            return
        for c, t, n in _fields:
            if t == 's':
                v = b''
            else:
                v = 0 if c == 1 else [0] * c
            setattr(self, n, v)

    @method
    def fromfile(self, f):
        d = f.read(_st.size)
        a = _st.unpack(d)
        i = 0
        for c, t, n in _fields:
            if c == 1 or t == 's':
                v = a[i]
            else:
                v = list(a[i:i + c])
            setattr(self, n, v)
            i += 1 if t == 's' else c

    @method
    def tofile(self, f):
        a = []
        for c, t, n in _fields:
            v = getattr(self, n)
            a += [v] if c == 1 or t == 's' else v
        d = _st.pack(*a)
        f.write(d)

    return type(clsname, (), _dict)

MAP_SIG = b'elmf'

MapHeader = namedstruct('MapHeader', '''
    4s signature
    I ground_xsize
    I ground_ysize
    I ground_offset
    I tile_offset
    I mesh_size
    I mesh_count
    I mesh_offset
    I quad_size
    I quad_count
    I quad_offset
    I light_size
    I light_count
    I light_offset
    ? interior
    B version
    2x unused
    3f ambient_light
    I fuzz_size
    I fuzz_count
    I fuzz_offset
    I segment_offset
    36x unused
''')

MeshRecord = namedstruct('MeshRecord', '''
    80s name
    3f position
    3f rotation
    ? unlit
    B blend
    2x unused
    3f color
    f scale
    20x unused
''')

QuadRecord = namedstruct('QuadRecord', '''
    80s name
    3f position
    3f rotation
    24x unused
''')

LightRecord = namedstruct('LightRecord', '''
    3f position
    3f color
    3B specular
    b direction_zsign
    H attenuation
    H range
    h cutoff
    h exponent
    2h direction
''')

FuzzRecord = namedstruct('FuzzRecord', '''
    80s name
    3f position
    12x unused
''')

class Tile:
    def __init__(self, x, y):
        self.x = x
        self.y = y

class Rect:
    def __init__(self, x0, y0, x1, y1):
        self.x0 = x0
        self.y0 = y0
        self.x1 = x1
        self.y1 = y1
        self.w = x1 - x0 + 1
        self.h = y1 - y0 + 1

    def as_tuple(s):
        return s.x0, s.y0, s.x1, s.y1

    def __contains__(self, t):
        return (self.x0 <= t[0] <= self.x1
            and self.y0 <= t[1] <= self.y1)

def open_map(p, m):
    if p.endswith('.gz'):
        return gzip.open(p, mode=m)
    return open(p, m)

def spatial_key(e):
    p = e.position
    x = floor(p[0] * 2.0)
    y = floor(p[1] * 2.0)
    return x, y

class Map:
    elements = (
        ('mesh', MeshRecord),
        ('quad', QuadRecord),
        ('light', LightRecord),
        ('fuzz', FuzzRecord),
    )
    for n, c in elements:
        c.prefix = n

    def __init__(self, path):
        p = path
        self.name = basename(p)
        self.path = abspath(p)
        self.spatial_hash = {}

    def load(self):
        self.spatial_hash.clear()
        p = self.path
        with open_map(p, 'rb') as f:
            self.read(f)

    def read(self, f):
        h = MapHeader(f)
        self.header = h
        if h.signature != MAP_SIG:
            raise IOError('Invalid map file:'
                ' %s (wrong signature)' % self.path)
        x = h.ground_xsize
        y = h.ground_ysize
        self.read_array(f, h, 'B', 'ground', x, y)
        tx = 6 * x
        ty = 6 * y
        self.read_array(f, h, 'B', 'tile', tx, ty)
        for e in Map.elements:
            self.read_elements(f, h, e)
        self.read_array(f, h, 'h', 'segment', tx, ty)

    def read_array(self, f, h, t, n, x, y):
        c = x * y
        o = getattr(h, '%s_offset' % n)
        f.seek(o)
        a = array(t)
        a.fromfile(f, c)
        if a.itemsize > 1 and sys.byteorder != 'little':
            a.byteswap()
        setattr(self, '%s_x' % n, x)
        setattr(self, '%s_y' % n, y)
        setattr(self, '%s_count' % n, c)
        setattr(self, '%s_array' % n, a)
    
    def read_elements(self, f, h, e):
        n, Record = e
        s = getattr(h, '%s_size' % n)
        c = getattr(h, '%s_count' % n)
        o = getattr(h, '%s_offset' % n)
        a = '%s_list' % n
        if s == 0 and c == 0:
            setattr(self, a, [])
            return
        if s != Record.size:
            raise IOError('Invalid %s element size:'
                ' %d' % (n, s))
        f.seek(o)
        l = []
        sh = self.spatial_hash
        for i in range(c):
            r = Record(f)
            l.append(r)
            k = spatial_key(r)
            b = sh.setdefault(k, [])
            b.append(r)
        setattr(self, a, l)

    def save(self, filename=None):
        b = BytesIO()
        self.write(b)
        p = filename
        if p is None:
            p = self.path
        with open_map(p, 'wb') as f:
            f.write(b.getvalue())

    def write(self, f):
        h = self.header
        f.seek(h.size)
        self.write_array(f, h, 'ground')
        self.write_array(f, h, 'tile')
        for e in Map.elements:
            self.write_elements(f, h, e)
        self.write_array(f, h, 'segment')
        f.seek(0)
        h.tofile(f)

    def write_array(self, f, h, n):
        o = f.tell()
        setattr(h, '%s_offset' % n, o)
        a = getattr(self, '%s_array' % n)
        if a.itemsize > 1 and sys.byteorder != 'little':
            a = deepcopy(a)
            a.byteswap()
        a.tofile(f)

    def write_elements(self, f, h, e):
        n, Record = e
        o = f.tell()
        l = getattr(self, '%s_list' % n)
        setattr(h, '%s_size' % n, Record.size)
        setattr(h, '%s_count' % n, len(l))
        setattr(h, '%s_offset' % n, o)
        for r in l:
            r.tofile(f)

    def bounds(self):
        w = self.tile_x
        h = self.tile_y
        return Rect(0, 0, w - 1, h - 1)

    def elements_in(self, r):
        h = self.spatial_hash
        for y in range(r.y0, r.y1 + 1):
            for x in range(r.x0, r.x1 + 1):
                b = h.get((x, y))
                if b is None:
                    continue
                for e in b:
                    yield e

    def ground_in(self, r):
        x0 = r.x0 // 6
        y0 = r.y0 // 6
        x1 = r.x1 // 6
        y1 = r.y1 // 6
        w = self.ground_x
        a = self.ground_array
        for y in range(y0, y1 + 1):
            o = w * y
            for x in range(x0, x1 + 1):
                g = a[x + o]
                yield x, y, g

    def tiles_in(self, r):
        w = self.tile_x
        a = self.tile_array
        x0, y0, x1, y1 = r.as_tuple()
        for y in range(y0, y1 + 1):
            o = w * y
            for x in range(x0, x1 + 1):
                t = a[x + o]
                yield x, y, t

    def set_ground(self, x, y, g):
        w = self.ground_x
        self.ground_array[x + w * y] = g

    def set_tile(self, x, y, t):
        w = self.tile_x
        self.tile_array[x + w * y] = t

    def add_element(self, e):
        n = e.__class__.prefix
        l = getattr(self, '%s_list' % n)
        l.append(e)
        k = spatial_key(e)
        h = self.spatial_hash
        b = h.setdefault(k, [])
        b.append(e)

    def get_segment(self, x, y):
        a = self.segment_array
        w = self.segment_x
        return a[x + w * y]

    def set_segment(self, x, y, v):
        a = self.segment_array
        w = self.segment_x
        a[x + w * y] = v

    def _filt(self, n, r):
        a = '%s_list' % n
        l = getattr(self, a)
        h = self.spatial_hash
        c = []
        for e in l:
            k = spatial_key(e)
            if k in r:
                h[k].remove(e)
            else:
                c.append(e)
        setattr(self, a, c)

    def remove_elements(self, r):
        self._filt('quad', r)
        self._filt('light', r)
        self._filt('fuzz', r)
        for e in self.elements_in(r):
            e.blend = 20

    def remove_ground(self, r):
        a = self.ground_array
        w = self.ground_x
        for x, y, g in self.ground_in(r):
            a[x + w * y] = 255

    def remove_tiles(self, r):
        a = self.tile_array
        w = self.tile_x
        for x, y, t in self.tiles_in(r):
            a[x + w * y] = 0

    def remove_segments(self, r):
        a = self.segment_array
        w = self.segment_x
        for x, y, t in self.tiles_in(r):
            a[x + w * y] = 0

def copy_ground(mi, s, mo, d):
    ox = d.x // 6 - s.x0 // 6
    oy = d.y // 6 - s.y0 // 6
    for x, y, g in mi.ground_in(s):
        mo.set_ground(ox + x, oy + y, g)

def copy_tiles(mi, s, mo, d):
    ox = d.x - s.x0
    oy = d.y - s.y0
    for x, y, t in mi.tiles_in(s):
        mo.set_tile(ox + x, oy + y, t)

def copy_segments(mi, s, mo, d):
    ox = d.x - s.x0
    oy = d.y - s.y0
    for x, y, t in mi.tiles_in(s):
        v = mi.get_segment(x, y)
        mo.set_segment(ox + x, oy + y, v)

def copy_elements(mi, s, mo, d):
    ox = (d.x - s.x0) * 0.5
    oy = (d.y - s.y0) * 0.5
    for e in mi.elements_in(s):
        if hasattr(e, 'blend') and e.blend == 20:
            continue
        c = deepcopy(e)
        c.position[0] += ox
        c.position[1] += oy
        mo.add_element(c)

def argparse_rect(s):
    a = [int(i) for i in s.split(',')]
    if len(a) != 4:
        e = 'Invalid rectangle: %s' % s
        raise argparse.ArgumentTypeError(e)
    return Rect(*a)

def argparse_tile(s):
    a = [int(i) for i in s.split(',')]
    if len(a) != 2:
        e = 'Invalid tile coordinates: %s' % s
        raise argparse.ArgumentTypeError(e)
    return Tile(*a)

def main():
    ap = ArgumentParser(
        description='Copy a rectangular region from one'
        ' Eternal Lands map to another.',
    )
    ap.add_argument('input',
        help='The map file to copy from.',
    )
    ap.add_argument('output',
        help='The map file to write to.',
    )
    ap.add_argument('-s', '--source',
        action='store', type=argparse_rect,
        metavar='xmin,ymin,xmax,ymax',
        help='The rectangular region to copy from,'
        ' in tile coordinates. The end points are'
        ' inclusive. If omitted, the entire map is'
        ' copied.',
    )
    ap.add_argument('-d', '--destination',
        action='append', type=argparse_tile,
        metavar='xmin,ymin',
        help='The tile coordinates to copy to. If'
        ' omitted, (0,0) is used. May be supplied'
        ' multiple times to make several copies.',
    )
    ap.add_argument('-r', '--remove',
        action='store_true',
        help='Remove existing tiles, meshes, lights,'
        ' etc., in the destination rectangles in the'
        ' output map.',
    )
    a = ap.parse_args()
    mi = Map(a.input)
    mi.load()
    mo = Map(a.output)
    mo.load()
    s = a.source or mi.bounds()
    l = a.destination or [Tile(0, 0)]
    for d in l:
        if a.remove:
            r = Rect(d.x, d.y, d.x + s.w - 1, d.y + s.h - 1)
            mo.remove_ground(r)
            mo.remove_tiles(r)
            mo.remove_segments(r)
            mo.remove_elements(r)
        copy_ground(mi, s, mo, d)
        copy_tiles(mi, s, mo, d)
        copy_segments(mi, s, mo, d)
        copy_elements(mi, s, mo, d)
    mo.save()

if __name__ == '__main__':
    main()
