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
#include "draw.h"

#include "options.h"
#include "utility.h"

#include <pango/pangocairo.h>

#include <math.h>

#define MARGIN 35.0
#define FONT_OUTLINE_FRACTION (1.0 / 8.0)
#define POINT_RADIUS 5.0
#define POINT_OUTLINE 2.0
#define ARROW_SIZE 20.0
#define ANCHOR_SIZE 20.0
#define SCALE_PART_LENGTH 66.0
#define SCALE_HEIGHT 10.0

typedef double color_t[3];

#define AS_COLORS(s, n, r, g, b) \
  n = {r, g, b},
static const color_t X_POINTS(AS_COLORS) _end;
#undef AS_COLORS

static void set_cairo_color(cairo_t *c, const color_t a) {
  cairo_set_source_rgba(c, a[0], a[1], a[2], 1);
}

static PangoLayout *create_label_layout(cairo_t *c, const note_t *n) {
  PangoFontDescription *d;
  PangoLayout *l;
  char b[256];
  const char *f, *t;
  int s;

  t = note_get_label(n);
  s = note_get_font_size(n);
  f = options.font_name;
  snprintf(b, sizeof b, "%s %dpx", f, s);

  l = pango_cairo_create_layout(c);
  pango_layout_set_text(l, t, -1);
  d = pango_font_description_from_string(b);
  pango_layout_set_font_description(l, d);
  pango_font_description_free(d);

  cairo_set_line_width(c, s * FONT_OUTLINE_FRACTION);

  return l;
}

static void draw_point_label(cairo_t *c, const note_t *n) {
  char o;
  double d, x, y, w, h, s;
  PangoRectangle r;
  PangoLayout *l;

  o = n->text[1];
  if (!o)
    return;

  l = create_label_layout(c, n);

  set_cairo_color(c, black);
  pango_cairo_update_layout(c, l);
  pango_layout_get_pixel_extents(l, NULL, &r);

  d = POINT_RADIUS + POINT_OUTLINE;
  w = r.width;
  h = r.height;
  s = 2.0;
  switch (o) {
  case 'l':
    x = - d - w - s;
    y = - 0.5 * h;
    break;
  case 'r':
    x = d + s;
    y = - 0.5 * h;
    break;
  case 'u':
    x = - 0.5 * w;
    y = - d - h - s;
    break;
  case 'd':
    x = - 0.5 * w;
    y = d + s;
    break;
  default:
    die("unrecognized point label orientation "
        "'%c' for note: '%s'", o, n->text);
  }
  cairo_move_to(c, n->x + x, n->y + y);
  pango_cairo_layout_path(c, l);
  cairo_stroke_preserve(c);
  set_cairo_color(c, white);
  cairo_fill(c);

  g_object_unref(l);
}

