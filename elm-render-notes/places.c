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
#include "places.h"

#include "utility.h"

typedef struct {
  const char *short_form;
  const char *long_form;
  int hash;
} place_name_t;

static place_name_t place_names[] = {
#define AS_STRUCT_INIT(s, l) { #s, l, 0 },
  X_PLACE_NAMES_FR(AS_STRUCT_INIT)
#undef AS_STRUCT_INIT
  {0}
};

static int place_name_hash(const char *s) {
  return s[0] + s[1] * 26 + s[2] * 26 * 26;
}

void places_initialize(void) {
  place_name_t *p;
  for (p = place_names; p->short_form; p++) {
    p->hash = place_name_hash(p->short_form);
  }
}

const char *places_get_long_name(const char *s) {
  int h;
  place_name_t *p;

  if (!s[0] || !s[1] || !s[2] || s[3])
    return NULL;

  h = place_name_hash(s);
  for (p = place_names; p->short_form; p++)
    if (h == p->hash)
      return p->long_form;

  die("unrecognized short place name '%s'", s);
}
