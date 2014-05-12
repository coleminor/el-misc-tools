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

#include "utility.h"

#include <zlib.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* type, name */
#define X_HEADER_ELEMENTS(X) \
  X(uint32_t, _signature) \
  X(uint32_t, terrain_x) \
  X(uint32_t, terrain_y) \
  X(uint32_t, terrain_offset) \
  X(uint32_t, tile_offset) \
  X(uint32_t, mesh_size) \
  X(uint32_t, mesh_count) \
  X(uint32_t, mesh_offset) \
  X(uint32_t, quad_size) \
  X(uint32_t, quad_count) \
  X(uint32_t, quad_offset) \
  X(uint32_t, light_size) \
  X(uint32_t, light_count) \
  X(uint32_t, light_offset) \
  X(uint8_t, internal) \
  X(uint8_t, version) \
  X(uint8_t, _unused1) \
  X(uint8_t, _unused2) \
  X(float, ambient_red) \
  X(float, ambient_green) \
  X(float, ambient_blue) \
  X(uint32_t, fuzz_size) \
  X(uint32_t, fuzz_count) \
  X(uint32_t, fuzz_offset) \
  X(uint32_t, segment_offset)

#define UNUSED(n) (#n[0] == '_')

typedef struct {
#define AS_FIELD(t, n) t n;
  X_HEADER_ELEMENTS(AS_FIELD)
#undef AS_FIELD
} header_t;

static int big_endian(void) {
  const union {
    uint32_t i;
    uint8_t b[4];
  } u = { 0x01020304 };
  return u.b[0] == 1;
}

static uint32_t extract_uint32_t(const uint8_t *b) {
  const uint32_t *p = (const uint32_t *) b;
  uint32_t v;
  if (big_endian()) {
    v = __builtin_bswap32(*p);
  } else {
    v = *p;
  }
  return v;
}

static uint8_t extract_uint8_t(const uint8_t *b) {
  return *b;
}

static float extract_float(const uint8_t *b) {
  union {
    uint32_t i;
    float f;
  } u;
  u.i = extract_uint32_t(b);
  return u.f;
}

map_t *map_new(const char *p) {
  uint8_t b[sizeof(header_t)], *d;
  int n, l, c;
  gzFile f;
  map_t *m;
  header_t h;

  if (!(f = gzopen(p, "rb")))
    die("gzopen failed: %s", strerror(errno));
  l = sizeof b;
  n = gzread(f, b, l);
  if (n == -1)
    die("gzread failed: %s", strerror(errno));
  if (n != l)
    die("map header is incomplete");

  if (memcmp("elmf", b, 4))
    die("map file signature not found");

  d = b;
#define AS_EXTRACT(t, n) \
  if (!UNUSED(n)) \
    h.n = extract_##t(d); \
  d += sizeof(t);
  X_HEADER_ELEMENTS(AS_EXTRACT)
#undef AS_EXTRACT

  c = 36 * h.terrain_x * h.terrain_y;
  d = malloc(c);

  if (-1 == gzseek(f, h.tile_offset, SEEK_SET))
    die("gzseek failed: %s", strerror(errno));

  n = gzread(f, d, c);
  if (n == -1)
    die("gzread failed: %s", strerror(errno));
  if (n != c)
    die("failed to read all %d map tiles", c);

  gzclose(f);
      
  m = calloc(1, sizeof *m);
  m->width = 6 * h.terrain_x;
  m->height = 6 * h.terrain_y;
  m->tiles = d;

  return m;
}

void map_free(map_t *m) {
  free(m->tiles);
  free(m);
}
