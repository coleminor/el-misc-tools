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
#ifndef ELM_RENDER_ROUTES_POINT_HPP
#define ELM_RENDER_ROUTES_POINT_HPP

#include <vector>

struct Point {
  double x, y;
  Point():x(), y() {}
  Point(double v):x(v), y(v) {}
  Point(double a, double b):x(a), y(b) {}
  Point(const Point &p):x(p.x), y(p.y) {}
  Point &operator-=(const Point &p) {
    x -= p.x;
    y -= p.y;
    return *this;
  }
};

typedef std::vector<Point> Points;

Point operator+(const Point &a, const Point &b) {
  return Point(a.x + b.x, a.y + b.y);
}
Point operator-(const Point &a, const Point &b) {
  return Point(a.x - b.x, a.y - b.y);
}
Point operator*(const Point &a, const Point &b) {
  return Point(a.x * b.x, a.y * b.y);
}
Point operator/(const Point &a, double v) {
  return Point(a.x / v, a.y / v);
}
Point operator/(const Point &a, const Point &b) {
  return Point(a.x / b.x, a.y / b.y);
}

#endif // ELM_RENDER_ROUTES_POINT_HPP
