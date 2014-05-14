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
#include "note.h"

#include "draw.h"
#include "options.h"
#include "places.h"
#include "utility.h"

#include <stdio.h>
#include <string.h>

int note_convert_coordinates(note_t *n) {
  double x, y, iw, ih, mw, mh;
  const options_t *o = &options;
  x = n->x;
  y = n->y;
  iw = o->image_width;
  ih = o->image_height;
  mw = o->map_width;
  mh = o->map_height;
  if (!(0 <= x && x < mw))
    return 0;
  if (!(0 <= y && y < mh))
    return 0;
  n->x = iw * (x + 0.5) / mw;
  n->y = ih * (mh - y - 1.0 + 0.5) / mh;
  return 1;
}

static int parse_xy(const char *s, note_t *n) {
  int x, y;
  if (2 != sscanf(s, "%d,%d", &x, &y))
    return 0;
  n->x = x;
  n->y = y;
  return 1;
}

int note_parse(note_t *n, const char *s) {
  char *t;
  if (!parse_xy(s, n))
    return 0;
  if (!(t = strchr(s, ' ')))
    return 0;
  t++;
  n->text = t;
  return 1;
}

void note_draw(const note_t *n, cairo_t *c) {
  switch (n->text[0]) {
  case '.':
  case '=':
    draw_place(c, n);
    break;
  case '#':
  case '>':
    draw_icon(c, n);
    break;
  case 's':
    draw_scale(c, n);
    break;
  default:
    draw_point(c, n);
    break;
  }
}

int note_get_font_size(const note_t *n) {
  const options_t *o = &options;
  switch (n->text[0]) {
  case '.': return o->place_size;
  case '=': return o->title_size;
  default: return o->route_size;
  }
}

const char *note_get_label(const note_t *n) {
  const char *t, *r;
  t = n->text;
  switch (*t) {
  case '.':
    r = places_get_long_name(t + 1);
    if (r)
      break;
  case '=':
    r = t + 1;
    break;
  case '>':
    r = places_get_long_name(t + 1);
    break;
  case 't':
    if (!t[1] || !t[2])
      return "";
    r = places_get_long_name(t + 2);
    if (!r)
      r = t + 2;
    break;
  default:
    if (!t[0] || !t[1] || !t[2])
      return "";
    r = t + 2;
  }
  return r;
}

