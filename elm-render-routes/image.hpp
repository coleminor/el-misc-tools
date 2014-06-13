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
#ifndef ELM_RENDER_ROUTES_IMAGE_HPP
#define ELM_RENDER_ROUTES_IMAGE_HPP

#include "utility.hpp"

#include <cairo.h>

#include <string>

class Color;

class Image {
  DISALLOW_COPY_AND_ASSIGNMENT(Image);
public:
  Image(const std::string &path);
  Image(int width, int height);
  ~Image();
  void clear();
  int width() const;
  int height() const;
  void get(int x, int y, Color &) const;
  void save(const std::string &path);
private:
  friend class Brush;
  cairo_surface_t *m_surface;
  int m_stride;
  const unsigned char *m_data;
};

#endif // ELM_RENDER_ROUTES_IMAGE_HPP
