//
//  Copyright (C) 2014 Cole Minor
//  This file is part of elm-annotate
//
//  elm-annotate is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  elm-annotate is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "map.hpp"

#include "endian.hpp"
#include "format.hpp"
#include "utility.hpp"

#include <stdint.h>
#include <zlib.h>

#include <cerrno>
#include <cstring>

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

struct Header {
#define AS_FIELDS(t, n) t n;
X_HEADER_ELEMENTS(AS_FIELDS)
  bool read(const std::string &path, std::string &e);
};

static const char *image_extensions[] = {
  ".jpg", ".png", ".dds", ".bmp", 0
};

static inline uint32_t extract_uint32_t(const uint8_t *d) {
  uint32_t v = *reinterpret_cast<const uint32_t *>(d);
  Endian::convert(v);
  return v;
}

static inline float extract_float(const uint8_t *d) {
  union {
    uint32_t i;
    float f;
  };
  i = extract_uint32_t(d);
  return f;
}

static inline uint8_t extract_uint8_t(const uint8_t *d) {
  return *d;
}

bool Header::read(const std::string &path, std::string &e) {
  gzFile g;
  const char *p = path.c_str();
  Format f;
  if (!(g = gzopen(p, "rb"))) {
    f = "gzopen failed: {}";
    e = f.bind(strerror(errno));
    return false;
  }
  SCOPE_FREE_USING(gzclose, gzFile, g);

  const size_t s = sizeof(Header);
  uint8_t b[s];
  int n = gzread(g, b, s);
  if (n == -1) {
    f = "gzread failed: {}";
    e = f.bind(strerror(errno));
    return false;
  }
  if (n != s) {
    f = "Failed to read a complete "
      "map header (need {} bytes but "
      "got only {})";
    e = f.bind(s, n);
    return false;
  }
  if (0 != memcmp("elmf", b, 4)) {
    e = "Map file signature not found";
    return false;
  }

  const uint8_t *d = b;
#define AS_EXTRACTION(t, n) \
  if (#n[0] != '_') \
    n = extract_##t(d); \
  d += sizeof(t);
X_HEADER_ELEMENTS(AS_EXTRACTION)

  return true;
}

Map::Map():
  m_name(),
  m_error(),
  m_image(),
  m_width(0),
  m_height(0)
{}

static std::string map_name(const std::string &p) {
  std::string r = p;
  strip_directory(r);
  strip_ending(r, ".gz");
  strip_ending(r, ".elm");
  return r;
}

static std::string find_image(const std::string &p) {
  std::string b = p;
  strip_ending(b, ".gz");
  strip_ending(b, ".elm");
  const char **a = image_extensions;
  for (const char **e = a; *e; e++) {
    std::string r = b + *e;
    if (file_exists(r))
      return r;
  }
  return b + a[0];
}

bool Map::load(const std::string &p) {
  Header h;
  if (!h.read(p, m_error))
    return false;

  m_name = map_name(p);
  m_image = find_image(p);
  m_width = 6 * h.terrain_x;
  m_height = 6 * h.terrain_y;

  return true;
}

void Map::set_size(int x, int y) {
  m_width = x;
  m_height = y;
}

void Map::set_image(const std::string &p) {
  std::string n = p;
  strip_directory(n);
  for (const char **e = image_extensions; *e; ++e)
    strip_ending(n, *e);
  m_name = n;
  m_image = p;
}

bool Map::match_extension(const std::string &p) {
  return ends_with(p, ".elm") || ends_with(p, ".elm.gz");
}
