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
#ifndef ELM_RENDER_ROUTES_COLOR_HPP
#define ELM_RENDER_ROUTES_COLOR_HPP

struct Color {
  typedef unsigned char byte;
  byte rgba[4];
  Color():rgba() {}
  Color(const Color &c):rgba() {
    rgba[0] = c.rgba[0];
    rgba[1] = c.rgba[1];
    rgba[2] = c.rgba[2];
    rgba[3] = c.rgba[3];
  }
  Color(byte r, byte g, byte b, byte a):rgba() {
    rgba[0] = r;
    rgba[1] = g;
    rgba[2] = b;
    rgba[3] = a;
  }
  Color(byte r, byte g, byte b):rgba() {
    rgba[0] = r;
    rgba[1] = g;
    rgba[2] = b;
    rgba[3] = 255;
  }
  Color(byte v[4]):rgba() {
    rgba[0] = v[0];
    rgba[1] = v[1];
    rgba[2] = v[2];
    rgba[3] = v[3];
  }
  Color &operator=(const byte v[4]) {
    rgba[0] = v[0];
    rgba[1] = v[1];
    rgba[2] = v[2];
    rgba[3] = v[3];
    return *this;
  }
  operator byte *() { return rgba; }

  static const Color &palette(int i);
};

extern const Color WHITE;
extern const Color BLACK;
extern const Color RED;
extern const Color GREEN;

#endif // ELM_RENDER_ROUTES_COLOR_HPP
