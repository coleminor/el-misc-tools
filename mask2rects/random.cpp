//
// Copyright 2013 Cole Minor <c.minor@inbox.com>
//
//    This file is part of mask2rects.
//
//    mask2rects is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    mask2rects is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with mask2rects.  If not, see <http://www.gnu.org/licenses/>.
//
#include <ctime>
#include <cstdlib>

#include "log.hpp"
#include "random.hpp"

static unsigned time_seed() {
  time_t now = std::time(0);
  unsigned char *p = (unsigned char *)&now;
  unsigned s = 0;
  for (size_t i = 0; i < sizeof(now); i++)
    s = s * 257 + p[i];
  return s;
}

void initialize_rng(int sv) {
  if (sv == -1) {
    unsigned s = time_seed();
    logver("RNG seed set to system time hash (%u)", s);
    std::srand(s);
  } else {
    logver("using %d as RNG seed", sv);
    std::srand(sv);
  }
}

int random_int(int upper_bound) {
  double d = std::rand() * (1.0 / (RAND_MAX + 1.0));
  int r = d * upper_bound;
  return r;
}
