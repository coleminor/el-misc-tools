/*
 *  Copyright (C) 2014 Cole Minor
 *  This file is part of elm-draw-tiles.
 *
 *  elm-draw-tiles is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  elm-draw-tiles is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "options.h"

#include "utility.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_OUTPUT "hmout.png"

options_t options;

void options_usage(void) {
  const char *u =
    "Usage: " EXECUTABLE_NAME " [options] MAP\n"
    "\n"
    "Options:\n"
    "  -o FILENAME      output image file name\n"
    "  -s WIDTH,HEIGHT  image dimensions in pixels\n"
    "\n"
    "MAP is a ELM file, optionally gzip compressed.\n"
    "\n"
    "If the -s option is omitted, the image will have\n"
    "dimensions equal to size in tiles of the map.\n"
    "\n"
    "If the -o option is omitted, the image is\n"
    "written to '" DEFAULT_OUTPUT "' in the current\n"
    "directory.\n"
    "\n";
  printf("%s", u);
  exit(1);
}

static void parse_xy(const char *s, int *x, int *y) {
  if (2 != sscanf(s, "%d,%d", x, y))
    die("invalid dimension argument: %s", s);
}

int options_parse(int c, char **a) {
  options_t *o = &options;
  int v;
  memset(o, 0, sizeof *o);
  while (-1 != (v = getopt(c, a, "ho:s:"))) {
    switch (v) {
    case 'o':
      o->output = optarg;
      break;
    case 's':
      parse_xy(optarg, &o->image_width, &o->image_height);
      break;
    case 'h':
    default:
      options_usage();
    }
  }
  return optind;
}

void options_validate(void) {
  options_t *o = &options;
  if (o->image_width < 1)
    o->image_width = 0;
  if (o->image_height < 1)
    o->image_height = 0;
  if (!o->output)
    o->output = DEFAULT_OUTPUT;
}

