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
#ifndef ELM_RENDER_ROUTES_OPTIONS_HPP
#define ELM_RENDER_ROUTES_OPTIONS_HPP

#include "utility.hpp"

struct Options {
  Options();
  void parse(int argc, char **argv);
  void usage() const;

  Strings arguments;
  std::string output_path;
  std::string routes_path;
  int cell_size;
  bool use_lines;
  bool draw_anchors;
  double land_cost;
  double cross_cost;
  Numbers dashes;
  double line_width;
  int heuristic;
  bool overlay;
  bool verbose;
};

extern Options *g_options;

#endif // ELM_RENDER_ROUTES_OPTIONS_HPP