void draw_point(cairo_t *c, const note_t *n) {
  const char *t = n->text;
  const double *f = NULL;

  if (0) {}
#define AS_ELSE_IF(s, n, r, g, b) \
  else if (*t == #s[0]) { f = n; }
  X_POINTS(AS_ELSE_IF)
#undef AS_ELSE_IF
  else {
    die("unrecognized point type: '%s'", t);
  }

  cairo_set_line_width(c, 2 * POINT_OUTLINE);
  set_cairo_color(c, black);
  cairo_arc(c, n->x, n->y, POINT_RADIUS, 0, 2*M_PI);
  cairo_stroke_preserve(c);
  set_cairo_color(c, f);
  cairo_fill(c);

  draw_point_label(c, n);
}

void draw_place(cairo_t *c, const note_t *n) {
  PangoLayout *l;
  PangoRectangle r;
  double x, y;

  l = create_label_layout(c, n);

  set_cairo_color(c, black);
  pango_cairo_update_layout(c, l);
  pango_layout_get_pixel_extents(l, NULL, &r);
  x = n->x - 0.5 * r.width;
  y = n->y - 0.5 * r.height;
  while (x - MARGIN < 0.0)
    x += 4.0;
  while (y - MARGIN < 0.0)
    y += 4.0;
  while (x + r.width + MARGIN > options.image_width)
    x -= 4.0;
  while (y + r.height + MARGIN > options.image_height)
    y -= 4.0;
  cairo_move_to(c, x, y);
  pango_cairo_layout_path(c, l);
  cairo_stroke_preserve(c);
  set_cairo_color(c, white);
  cairo_fill(c);

  g_object_unref(l);
}

static double get_arrow_rotation(const note_t *n) {
  double x, y, w, h, s;
  x = n->x;
  y = n->y;
  w = options.image_width;
  h = options.image_height;
  s = h / w;
  if (y <= s * x && y <= h - s * x) {
    return (3.0/2.0) * M_PI;
  } else if (y <= s * x && y > h - s * x) {
    return 0;
  } else if (y > s * x && y > h - s * x) {
    return 0.5 * M_PI;
  } else if (y > s * x && y <= h - s * x) {
    return M_PI;
  }
  return 0;
}

static void draw_arrow(cairo_t *c, const note_t *n) {
  double w, h;
  w = ARROW_SIZE;
  h = ARROW_SIZE;
  cairo_save(c);
  cairo_move_to(c, n->x, n->y);
  cairo_rotate(c, get_arrow_rotation(n));
  cairo_rel_move_to(c, -0.3 * w, - 0.1 * h);
  cairo_rel_line_to(c, 0.5 * w, 0);
  cairo_rel_line_to(c, 0, - 0.1 * h);
  cairo_rel_line_to(c, 0.2 * w, 0.2 * h);
  cairo_rel_line_to(c, -0.2 * w, 0.2 * h);
  cairo_rel_line_to(c, 0, - 0.1 * h);
  cairo_rel_line_to(c, -0.5 * w, 0);
  cairo_close_path(c);
  cairo_set_line_width(c, 4);
  set_cairo_color(c, black);
  cairo_stroke_preserve(c);
  set_cairo_color(c, white);
  cairo_fill(c);
  cairo_restore(c);
}

static void draw_anchor(cairo_t *c, const note_t *n) {
  double x, y, s, p;
  x = n->x;
  y = n->y;
  s = ANCHOR_SIZE;
  p = M_PI;
  cairo_save(c);
  cairo_move_to(c, x, y - 0.2 * s);
  cairo_arc(c, x, y - 0.35 * s, 0.15 * s, p/2, p*5/2);
  cairo_rel_line_to(c, 0, 0.6 * s);
  cairo_arc(c, x, y + 0.1 * s, 0.4 * s, p/2, p*7/8);
  cairo_move_to(c, x, y + 0.5 * s);
  cairo_arc_negative(c, x, y + 0.1 * s, 0.4 * s, p/2, p/8);
  cairo_move_to(c, x - 0.25 * s, y);
  cairo_rel_line_to(c, 0.5 * s, 0);
  cairo_set_line_cap(c, CAIRO_LINE_CAP_ROUND);
  cairo_set_line_width(c, 5);
  set_cairo_color(c, black);
  cairo_stroke_preserve(c);
  cairo_set_line_cap(c, CAIRO_LINE_CAP_SQUARE);
  cairo_set_line_width(c, 2);
  set_cairo_color(c, white);
  cairo_stroke(c);
  cairo_restore(c);
}

void draw_icon(cairo_t *c, const note_t *n) {
  double w, x, y;
  PangoLayout *l;
  PangoRectangle r;

  if (n->text[0] == '@') {
    draw_anchor(c, n);
    w = ANCHOR_SIZE;
  } else {
    draw_arrow(c, n);
    w = ARROW_SIZE;
  }

  l = create_label_layout(c, n);

  set_cairo_color(c, black);
  cairo_set_line_width(c, 2);
  pango_cairo_update_layout(c, l);
  pango_layout_get_pixel_extents(l, NULL, &r);
  x = n->x + 0.5 * w;
  y = n->y - 0.5 * r.height;
  if (x + r.width + MARGIN > options.image_width)
    x = n->x - 0.5 * w - r.width;
  cairo_move_to(c, x, y);
  pango_cairo_layout_path(c, l);
  cairo_stroke_preserve(c);
  set_cairo_color(c, white);
  cairo_fill(c);

  g_object_unref(l);
}

static void draw_scale_label(cairo_t *c, const note_t *n) {
  PangoFontDescription *d;
  PangoLayout *l;
  PangoRectangle r;
  const char *f;
  char b[256];
  int s, i;
  double v, x, y;
  const options_t *o;

  o = &options;
  f = o->font_name;
  s = 28;
  snprintf(b, sizeof b, "%s %dpx", f, s);
  d = pango_font_description_from_string(b);

  l = pango_cairo_create_layout(c);
  v = 2.0 * SCALE_PART_LENGTH
      * o->scale_multiplier
      * o->map_width / o->image_width;
  i = round(v);
  snprintf(b, sizeof b, "%d", i);
  pango_layout_set_text(l, b, -1);
  pango_layout_set_font_description(l, d);
  pango_font_description_free(d);

  cairo_set_line_width(c, s * FONT_OUTLINE_FRACTION);
  set_cairo_color(c, black);
  pango_cairo_update_layout(c, l);
  pango_layout_get_pixel_extents(l, NULL, &r);
  x = n->x - 0.5 * r.width;
  y = n->y + 0.5 * SCALE_HEIGHT;
  cairo_move_to(c, x, y);
  pango_cairo_layout_path(c, l);
  cairo_stroke_preserve(c);
  set_cairo_color(c, white);
  cairo_fill(c);

  g_object_unref(l);
}

void draw_scale(cairo_t *c, const note_t *n) {
  double x, y, w, h, l;

  l = SCALE_PART_LENGTH;
  w = 2 * l;
  h = SCALE_HEIGHT;

  x = n->x - 0.5 * w;
  y = n->y - 0.5 * h;
  set_cairo_color(c, black);
  cairo_rectangle(c, x, y, w, h);
  cairo_fill(c);
  set_cairo_color(c, white);
  cairo_rectangle(c, x + 1, y + 1, l - 1, h - 2);
  cairo_fill(c);

  draw_scale_label(c, n);
}
