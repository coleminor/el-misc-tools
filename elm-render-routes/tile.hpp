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
#ifndef ELM_RENDER_ROUTES_TILE_HPP
#define ELM_RENDER_ROUTES_TILE_HPP

#include <string>
#include <vector>

struct Tile {
  int x, y;
  Tile():x(), y() {}
  Tile(int a, int b):x(a), y(b) {}
  Tile(const Tile &t):x(t.x), y(t.y) {}
  Tile &operator=(const Tile &t) {
    x = t.x;
    y = t.y;
    return *this;
  }
  Tile operator+(int v) {
    return Tile(x + v, y + v);
  }
  Tile operator*(int v) {
    return Tile(x * v, y * v);
  }
};

typedef std::vector<Tile> Tiles;

static inline bool operator==(const Tile &a, const Tile &b) {
  return a.x == b.x && a.y == b.y;
}

static inline Tile operator/(const Tile &a, int v) {
  return Tile(a.x / v, a.y / v);
}

size_t to_chars(const Tile &t, char *b, size_t l, const char *);

#endif // ELM_RENDER_ROUTES_TILE_HPP
