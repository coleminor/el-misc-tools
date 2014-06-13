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
#ifndef ELM_RENDER_ROUTES_HEURISTIC_HPP
#define ELM_RENDER_ROUTES_HEURISTIC_HPP

#include "tile.hpp"

#include <algorithm>
#include <cmath>

#define X_HEURISTIC_TYPES(X) \
  X(MANHATTAN, 0, "dx + dy") \
  X(CHEBYSHEV, 1, "max(dx, dy)") \
  X(DIAGONAL, 2, "dx+dy - (2-sqrt(2))*min(dx, dy)") \
  X(EUCLIDEAN, 3, "sqrt(dx^2 + dy^2)")

struct Heuristic {
  enum Type {
#define AS_ENUM(n, v, d) n = v,
    X_HEURISTIC_TYPES(AS_ENUM)
#undef AS_ENUM
    NUMBER_OF_TYPES    
  };
  Type type;
  Heuristic(int t):type(lookup(t)) {}

  float estimate(const Tile &a, const Tile &b) const {
    float dx = fabsf(a.x - b.x);
    float dy = fabsf(a.y - b.y);
    if (type == MANHATTAN) {
      return dx + dy;
    } else if (type == CHEBYSHEV) {
      return std::max(dx, dy);
    } else if (type == DIAGONAL) {
      static const float c = 0.5857864376f;
      return dx + dy - c * std::min(dx, dy);
    } else if (type == EUCLIDEAN) {
      return sqrtf(dx * dx + dy * dy);
    }
    return 1.0f;
  }
private:
  Type lookup(int t) const;
};

#endif // ELM_RENDER_ROUTES_HEURISTIC_HPP
