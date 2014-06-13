//
//  Copyright (C) 2014 Cole Minor
//  This file is part of elm-render-routes
//
//  elm-render-routes is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  elm-render-routes is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef ELM_RENDER_ROUTES_UTILITY_HPP
#define ELM_RENDER_ROUTES_UTILITY_HPP

#include "preprocessor.hpp"

#include <string>
#include <vector>

typedef std::vector<std::string> Strings;
typedef std::vector<double> Numbers;

#define DISALLOW_COPY_AND_ASSIGNMENT(C) \
  private: C(const C &); void operator=(const C &)

template<typename T, size_t N>
char (&_countof(T (&a)[N]))[N];
#define COUNTOF(a) (sizeof(_countof(a)))

#define STATIC_ASSERT(b) \
  typedef char PP_ID(static_assertion)[(b)?1:-1]

static inline double clamp(double v, double a, double b) {
  const double t = v < a ? a : v;
  return t > b ? b : t;
}

#endif // ELM_RENDER_ROUTES_UTILITY_HPP
