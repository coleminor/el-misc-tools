//
// Copyright 2013 Cole Minor <c.minor@inbox.com>
//
//    This file is part of mask2rects.
//
//    mask2rects is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    mask2rects is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with mask2rects.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef MASK2RECTS_OPTIONS_HPP
#define MASK2RECTS_OPTIONS_HPP

#include <string>
#include <vector>

struct Options {
  size_t minimum_box_area;
  size_t maximum_box_count;
  bool flip_rectangle_y_coordinates;
  size_t maximum_iterations;
  std::string output_rects_file;
  std::string output_image_file;
  int random_seed;
  size_t maximum_uncovered_points;
  bool verbose;
  bool debug;

  std::vector<std::string> arguments;
};

extern Options opts;

bool parse_command_line(int argc, char **argv);

#endif // MASK2RECTS_OPTIONS_HPP
