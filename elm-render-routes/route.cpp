//
//  Copyright (C) 2014 Cole Minor
//  This file is part of elm-render-routes
//
//  elm-render-routes is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  elm-render-routes is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "route.hpp"

#include "format.hpp"

#include <cstdio>
#include <stdexcept>

void Route::parse(const char *s) {
  int ax, ay, bx, by;
  const char *t = "%d,%d %d,%d";
  if (4 != sscanf(s, t, &ax, &ay, &bx, &by)) {
    Format f = "Failed to parse route '{}'";
    f.bind(s);
    throw ParseError(f.result());
  }
  start = Tile(ax, ay);
  end = Tile(bx, by);
}

size_t to_chars(const Route &r, char *b, size_t l, const char *) {
  const Tile &s = r.start;
  const Tile &e = r.end;
  size_t n = snprintf(b, l, "%d,%d-%d,%d", s.x, s.y, e.x, e.y);
  return n > l ? 0 : n;
}
