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
#include "options.hpp"

#include "defines.hpp"
#include "format.hpp"
#include "heuristic.hpp"
#include "report.hpp"

#include <getopt.h>

#include <cstdlib>
#include <stdexcept>

#define DEFAULT_OUTPUT_PATH "./rrout.png"
#define DEFAULT_ROUTES_PATH "./routes.txt"
#define DEFAULT_CELL_SIZE 12
#define DEFAULT_LAND_COST 200.0
#define DEFAULT_CROSS_COST 10.0
#define DEFAULT_LINE_WIDTH 4.0
#define DEFAULT_HEURISTIC Heuristic::MANHATTAN

static Options s_options;
Options *g_options = &s_options;

static option long_options[] = {
  {"anchors", no_argument, 0, 'a'},
  {"cell-size", required_argument, 0, 'c'},
  {"cross-cost", required_argument, 0, 'x'},
  {"dashes", required_argument, 0, 'd'},
  {"help", no_argument, 0, 'h'},
  {"heuristic", required_argument, 0, 'H'},
  {"land-cost", required_argument, 0, 'm'},
  {"lines", no_argument, 0, 'l'},
  {"output", required_argument, 0, 'o'},
  {"overlay", no_argument, 0, 'O'},
  {"routes", required_argument, 0, 'r'},
  {"verbose", no_argument, 0, 'v'},
  {"version", no_argument, 0, 'V'},
  {"width", required_argument, 0, 'w'},
  {0, 0, 0, 0}
};

static const char *short_options
  = "ac:d:hH:lm:o:Or:vVw:x:";

void Options::usage() const {
  const char *s =
    "Usage: " EXECUTABLE_NAME " [options] PNG-IMAGE\n"
    "\n"
    "Options:\n"
    "  -a --anchors            draw endpoint anchors\n"
    "  -c --cell-size NUMBER   pixels per cell side\n"
    "  -d --dashes NUMBER-LIST dash pattern lengths\n"
    "  -h --help               print this message\n"
    "  -H --heuristic ID       path distance estimator\n"
    "  -l --lines              draw lines not curves\n"
    "  -m --land-cost NUMBER   obstacle movement cost\n"
    "  -o --output PATH        write PNG image here\n"
    "  -O --overlay            draw on mask image\n"
    "  -r --routes PATH        route list file\n"
    "  -w --width NUMBER       line width\n"
    "  -v --verbose            print more messages\n"
    "  -V --version            print program version\n"
    "  -x --cross-cost NUMBER  existing path cost\n"
    "\n"
    "The image argument is a grayscale mask giving the\n"
    "location of obstacles on the grid, with white pixels\n"
    "corresponding to tiles of maximum cell movement\n"
    "cost.\n";
  report(s);
  const char *d =
    "The heuristic is used to estimate the cost of the\n"
    "path to the goal from a given location. The available\n"
    "choices are:\n"
    "\n"
#define AS_HELP(n, v, d) "  " #v " - " #n ": " d "\n"
    X_HEURISTIC_TYPES(AS_HELP)
#undef AS_HELP
    "";
  report(d);
  exit(1);
}

static void print_version() {
  Format f = "{} v{}.{}";
  f.bind(EXECUTABLE_NAME, VERSION_MAJOR, VERSION_MINOR);
  report(f);
  exit(0);
}

Options::Options():
  arguments(),
  output_path(),
  routes_path(),
  cell_size(),
  use_lines(),
  draw_anchors(),
  land_cost(),
  cross_cost(),
  dashes(),
  line_width(),
  heuristic(),
  overlay(),
  verbose()
{}

static void parse_number_list(const char *s, Numbers &v) {
  char b[64], *p;
  const char *const e = b + sizeof b;
  while (1) {
    p = b;
    while (*s && *s != ',' && p < e)
      *p++ = *s++;
    *p = '\0';
    double d = atof(b);
    if (d > 0.1)
      v.push_back(d);
    if (*s)
      s++;
    else
      break;
  }
}

void Options::parse(int argc, char **argv) {
  while (1) {
    int i = 0, c;
    c = getopt_long(argc, argv, short_options,
                    long_options, &i);
    if (c == -1)
      break;
    switch (c) {
    case 'a':
      draw_anchors = true;
      break;
    case 'c':
      cell_size = atoi(optarg);
      break;
    case 'd':
      parse_number_list(optarg, dashes);
      break;
    case 'h':
      usage();
      break;
    case 'H':
      heuristic = atoi(optarg);
      break;
    case 'l':
      use_lines = true;
      break;
    case 'm':
      land_cost = atof(optarg);
      break;
    case 'o':
      output_path = optarg;
      break;
    case 'O':
      overlay = true;
      break;
    case 'r':
      routes_path = optarg;
      break;
    case 'v':
      verbose = true;
      break;
    case 'V':
      print_version();
      break;
    case 'w':
      line_width = atof(optarg);
      break;
    case 'x':
      cross_cost = atof(optarg);
      break;
    default:
      exit(1);
    }
  }
  arguments.assign(argv + optind, argv + argc);

  if (output_path.empty())
    output_path = DEFAULT_OUTPUT_PATH;
  if (routes_path.empty())
    routes_path = DEFAULT_ROUTES_PATH;
  if (cell_size < 1)
    cell_size = DEFAULT_CELL_SIZE;
  if (land_cost < 1.0)
    land_cost = DEFAULT_LAND_COST;
  if (cross_cost < 1.0)
    cross_cost = DEFAULT_CROSS_COST;
  if (line_width < 0.1)
    line_width = DEFAULT_LINE_WIDTH;
  int &h = heuristic;
  if (h < 0 || h >= Heuristic::NUMBER_OF_TYPES) {
    h = DEFAULT_HEURISTIC;
    Format f = "Using heuristic {}";
    warn(f.bind(h));
  }
}
