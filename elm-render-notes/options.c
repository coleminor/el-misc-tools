/*
 *  Copyright (C) 2014 Cole Minor
 *  This file is part of elm-render-notes.
 *
 *  elm-render-notes is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  elm-render-notes is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "options.h"

#include "draw.h"
#include "places.h"
#include "utility.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_OUTPUT "rnout.png"
#define DEFAULT_TITLE_SIZE 64
#define DEFAULT_PLACE_SIZE 30
#define DEFAULT_ROUTE_SIZE 22

options_t options;

void options_usage(void) {
  const char *u =
    "Usage: " EXECUTABLE_NAME " [OPTIONS] NOTEFILE\n"
    "\n"
    "Options:\n"
    "  -f FONT          font name used for text\n"
    "  -i WIDTH,HEIGHT  image dimensions in pixels\n"
    "  -l               list place name short forms\n"
    "  -m WIDTH,HEIGHT  map dimensions in tiles\n"
    "  -n               print note format help\n"
    "  -o FILENAME      output image file name\n"
    "  -p PIXELS        size of place names\n"
    "  -r PIXELS        size of route names\n"
    "  -s FLOAT         scale multiplier\n"
    "  -t PIXELS        size of map title\n"
    "  -v               print program version\n"
    "\n"
    "NOTEFILE is a UTF-8 text file containing\n"
    "one map note per line, as created by the\n"
    "program elm-annotate.\n"
    "\n"
    "If the -o option is omitted, the image is\n"
    "written to '" DEFAULT_OUTPUT "' in the current\n"
    "directory.\n"
    "\n"
    "Use the -n option for a detailed description\n"
    "of the note file syntax.\n";
  puts(u);
  exit(1);
}

static void print_version(void) {
  printf("%s v%d.%d\n",
         EXECUTABLE_NAME,
         VERSION_MAJOR,
         VERSION_MINOR);
  exit(0);
}

static void print_note_help(void) {
  const char *s =
    "Notes are stored in UTF-8 encoded text files.\n"
    "Each line in the file is of the form:\n"
    "\n"
    "  X,Y NOTE\n"
    "\n"
    "where X,Y are tile coordinates and NOTE is a\n"
    "map mark symbol with label:\n"
    "\n"
    "  =TEXT    map title\n"
    "  .TEXT    place name\n"
    "  >MAP     route to another map\n"
    "\n"
    "otherwise the symbol designates a point:\n";
  puts(s);
#define AS_HELP(s, c, r, g, b) \
  if (#s[0] != '_') \
    puts("  " #s " - " #c);
  X_POINTS(AS_HELP)
  puts("");
#undef AS_HELP
  s = "Points may be optionally followed by an\n"
    "orientation hint and text or place name:\n"
    "\n"
    "  l - position text on the left\n"
    "  r - on the right\n"
    "  u - up, or above the point\n"
    "  d - down, or below the point\n"
    "\n"
    "For a list of place name abbreviations valid\n"
    "for MAP, use the -l option.\n";
  puts(s);
  exit(0);
}

static void print_place_names(void) {
#define AS_HELP(s, m) \
  puts(#s " - " m);
  X_PLACE_NAMES_FR(AS_HELP)
#undef AS_HELP
  exit(0);
}

static void parse_xy(const char *s, int *x, int *y) {
  if (2 != sscanf(s, "%d,%d", x, y))
    die("invalid dimension argument: %s", s);
}

int options_parse(int c, char **a) {
  options_t *o = &options;
  int v;
  memset(o, 0, sizeof *o);
  while (-1 != (v = getopt(c, a, "f:hi:lm:no:p:r:s:t:v"))) {
    const char *s = optarg;
    switch (v) {
    case 'f':
      o->font_name = s;
      break;
    case 'i':
      parse_xy(s, &o->image_width, &o->image_height);
      break;
    case 'h':
      options_usage();
      break;
    case 'l':
      print_place_names();
      break;
    case 'm':
      parse_xy(s, &o->map_width, &o->map_height);
      break;
    case 'n':
      print_note_help();
      break;
    case 'o':
      o->output = s;
      break;
    case 'p':
      o->place_size = atoi(s);
      break;
    case 'r':
      o->route_size = atoi(s);
      break;
    case 's':
      o->scale_multiplier = atof(s);
      break;
    case 't':
      o->title_size = atoi(s);
      break;
    case 'v':
      print_version();
      break;
    default:
      options_usage();
    }
  }
  return optind;
}

void options_validate(void) {
  options_t *o = &options;
  if (o->image_width < 1)
    die("invalid image width");
  if (o->image_height < 1)
    die("invalid image height");
  if (o->map_width < 1)
    die("invalid map width");
  if (o->map_height < 1)
    die("invalid map height");
  if (!o->font_name)
    o->font_name = "Serif";
  if (!o->output)
    o->output = DEFAULT_OUTPUT;
  if (o->title_size < 1)
    o->title_size = DEFAULT_TITLE_SIZE;
  if (o->place_size < 1)
    o->place_size = DEFAULT_PLACE_SIZE;
  if (o->route_size < 1)
    o->route_size = DEFAULT_ROUTE_SIZE;
  if (o->scale_multiplier < 0.01)
    o->scale_multiplier = 1.0;
}

