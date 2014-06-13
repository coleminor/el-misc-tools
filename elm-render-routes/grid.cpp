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
#include "grid.hpp"

#include "utility.hpp"

Grid::Grid():
  m_width(),
  m_height(),
  m_costs()
{}

void Grid::resize(int w, int h) {
  m_width = w;
  m_height = h;
  m_costs.reserve(w * h);
}

size_t Grid::adjacent(const Tile &t, Tile *v, size_t m) const {
  static const int o[][2] = {
    {-1, -1}, {0, -1}, {1, -1},
    {-1,  0},          {1,  0},
    {-1,  1}, {0,  1}, {1,  1}
  };
  size_t r = 0;
  for (size_t i = 0; r < m && i < COUNTOF(o); ++i) {
    int x = t.x + o[i][0], y = t.y + o[i][1];
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
      continue;
    v[r].x = x;
    v[r].y = y;
    ++r;
  }
  return r;
}
