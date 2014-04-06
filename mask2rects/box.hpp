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
#ifndef MASK2RECTS_BOX_HPP
#define MASK2RECTS_BOX_HPP

#include "point.hpp"

struct Box {
  int x0, y0, x1, y1;
  Box():x0(0), y0(0), x1(0), y1(0) {}
  Box(int x, int y):x0(x), y0(y), x1(x), y1(y) {}
  Box(const Point &p):x0(p.x), y0(p.y), x1(p.x), y1(p.y) {}

  int width() const { return x1 - x0 + 1; }
  int height() const { return y1 - y0 + 1; }
  unsigned area() const { return width() * height(); }
  Point origin() const { return Point(x0, y0); }

  void add(int x, int y);
  void expand(int d);
};

#endif // MASK2RECTS_BOX_HPP
