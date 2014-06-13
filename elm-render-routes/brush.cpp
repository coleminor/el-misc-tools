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
#include "brush.hpp"

#include "color.hpp"
#include "image.hpp"
#include "point.hpp"
#include "tile.hpp"

#include <cmath>

Brush::Brush(Image &i):
  m_context(cairo_create(i.m_surface))
{}

Brush::~Brush() {
  cairo_destroy(m_context);
}

void Brush::point(const Tile &t) {
  double x = t.x, y = t.y;
  cairo_rectangle(m_context, x, y, 1, 1);
  cairo_fill(m_context);
}

void Brush::color(const Color &c) {
  double v[4];
  for (size_t i = 0; i < 4; ++i)
    v[i] = c.rgba[i] / 255.0;
  cairo_set_source_rgba(m_context, v[0], v[1], v[2], v[3]);
}

void Brush::line(const Tile &a, const Tile &b) {
  cairo_t *c = m_context;
  cairo_move_to(c, a.x, a.y);
  cairo_line_to(c, b.x, b.y);
  cairo_stroke(c);
}

void Brush::width(double w) {
  cairo_set_line_width(m_context, w);
}

void Brush::dashes(const Numbers &v) {
  cairo_set_dash(m_context, &v[0], v.size(), 0.0);
}

static void solve(const Points &a, Points &p) {
  const size_t n = a.size();
  p.reserve(n);
  Points t(n);
  Point b = 2.0;
  p[0] = a[0] / b;
  for (size_t i = 1; i < n; ++i) {
    t[i] = 1.0 / b;
    b = (i < n - 1 ? 4.0 : 3.5) - t[i];
    p[i] = (a[i] - p[i-1]) / b;
  }
  for (size_t i = 1; i < n; ++i)
    p[n-i-1] -= t[n-i] * p[n-i];
}

// Bézier spline
static void derive(const Points &v, Points &c1, Points &c2) {
  const size_t n = v.size() - 1;
  c1.reserve(n);
  c2.reserve(n);

  Points r(n);
  r[0] = v[0] + 2.0 * v[1];
  for (size_t i = 1; i < n - 1; ++i)
    r[i] = 4.0 * v[i] + 2.0 * v[i+1];
  r[n-1] = 0.5 * (8.0 * v[n-1] + v[n]);

  Points p;
  solve(r, p);

  for (size_t i = 0; i < n - 1; ++i) {
    c1[i] = p[i];
    c2[i] = 2.0 * v[i+1] - p[i+1];
  }
  c1[n-1] = p[n-1];
  c2[n-1] = (v[n] + p[n-1]) / 2.0;
}

void Brush::curve(const Tiles &v) {
  Points k, c1, c2;
  for (size_t i = 0; i < v.size(); ++i)
    k.push_back(Point(v[i].x, v[i].y));
  derive(k, c1, c2);

  cairo_t *c = m_context;
  cairo_move_to(c, k[0].x, k[0].y);
  for (size_t i = 0; i < k.size() - 1; ++i) {
    cairo_curve_to(c, c1[i].x, c1[i].y,
                   c2[i].x, c2[i].y,
                   k[i+1].x, k[i+1].y);
  }
  cairo_stroke(c);
}

void Brush::lines(const Tiles &v) {
  cairo_t *c = m_context;
  cairo_move_to(c, v[0].x, v[0].y);
  for (size_t i = 1; i < v.size(); ++i)
    cairo_line_to(c, v[i].x, v[i].y);
  cairo_stroke(c);
}

void Brush::anchor(const Tile &t) {
  Color f = color();
  const double s = 16, p = M_PI;
  const double x = t.x, y = t.y;
  cairo_t *c = m_context;
  cairo_save(c);
  cairo_move_to(c, x, y - 0.2 * s);
  cairo_arc(c, x, y - 0.35 * s, 0.15 * s, p/2, p*5/2);
  cairo_rel_line_to(c, 0, 0.6 * s);
  cairo_arc(c, x, y + 0.1 * s, 0.4 * s, p/2, p*7/8);
  cairo_move_to(c, x, y + 0.5 * s);
  cairo_arc_negative(c, x, y + 0.1 * s, 0.4 * s, p/2, p/8);
  cairo_move_to(c, x - 0.25 * s, y);
  cairo_rel_line_to(c, 0.5 * s, 0);
  cairo_set_line_cap(c, CAIRO_LINE_CAP_ROUND);
  width(5);
  color(BLACK);
  cairo_stroke_preserve(c);
  cairo_set_line_cap(c, CAIRO_LINE_CAP_SQUARE);
  width(2);
  color(f);
  cairo_stroke(c);
  cairo_restore(c);
}

Color Brush::color() const {
  cairo_pattern_t *p;
  p = cairo_get_source(m_context);
  double d[4];
  cairo_pattern_get_rgba(p, d, d+1, d+2, d+3);
  Color c;
  for (size_t i = 0; i < 4; ++i)
    c.rgba[i] = clamp(0.0, 255.0 * d[i], 255.0);
  return c;
}
