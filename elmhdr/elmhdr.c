/*
 * elmhdr - Print ELM file header
 * Copyright (C) 2014 Cole Minor
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

static void die(const char *f, ...) {
  char b[1024];
  va_list a;
  va_start(a, f);
  vsnprintf(b, sizeof b, f, a);
  va_end(a);
  fprintf(stderr, "%s\n", b);
  exit(1);
}

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

static void read_header(const char *p, uint8_t *b, int l) {
  gzFile f;
  int n;
  if (!(f = gzopen(p, "rb"))) {
    die("%s: gzopen failed: %s", p, strerror(errno));
  }
  n = gzread(f, b, l);
  if (n == -1) {
    die("%s: gzread failed: %s", p, strerror(errno));
  }
  if (n != l) {
    die("%s: failed to read a complete map header "
        " (tried to read %d bytes, but got only %d)",
        p, l, n);
  }
  gzclose(f);
  if (memcmp("elmf", b, 4)) {
    die("%s: map file signature not found", p);
  }
}

/* arguments: type, name */
#define X_ELEMENTS(X) \
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

typedef struct {
#define AS_FIELD(t, n) \
  t n;
X_ELEMENTS(AS_FIELD)
} header_t;

#define FMT_uint32_t PRIu32
#define FMT_uint8_t PRIu8
#define FMT_float "f"
#define FMT(x) FMT_##x
#define UNUSED(n) (#n[0] == '_')

static void print_header(uint8_t *d) {

#define AS_VARIABLE(t, n) \
  t n;
X_ELEMENTS(AS_VARIABLE)

#define AS_EXTRACT(t, n) \
  if (!UNUSED(n)) { \
    n = extract_##t(d); \
  } \
  d += sizeof(t);
X_ELEMENTS(AS_EXTRACT)

#define AS_PRINT(t, n) \
  if (!UNUSED(n)) { \
    printf(#n " = %" FMT(t) "\n", n); \
  }
X_ELEMENTS(AS_PRINT)

}

int main(int argc, char **argv) {
  uint8_t b[sizeof(header_t)];
  int i;

  if (argc < 2) {
    die("usage: %s map.elm.gz", argv[0]);
    return 1;
  }
  for (i = 1; i < argc; i++) {
    read_header(argv[i], b, sizeof b);
    print_header(b);
  }

  return 0;
}
