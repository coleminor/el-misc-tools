/*
 *  elm-render-notes - Renders Eternal-Lands map notes to an image.
 *  Copyright (C) 2014 Cole Minor
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "image.h"
#include "note.h"
#include "options.h"
#include "places.h"
#include "utility.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static void render_notes(const char *p) {
  cairo_surface_t *s;
  cairo_t *c;
  char b[1024];
  const char *o;
  note_t _n, *n = &_n;
  FILE *f;
  int l;

  if (!(f = fopen(p, "r")))
    die("failed to open '%s': %s", p, strerror(errno));

  s = image_create();
  c = cairo_create(s);
  for (l = 1; fgets(b, sizeof b, f); l++) {
    chomp(b);
    if (!note_parse(n, b))
      die("invalid note on line %d: '%s'", l, b);
    if (!note_convert_coordinates(n))
      die("invalid map tile coordinates "
          "on line %d: '%s'", l, b);
    note_draw(n, c);
  }
  fclose(f);
  cairo_destroy(c);

  o = options.output;
  cairo_surface_write_to_png(s, o);
  cairo_surface_destroy(s);
  printf("wrote image to '%s'\n", o);
}

int main(int argc, char **argv) {
  int i;

  if (argc < 2)
    options_usage();
  i = options_parse(argc, argv);
  if (i >= argc)
    die("missing note file argument");
  if (argc - i > 1)
    die("too many arguments");
  options_validate();

  places_initialize();
  render_notes(argv[i]);

  return 0;
}
