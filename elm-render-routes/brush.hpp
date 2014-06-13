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
#ifndef ELM_RENDER_ROUTES_BRUSH_HPP
#define ELM_RENDER_ROUTES_BRUSH_HPP

#include "tile.hpp"
#include "utility.hpp"

#include <cairo.h>

class Color;
class Image;

class Brush {
  DISALLOW_COPY_AND_ASSIGNMENT(Brush);
public:
  Brush(Image &);
  ~Brush();
  void color(const Color &);
  void width(double);
  void dashes(const Numbers &);
  void point(const Tile &);
  void line(const Tile &a, const Tile &b);
  void lines(const Tiles &);
  void curve(const Tiles &);
  void anchor(const Tile &);
private:
  Color color() const;
  cairo_t *m_context;
};

#endif // ELM_RENDER_ROUTES_BRUSH_HPP
