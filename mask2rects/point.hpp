//
// Copyright 2013 Cole Minor <c.minor@inbox.com>
//
//    This file is part of mask2rects.
//
//    mask2rects is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    mask2rects is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with mask2rects.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef MASK2RECTS_POINT_HPP
#define MASK2RECTS_POINT_HPP

// The convention used here is:
//   north +y
//   east  +x
//   south -y
//   west  -x
enum directions { D_NORTH = 0, D_EAST, D_SOUTH, D_WEST };

struct Point {
  int x, y;
  Point():x(0), y(0) {}
  Point(int x, int y):x(x), y(y) {}
};

#endif // MASK2RECTS_POINT_HPP
