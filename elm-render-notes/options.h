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
#ifndef ELM_NOTE_RENDER_OPTIONS_H
#define ELM_NOTE_RENDER_OPTIONS_H

typedef struct {
  int image_width;
  int image_height;
  int map_width;
  int map_height;
  const char *font_name;
  int title_size;
  int place_size;
  int route_size;
  const char *output;
  double scale_multiplier;
} options_t;

extern options_t options;

void options_usage(void)
  __attribute__((noreturn));
int options_parse(int argc, char **argv);
void options_validate(void);

#endif /* ELM_NOTE_RENDER_OPTIONS_H */
