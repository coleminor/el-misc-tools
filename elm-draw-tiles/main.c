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
#include "map.h"
#include "options.h"
#include "utility.h"

#include <cairo.h>

#include <stdio.h>

static cairo_surface_t *create_surface(void) {
  cairo_surface_t *s;
  cairo_format_t f;
  int w, h;

  f = CAIRO_FORMAT_ARGB32;
  w = options.image_width;
  h = options.image_height;
  s = cairo_image_surface_create(f, w, h);
  return s;
}

static void render_heightmap(const char *p) {
  cairo_surface_t *s;
  cairo_t *c;
  const char *o;
  map_t *m;
  int x, y, mw, mh, *pw, *ph;
  uint8_t t;
  double u, v, tw, th;

  m = map_new(p);
  mw = m->width;
  mh = m->height;
  pw = &options.image_width;
  ph = &options.image_height;
  if (!*pw)
    *pw = mw;
  if (!*ph)
    *ph = mh;
  tw = (double) *pw / mw;
  th = (double) *ph / mh;

  s = create_surface();
  c = cairo_create(s);
  cairo_set_source_rgb(c, 0, 0, 0);
  cairo_paint(c);
  cairo_set_source_rgb(c, 1, 1, 1);
  for (y = 0; y < mh; y++) {
    for (x = 0; x < mw; x++) {
      t = m->tiles[mw * y + x];
      if (!t)
        continue;
      u = tw * x;
      v = th * (mh - y - 1);
      cairo_rectangle(c, u, v, tw, th);
      cairo_fill(c);
    }
  }
  cairo_destroy(c);

  o = options.output;
  cairo_surface_write_to_png(s, o);
  cairo_surface_destroy(s);
  printf("wrote %dx%d PNG image '%s'\n",
         *pw, *ph, o);

  map_free(m);
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

  render_heightmap(argv[i]);
  return 0;
}
