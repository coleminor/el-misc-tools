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
#ifndef ELM_RENDER_ROUTES_ROUTE_HPP
#define ELM_RENDER_ROUTES_ROUTE_HPP

#include "tile.hpp"

#include <stdexcept>

struct Route {
  Tile start, end;
  Route():start(), end() {}
  void parse(const char *);

  struct ParseError : std::runtime_error {
    ParseError(const char *w):
      std::runtime_error(w)
    {}
  };
};

size_t to_chars(const Route &r, char *b, size_t l, const char *);

#endif // ELM_RENDER_ROUTES_ROUTE_HPP
