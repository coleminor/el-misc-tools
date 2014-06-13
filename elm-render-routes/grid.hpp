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
#ifndef ELM_RENDER_ROUTES_GRID_HPP
#define ELM_RENDER_ROUTES_GRID_HPP

#include "tile.hpp"

#include <vector>

class Grid {
public:
  Grid();
  void resize(int w, int h);
  int width() const { return m_width; }
  int height() const { return m_height; }
  float get(const Tile &t) const {
    return get(t.x, t.y);
  }
  void set(const Tile &t, float c) {
    set(t.x, t.y, c);
  }
  float get(int x, int y) const {
    return m_costs[x + m_width * y];
  }
  void set(int x, int y, float c) {
    m_costs[x + m_width * y] = c;
  }
  size_t adjacent(const Tile &t, Tile *v, size_t m) const;
private:
  typedef std::vector<float> Costs;
  int m_width;
  int m_height;
  Costs m_costs;
};

#endif // ELM_RENDER_ROUTES_GRID_HPP
