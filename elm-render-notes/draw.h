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
#ifndef ELM_NOTE_RENDER_DRAW_H
#define ELM_NOTE_RENDER_DRAW_H

#include "note.h"

#include <cairo.h>

void draw_point(cairo_t *c, const note_t *n);
void draw_place(cairo_t *c, const note_t *n);
void draw_icon(cairo_t *c, const note_t *n);
void draw_scale(cairo_t *c, const note_t *n);

/* symbol, color name, R, G, B */
#define X_POINTS(X) \
  X(w, white, 1, 1, 1) \
  X(_, black, 0, 0, 0) \
  X(l, red, 0.99, 0.02, 0.04) \
  X(o, orange, 0.99, 0.47, 0) \
  X(r, yellow, 0.89, 1, 0) \
  X(m, green, 0.31, 0.94, 0.16) \
  X(t, cyan, 0, 0.99, 1) \
  X(f, purple, 0.71, 0.02, 1) \
  X(p, pink, 1, 0.58, 0.96) \
  X(d, brown, 0.65, 0.39, 0.17) \
  X(a, grey, 0.51, 0.57, 0.61)

#endif /* ELM_NOTE_RENDER_DRAW_H */
