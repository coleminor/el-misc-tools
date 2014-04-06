#!/usr/bin/env python3

#
#    elm-remove-bad-lights.py
#       - Remove invalid lights from Eternal Lands map files
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

__version__ = '0.1.0'

import sys
import os
import re
import struct
import gzip
from argparse import ArgumentParser
from os.path import basename, abspath
from array import array
from io import BytesIO
from copy import deepcopy
from time import strftime
from shutil import move

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

MAP_FILE_SIGNATURE = b'elmf'

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

def open_map(p, m):
    if p.endswith('.gz'):
        return gzip.open(p, mode=m)
    return open(p, m)

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

    def load(self):
        p = self.path
        with open_map(p, 'rb') as f:
            self.read(f)

    def read(self, f):
        h = MapHeader(f)
        self.header = h
        if h.signature != MAP_FILE_SIGNATURE:
            raise ValueError('Invalid map file:'
                ' %s (wrong signature)' % self.path)
        x = h.ground_xsize
        y = h.ground_ysize
        self.read_array(f, h, 'B', 'ground', x, y)
        tx = 6 * x
        ty = 6 * y
        self.read_array(f, h, 'B', 'tile', tx, ty)
        for e in Map.elements:
            self.read_elements(f, h, e)
        if h.segment_offset:
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
            raise ValueError('Invalid %s element size:'
                ' %d' % (n, s))
        f.seek(o)
        l = []
        for i in range(c):
            r = Record(f)
            r.id = i
            l.append(r)
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
        if h.segment_offset:
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

def valid_light(l, b):
    p = l.position
    c = l.color
    for i in (0, 1, 2):
        if not (b[i][0] < p[i] < b[i][1]):
            return False
        if not (-1 < c[i] < 1000):
            return False
    return True    

def world_bounds(m):
    h = m.header
    x = 0.5 * 6 * h.ground_xsize
    y = 0.5 * 6 * h.ground_ysize
    e = ((-10, x + 10), (-10, y + 10), (-10, 100))
    return e

def print_light(n, l):
    s = '%s: light id:%d' \
        ' position-xyz:(%.1f, %.1f, %.1f)' \
        ' color-rgb:[%.2f, %.2f, %.2f]'
    p = l.position
    c = l.color
    a = (n, l.id,) + tuple(p) + tuple(c)
    print(s % a)

def list_bad_lights(m):
    b = world_bounds(m)
    n = m.name
    for l in m.light_list:
        if not valid_light(l, b):
            print_light(n, l)

def remove_bad_lights(m):
    b = world_bounds(m)
    o = m.light_list
    n = [l for l in o if valid_light(l, b)]
    d = len(o) - len(n)
    if d:
        m.light_list = n
        print('%s: removed %d bad lights' % (m.name, d))
    return d

def backup_filename(n):
    return n + strftime('-%Y%m%d_%H%M%S.bak')

def main():
    ap = ArgumentParser(
        description='Remove invalid lights from'
        ' Eternal Lands map files.',
    )
    ap.add_argument('maps', nargs='+',
        help='the map files to fix',
    )
    ap.add_argument('-l', '--list',
        action='store_true',
        help='just list all bad lights found',
    )
    ap.add_argument('-N', '--nobackup',
        action='store_true',
        help='do not create backup copies',
    )
    a = ap.parse_args()
    for p in a.maps:
        m = Map(p)
        m.load()
        n = m.name
        if a.list:
            list_bad_lights(m)
            continue
        if not remove_bad_lights(m):
            print('%s: no bad lights found' % n)
            continue
        if not a.nobackup:
            b = backup_filename(p)
            move(p, b)
            print('%s: backup saved to %s' % (n, b))
        m.save()

if __name__ == '__main__':
    main()
