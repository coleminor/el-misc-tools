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
#include "color.hpp"

#include "utility.hpp"

const Color WHITE(255, 255, 255);
const Color BLACK(0, 0, 0);
const Color RED(255, 50, 50);
const Color GREEN(50, 255, 50);

static const Color colors[] = {
  Color(255, 179, 0),
  Color(128, 62, 117),
  Color(255, 104, 0),
  Color(166, 189, 215),
  Color(193, 0, 32),
  Color(206, 162, 98),
  Color(129, 112, 102),
  Color(0, 125, 52),
  Color(246, 118, 142),
  Color(83, 55, 122),
  Color(255, 142, 0),
  Color(179, 40, 81),
  Color(244, 200, 0),
  Color(127, 24, 13),
  Color(147, 170, 0),
  Color(89, 51, 21),
  Color(241, 58, 19),
  Color(35, 44, 22),
  Color(255, 122, 92)
};

const Color &Color::palette(int i) {
  return colors[i % COUNTOF(colors)];
}
