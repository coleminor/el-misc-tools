//
//  Copyright (C) 2014 Cole Minor
//  This file is part of elm-annotate
//
//  elm-annotate is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  elm-annotate is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "options.hpp"

#include "defines.hpp"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

static option long_options[] = {
  {"help", no_argument, 0, 'h'},
  {"controls", no_argument, 0, 'c'},
  {"notes", required_argument, 0, 'n'},
  {"reference", required_argument, 0, 'r'},
  {0, 0, 0, 0}
};
static const char *short_options = "chn:r:s:";

void Options::usage() const {
  const char *s =
    "Usage: " EXECUTABLE_NAME " [options] FILES\n"
    "\n"
    "Options:\n"
    "  -c --controls          show interface controls\n"
    "  -h --help              show this message\n"
    "  -n --notes PATH        note output directory\n"
    "  -r --reference PATH    reference image directory\n"
    "\n"
    "FILES may be one or more possibly gzip compressed\n"
    "map files, or images, or directories. All map files\n"
    "found in each directory will be loaded.\n"
    "\n"
    "If an image is loaded directly, the note coordinates\n"
    "will be pixels, otherwise for maps they are in tiles.\n"
    "In both cases the origin is the lower left corner.\n"
    "\n"
    "Notes are saved to UTF-8 text files of the form\n"
    "\"NAME-notes.txt\" in the output directory, or the\n"
    "current working directory if unspecified. NAME is\n"
    "the base name of the input file without extensions.\n"
    "\n";
  printf("%s", s);
}

static void print_controls() {
  const char *s =
    "Controls:\n"
    "  left-click  create note or toggle note selection\n"
    "  right-click remove note\n"
    "  A           toggle aspect preservation\n"
    "  B           previous map\n"
    "  L           reload current map\n"
    "  M           toggle mark display\n"
    "  N           next map\n"
    "  Q           quit\n"
    "  R           toggle reference image display\n"
    "  S           save current map notes\n"
    "  DELETE      remove current note\n"
    "  ENTER       select next empty note, or next map\n"
    "  ESCAPE      deselect note\n"
    "  PAGEDOWN    go forward 10 maps\n"
    "  SPACE       next map\n"
    "  PAGEUP      go back 10 maps\n"
    "  BACKSPACE   previous map\n"
    "  TAB         cycle through notes\n"
    "\n";
  printf("%s", s);
}

Options::Options():
  arguments(),
  compare(false),
  reference_path(),
  notes_path()
{}

static void normalize_path(std::string &p) {
  if (p.size() > 1)
    strip_ending(p, "/");
  if (p.empty())
    p = ".";
}

bool Options::parse(int argc, char **argv) {
  while (1) {
    int i = 0, c;
    c = getopt_long(argc, argv, short_options,
                    long_options, &i);
    if (c == -1)
      break;
    switch (c) {
    case 'c':
      print_controls();
      return false;
    case 'n':
      notes_path = optarg;
      break;
    case 'r':
      compare = true;
      reference_path = optarg;
      break;
    case 'h':
    default:
      usage();
      return false;
    }
  }
  arguments.assign(argv + optind, argv + argc);
  if (arguments.empty()) {
    usage();
    return false;
  }

  normalize_path(reference_path);
  normalize_path(notes_path);
  return true;
}
