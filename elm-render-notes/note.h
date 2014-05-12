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
#ifndef ELM_NOTE_RENDER_NOTE_H
#define ELM_NOTE_RENDER_NOTE_H

#include <cairo.h>

typedef struct {
  const char *text;
  double x, y;
} note_t;

int note_parse(note_t *n, const char *s);
int note_convert_coordinates(note_t *n);
const char *note_get_label(const note_t *n);
int note_get_font_size(const note_t *n);
void note_draw(const note_t *n, cairo_t *c);

#endif /* ELM_NOTE_RENDER_NOTE_H */
