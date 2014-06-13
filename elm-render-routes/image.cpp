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
#include "image.hpp"

#include "color.hpp"
#include "defines.hpp"
#include "format.hpp"

#include <cairo.h>

#include <stdexcept>

Image::Image(const std::string &p):
  m_surface(),
  m_stride(),
  m_data()
{
  const char *t = p.c_str();
  cairo_surface_t *s;
  s = cairo_image_surface_create_from_png(t);
  cairo_status_t r;
  r = cairo_surface_status(s);
  if (r != CAIRO_STATUS_SUCCESS) {
    cairo_surface_destroy(s);
    const char *e = cairo_status_to_string(r);
    Format f = "Failed to load '{}': {}";
    f.bind(t, e);
    throw std::runtime_error(f.result());
  }
  m_surface = s;
  m_stride = cairo_image_surface_get_stride(s);
  m_data = cairo_image_surface_get_data(s);
}

Image::Image(int w, int h):
  m_surface(),
  m_stride(),
  m_data()
{
  cairo_format_t f = CAIRO_FORMAT_ARGB32;
  cairo_surface_t *s;
  s = cairo_image_surface_create(f, w, h);
  m_surface = s;
  m_stride = cairo_image_surface_get_stride(s);
  m_data = cairo_image_surface_get_data(s);
}

Image::~Image() {
  clear();
}

void Image::clear() {
  if (cairo_surface_t *&s = m_surface) {
    cairo_surface_destroy(s);
    s = 0;
    m_stride = 0;
    m_data = 0;
  }
}

void Image::save(const std::string &p) {
  const char *t = p.c_str();
  cairo_status_t r;
  r = cairo_surface_write_to_png(m_surface, t);
  if (r != CAIRO_STATUS_SUCCESS) {
    const char *e = cairo_status_to_string(r);
    Format f = "Failed to write PNG image '{}': {}";
    f.bind(t, e);
    throw std::runtime_error(f.result());
  }
}

int Image::width() const {
  return cairo_image_surface_get_width(m_surface);
}

int Image::height() const {
  return cairo_image_surface_get_height(m_surface);
}

void Image::get(int x, int y, Color &c) const {
  const unsigned char *d = m_data;
  d += 4 * x + y * m_stride;
  unsigned char *v = c.rgba;
#if IS_BIG_ENDIAN
  v[0] = d[1];
  v[1] = d[2];
  v[2] = d[3];
  v[3] = d[0];
#else
  v[0] = d[2];
  v[1] = d[1];
  v[2] = d[0];
  v[3] = d[3];
#endif
}
