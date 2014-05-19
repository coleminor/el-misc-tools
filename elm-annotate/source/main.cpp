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
#include "application.hpp"
#include "options.hpp"

#include <locale.h>

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");

  Options o;
  if (!o.parse(argc, argv))
    return 1;

  Application a;
  if (!a.initialize(o))
    return 1;
  a.run();
  a.shutdown();
  
  return 0;
}
