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
#include "name.hpp"

#include <stdint.h>
#include <string.h>

static uint32_t hash(const char *, size_t);

Name::Name(const char *s):
  value(s),
  digest(hash(s, strlen(s)))
{}

/*
 * SuperFastHash - http://www.azillionmonkeys.com/qed/hash.html
 * Copyright (C) 2004-2008 Paul Hsieh
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#undef GET16BITS
#if (defined(__GNUC__) && defined(__i386__)) \
  || defined(__WATCOMC__) || defined(_MSC_VER) \
  || defined(__BORLANDC__) || defined(__TURBOC__)
#define GET16BITS(d) (*((const uint16_t *)(d)))
#endif

#if !defined(GET16BITS)
#define GET16BITS(d) \
  ((((uint32_t)(((const uint8_t *)(d))[1])) << 8) \
   +(uint32_t)(((const uint8_t *)(d))[0]))
#endif

static uint32_t hash(const char *d, size_t l) {
  if (!d || !l)
    return 0;

  size_t r = l & 3;
  l >>= 2;

  uint32_t h = l, t;
  for (; l > 0; l--) {
    h += GET16BITS(d);
    t = (GET16BITS(d + 2) << 11) ^ h;
    h = (h << 16) ^ t;
    d += 4;
    h += h >> 11;
  }

  switch (r) {
  case 3:
    h += GET16BITS(d);
    h ^= h << 16;
    h ^= static_cast<signed char>(d[2]) << 18;
    h += h >> 11;
    break;
  case 2:
    h += GET16BITS(d);
    h ^= h << 11;
    h += h >> 17;
    break;
  case 1:
    h += static_cast<signed char>(*d);
    h ^= h << 10;
    h += h >> 1;
  }

  h ^= h << 3;
  h += h >> 5;
  h ^= h << 4;
  h += h >> 17;
  h ^= h << 25;
  h += h >> 6;

  return h;
}
