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
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <vector>

using namespace std;

#include "options.hpp"

Options opts;

static void usage(void) {
  fprintf(stderr,
    "\n"
    "Usage: mask2rects [options] <PNG image mask>\n\n"
    "Options: \n\n"
    "-a <number>    default 1\n"
    "    Minimum area of covering rectangles. Any rectangles\n"
    "    in the computed covering whose area is less than this\n"
    "    value will be skipped.\n\n"
    "-c <number>    default 50\n"
    "    Maximum number of rectangles used to cover each region.\n\n"
    "-d             boolean option\n"
    "    Print all verbose message along with extra debugging\n"
    "    information.\n\n"
    "-f             boolean option\n"
    "    Flip all y-coordinate (vertical) values in the output\n"
    "    rectangle list.\n\n"
    "-i <number>    default 10\n"
    "    Maximum number of iterations of the fitting algorithm.\n"
    "    A good cover will try to be found for each region this\n"
    "    number of times before giving up.\n\n"
    "-o <filename>  default \"output-rects.txt\"\n"
    "    The computed rectangles will be written to this file.\n\n"
    "-r <filename>  string option\n"
    "    If this option is set, a PNG image will be written to the\n"
    "    given file showing the regions and rectangle covers found.\n\n"
    "-s <number>    default -1\n"
    "    Seed used for random number generation. The default value\n"
    "    causes the current system time to be used.\n\n"
    "-u <number>    default 0\n"
    "    Maximum number of uncovered points to permit per region.\n"
    "    Points that would have been covered by rectangles omitted\n"
    "    via the -a option are not included.\n\n"
    "-v             boolean option\n"
    "    Enable verbose messages. More detailed information will\n"
    "    be printed to stdout about what is going on.\n\n"
  );
}

static void set_option_defaults() {
  opts.minimum_box_area = 1;
  opts.maximum_box_count = 50;
  opts.flip_rectangle_y_coordinates = false;
  opts.maximum_iterations = 10;
  opts.output_rects_file = "output-rects.txt";
  opts.output_image_file = "";
  opts.random_seed = -1;
  opts.maximum_uncovered_points = 0;
  opts.verbose = false;
  opts.debug = false;
  opts.arguments.clear();
}

bool parse_command_line(int argc, char **argv) {

  set_option_defaults();

  int o;
  while ((o = getopt(argc, argv, "a:c:dfhi:o:r:s:u:v")) != -1) {
    switch (o) {
    case 'a':
      opts.minimum_box_area = atoi(optarg);
      break;
    case 'c':
      opts.maximum_box_count = atoi(optarg);
      break;
    case 'd':
      opts.debug = true;
      break;
    case 'f':
      opts.flip_rectangle_y_coordinates = 1;
      break;
    case 'h':
      usage();
      return false;
    case 'i':
      opts.maximum_iterations = atoi(optarg);
      break;
    case 'o':
      opts.output_rects_file = optarg;
      break;
    case 'r':
      opts.output_image_file = optarg;
      break;
    case 's':
      opts.random_seed = atoi(optarg);
      break;
    case 'u':
      opts.maximum_uncovered_points = atoi(optarg);
      break;
    case 'v':
      opts.verbose = true;
      break;
    case '?':
    default:
      return false;
    }
  }

  while (optind < argc)
    opts.arguments.push_back(argv[optind++]);

  return true;
}
